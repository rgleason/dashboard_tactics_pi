/******************************************************************************
* $Id: TimesTUI.h, v1.0 2019/11/30 VaderDarth Exp $
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

#ifndef __TIMESTUI_H__
#define __TIMESTUI_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#define __DERIVEDTIMEOUTJS_OVERRIDE__
#include "InstruJS.h"

/*
  The default window size value are depending both of the HTML-file
  and the JavaScript C++ instrument, here we have set the values to
  the JustGauge scalable, SVG-drawn instrument.
  See enginedjg.css: skpath, gauge and bottom classes for the ratios.
*/
#ifdef __WXMSW__
#define TIMESTUI_V_TITLEH    18
#define TIMESTUI_V_WIDTH    500
#define TIMESTUI_V_HEIGHT   230
#define TIMESTUI_V_BOTTOM     8
#define TIMESTUI_H_TITLEH    19
#define TIMESTUI_H_WIDTH    500
#define TIMESTUI_H_HEIGHT   230
#define TIMESTUI_H_BOTTOM     9
#else
#define TIMESTUI_V_TITLEH    18
#define TIMESTUI_V_WIDTH    500
#define TIMESTUI_V_HEIGHT   230
#define TIMESTUI_V_BOTTOM     8
#define TIMESTUI_H_TITLEH    19
#define TIMESTUI_H_WIDTH    500
#define TIMESTUI_H_HEIGHT   230
#define TIMESTUI_H_BOTTOM     9
#endif // ifdef __WXMSW__
#define TIMESTUI_V_MIN_WIDTH  TIMESTUI_V_WIDTH
#define TIMESTUI_V_MIN_HEIGHT (TIMESTUI_V_TITLEH + TIMESTUI_V_HEIGHT + TIMESTUI_V_BOTTOM)
#define TIMESTUI_H_MIN_WIDTH  TIMESTUI_H_WIDTH
#define TIMESTUI_H_MIN_HEIGHT (TIMESTUI_H_TITLEH + TIMESTUI_H_HEIGHT + TIMESTUI_H_BOTTOM)

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    DashboardInstrument_TimesTUI
//|
//| DESCRIPTION:
//|    This instrument provides numerical engine monitoring information
//+------------------------------------------------------------------------------

class DashboardInstrument_TimesTUI : public InstruJS
{
public:
    DashboardInstrument_TimesTUI(
        TacticsWindow *pparent, wxWindowID id, wxString ids,
        PI_ColorScheme cs, wxString format = "" );
    ~DashboardInstrument_TimesTUI(void);
    void SetData(unsigned long long, double, wxString, long long timestamp=0LL );
#ifndef __TIMESTUI_DERIVEDTIMEOUT_OVERRIDE__
    virtual void derived2TimeoutEvent(void){};
#else
    virtual void derived2TimeoutEvent(void) = 0;
#endif // __DERIVEDTIMEOUT_OVERRIDE__
    virtual void derivedTimeoutEvent(void);
    virtual wxSize GetSize( int orient, wxSize hint ) override;
    bool LoadConfig(void);
    void SaveConfig(void);
    
protected:
    TacticsWindow       *m_pparent;
    int                  m_orient;
    bool                 m_htmlLoaded;
    wxTimer             *m_pThreadTimesTUITimer;
    wxFileConfig        *m_pconfig;
    wxString             m_fullPathHTML;
    
    wxDECLARE_EVENT_TABLE();

    void OnThreadTimerTick( wxTimerEvent& event );
    void OnClose(wxCloseEvent& event);

};

#endif // __TIMESTUI_H__
