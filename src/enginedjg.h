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
/*
  The default window size value are depending both of the HTML-file
  and the JavaScript instrument, here we have set the values to
  the JustGauge scalable, SVG-drawn instrument, experimentally: it
  appears to scale down to the below window on supported platforms,
  also note the anti-scroll bar border (like for IE).
*/
#define ENGINED_WINDOW_DEFAULT_WIDTH         230
#define ENGINED_WINDOW_DEFAULT_HEIGHT        185
#define ENGINED_WINDOW_ANTISCROLLBAR_BORDER    1
#define ENGINED_WINDOW_MINIMUM_WIDTH         (ENGINED_WINDOW_DEFAULT_WIDTH + ENGINED_WINDOW_ANTISCROLLBAR_BORDER)
#define ENGINED_WINDOW_MINIMUM_HEIGHT        ENGINED_WINDOW_DEFAULT_HEIGHT

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
        TacticsWindow *pparent, wxWindowID id, sigPathLangVector* sigPaths, wxBoxSizer* iBoxSizer,
        wxString format = "" );
    ~DashboardInstrument_EngineDJG(void);
    void SetData(unsigned long long, double, wxString, long long timestamp=0LL );
    void PushData(double, wxString, long long timestamp=0LL );
#ifndef __ENGINEDJG_DERIVEDTIMEOUT_OVERRIDE__
    virtual void derivedTimeoutEvent(void){};
#else
    virtual void derivedTimeoutEvent(void) = 0;
#endif // __DERIVEDTIMEOUT_OVERRIDE__
    virtual wxSize GetSize( int orient, wxSize hint ) override;
    virtual void SetMinSize( wxSize minSize ) override;
    bool LoadConfig(void);
    
protected:
    TacticsWindow       *m_pparent;
    wxWindowID           m_id;
    wxString             m_path;
    wxString             m_format;
    sigPathLangVector   *m_pSigPathLangVector;
    wxTimer             *m_pThreadEngineDJGTimer;
    bool                 m_threadRunning;
    int                  m_threadRunCount;
    callbackFunction     m_pushHere;
    wxString             m_pushHereUUID;
    wxString             m_fullPathHTML;

    wxDECLARE_EVENT_TABLE();

    void OnThreadTimerTick( wxTimerEvent& event );
    void OnClose(wxCloseEvent& event);

};

#endif // __ENGINEDJG_H__
