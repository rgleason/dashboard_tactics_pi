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

    // Create virtual file system and files in the memory
    wxFileSystem::AddHandler(new wxMemoryFSHandler);
    wxMemoryFSHandler::AddFile(
        "index.html",
        "<html><head><meta charset=\"utf-8\">"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=yes\">"
        "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
        "<link rel=\"stylesheet\" type=\"text/css\" href=\"memory:engined1.css\">"
        "</head>"
        "<body><h1>InstruJS</h></div>"
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
    //    m_webpanel = wxWebView::New( m_pparent, m_id, wxWebViewDefaultURLStr, pos, size );
    m_webpanel = wxWebView::New( );
#if wxUSE_WEBVIEW_IE
    wxWebViewIE::MSWSetModernEmulationLevel();
#endif
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

    
     // Start the instrument pane thread
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
        m_threadRunCount++;
        wxString javascript = wxString::Format(L"%s %d %s",
                                               "document.write(\"Hello World!",
                                               m_threadRunCount,
                                               " \n\");");
        RunScript( javascript );
    } // then all code loaded

    else {

        if ( !m_webpanelCreated && !m_webpanelCreateWait ) {
            wxPoint pos( 0, 0 );
            wxSize size( 400, 400 );
            m_webpanel->Create( m_pparent, m_id, "memory:engined1.html", pos, size ); 
            m_webpanelCreateWait = true;
        }
        if ( !m_webpanelCreated && m_webpanelCreateWait ) {
            if ( !m_webpanel->IsBusy() ) {
                m_webpanelCreateWait = false;
                m_webpanelCreated = true;
            }
        } //then poll until the initial page is loaded (load event not working)
        // IE is a bit shaky, cannot trust the above, let's try to run a script
        if ( m_webpanelCreated ) {
            if ( !m_webpanelInitiated ) {
                m_threadRunCount++;
                wxString javascript = wxString::Format(L"%s %d %s",
                                                       "document.write(\"Hello World!",
                                                       m_threadRunCount,
                                                       " \n\");");
                wxString retval;
                retval = RunScript( javascript );
                if ( !retval.Cmp("NotReady") == 0 ) { // for the test script, no return value
                    m_webpanelInitiated = true;
                    m_threadInstruJSTimer->Stop();
                    m_threadInstruJSTimer->Start(1000, wxTIMER_CONTINUOUS);
                } // then scripts are working, (IE has finished loading)
                else {
                    m_threadRunCount--;
                } // then scripts are yet working, (IE is probably still loading)
            } //then attempt to run script until everything is loaded
        } // Then the base module has been created but not sure if it is loaded

    } // else the webpanel is not yet loaded / scripts are not running
}

