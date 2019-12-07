/******************************************************************************
* $Id: instrujs.h, v1.0 2019/11/30 VaderDarth Exp $
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

#ifndef __INSTRUJS_H__
#define __INSTRUJS_H__
using namespace std;

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/webview.h>
#if wxUSE_WEBVIEW_IE
#include "wx/msw/webview_ie.h"
#endif
#include <wx/webviewfshandler.h>
#include <wx/filesys.h>
#include <wx/fs_mem.h>

#if !wxUSE_WEBVIEW_WEBKIT && !wxUSE_WEBVIEW_WEBKIT2 && !wxUSE_WEBVIEW_IE
#error "A wxWebView backend is required by InstruJS"
#endif

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    InstruJS
//|
//| DESCRIPTION:
//|    This instrument supporting class provides Web Kit (like) services
//+------------------------------------------------------------------------------

class InstruJS : public wxFrame
{
public:
    InstruJS( wxWindow *pparent, wxWindowID id, wxString title = L"" );
    ~InstruJS(void);
    
protected:
    wxWindow            *m_pparent;
    wxWindowID           m_id;
    wxWebView           *m_webpanel;
    wxTimer             *m_threadInstruJSTimer;
    bool                 m_threadRunning;
    int                  m_threadRunCount;
    bool                 m_webpanelCreated;
    bool                 m_webpanelInitiated;
    bool                 m_webpanelLoaded;
    int                  m_webpanelError;
    wxString             m_webpanelErrorMsg;

    wxDECLARE_EVENT_TABLE();

    void OnClose( wxCloseEvent& event );
    void OnThreadTimerTick( wxTimerEvent& event);
    void OnPageLoaded(wxWebViewEvent& event);
    void OnPageError(wxWebViewEvent& event);
    wxString RunScript(const wxString& javascript);

};

#endif // __INSTRUJS_H__
