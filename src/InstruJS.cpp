/******************************************************************************
* $Id: InstruJS.cpp, v1.0 2019/11/30 VaderDarth Exp $
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
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
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
#include <wx/sstream.h>
#include <wx/url.h>
#include <wx/socket.h>

#include <functional>
#include <mutex>
#include "InstruJS.h"
using namespace std::placeholders;
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
                    PI_ColorScheme cs, unsigned long ds, bool isInit ) :
                    DashboardInstrument( pparent, id, "---", 0LL, true )
{
    m_pparent = pparent;

    m_istate = JSI_UNDEFINED;
    m_handshake = JSI_HDS_NO_REQUEST;
    m_requestServed = wxEmptyString;
    m_hasRequestedId = false;
    m_setAllPathGraceCount = -1;
    m_dsDataSource = ds;
    m_isInit = isInit;
    m_dsRequestedInSource = JSI_IS_UNDEFINED;
    m_pushHereUUID = wxEmptyString;
    m_subscribedPath = wxEmptyString;
    m_hasSchemDataCollected = false;

    m_id = id;
    m_ids = ids;
    m_substyle = "day";
    if ( cs == PI_GLOBAL_COLOR_SCHEME_DUSK )
        m_substyle = "dusk";
    if ( cs == PI_GLOBAL_COLOR_SCHEME_NIGHT )
        m_substyle = "night";
    m_newsubstyle = wxEmptyString;
    m_title = L"InstruJS";
    m_format = L"%.2e"; /* unlike trad. Dashboard instrument, class manages the
                           format */
    m_data = wxString::Format( m_format, 0.0 );
    m_fData = 0.0;
    m_lastdataout = wxString::Format( m_format, 9.9 ); // just make a diff
    m_threadRunning = false;
    m_pThreadInstruJSTimer = nullptr;
    std::unique_lock<std::mutex> init_m_mtxScriptRun(
        m_mtxScriptRun, std::defer_lock );
    m_closing = false;
    m_webpanelCreated = false;
    m_webpanelCreateWait = false;
    m_webpanelReloadWait = false;
    m_webPanelSuspended  = false;
    m_webpanelStopped = false;

    // Create the WebKit (type of - implementation varies) view
   m_istate = JSI_NO_WINDOW;
#if wxUSE_WEBVIEW_IE
    m_pWebPanel = wxWebView::New( );
    wxWebViewIE::MSWSetModernEmulationLevel();
   Bind( wxEVT_WEBVIEW_LOADED, &InstruJS::OnWebViewLoaded,
         this, m_pWebPanel->GetId() );
   Bind( wxEVT_WEBVIEW_ERROR, &InstruJS::OnWebViewError,
         this, m_pWebPanel->GetId() );
#else
    m_pWebPanel = nullptr;
#endif
   m_pThreadInstruJSTimer = new wxTimer( this, myID_TICK_INSTRUJS );

   m_lastSize = wxControl::GetSize();
   m_initialSize = m_lastSize;
   m_fullPath = wxEmptyString;
}

InstruJS::~InstruJS(void)
{
    if ( m_pThreadInstruJSTimer ) {
        m_pThreadInstruJSTimer->Stop();
        delete m_pThreadInstruJSTimer;
    }
    if ( !m_pushHereUUID.IsEmpty() ) {
        m_pparent->unsubscribeFrom( m_pushHereUUID );
    }
    if ( m_pWebPanel ) {
        /* On WebKit based implementation, prefer to delete
           in Close event handler: it gives more time for
           the pthread of WebKit2 to die before the host window
           goes away. */
           delete m_pWebPanel;
    }
}

void InstruJS::OnClose( wxCloseEvent &event )
{
    std::unique_lock<std::mutex> lckmRunScript( m_mtxScriptRun );
    m_closing = true;

    if ( m_pThreadInstruJSTimer ) {
        m_pThreadInstruJSTimer->Stop();
    }
    if ( !m_pushHereUUID.IsEmpty() ) {
        m_pparent->unsubscribeFrom( m_pushHereUUID );
        m_pushHereUUID = wxEmptyString;
    }
    if ( m_istate >= JSI_WINDOW ) {
        if ( m_pWebPanel ) {
            if ( m_istate == JSI_SHOWDATA ) {
                wxString javascript =
                    wxString::Format(
                    L"%s",
                    "window.iface.setclosing();");
                RunScript( javascript );
                m_istate = JSI_NO_WINDOW;
            } /* then allow the instrument code to gracefully close
                 if showing data but do not wait any answer  */
            m_pWebPanel->Stop();
            m_webpanelStopped = true;
            delete m_pWebPanel;
            m_pWebPanel = nullptr;
        }
    }
    event.Skip(); // Destroy() must be called
}

bool InstruJS::testHTTPServer( wxString urlIpOrNamePort ) {
    if ( urlIpOrNamePort.Cmp(_T("about:blank")) == 0 ) {
        return true;
    } // then useful for debugging the target wxWebView back-end
    wxSocketClient testSocket;
    testSocket.SetTimeout( 1 );
    testSocket.SetFlags( wxSOCKET_BLOCK );
    wxIPV4address  *address = new wxIPV4address();
    wxUniChar separator = 0x3a;
    address->Hostname(urlIpOrNamePort.BeforeFirst(separator));
    address->Service(urlIpOrNamePort.AfterFirst(separator));
    bool retval = false;
    if ( testSocket.Connect( *address ) )
        retval = true;
    testSocket.Close();
    delete address;
    return retval;
}

wxString InstruJS::testURLretHost( wxString url ) {
    if ( url.Cmp(_T("about:blank")) == 0 ) {
        return url;
    } // then useful for debugging the target wxWebView back-end
    wxURL testURL( url );
    if ( testURL.GetError() != wxURL_NOERR )
        return wxEmptyString;
    wxString restStr = wxEmptyString;
    if ( !url.StartsWith( "http://", &restStr ) )
        return wxEmptyString;
    wxString hostOrIp = wxEmptyString;
    hostOrIp = restStr.BeforeFirst('/');
    return hostOrIp;
}

void InstruJS::stopScript( )
{
    std::unique_lock<std::mutex> lckmRunScript( m_mtxScriptRun );
    if ( m_pThreadInstruJSTimer != NULL ) {
        m_pThreadInstruJSTimer->Stop();
    }
    if ( (m_webpanelCreated || m_webpanelCreateWait) && !m_webpanelStopped ) {
        if ( m_pWebPanel ) {
            m_pWebPanel->Stop();
            m_webpanelStopped = true;
        }
    }
}

void InstruJS::timeoutEvent()
{
    derivedTimeoutEvent();
}

void InstruJS::PushData(double data, wxString unit, long long timestamp)
{
    if ( !std::isnan(data) ) {
        setTimestamp( timestamp );  // Triggers also base class' watchdog
        /* Suspected bugs in Signal K conversions and their corrections
           (if not possible to fix in JS) */
        /* Signal K v1.21.0 suspected not to scale to 0.01 % negative trim
           position values, debug shows the following : */
        if ( data < 0.0 ) {
            if ( m_subscribedPath.Find( ".drive.trimState" ) != wxNOT_FOUND ) {
                data *= 0.01;
            }
        } // else detected issues with negative value conversions
        // wxString temp = wxString::Format( m_format, data ); // DEBUG
        m_fData = data;
        m_data = wxString::Format( m_format, data );
    } // then valid data
}

wxString InstruJS::RunScript( const wxString &javascript )
{
    wxString result = wxEmptyString;
    if ( m_pWebPanel ) {
#if wxCHECK_VERSION(3,1,0)
        if ( !m_pWebPanel->RunScript( javascript, &result ) ) {
            result = "NotReady";
        }
#else
        m_pWebPanel->RunScript( javascript );
#endif
    }
    return result;
}

void InstruJS::OnWebViewLoaded( wxWebViewEvent &event )
{
    wxString eventURL = event.GetURL();
    
    if ( (m_istate >= JSI_WINDOW) &&
         (m_istate <= JSI_WINDOW_RELOADED) ) {
        if ( eventURL == m_pWebPanel->GetCurrentURL() ) {
            // Something for us
            if ( m_istate == JSI_WINDOW ) {
                m_istate = JSI_WINDOW_LOADED;
                wxSizer *thisSizer = GetSizer();
                m_pWebPanel->SetSizer( thisSizer );
                m_pWebPanel->SetAutoLayout( true );
                m_pWebPanel->SetInitialSize( m_initialSize );
                FitIn();
                m_webpanelCreateWait = true;
            } // then first load (this is common for IE and WebKit2)
#ifdef __WXGTK__
            else {
                switch ( m_istate ) {
                case JSI_WINDOW_LOADED:
                    m_istate = JSI_WINDOW_URLLOADED;
                    break;
                case JSI_WINDOW_URLLOADED:
                    if ( eventURL.Cmp( m_fullPath ) != 0 ) {
                        wxLogMessage(
                            "dashboard_tactics_pi: ERROR : OnWebViewLoaded ("
                            " %s ), event URL does not match reload request: "
                            "m_fullPath ( %s ), m_istate( %d )",
                            eventURL, m_fullPath, m_istate );
                        m_istate = JSI_WINDOW_ERR;
                    }
                    else
                        m_istate = JSI_WINDOW_RELOADED;
                    break;
                default:
                    wxLogMessage(
                        "dashboard_tactics_pi: ERROR : OnWebViewLoaded ( %s ), "
                        "state machine : unexpected : m_istate( %d ), "
                        "m_fullPath ( %s ), "
                        "m_pWebPanel(%p)->GetCurrentURL() (%s), ",
                        eventURL, m_istate, m_fullPath, m_pWebPanel,
                        m_pWebPanel->GetCurrentURL() );
                    m_istate = JSI_WINDOW_ERR;
                }
            } // else reload (on IE, does not come here, got to do polling!)
#endif
            if ( m_pThreadInstruJSTimer )
                m_pThreadInstruJSTimer->Start(
#ifdef __WXGTK__
                    GetRandomNumber( 150,249 ), // WebKit and pthreads
#else
                    GetRandomNumber( 100,199 ), // IE event on first load only
#endif
                    wxTIMER_CONTINUOUS);
        } // then event and the window URL match
        else {
            m_istate = JSI_WINDOW_ERR;
            wxLogMessage(
                "dashboard_tactics_pi: ERROR : OnWebViewLoaded ( %s ), "
                "panel URL does not match: m_fullPath ( %s ), "
                "m_pWebPanel(%p)->GetCurrentURL() (%s), "
                "m_istate( %d )",
                eventURL, m_fullPath, m_pWebPanel,
                m_pWebPanel->GetCurrentURL(), m_istate );
        } // else the event and the window URL do not match
    } // then the state machine is in the window loading phase
    else {
        m_istate = JSI_WINDOW_ERR;
        wxLogMessage(
            "dashboard_tactics_pi: ERROR : OnWebViewLoaded ( %s ), "
            "a load event occurred but non-expected state: "
            "m_istate( %d ), m_fullPath ( %s )",
            eventURL, m_istate, m_fullPath );
    } // else fatal state violation, probably no way out, report.
}

void InstruJS::OnWebViewError( wxWebViewEvent &event )
{
#define WX_WEBVIEW_ERROR_CASE(type) \
    case type: \
        category = #type; \
        break;

    wxString category;
    switch (event.GetInt())
    {
        WX_WEBVIEW_ERROR_CASE(wxWEBVIEW_NAV_ERR_CONNECTION);
        WX_WEBVIEW_ERROR_CASE(wxWEBVIEW_NAV_ERR_CERTIFICATE);
        WX_WEBVIEW_ERROR_CASE(wxWEBVIEW_NAV_ERR_AUTH);
        WX_WEBVIEW_ERROR_CASE(wxWEBVIEW_NAV_ERR_SECURITY);
        WX_WEBVIEW_ERROR_CASE(wxWEBVIEW_NAV_ERR_NOT_FOUND);
        WX_WEBVIEW_ERROR_CASE(wxWEBVIEW_NAV_ERR_REQUEST);
        WX_WEBVIEW_ERROR_CASE(wxWEBVIEW_NAV_ERR_USER_CANCELLED);
        WX_WEBVIEW_ERROR_CASE(wxWEBVIEW_NAV_ERR_OTHER);
    }
    
    m_istate = JSI_WINDOW_ERR;
    wxLogMessage( "dashboard_tactics_pi: ERROR : WebViewError ( %s ), "
                  "m_fullPath ( %s )",
                  category, m_fullPath );
}

void InstruJS::loadHTML( wxString fullPath, wxSize initialSize )
{
    if ( !m_webpanelCreated && !m_webpanelCreateWait && !m_webpanelReloadWait ) {
        m_initialSize = initialSize;
        m_fullPath = fullPath;
#ifdef __WXGTK__
        if ( m_pThreadInstruJSTimer )
            m_pThreadInstruJSTimer->Start(
                GetRandomNumber( 1000,1999 ),
                wxTIMER_CONTINUOUS); // Pane content creation by the thread
#else
        m_istate = JSI_WINDOW;
        m_pWebPanel->Create( this, wxID_ANY, fullPath ); // triggers load event
#endif
    }
}

void InstruJS::FitIn()
{
    if ( m_pWebPanel ) {
        wxSize newSize = wxControl::GetSize();
        if ( newSize != m_lastSize ) {
            m_lastSize = newSize;
            if ( m_webpanelCreated || m_webpanelCreateWait ) {
                m_pWebPanel->SetSize( newSize );
                /* Note: do not call here Layout() even this is used also by
                   OnSize() event:
                   the WebPanel is attached to the DashboardWindow object's
                   (top level) sizer */
            }
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
        m_pThreadInstruJSTimer->Start( GetRandomNumber( 900,1099 ),
                                       wxTIMER_CONTINUOUS);
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
    if ( m_closing )
        return;

    m_pThreadInstruJSTimer->Stop();
    m_threadRunning = true;
    if ( !m_webPanelSuspended && (m_istate >= JSI_NO_REQUEST) &&
         m_pWebPanel ) {
        // see  ../instrujs/src/iface.js for the interface,
        // see  ../instrujs/<instrument>/src/statemachine.js for the states
        wxString request = wxEmptyString;
        wxString pgtext  = wxEmptyString;
        pgtext  = m_pWebPanel->GetPageText();
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
                m_requestServed = wxEmptyString;
                if ( m_istate != JSI_SHOWDATA )
                    m_istate = JSI_NO_REQUEST;
                break;
            }
            m_handshake = JSI_HDS_SERVED;
        case JSI_HDS_SERVED:
            if ( (request == wxEmptyString) ) {
                m_requestServed = wxEmptyString;
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
                m_requestServed = wxEmptyString;
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
                m_requestServed = wxEmptyString;
                m_handshake = JSI_HDS_NO_REQUEST;
                break;
            }
            if ( request != m_requestServed ) {
                m_handshake = JSI_HDS_SERVING;
            } // then another request, let's pass to execution, nevertheless
            else
                break;
        case JSI_HDS_SERVING:
            /*
              Developer's note: please read the WebView dev. note about the
              state machine implementation which, for now is based on one
              single and unique JavaScript interface module definition.
                  The below structure of if-then-else-if statements is
              therefore forced to recognize all the commands and requests from
              all instrument types. Also, this base class needs to define
              virtual service functions for those, which the instrument
              implementation classes needs to override.
                  Maybe the future versions, or new instruments can create
              their own, specific interfaces and we can identify the common,
              nominating functions which shall stay in the commong interface,
              i.e. served here.
            */
            if ( request == wxEmptyString ) {
                m_requestServed = wxEmptyString;
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
                if ( m_setAllPathGraceCount == -1 ) {
                    m_pparent->collectAllSignalKDeltaPaths();
                    m_setAllPathGraceCount = JSI_GETALL_GRACETIME;
                }
                else if ( m_setAllPathGraceCount == 0 ) {
                    wxString allPathsJsList =
                        m_pparent->getAllNMEA2000JsOrderedList();
                    if ( allPathsJsList != wxEmptyString ) {
                        m_istate = JSI_GETALL;
                        wxString javascript =
                            wxString::Format(
                                L"%s%s%s",
                                "window.iface.setall('",
                                allPathsJsList,
                                "');");
                        RunScript( javascript );
                        m_handshake = JSI_HDS_SERVED;
                    }
                }
                m_setAllPathGraceCount--;
                break;
            }
            else if ( request.CmpNoCase("getalldb") == 0 ) {
                if ( m_setAllPathGraceCount == -1 ) {
                    m_pparent->collectAllDbSchemaPaths();
                    m_setAllPathGraceCount = JSI_GETALLDB_GRACETIME;
                }
                else if ( m_setAllPathGraceCount == 0 ) {
                    wxString allDBSchemasPathsJsList =
                        m_pparent->getAllDbSchemasJsOrderedList();
                    if ( allDBSchemasPathsJsList != wxEmptyString ) {
                        m_istate = JSI_GETDBOUT;
                        m_hasSchemDataCollected = true;
                        wxString javascript =
                            wxString::Format(
                                L"%s%s%s",
                                "window.iface.setalldb('",
                                allDBSchemasPathsJsList,
                                "');");
                        RunScript( javascript );
                        m_handshake = JSI_HDS_SERVED;
                    }
                }
                m_setAllPathGraceCount--;
                break;
            }
            else if ( request.CmpNoCase("getrdy") == 0 ) {
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setchkrdy(",
                    (instruIsReady() ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                break;
            }
            else if ( request.CmpNoCase("getusrsl") == 0 ) {
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setusersl(",
                    (userHasStartline() ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                break;
            }
            else if ( request.CmpNoCase("dropstbd") == 0 ) {
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setmarkack(",
                    (dropStarboardMark() ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                break;
            }
            else if ( request.CmpNoCase("dropport") == 0 ) {
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setmarkack(",
                    (dropPortMark() ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                break;
            }
            else if ( request.CmpNoCase("getsldata") == 0 ) {
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setsldataack(",
                    (sendSlData() ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                m_istate = JSI_SHOWDATA;
                m_dsRequestedInSource = JSI_IS_RACESTART_STARTLINE;
                break;
            }
            else if ( request.CmpNoCase("stopsldata") == 0 ) {
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setsldstopack(",
                    (stopSlData() ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                m_istate = JSI_NO_REQUEST;
                m_dsRequestedInSource = JSI_IS_UNDEFINED;
                break;
            }
            else if ( request.CmpNoCase("getmrkdata") == 0 ) {
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setmrkdataack(",
                    (sendRmData() ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                m_istate = JSI_SHOWDATA;
                m_dsRequestedInSource = JSI_IS_RACESTART_MARK;
                break;
            }
            else if ( request.CmpNoCase("stopmrkdata") == 0 ) {
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setmrkdataack(",
                    (stopRmData() ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                m_istate = JSI_SHOWDATA;
                m_dsRequestedInSource = JSI_IS_UNDEFINED;
                break;
            }
            else if ( request.CmpNoCase("mutechart") == 0 ) {
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setmrkmteaack(",
                    (hideChartOverlay() ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                break;
            }
            else if ( request.CmpNoCase("resumechart") == 0 ) {
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setmrkumteack(",
                    (showChartOverlay() ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                break;
            }
            else if ( request.CmpNoCase("getdisf") == 0 ) {
                bool disfRetval = false;
                wxString sUsrUnit = getUsrDistanceUnit_Plugin(); // get O global
                if ( (sUsrUnit.Cmp("mi") == 0) || (sUsrUnit.Cmp("ft") == 0) )
                     disfRetval = true;
                wxString javascript = wxString::Format(
                    L"%s%s%s",
                    "window.iface.setgetfeet(",
                    (disfRetval ? "true" : "false"),
                    ");");
                RunScript( javascript );
                m_handshake = JSI_HDS_SERVED;
                break;
            }
            else if ( request.Find(".") != wxNOT_FOUND ) {
                if ( (m_dsDataSource & JSI_DS_INCOMING_DATA_SUBSCRIPTION) != 0 ) {
                    m_istate = JSI_GETPATH;
                    if ( m_pushHere == NULL )
                        m_pushHere = std::bind(&InstruJS::PushData,
                                               this, _1, _2, _3 );
                    if ( !m_pushHereUUID.IsEmpty() )
                        m_pparent->unsubscribeFrom ( m_pushHereUUID );
                    m_data = wxString::Format( m_format, 0.0 );
                    m_subscribedPath = request;
                    m_pushHereUUID = m_pparent->subscribeTo (
                        m_subscribedPath, m_pushHere );
                    wxString javascript =
                        wxString::Format(
                            L"%s%s%s",
                            "window.iface.acksubs('",
                            m_subscribedPath,
                            "');");
                    RunScript( javascript );
                    m_istate = JSI_SHOWDATA;
                    m_dsRequestedInSource = JSI_IS_INSTRUJS_DATA_SUBSCRIPTION;
                    m_handshake = JSI_HDS_SERVED;
                } // then knows path wants to subscribe to incoming data
                if ( (m_dsDataSource & JSI_DS_EXTERNAL_DATABASE) != 0 ) {
                    if ( !m_hasSchemDataCollected ) {
                        if ( m_setAllPathGraceCount == -1 ) {
                            m_setAllPathGraceCount = JSI_GETALLDB_GRACETIME;
                            m_pparent->collectAllDbSchemaPaths();
                        }
                        else if ( m_setAllPathGraceCount == 0 ) {
                            wxString allDBSchemasPathsJsList =
                                m_pparent->getAllDbSchemasJsOrderedList();
                            if ( allDBSchemasPathsJsList != wxEmptyString ) {
                                m_hasSchemDataCollected = true;
                            } // then there are schemas
                        } // else then gracetime passed
                        m_setAllPathGraceCount--;
                    } /* then instrument with a memory of its path,
                         needs schemas */
                    if ( m_hasSchemDataCollected ) {
                        m_istate = JSI_GETPATH;
                        m_subscribedPath = request;
                        wxString schemaJSclass =
                            m_pparent->getDbSchemaJs( &m_subscribedPath );
                        wxString javascript = wxString::Format(
                            L"%s", "window.iface.ackschema('" );
                        javascript = javascript + schemaJSclass;
                        javascript = javascript +
                            wxString::Format( L"%s", "');" );
                        RunScript( javascript );
                        m_istate = JSI_SHOWDATA;
                        m_handshake = JSI_HDS_SERVED;
                    } /* has got all DB paths, now wants a DB schema
                         of one of those */
                } // then a data source from external database, needs a schema
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
            if ( m_dsRequestedInSource == JSI_IS_INSTRUJS_DATA_SUBSCRIPTION ) {
                if ( (m_dsDataSource & JSI_DS_INCOMING_DATA_SUBSCRIPTION) != 0 ) {
                    /*
                      We not load up the system with the script execution
                      if the subsribed data remains unchanged.
                    */
                    if ( m_data != m_lastdataout ) {
                        wxString javascript =
                            wxString::Format(
                                L"%s%s%s",
                                "window.iface.newdata(",
                                m_data,
                                ");");
                        RunScript( javascript );
                        m_lastdataout = m_data;
                        m_pparent->SendPerfSentenceToAllInstruments(
                            OCPN_DBP_STC_SKSUBSCRIBE,
                            m_fData,
                            m_subscribedPath,
                            getTimestamp() );
                    } // then send data which has changed
                } // then subscription is valid
            } // then subscription to single incoming data
            else if ( m_dsRequestedInSource == JSI_IS_RACESTART_STARTLINE ) {
                wxString sCogDist; // Course on ground distance nautical miles
                wxString sDist;    // Shortest distance in nautical miles
                wxString sBias;    // Windshift bias angle in degrees w/ sign
                wxString sAdv;     // Bias caused advantage in meters
                getSlData( sCogDist, sDist, sBias, sAdv );
                wxString javascript =
                    wxString::Format(
                        L"%s%s%s%s%s%s%s%s%s",
                        "window.iface.newsldata(",
                        sCogDist, ",", sDist, ",", sBias, ",", sAdv,
                        ");");
                RunScript( javascript );
            } // else if data source requested is startline calculated data
            else if ( m_dsRequestedInSource == JSI_IS_RACESTART_MARK ) {
                raceRouteJSData *rd = getRmDataPtr();
                if ( rd ) {
                    wxString javascript =
                        wxString::Format( L"%s", "window.iface.newmrkdata(" );
                    javascript +=
                        wxString::Format(
                            L"%s%s%s%s",
                            rd->hasActiveRoute, ",", rd->instruIsReady, "," );
                    javascript +=
                        wxString::Format(
                            L"%s%s%s%s%s%s%s",
                            "'", rd->mark1Name, "','", rd->mark2Name, "','",
                            rd->mark3Name, "'," );
                    javascript +=
                        wxString::Format(
                            L"%s%s%s%s",
                            rd->thisLegTwa, ",", rd->thisLegTwaShortAvg, "," );
                    javascript +=
                        wxString::Format(
                            L"%s%s%s%s",
                            rd->thisLegTwaAvg, ",", rd->thisLegCurrent, "," );
                    javascript +=
                        wxString::Format(
                            L"%s%s%s%s",
                            rd->nextLegTwa, ",", rd->nextLegTwaShortAvg, "," );
                    javascript +=
                        wxString::Format(
                            L"%s%s%s%s",
                            rd->nextLegTwaAvg, ",", rd->nextLegCurrent, "," );
                    javascript +=
                        wxString::Format(
                            L"%s%s%s%s",
                            rd->nextNextLegTwa, ",",
                            rd->nextNextLegTwaShortAvg, "," );
                    javascript +=
                        wxString::Format(
                            L"%s%s%s%s",
                            rd->nextNextLegTwaAvg, ",",
                            rd->nextNextLegCurrent, "," );
                    javascript +=
                        wxString::Format( L"%s", rd->bearingBack );
                    javascript +=
                        wxString::Format( L"%s", ");");
                    RunScript( javascript );
                } // then there is a structure filled by derived class
            } // else if data source requested is startline calculated data
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

        m_pThreadInstruJSTimer->Start( GetRandomNumber( 900,1099 ), wxTIMER_CONTINUOUS);
        
    } // then all code loaded
    
    else {
        /* Note: reverse order in event polling - this to assure
           that the events will be detected either after the
           polling delay (IE) or after the event handler has
           executed and set its own delay for this thread.
        */
#if wxUSE_WEBVIEW_IE
        if ( m_webpanelCreated && !m_webpanelReloadWait &&
             (m_istate == JSI_WINDOW_RELOADED) ) {
            if ( !m_pWebPanel->IsBusy() ) {
                m_istate = JSI_NO_REQUEST;
            } // then page is reloaded + 1 poll tick wait
        } // then poll until confirmed by previous poll tick
        if ( m_webpanelCreated && m_webpanelReloadWait &&
             (m_istate == JSI_WINDOW_RELOADING) ) {
            if ( !m_pWebPanel->IsBusy() ) {
                m_webpanelReloadWait = false;
                m_istate = JSI_WINDOW_RELOADED;
            } // then page is reloaded with URL loaded in creation
        } // then poll until page reloaded, WebView IE no reload event
        if ( !m_webpanelCreated && m_webpanelCreateWait &&
             (m_istate >= JSI_WINDOW) ) {
            if ( m_isInit ) { // get the latest HTML5/JS code
                if ( !m_pWebPanel->IsBusy() ) {
                    m_webpanelCreateWait = false;
                    m_webpanelCreated = true;
                    m_pWebPanel->Reload(wxWEBVIEW_RELOAD_NO_CACHE);
                    m_webpanelReloadWait = true;
                    m_istate = JSI_WINDOW_RELOADING;
                } // then, apparently (for IE) the page is loaded (poll)
            } // then this object has been created at the initialization
            else {
                    m_webpanelCreateWait = false;
                    m_webpanelCreated = true;
                    m_webpanelReloadWait = false;
                    m_istate = JSI_NO_REQUEST;
            } // else this object has been reconstructed, speedier start
        } // then check if there is a need to reload HTML/JS code
        m_pThreadInstruJSTimer->Start( GetRandomNumber( 1500,2499 ),
                                       wxTIMER_CONTINUOUS);
#else
        bool eventHandlerArmsTimer = false;
        if ( m_webpanelCreated && m_webpanelReloadWait &&
             (m_istate == JSI_WINDOW_RELOADED) ) {
            if ( !m_pWebPanel->IsBusy() ) {
                m_webpanelReloadWait = false;
                m_istate = JSI_NO_REQUEST;
            } // then page is reloaded, confirmed, ready
        } // then poll until page re-loaded event occurred
        if ( m_webpanelCreated && !m_webpanelCreateWait &&
             (m_istate == JSI_WINDOW_URLLOADED) ) {
            if ( m_isInit ) { // get the latest HTML5/JS code
                if ( !m_pWebPanel->IsBusy() ) {
                    m_webpanelReloadWait = true;
                    m_pWebPanel->Reload(wxWEBVIEW_RELOAD_NO_CACHE);
                    eventHandlerArmsTimer = true;
                } // then page is loaded, reload
            }
            else {
                    m_webpanelReloadWait = false;
                    m_istate = JSI_NO_REQUEST;
            } // else this object has been reconstructed, speedier start
        } // then poll until page url loaded event occurred
        if ( !m_webpanelCreated && m_webpanelCreateWait &&
             (m_istate >= JSI_WINDOW_LOADED) ) {
            if ( !m_pWebPanel->IsBusy() ) {
                m_webpanelCreateWait = false;
                m_webpanelCreated = true;
                m_pWebPanel->LoadURL( m_fullPath );
                eventHandlerArmsTimer = true;
            }
        } // then the page is loaded - initial load event (without URL here)
        if ( !m_webpanelCreated &&
             (m_istate >= JSI_NO_WINDOW) ) {
            if ( m_pWebPanel ) {
                if ( !m_pWebPanel->IsBusy() ) {
                    m_istate = JSI_WINDOW;
                    m_pWebPanel->Create( this, wxID_ANY );
                    eventHandlerArmsTimer = true;
                } // then webpanel is not busy
            } // there is a web panel
            else {
                m_pWebPanel = wxWebView::New( );
                Bind( wxEVT_WEBVIEW_LOADED, &InstruJS::OnWebViewLoaded,
                      this, m_pWebPanel->GetId() );
                Bind( wxEVT_WEBVIEW_ERROR, &InstruJS::OnWebViewError,
                      this, m_pWebPanel->GetId() );
            } // else create a web panel as a delayed action in WebKit2
        } // then delayed creation of the window, no URL-load
        
        if ( !eventHandlerArmsTimer )
            m_pThreadInstruJSTimer->Start( GetRandomNumber( 150,249 ),
                                           wxTIMER_CONTINUOUS);
#endif

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
