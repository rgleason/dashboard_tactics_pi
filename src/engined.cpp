/******************************************************************************
* $Id: engined.cpp, v1.0 2019/11/30 VaderDarth Exp $
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

#include <algorithm>
#include <functional>

#include "dashboard_pi.h"
#include "engined.h"
#include "plugin_ids.h"

// --- the following is probably needed only for demonstration and testing! ---
extern int GetRandomNumber(int, int);

wxBEGIN_EVENT_TABLE (DashboardInstrument_EngineD, DashboardInstrument)
   EVT_TIMER (myID_TICK_ENGINED, DashboardInstrument_EngineD::OnThreadTimerTick)
   EVT_WEBVIEW_LOADED (myID_WEBVIEW_LOADED_ENGINED, DashboardInstrument_EngineD::OnPageLoaded)
   EVT_WEBVIEW_ERROR (myID_WEBVIEW_ERROR_ENGINED, DashboardInstrument_EngineD::OnPageError)
   EVT_CLOSE (DashboardInstrument_EngineD::OnClose)
wxEND_EVENT_TABLE ()
//************************************************************************************************************************
// Numerical instrument for engine monitoring data
//************************************************************************************************************************

DashboardInstrument_EngineD::DashboardInstrument_EngineD(
                             DashboardWindow *pparent, wxWindowID id, sigPathLangVector *sigPaths, wxString format ) :
                             DashboardInstrument ( pparent, id, L"", 0LL )
{
    // pro-forma, this class actually overrides the Paint() method of the base class, to avoid any flickering
    SetDrawSoloInPane( true );

    m_data = L"---";
    m_title = L"";
    m_pparent = pparent;
    m_id = id;
    previousTimestamp = 0LL;
    m_path = wxEmptyString;
    m_sigPathLangVector = sigPaths;
    m_pushHereUUID = wxEmptyString;
    m_threadRunning = false;
    m_threadRunCount = 0;
    m_webpanelCreated = false;
    m_webpanelInitiated  = false;
    m_webpanelLoaded = false;
    m_webpanelError = 0;
    m_webpanelErrorMsg = wxEmptyString;

    // Create virtual file system and files in the memory
    wxFileSystem::AddHandler(new wxMemoryFSHandler);
    wxMemoryFSHandler::AddFile(
        "index.html",
        "<html><head><meta charset=\"utf-8\">"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=yes\">"
        "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
        "<link rel=\"stylesheet\" type=\"text/css\" href=\"memory:engined1.css\">"
        "</head>"
        "<body><h1>EngineD</h></div>"
        "</body>"
        );
    wxMemoryFSHandler::AddFile(
        "engined1.html",
        "<html><head><meta charset=\"utf-8\">"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=yes\">"
        "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
        // "<link rel=\"stylesheet\" type=\"text/css\" href=\"memory:bootstrap.min.css\">"
        "<link rel=\"stylesheet\" type=\"text/css\" href=\"memory:engined1.css\">"
        "</head>"
        "<body><div id=\"content\" onclick=\"\"></div>"
        "</body>"
        "<script src=\"memory:engined1.js\"></script>"
        );
    wxMemoryFSHandler::AddFile("engined1.css", "h1 {color: blue;}");
    wxMemoryFSHandler::AddFile(
        "engined1.js", "var func = function() {"
        "document.write(\"Hello World!\");"
        "};"
        "window.onload = func;");
   
    // Create the WebKit (type of - implementation varies) view
    wxPoint pos( 0, 0 );
    wxSize size( 400, 400 );
#if wxUSE_WEBVIEW_IE
    wxWebViewIE::MSWSetModernEmulationLevel();
#endif
    m_webpanel = wxWebView::New( m_pparent, m_id, wxWebViewDefaultURLStr, pos, size );
    m_webpanel->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));

    // new wxLogWindow(this, _("Logging"), true, false);
    
    // if ( !m_webpanelCreated ) {
    //     wxPoint pos( 0, 0 );
    //     wxSize size( 400, 400 );
    //     m_webpanel->Create( m_pparent, m_id, "memory:index.html", pos, size ); 
    //     m_webpanelCreated = true;
    // }
    // if ( !m_webpanelInitiated ) {
    //     m_webpanel->LoadURL("memory:engined1.html");
    //     m_webpanelInitiated  = true;
    // }

    
     // Start the instrument thread
    m_threadEngineDTimer = new wxTimer( this, myID_TICK_ENGINED );
    m_threadEngineDTimer->Start(1000, wxTIMER_CONTINUOUS);
}
DashboardInstrument_EngineD::~DashboardInstrument_EngineD(void)
{
    this->m_threadEngineDTimer->Stop();
    delete this->m_threadEngineDTimer;
    if ( !m_pushHereUUID.IsEmpty() ) // if parent window itself is Delete()d
        this->m_pparent->unsubscribeFrom( m_pushHereUUID );
    return;
}
void DashboardInstrument_EngineD::OnClose( wxCloseEvent &event )
{
    if ( !m_pushHereUUID.IsEmpty() ) { // civilized parent window informs: Close()
        m_pparent->unsubscribeFrom( m_pushHereUUID );
        m_pushHereUUID = wxEmptyString;
    }
    event.Skip(); // Destroy() must be called
}
void DashboardInstrument_EngineD::OnPageLoaded(wxWebViewEvent &event)
{
    m_webpanelLoaded = true;
}
void DashboardInstrument_EngineD::OnPageError(wxWebViewEvent &event)
{
#define WX_ERROR_CASE(type) \
    case type: \
        m_webpanelErrorMsg = #type; \
        break;
    m_webpanelError = event.GetInt();
    
    switch ( m_webpanelError )
    {
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_CONNECTION);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_CERTIFICATE);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_AUTH);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_SECURITY);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_NOT_FOUND);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_REQUEST);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_USER_CANCELLED);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_OTHER);
    }
    
    wxLogMessage("%s", "dashboard_tactics_pi: EngineD Error; url='" +
                 event.GetURL() + "', error='" + m_webpanelErrorMsg +
                 " (" + event.GetString() + ")'");

}

void DashboardInstrument_EngineD::SetData(
    unsigned long long st, double data, wxString unit, long long timestamp)
{
    return; // this derived class gets its data from the multiplexer through a callback PushData()
}

void DashboardInstrument_EngineD::PushData( // for demo/testing purposes in this simple instrument
    double data, wxString unit, long long timestamp)
{
    if( !std::isnan(data) && (data < 9999.9) ) {
        setTimestamp( timestamp );                               // Triggers also base class' watchdog
        m_data = wxString::Format(m_format, data) + L" " + unit; // FYI, m_data is the string base class will draw
    } // then valid datea 
}

wxString DashboardInstrument_EngineD::RunScript( const wxString &javascript )
{
    wxString result = wxEmptyString;
#if wxUSE_WEBVIEW_IE
    m_webpanel->RunScript( javascript, &result );
#else
    m_webpanel->RunScript( javascript );
#endif
    return result;
}

void DashboardInstrument_EngineD::timeoutEvent()
{
    m_data = L"---"; // No data seems to come in (anymore)
    derivedTimeoutEvent();
}

void DashboardInstrument_EngineD::OnThreadTimerTick( wxTimerEvent &event )
{
    m_threadRunning = true;
    m_threadRunCount++;

    // if ( !m_webpanelCreated ) {
    //     wxPoint pos( 0, 0 );
    //     wxSize size( 400, 400 );
    //     m_webpanel->Create( m_pparent, m_id, "memory:index.html", pos, size ); 
    //     m_webpanelCreated = true;
    // }
    if ( !m_webpanelInitiated ) {
        m_webpanel->LoadURL("memory:engined1.html");
        m_webpanelInitiated  = true;
    }

    wxString javascript = wxString::Format(L"%s %d %s",
                                           "document.write(\"Hello World!",
                                           m_threadRunCount,
                                           " \n\");");

    RunScript( javascript );

    if ( m_path.IsEmpty() ) {
        /*
          We will emulate in this event the right click event for a selection a signal path from a list,
          given by the hosting application in the constructor (see below how to use). We'll
          make a simple random simulator (not to implement any GUI features for now)
        */
        wxString sTestingOnly[3];
        sTestingOnly[0] = L"propulsion.port.revolutions";
        sTestingOnly[1] = L"propulsion.port.oilPressure";
        sTestingOnly[2] = L"propulsion.port.temperature";
        m_path = sTestingOnly[ GetRandomNumber(0,2) ]; // let Mme Fortuna to be the user, for testing!!!!!!!
        /*
          Get the titles, descriptions and user's language for his selection from the hosting application
        */
        sigPathLangVector::iterator it = std::find_if(
            m_sigPathLangVector->begin(), m_sigPathLangVector->end(),
            [this](const sigPathLangTuple& e){return std::get<0>(e) == m_path;});
        if ( it != m_sigPathLangVector->end() ) {
            sigPathLangTuple sigPathWithLangFeatures = *it;
            // the window title is changed in the base class, see instrument.h
            m_title = std::get<1>(sigPathWithLangFeatures);
            // Subscribe to the signal path data with this object's method to call back
            m_pushHere = std::bind(&DashboardInstrument_EngineD::PushData,
                                   this, _1, _2, _3 );
            m_pushHereUUID = m_pparent->subscribeTo ( m_path, m_pushHere );
        } // then found user selection from the available signal paths for subsribtion
        /*
         Run the JavaScript engine
        */
        // if ( !m_javascriptRunning ) {
//             wxString javascript = "<script>"
// "var canvas = document.getElementById('myCanvas');"
// "var ctx = canvas.getContext('2d');"
// "ctx.font = '30px Arial';"
// "ctx.fillText('Hello World',10,50);"
//                 "</script>";
//             RunScript( javascript );
        //     RunScript("document.write(\"Hello World!\");");
        //     m_javascriptRunning = true;
        // }
    } // then no subscription to a signal path
}

wxSize DashboardInstrument_EngineD::GetSize( int orient, wxSize hint )
{
    wxClientDC dc(this);
    int w;
    dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
    if( orient == wxHORIZONTAL ) {
        w = wxMax(hint.y, DefaultWidth+m_TitleHeight);
        return wxSize( w-m_TitleHeight, w );
    } else {
        w = wxMax(hint.x, DefaultWidth);
        return wxSize( w, m_TitleHeight+w );
    }
}

void DashboardInstrument_EngineD::Draw(wxGCDC* bdc)
{
    return;
}

void DashboardInstrument_EngineD::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    return;
}

