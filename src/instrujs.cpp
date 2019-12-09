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

#include "instrujs.h"
#include "plugin_ids.h"

wxBEGIN_EVENT_TABLE (InstruJS, wxFrame)
   EVT_TIMER (myID_TICK_INSTRUJS, InstruJS::OnThreadTimerTick)
   EVT_CLOSE (InstruJS::OnClose)
wxEND_EVENT_TABLE ()

//************************************************************************************************************************
// Numerical instrument for engine monitoring data
//************************************************************************************************************************

InstruJS::InstruJS( wxWindow *pparent, wxWindowID id, wxString title ) :
                             wxFrame ( NULL, wxID_ANY, title )
{
    m_pparent = pparent;
    m_id = id;
    m_threadRunning = false;
    m_threadRunCount = 0;
    m_webpanelCreated = false;
    m_webpanelCreateWait = false;
    m_webpanelInitiated  = false;

    // Create the WebKit (type of - implementation varies) view
    wxPoint pos( 0, 0 );
    wxSize size( 400, 400 );
    //    m_webpanel = wxWebView::New( m_pparent, m_id, wxWebViewDefaultURLStr, pos, size );
    m_webpanel = wxWebView::New( );
#if wxUSE_WEBVIEW_IE
    wxWebViewIE::MSWSetModernEmulationLevel();
#endif
    m_webpanel->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));

    // Start the instrument pane thread (faster polling 1/10 seconds for initial loading)
    m_threadInstruJSTimer = new wxTimer( this, myID_TICK_INSTRUJS );
    m_threadInstruJSTimer->Start(100, wxTIMER_CONTINUOUS);
}

InstruJS::~InstruJS(void)
{
    this->m_threadInstruJSTimer->Stop();
    delete this->m_threadInstruJSTimer;
}

void InstruJS::OnClose( wxCloseEvent &event )
{
    this->m_threadInstruJSTimer->Stop();
    event.Skip(); // Destroy() must be called
}

wxString InstruJS::RunScript( const wxString &javascript )
{
    wxString result = wxEmptyString;
#if wxCHECK_VERSION(3,1,0)
    if ( !m_webpanel->RunScript( javascript, &result ) ) {
        result = "NotReady";
    }
#else
    m_webpanel->RunScript( javascript );
#endif
    return result;
}

void InstruJS::OnThreadTimerTick( wxTimerEvent &event )
{
    m_threadRunning = true;

    if ( m_webpanelInitiated ) {
        // Demonstrate the passing of a value "à la numerical DashboardInstrument" to WebView
        m_threadRunCount++;
        wxString javascript = wxString::Format(L"%s%d%s",
                                               "func(",
                                               m_threadRunCount,
                                               ");");
      RunScript( javascript );
    } // then all code loaded

    else {

        if ( !m_webpanelCreated && !m_webpanelCreateWait ) {
            wxPoint pos( 0, 0 );
            wxSize size( 400, 400 );
            
            m_webpanel->Create(
#ifdef __WXMSW__
                m_pparent, m_id, "file:///C:/Program Files (x86)/OpenCPN/plugins/dashboard_tactics_pi/data/enginedjg.html", pos, size );
#else
                m_pparent, m_id, "file:////usr/share/opencpn/plugins/dashoard_tactics_pi/data/engined2.html", pos, size );
#endif // __WXMSW__
            //            m_webpanel->Create( m_pparent, m_id, "memory:engined1.html", pos, size ); 
            m_webpanelCreateWait = true;
        }
        if ( !m_webpanelCreated && m_webpanelCreateWait ) {
            if ( !m_webpanel->IsBusy() ) {
                m_webpanelCreateWait = false;
                m_webpanelCreated = true;
                m_webpanelInitiated = true;
                m_threadInstruJSTimer->Stop();
                m_threadInstruJSTimer->Start(1000, wxTIMER_CONTINUOUS);
            } // then, apparently (for IE), the page is loaded - handler also in JS
        } //then poll until the initial page is loaded (load event _not_ working down here)
    } // else the webpanel is not yet loaded / scripts are not running
}

