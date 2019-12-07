/******************************************************************************
* $Id: engined.h, v1.0 2019/11/30 VaderDarth Exp $
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

#ifndef __ENGINED_H__
#define __ENGINED_H__
using namespace std;
using namespace std::placeholders;

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
#error "A wxWebView backend is required by DashboardInstrument_EngineD"
#endif

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    DashboardInstrument_EngineD
//|
//| DESCRIPTION:
//|    This instrument provides numerical engine monitoring information
//+------------------------------------------------------------------------------

class DashboardInstrument_EngineD : public DashboardInstrument
{
public:
    DashboardInstrument_EngineD(
        DashboardWindow *pparent, wxWindowID id, sigPathLangVector* sigPaths,
        wxString format = "" );
    ~DashboardInstrument_EngineD(void);
    void SetData(unsigned long long, double, wxString, long long timestamp=0LL );
    void PushData(double, wxString, long long timestamp=0LL );
    void timeoutEvent(void);
#ifndef __ENGINED_DERIVEDTIMEOUT_OVERRIDE__
    virtual void derivedTimeoutEvent(void){};
#else
    virtual void derivedTimeoutEvent(void) = 0;
#endif // __DERIVEDTIMEOUT_OVERRIDE__
    wxSize GetSize( int orient, wxSize hint );
    void OnPaint(wxPaintEvent& WXUNUSED(event)) override;    
    
protected:
    DashboardWindow     *m_pparent;
    wxWindowID           m_id;
    wxWebView           *m_webpanel;
    wxString             m_title;
    wxString             m_path;
    wxString             m_data;
    wxString             m_format;
    sigPathLangVector   *m_sigPathLangVector;
    wxTimer             *m_threadEngineDTimer;
    bool                 m_threadRunning;
    int                  m_threadRunCount;
    bool                 m_webpanelCreated;
    bool                 m_webpanelInitiated;
    bool                 m_webpanelLoaded;
    int                  m_webpanelError;
    wxString             m_webpanelErrorMsg;
    callbackFunction     m_pushHere;
    wxString             m_pushHereUUID;

    wxDECLARE_EVENT_TABLE();

    void OnThreadTimerTick( wxTimerEvent& );
    void OnClose(wxCloseEvent& event);
    void OnPageLoaded(wxWebViewEvent& event);
    void OnPageError(wxWebViewEvent& event);
    wxString RunScript(const wxString& javascript);

    virtual void Draw(wxGCDC* dc) override;
};

#endif // __ENGINED_H__
