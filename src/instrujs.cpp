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


wxBEGIN_EVENT_TABLE (InstruJS, DashboardInstrument)
   EVT_TIMER (myID_TICK_INSTRUJS, InstruJS::OnThreadTimerTick)
   EVT_CLOSE (InstruJS::OnClose)
wxEND_EVENT_TABLE ()

//************************************************************************************************************************
// Numerical instrument for engine monitoring data
//************************************************************************************************************************

InstruJS::InstruJS( TacticsWindow *pparent, wxWindowID id, wxBoxSizer *iBoxSizer ) :
DashboardInstrument( pparent, id, "---", 0LL, true )
{
    m_data = L"---";
    m_title = L"";
    m_pparent = pparent;
    m_id = id;
    m_threadRunning = false;
    m_threadRunCount = 0;
    m_webpanelCreated = false;
    m_webpanelCreateWait = false;
    m_webpanelInitiated  = false;
    m_webpanelStopped = false;

    // Create the WebKit (type of - implementation varies) view
    m_webpanel = wxWebView::New( );
#if wxUSE_WEBVIEW_IE
    wxWebViewIE::MSWSetModernEmulationLevel();
#endif
    m_webpanelSizer = iBoxSizer;
    SetSizer( m_webpanelSizer ); // this panel has its own sizer
    m_threadInstruJSTimer = NULL;
}

InstruJS::~InstruJS(void)
{
    if ( this->m_threadInstruJSTimer != NULL ) {
        this->m_threadInstruJSTimer->Stop();
        delete this->m_threadInstruJSTimer;
    }
    if ( (this->m_webpanelCreated || this->m_webpanelCreateWait)
         && !this->m_webpanelStopped ) {
        this->m_webpanel->Stop();
    }
    this->m_webpanelSizer->Detach( this->m_webpanel );
    delete this->m_webpanel;
}

void InstruJS::stopScript( )
{
    if ( this->m_threadInstruJSTimer != NULL ) {
        this->m_threadInstruJSTimer->Stop();
    }
    if ( (m_webpanelCreated || m_webpanelCreateWait) && !m_webpanelStopped ) {
        m_webpanel->Stop();
        m_webpanelStopped = true;
    }
}

void InstruJS::OnClose( wxCloseEvent &event )
{
    if ( this->m_threadInstruJSTimer != NULL ) {
        this->m_threadInstruJSTimer->Stop();
    }
    if ( (this->m_webpanelCreated || this->m_webpanelCreateWait)
         && !this->m_webpanelStopped ) {
        this->m_webpanel->Stop();
        this->m_webpanelStopped = true;
    }
    event.Skip(); // Destroy() must be called
}

void InstruJS::timeoutEvent()
{
    derivedTimeoutEvent();
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

void InstruJS::loadHTML( wxString fullPath )
{
    if ( !m_webpanelCreated && !m_webpanelCreateWait ) {
        m_webpanel->Create(
            this, wxID_ANY, "file://" + fullPath );
        m_webpanelSizer->Add( m_webpanel, wxSizerFlags().Expand().Proportion(1) );
        //m_webpanel->SetSizerAndFit( m_webpanelSizer );
        Fit();
        m_webpanelCreateWait = true;
        // Start the instrument pane control thread (faster polling 1/10 seconds for initial loading)
        m_threadInstruJSTimer = new wxTimer( this, myID_TICK_INSTRUJS );
        m_threadInstruJSTimer->Start(100, wxTIMER_CONTINUOUS);
    }
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

void InstruJS::Draw(wxGCDC* bdc)
{
    return;
}

void InstruJS::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    return;
}


