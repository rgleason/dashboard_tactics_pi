/******************************************************************************
* $Id: TimesTUI.cpp, v1.0 2019/11/30 VaderDarth Exp $
*
* Project:  OpenCPN
* Purpose:  dahboard_tactics_pi plug-in
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
using namespace std;

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/version.h>

#include <algorithm>
#include <functional>

#include "dashboard_pi.h"
#include "TimesTUI.h"
#include "plugin_ids.h"

extern int GetRandomNumber(int, int);


wxBEGIN_EVENT_TABLE (DashboardInstrument_TimesTUI, InstruJS)
   EVT_TIMER (myID_TICK_TIMESTUI, DashboardInstrument_TimesTUI::OnThreadTimerTick)
   EVT_CLOSE (DashboardInstrument_TimesTUI::OnClose)
wxEND_EVENT_TABLE ()
//************************************************************************************************************************
// Time-series data presenting graphical instrument
//************************************************************************************************************************

DashboardInstrument_TimesTUI::DashboardInstrument_TimesTUI(
    TacticsWindow *pparent, wxWindowID id, wxString ids,
    PI_ColorScheme cs, wxString format ) :
    InstruJS ( pparent, id, ids, cs, JSI_DS_EXTERNAL_DATABASE )
{
    m_pparent = pparent;
    previousTimestamp = 0LL; // dashboard instru base class
    m_orient = wxHORIZONTAL;
    m_htmlLoaded= false;
    m_goodHttpServerDetects = -1; // default is: there is a server at startup
    m_pconfig = GetOCPNConfigObject();
    m_fullPathHTML = wxEmptyString;
    m_httpServer = wxEmptyString;

    if ( !LoadConfig() )
        return;

    if ( !m_fullPathHTML.IsEmpty() ) {
        m_pThreadTimesTUITimer = new wxTimer( this, myID_TICK_TIMESTUI );
        wxMilliSleep( GetRandomNumber( 800,1600 ) ); // avoid start loading all instruments simultaneously
        m_pThreadTimesTUITimer->Start(1000, wxTIMER_CONTINUOUS);
    } // then a reason to launch InstruJS, there is a page to load
}
DashboardInstrument_TimesTUI::~DashboardInstrument_TimesTUI(void)
{
    this->m_pThreadTimesTUITimer->Stop();
    delete this->m_pThreadTimesTUITimer;
    SaveConfig();
    return;
}
void DashboardInstrument_TimesTUI::OnClose( wxCloseEvent &event )
{
    this->m_pThreadTimesTUITimer->Stop();
    this->stopScript(); // base class implements, we are first to be called
    event.Skip(); // Destroy() must be called
}

void DashboardInstrument_TimesTUI::derivedTimeoutEvent()
{
    m_data = L"0.0";
    derived2TimeoutEvent();
}

void DashboardInstrument_TimesTUI::SetData(
    unsigned long long st, double data, wxString unit, long long timestamp)
{
    return; // this derived class gets its data from the multiplexer through a callback PushData()
}

void DashboardInstrument_TimesTUI::OnThreadTimerTick( wxTimerEvent &event )
{

    m_pThreadTimesTUITimer->Stop();
    if ( !m_htmlLoaded) {
        if ( testHTTPServer( m_httpServer ) ) {
            if ( (m_goodHttpServerDetects == -1) ||
                 (m_goodHttpServerDetects >= TIMESTUI_WAIT_NEW_HTTP_SERVER_TICKS) ) {
                wxSize thisSize = wxControl::GetSize();
                wxSize thisFrameInitSize = GetSize( m_orient, thisSize );
                SetInitialSize ( thisFrameInitSize );
                wxSize webViewInitSize = thisFrameInitSize;
                this->loadHTML( m_fullPathHTML, webViewInitSize );
                // No more threaded jobs, InstruJS is working now
                m_htmlLoaded= true;
            } // then either a straigth start with server or detected a stable server
            else {
                m_goodHttpServerDetects += 1;
                m_pThreadTimesTUITimer->Start( GetRandomNumber( 800,1100 ), wxTIMER_CONTINUOUS);
            }
        } // then there is a server serving the page, can ask content to be loaded
        else {
            m_goodHttpServerDetects = 0;
            m_pThreadTimesTUITimer->Start( GetRandomNumber( 8000,11000 ), wxTIMER_CONTINUOUS);
        }  // else need to wait until a server appears
    } // else thread is not running (no JS instrument created in this frame, create one)

}

wxSize DashboardInstrument_TimesTUI::GetSize( int orient, wxSize hint )
{
    int x,y;
    m_orient = orient;
    if( m_orient == wxHORIZONTAL ) {
        x = TIMESTUI_H_MIN_WIDTH;
        y = wxMax( hint.y, TIMESTUI_H_MIN_HEIGHT );
    }
    else {
        x = wxMax( hint.x, TIMESTUI_V_MIN_WIDTH );
        y = TIMESTUI_V_MIN_HEIGHT;
    }
    return wxSize( x, y );
}

bool DashboardInstrument_TimesTUI::LoadConfig()
{
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return false;
    
    // Make a proposal for the defaul path _and_ the protocool, which user can then override in the file:
    wxString sFullPathHTML = "http://127.0.0.1:8088/timestui/";

    pConf->SetPath(_T("/PlugIns/DashT/WebView/TimesTUI/"));
    pConf->Read(_T("instrujsURL"), &m_fullPathHTML, sFullPathHTML );

    m_httpServer = this->testURLretHost( m_fullPathHTML );
    
    if ( m_httpServer.IsEmpty() ) {
        wxString message( _("Malformed URL string in WebView/TimesTUI ini-file entry: ") + "\n" );
        message += m_fullPathHTML;
        wxMessageDialog *dlg = new wxMessageDialog(
            GetOCPNCanvasWindow(), message, _T("DashT Line Chart"), wxOK|wxICON_ERROR);
        (void) dlg->ShowModal();
        m_fullPathHTML = wxEmptyString;
    }
    
    return true;
}

void DashboardInstrument_TimesTUI::SaveConfig()
{
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return;

    pConf->SetPath(_T("/PlugIns/DashT/WebView/TimesTUI/"));
    pConf->Write(_T("instrujsURL"), m_fullPathHTML );
    
    return;
}
