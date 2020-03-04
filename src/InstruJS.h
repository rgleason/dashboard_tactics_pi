/******************************************************************************
* $Id: InstruJS.h, v1.0 2019/11/30 VaderDarth Exp $
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

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/webview.h>
#ifdef __WXMSW__
#if wxUSE_WEBVIEW_IE
#include "wx/msw/webview_ie.h"
#endif
#endif // __WXMSW__
#include <wx/webviewfshandler.h>
#include <wx/filesys.h>
#include <wx/fs_mem.h>

#if !wxUSE_WEBVIEW_WEBKIT && !wxUSE_WEBVIEW_WEBKIT2 && !wxUSE_WEBVIEW_IE
#error "A wxWebView backend is required by InstruJS"
#endif

#include "TacticsWindow.h"
#include "instrument.h"
#include "ocpn_plugin.h"

enum instruState {
    JSI_UNDEFINED,
    JSI_NO_WINDOW,
    JSI_WINDOW,
    JSI_WINDOW_LOADED,
    JSI_NO_REQUEST,
    JSI_GETID,
    JSI_GETALL,
    JSI_GETPATH,
    JSI_SHOWDATA,
    JSI_GETDBOUT,
    JSI_GETSCHEMA,
    JSI_NOF_STATES
};
enum instruHandShake {
    JSI_HDS_NO_REQUEST,
    JSI_HDS_REQUEST,
    JSI_HDS_SERVING,
    JSI_HDS_SERVED,
    JSI_HDS_ACKNOWLEDGED
};

#define JSI_GETALL_GRACETIME 8 // ticks (roughly = seconds)

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    InstruJS
//|
//| DESCRIPTION:
//|    This instrument supporting class provides Web Kit (like) services
//+------------------------------------------------------------------------------

//class InstruJS : public wxControl
class InstruJS : public DashboardInstrument
{
public:
    InstruJS( TacticsWindow* pparent, wxWindowID id, wxString ids,
              PI_ColorScheme cs );
    ~InstruJS(void);

    virtual void loadHTML( wxString fullPath, wxSize initialSize );
    virtual bool instrIsRunning(void) { return !m_webPanelSuspended; };
    virtual void suspendInstrument(void);
    virtual bool instrIsReadyForConfig(void) { return m_webpanelCreated; };
    virtual void setNewConfig ( wxString newSkPath );
    virtual void setColorScheme ( PI_ColorScheme cs ) override;
    virtual void restartInstrument(void) { m_webPanelSuspended = false; };
    virtual void stopScript(void); // if overriding OnClose(), call this
    virtual void PushData(double data, wxString unit, long long timestamp=0LL);

    void timeoutEvent(void) override;
#ifndef __DERIVEDTIMEOUTJS_OVERRIDE__
    virtual void derivedTimeoutEvent(void){};
#else
    virtual void derivedTimeoutEvent(void) = 0;
#endif // __DERIVEDTIMEOUTJS_OVERRIDE__
    
    virtual wxSize GetSize( int orient, wxSize hint ) = 0;
    virtual void OnPaint(wxPaintEvent& WXUNUSED(event)) final;
    virtual void FitIn(void) final;
    
protected:
    TacticsWindow       *m_pparent;
    instruState          m_istate;
    instruHandShake      m_handshake;
    wxString             m_requestServed;
    bool                 m_hasRequestedId;
    int                  m_setAllPathGraceCount;
    wxString             m_pushHereUUID;
    wxString             m_subscribedPath;
    wxWindowID           m_id;
    wxString             m_ids;
    wxString             m_substyle;
    wxString             m_newsubstyle;
    wxString             m_title;
    wxString             m_data;
    double               m_fData;
    wxString             m_format;
    wxString             m_lastdataout;
    std::mutex           m_mtxScriptRun;
    bool                 m_threadRunning;
    bool                 m_webpanelCreated;
    bool                 m_webpanelCreateWait;
    bool                 m_webPanelSuspended;
    bool                 m_webpanelStopped;
    wxWebView           *m_pWebPanel;
    wxTimer             *m_pThreadInstruJSTimer;
    wxSize               m_lastSize;

    callbackFunction     m_pushHere;

    wxDECLARE_EVENT_TABLE();

    void OnClose( wxCloseEvent& event );
    void OnSize( wxSizeEvent& event );
    void OnThreadTimerTick( wxTimerEvent& event);
    wxString RunScript(const wxString& javascript);
    
    virtual void Draw(wxGCDC* dc) final;

};

#endif // __INSTRUJS_H__
