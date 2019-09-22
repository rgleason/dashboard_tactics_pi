/******************************************************************************
 * $Id: baro_history.h, v1.0 2014/02/10 tom-r Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   stedy
 * Based on code from  Thomas Rauch
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

#ifndef __BARO_HISTORY_H__
#define __BARO_HISTORY_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "instrument.h"
#include "dial.h"

#ifdef _TACTICSPI_H_
#include <wx/filename.h>
#define BARO_RECORD_COUNT 1500 // w/ 5s. tick, 1500 points = 7,500 sec = 125 minutes = env 2h
#define BARO_START_AVG_CNT 5 // 5 or higher
#else
#define BARO_RECORD_COUNT 3000
#endif // _TACTICSPI_H_

class DashboardInstrument_BaroHistory: public DashboardInstrument
{
public:
    DashboardInstrument_BaroHistory( wxWindow *parent, wxWindowID id, wxString title);

#ifdef _TACTICSPI_H_
    ~DashboardInstrument_BaroHistory(void);
#else
    ~DashboardInstrument_BaroHistory(void){}
#endif // _TACTICSPI_H_

#ifdef _TACTICSPI_H_
    void SetData(unsigned long long st, double data, wxString unit, long long timestamp=0LL);
    void OnBaroHistUpdTimer(wxTimerEvent &event);
    virtual void timeoutEvent(void){};
#else
    void SetData(int, double, wxString);
#endif // _TACTICSPI_H_
    wxSize GetSize( int orient, wxSize hint );


private:
#ifdef _TACTICSPI_H_
    double m_LastReceivedPressure;
    wxDateTime::Tm m_LastReceivedTime;
    int m_PressRecCnt;
    int m_PressStartVal[BARO_START_AVG_CNT];
#else
    int m_SpdRecCnt, m_DirRecCnt, m_SpdStartVal,m_DirStartVal;
#endif // _TACTICSPI_H_
#ifdef _TACTICSPI_H_
    wxFileConfig  *m_pconfig;
    bool LoadConfig(void);
    bool SaveConfig(void);
    void ExportData(void);
    wxTimer *m_BaroHistUpdTimer;
#endif // _TACTICSPI_H_
#ifdef _TACTICSPI_H_
    wxDECLARE_EVENT_TABLE();
#endif // _TACTICSPI_H_

protected:
#ifndef _TACTICSPI_H_
    double alpha;
#endif // _TACTICSPI_H_
    double m_ArrayPressHistory[BARO_RECORD_COUNT];
#ifndef _TACTICSPI_H_
    double m_ExpSmoothArrayPressure[BARO_RECORD_COUNT];
#endif // _TACTICSPI_H_
    wxDateTime::Tm m_ArrayRecTime[BARO_RECORD_COUNT];

    double m_MaxPress;  //...in array
    double m_MinPress;  //...in array
    double m_TotalMaxPress; // since O is started
    double m_TotalMinPress;
    double m_Press;
    double m_PressScale;
    double m_MaxPressScale;
    double m_ratioW;

    bool m_IsRunning;
    int m_SampleCount;

    wxRect m_WindowRect;
    wxRect m_DrawAreaRect; //the coordinates of the real darwing area
    int m_TopLineHeight;
    int m_LeftLegend;
    int m_RightLegend;
#ifdef _TACTICSPI_H_
    wxString    m_logfile;        //for data export
    wxFile     *m_ostreamlogfile; //for data export
    bool        m_isExporting;      //for data export
    int         m_exportInterval; //for data export
    wxButton   *m_LogButton;     //for data export
    wxMenu     *m_pExportmenu;//for data export
    wxMenuBar  *m_pExportmenuBar;//for data export
    wxMenuItem *btn10Sec;
    wxMenuItem *btn20Sec;
    wxMenuItem *btn60Sec;
#endif // _TACTICSPI_H_

    void Draw(wxGCDC* dc);
    void DrawBackground(wxGCDC* dc);
    void DrawForeground(wxGCDC* dc);
#ifdef _TACTICSPI_H_
    void DrawPressureScale(wxGCDC* dc);
    void OnLogDataButtonPressed(wxCommandEvent& event);
#else
    void SetMinMaxWindScale();
    void DrawWindSpeedScale(wxGCDC* dc);
#endif // _TACTICSPI_H_
};



#endif // __BARO_HISTORY_H__

