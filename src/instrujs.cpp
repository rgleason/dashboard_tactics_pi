/******************************************************************************
* $Id: instrujs.cpp, v1.0 2019/11/30 VaderDarth Exp $
*
* Project:  OpenCPN
* Purpose:  dahbooard_tactics_pi plug-in
* Author:   Jean-Eudes Onfray
* 
***************************************************************************
*   Copyright (C) 2010 by David S. Register   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
***************************************************************************
*/

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/version.h>
#include <wx/event.h>

#include "instrujs.h"
#include "plugin_ids.h"

extern int GetRandomNumber(int, int);

wxBEGIN_EVENT_TABLE (InstruJS, DashboardInstrument)
   EVT_TIMER (myID_TICK_INSTRUJS, InstruJS::OnThreadTimerTick)
   EVT_SIZE (InstruJS::OnSize)
   EVT_CLOSE (InstruJS::OnClose)
wxEND_EVENT_TABLE ()

//************************************************************************************************************************
// Numerical instrument for engine monitoring data
//************************************************************************************************************************

InstruJS::InstruJS( TacticsWindow *pparent, wxWindowID id, wxString ids,
                    PI_ColorScheme cs ) :
                    DashboardInstrument( pparent, id, "---", 0LL, true )
{
    m_pparent = pparent;

    m_istate = JSI_UNDEFINED;
    m_handshake = JSI_HDS_NO_REQUEST;
    m_requestServed = wxEmptyString;
    m_hasRequestedId = false;
    m_setAllPathGraceCount = JSI_GETALL_GRACETIME;
    m_pushHereUUID = wxEmptyString;

    m_id = id;
    m_ids = ids;
    m_substyle = "day";
    if ( cs == PI_GLOBAL_COLOR_SCHEME_DUSK )
        m_substyle = "dusk";
    if ( cs == PI_GLOBAL_COLOR_SCHEME_NIGHT )
        m_substyle = "night";
    m_newsubstyle = wxEmptyString;
    m_title = L"InstruJS";
    m_format = L"%.1f"; // unlike trad. Dashboard instrument, we manage the format
    m_data = wxString::Format( m_format, 0.0 );
    m_dataout = L"";
    m_threadRunning = false;
    std::unique_lock<std::mutex> init_m_mtxScriptRun( m_mtxScriptRun, std::defer_lock );
    m_webpanelCreated = false;
    m_webpanelCreateWait = false;
    m_webPanelSuspended  = false;
    m_webpanelStopped = false;

    // Create the WebKit (type of - implementation varies) view
    m_pWebPanel = wxWebView::New( );
#if wxUSE_WEBVIEW_IE
    wxWebViewIE::MSWSetModernEmulationLevel();
#endif
    m_istate = JSI_NO_WINDOW;
    m_pThreadInstruJSTimer = NULL;
    m_lastSize = wxControl::GetSize();
}

InstruJS::~InstruJS(void)
{
    if ( this->m_pThreadInstruJSTimer != NULL ) {
        this->m_pThreadInstruJSTimer->Stop();
        delete this->m_pThreadInstruJSTimer;
    }
    if ( !m_pushHereUUID.IsEmpty() ) // if parent window itself is going away
        this->m_pparent->unsubscribeFrom( m_pushHereUUID );

    if ( (this->m_webpanelCreated || this->m_webpanelCreateWait)
         && !this->m_webpanelStopped ) {
        this->m_pWebPanel->Stop();
    }
    if ( this->m_webpanelCreated || this->m_webpanelCreateWait ) {
        delete this->m_pWebPanel;
    } 
}

void InstruJS::stopScript( )
{
    if ( this->m_pThreadInstruJSTimer != NULL ) {
        this->m_pThreadInstruJSTimer->Stop();
    }
    if ( (m_webpanelCreated || m_webpanelCreateWait) && !m_webpanelStopped ) {
        m_pWebPanel->Stop();
        m_webpanelStopped = true;
    }
}

void InstruJS::OnClose( wxCloseEvent &event )
{
    if ( this->m_pThreadInstruJSTimer != NULL ) {
        this->m_pThreadInstruJSTimer->Stop();
    }
    if ( !m_pushHereUUID.IsEmpty() ) { // civilized parent window informs: Close()
        m_pparent->unsubscribeFrom( m_pushHereUUID );
        m_pushHereUUID = wxEmptyString;
    }
    if ( (this->m_webpanelCreated || this->m_webpanelCreateWait)
         && !this->m_webpanelStopped ) {
        this->m_pWebPanel->Stop();
        this->m_webpanelStopped = true;
    }
    event.Skip(); // Destroy() must be called
}

void InstruJS::timeoutEvent()
{
    derivedTimeoutEvent();
}

void InstruJS::PushData(double data, wxString unit, long long timestamp)
{
    if ( !std::isnan(data) ) {
        setTimestamp( timestamp );  // Triggers also base class' watchdog
        m_data = wxString::Format( m_format, data );
    } // then valid data 
}

wxString InstruJS::RunScript( const wxString &javascript )
{
    wxString result = wxEmptyString;
#if wxCHECK_VERSION(3,1,0)
    if ( !m_pWebPanel->RunScript( javascript, &result ) ) {
        result = "NotReady";
    }
#else
    m_pWebPanel->RunScript( javascript );
#endif
    return result;
}

void InstruJS::loadHTML( wxString fullPath, wxSize initialSize )
{
    if ( !m_webpanelCreated && !m_webpanelCreateWait ) {
        m_pWebPanel->Create(
            this, wxID_ANY, fullPath );
        m_istate = JSI_WINDOW;
        wxSizer *thisSizer = GetSizer();
        m_pWebPanel->SetSizer( thisSizer ); 
        m_pWebPanel->SetAutoLayout( true );
        m_pWebPanel->SetInitialSize( initialSize );
        FitIn();
        m_webpanelCreateWait = true;
        // Start the instrument pane control thread (faster polling 1/10 seconds for initial loading)
        m_pThreadInstruJSTimer = new wxTimer( this, myID_TICK_INSTRUJS );
        m_pThreadInstruJSTimer->Start(100, wxTIMER_CONTINUOUS);
    }
}

void InstruJS::FitIn()
{
    wxSize newSize = wxControl::GetSize();
    if ( newSize != m_lastSize ) {
        m_lastSize = newSize;
        if ( m_webpanelCreated || m_webpanelCreateWait ) {
            m_pWebPanel->SetSize( newSize );
            /* Note: do not call here Layout() even this is used also by OnSize() event:
               the WebPanel is attached to the DashboardWindow object's (top level) sizer */
        }
    }
}

void InstruJS::OnSize( wxSizeEvent &event )
{
    event.Skip();
    FitIn();
}

void InstruJS::suspendInstrument()
{
    std::unique_lock<std::mutex> lckmRunScript( m_mtxScriptRun );
    if ( instrIsRunning() ) {
        m_pThreadInstruJSTimer->Stop();
        m_webPanelSuspended = true;
        wxMilliSleep( GetRandomNumber( 100,199 ) ); // avoid running all updates at the same time
        m_pThreadInstruJSTimer->Start(1000, wxTIMER_CONTINUOUS);
    } // then running, can be suspended but keep thread running
}

void InstruJS::setNewConfig( wxString newSkPath )
{
    std::unique_lock<std::mutex> lckmRunScript( m_mtxScriptRun );
    if ( !instrIsReadyForConfig() )
        return;
    wxString javascript = wxString::Format(L"%s%s%s%s%s%s%s",
                                           "setconf(\"",m_ids,
                                           "\",\"",newSkPath,
                                           "\",",m_data,
                                           ");");
    RunScript( javascript );
}

void InstruJS::setColorScheme ( PI_ColorScheme cs )
{
    m_newsubstyle = "day";
    if ( cs == PI_GLOBAL_COLOR_SCHEME_DUSK )
        m_newsubstyle = "dusk";
    if ( cs == PI_GLOBAL_COLOR_SCHEME_NIGHT )
        m_newsubstyle = "night";
}

void InstruJS::OnThreadTimerTick( wxTimerEvent &event )
{
    std::unique_lock<std::mutex> lckmRunScript( m_mtxScriptRun );
    m_threadRunning = true;
    if ( !m_webPanelSuspended && (m_istate >= JSI_WINDOW_LOADED) ) {
        // see  ../instrujs/src/iface.js for the interface,
        // see  ../instrujs/<instrument>/src/statemachine.js for the states
        wxString request = wxEmptyString;
        wxString pgtext  = m_pWebPanel->GetPageText();
        int pos = pgtext.Find("instrujs:");
        if ( pos != wxNOT_FOUND ) {
            wxString restOfTxt = pgtext.Mid( (pos + 9) );
            wxUniChar termination = 0x21; // !
            wxString cmdTxt = restOfTxt.BeforeFirst(termination);
            request = cmdTxt;
        }
        switch ( m_handshake ) {
        case JSI_HDS_NO_REQUEST:
            if ( request == wxEmptyString ) {
                if ( m_istate != JSI_SHOWDATA )
                    m_istate = JSI_NO_REQUEST;
                break;
            }
            m_handshake = JSI_HDS_SERVED;
        case JSI_HDS_SERVED:
            if ( (request == wxEmptyString) ) {
                m_handshake = JSI_HDS_ACKNOWLEDGED;
                if ( m_istate != JSI_SHOWDATA )
                    m_istate = JSI_NO_REQUEST;
                break;
            }
            if ( request != m_requestServed ) {
                m_handshake = JSI_HDS_ACKNOWLEDGED;
                if ( m_istate != JSI_SHOWDATA )
                    m_istate = JSI_NO_REQUEST;
            } // then another request, let's pass by acknwowledge, nevertheless
            else
                break;
        case JSI_HDS_ACKNOWLEDGED:
            if ( request == wxEmptyString ) {
                m_handshake = JSI_HDS_NO_REQUEST;
                if ( m_istate != JSI_SHOWDATA )
                    m_istate = JSI_NO_REQUEST;
                break;
            }
            if ( request != m_requestServed ) {
                m_handshake = JSI_HDS_REQUEST;
             } // then another request, let's pass to execution, nevertheless
            else
                break;
        case JSI_HDS_REQUEST:
            if ( request == wxEmptyString ) {
                m_handshake = JSI_HDS_NO_REQUEST;
                break;
            }
            m_handshake = JSI_HDS_SERVING;
        case JSI_HDS_SERVING:
            if ( request == wxEmptyString ) {
                m_handshake = JSI_HDS_ACKNOWLEDGED;
                break;
            }
            m_requestServed = request;
            if ( request.CmpNoCase("getid") == 0 ) {
                m_istate = JSI_GETID;
                wxString javascript =
                    wxString::Format(
                        L"%s%s%s",
                        "window.iface.setid('",
                        m_ids,
                        "');");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                m_hasRequestedId = true;
                break;
            }
            else if ( request.CmpNoCase("getall") == 0 ) {
                wxString allPathsJsList = m_pparent->getAllNMEA2000JsOrderedList();
                if ( allPathsJsList != wxEmptyString ) {
                    if ( m_setAllPathGraceCount == 0 ) {
                        m_istate = JSI_GETALL;
                        wxString javascript =
                            wxString::Format(
                                L"%s%s%s",
                                "window.iface.setall('",
                                allPathsJsList,
                                "');");
                        RunScript( javascript );
                        m_handshake = JSI_HDS_SERVED;
                        m_hasRequestedId = true;
                        m_setAllPathGraceCount = JSI_GETALL_GRACETIME;
                    }
                    else {
                        m_setAllPathGraceCount--;
                    }
                } // then we are connected to a NMEA2000 source
                break;
            }
            else if ( request.Find(".") != wxNOT_FOUND ) {
                m_istate = JSI_GETPATH;
                m_pushHere = std::bind(&InstruJS::PushData,
                                       this, _1, _2, _3 );
                m_pushHereUUID = m_pparent->subscribeTo ( request, m_pushHere );
                wxString javascript = wxString::Format(L"%s",
                                                       "window.iface.acksubs();");
                RunScript( javascript );
                m_istate = JSI_SHOWDATA;
                m_handshake = JSI_HDS_SERVED;
                break;
            }
            else
                break;
        default:
            m_handshake = JSI_HDS_NO_REQUEST;
            if ( m_istate != JSI_SHOWDATA )
                m_istate = JSI_NO_REQUEST;
        }
        if ( m_istate == JSI_SHOWDATA ) {
            wxString javascript = wxString::Format(L"%s%s%s",
                                                   "window.iface.newdata(",
                                                   m_data,
                                                   ");");
            RunScript( javascript );
        } // the instrument is ready for data
        if ( m_hasRequestedId ) {
            bool sendNewValue = false;
            if ( m_newsubstyle.IsEmpty() ) {
                if ( m_substyle != "day" ) {
                    sendNewValue = true;
                } // then need to change the initial luminosity style
            } // then luminosity style comes from the constructor (from plug-in)
            else {
                if ( m_newsubstyle != m_substyle ) {
                    m_substyle = m_newsubstyle;
                    sendNewValue = true;
                } // then need to change
            } /// else the luminosity style comes from the OpenCPN via plug-in
            if ( sendNewValue ) {
                wxString javascript =
                    wxString::Format(
                        L"%s%s%s",
                        "window.iface.setluminsty('",
                        m_substyle,
                        "');");
                RunScript( javascript );
            } // then there is a reason to ask the instrument to change style
        } // then instru state machine allows luminosity changes
        
        /*
        if ( !m_dataout.IsSameAs( m_data ) ) {
            m_dataout = m_data;
            wxString javascript = wxString::Format(L"%s%s%s",
                                                   "setval(",
                                                   m_dataout,
                                                   ");");
            RunScript( javascript );
            } // then the instrument is runnng and in service
        */
    } // then all code loaded
    else {
        if ( !m_webpanelCreated && m_webpanelCreateWait ) {
            if ( !m_pWebPanel->IsBusy() ) {
                m_webpanelCreateWait = false;
                m_webpanelCreated = true;
                m_istate = JSI_WINDOW_LOADED;
                m_pThreadInstruJSTimer->Stop();
                wxMilliSleep( GetRandomNumber( 100,999 ) ); // avoid running all updates at the same time
                m_pThreadInstruJSTimer->Start(1000, wxTIMER_CONTINUOUS);
            } // then, apparently (for IE), the page is loaded - handler also in JS
        } //then poll until the initial page is loaded (load event _not_ working down here)
    } // else the webpanel is not yet loaded / scripts are not running
}

void InstruJS::Draw(wxGCDC* dc)
{
    return;
}

void InstruJS::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    return;
}


