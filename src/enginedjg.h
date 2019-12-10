/******************************************************************************
* $Id: enginedjg.h, v1.0 2019/11/30 VaderDarth Exp $
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

#ifndef __ENGINEDJG_H__
#define __ENGINEDJG_H__
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

#include "instrujs.h"

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    DashboardInstrument_EngineDJG
//|
//| DESCRIPTION:
//|    This instrument provides numerical engine monitoring information
//+------------------------------------------------------------------------------

class DashboardInstrument_EngineDJG : public InstruJS
{
public:
    DashboardInstrument_EngineDJG(
        DashboardWindow *pparent, wxWindowID id, sigPathLangVector* sigPaths,
        wxString format = "" );
    ~DashboardInstrument_EngineDJG(void);
    void SetData(unsigned long long, double, wxString, long long timestamp=0LL );
    void PushData(double, wxString, long long timestamp=0LL );
#ifndef __ENGINEDJG_DERIVEDTIMEOUT_OVERRIDE__
    virtual void derivedTimeoutEvent(void){};
#else
    virtual void derivedTimeoutEvent(void) = 0;
#endif // __DERIVEDTIMEOUT_OVERRIDE__
    void OnPaint(wxPaintEvent& WXUNUSED(event)) override;
    virtual wxSize GetSize( int orient, wxSize hint ) override;
    bool LoadConfig(void);
    
protected:
    DashboardWindow     *m_pparent;
    wxWindowID           m_id;
    //    InstruJS            *m_instruJS;
    wxString             m_title;
    wxString             m_path;
    wxString             m_data;
    wxString             m_format;
    sigPathLangVector   *m_sigPathLangVector;
    wxTimer             *m_threadEngineDJGTimer;
    bool                 m_threadRunning;
    int                  m_threadRunCount;
    callbackFunction     m_pushHere;
    wxString             m_pushHereUUID;
    wxString             m_fullPathHTML;

    wxDECLARE_EVENT_TABLE();

    void OnThreadTimerTick( wxTimerEvent& event );
    void OnClose(wxCloseEvent& event);

    virtual void Draw(wxGCDC* dc) override;

};

#endif // __ENGINEDJG_H__
