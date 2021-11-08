/******************************************************************************
 * $Id: Odograph.h, v1.0 2020/11/30 VaderDarth Exp $
 *
 * Project:  OpenCPN
 * Purpose:  DashT Plugin
 * Author:   Thomas Rauch / Jean-Eudes Onfray
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************---
 */

#ifndef __ODOGRAPH_H__
#define __ODOGRAPH_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#define __DERIVEDTIMEOUT_OVERRIDE__
#include "instrument.h"

#include "TacticsWindow.h"

#include <wx/filename.h>
#include <wx/bitmap.h>

#define ODOGRAPH_DATAPOINT_DEF_TIME 30 // (full tens) 60s at 5knts = 77 meters
#define ODOGRAPH_DATAPOINT_MIN_TIME 10 // (full tens) 10s at 5knts = 26 meters
#define ODOM_RECORD_COUNT 240 // at 5 knts, 30s sampling = 10 nm
#define ODOGRAPH_DEF_LOG_GRAPH_RANGE_LOW_DEF 0
#define ODOGRAPH_DEF_LOG_GRAPH_RANGE_UP_DEF  1

class DashboardInstrument_Odograph: public DashboardInstrument
{
public:
    DashboardInstrument_Odograph( TacticsWindow *pparent, wxWindowID id,
                                  wxString title);
    ~DashboardInstrument_Odograph(void);
    void SetData(
        unsigned long long, double, wxString,
        long long timestamp=0LL ) override {;};
    void OnLogHistUpdTimer(wxTimerEvent &event);
    virtual void timeoutEvent(void){};
    wxSize GetSize( int orient, wxSize hint );

#ifndef __ODOGRAPH_DERIVEDTIMEOUT_OVERRIDE__
    virtual void derived2TimeoutEvent(void){};
#else
    virtual void derived2TimeoutEvent(void) = 0;
#endif // __ODOGRAPH_DERIVEDTIMEOUT_OVERRIDE__
    virtual void derivedTimeoutEvent(void);
    
    virtual void PushLogHere(
        double data, wxString unit, long long timestamp=0LL);
    virtual void PushSatHere(
        double data, wxString unit, long long timestamp=0LL);
    virtual void PushLatHere(
        double data, wxString unit, long long timestamp=0LL);
    virtual void PushLonHere(
        double data, wxString unit, long long timestamp=0LL);


private:
    wxFileConfig        *m_pconfig;
    void EmptyVectors(void);
    void ClearTotal(void);
    bool LoadConfig(void);
    bool SaveConfig(void);
    bool SaveGrandTotal(void);

    wxDECLARE_EVENT_TABLE();

protected:
    TacticsWindow       *m_pparent;
    bool                 m_IsRunning;
    double               m_gnssTotal;
    bool                 m_useLog;
    double               m_logTotal;
    int                  m_logRangeLow;
    int                  m_logRangeUp;
    double               m_ArrayOdometer[ODOM_RECORD_COUNT];
    double               m_ArrayDistance[ODOM_RECORD_COUNT];
    double               m_ArrayDirection[ODOM_RECORD_COUNT];
    double               m_ArrayLat[ODOM_RECORD_COUNT];
    double               m_ArrayLon[ODOM_RECORD_COUNT];
    double               m_ArrayLog[ODOM_RECORD_COUNT];
    double               m_ArrayLogOdom[ODOM_RECORD_COUNT];
    double               m_ArrayLogDist[ODOM_RECORD_COUNT];
    wxDateTime::Tm       m_ArrayRecTime[ODOM_RECORD_COUNT];
    int                  m_SampleCount;

    wxTimer             *m_OdographUpdTimer;

    wxRect               m_WindowRect;
    wxRect               m_DrawAreaRect;
    int                  m_TopLineHeight;
    int                  m_width;
    int                  m_height;
    int                  m_LeftLegend;
    int                  m_RightLegend;
    double               m_ratioW;

    wxString             m_logfile;
    wxFile              *m_ostreamlogfile;
    bool                 m_isExporting;
    int                  m_datapointInterval;
    double               m_grandTotal;
    bool                 m_saveAllConf;

    wxButton            *m_LogButton;
    wxMenu              *m_pExportmenu;
    wxMenuItem          *m_btnStartStop;
    wxMenuItem          *m_btnStartStopSep;
    wxBitmap            *m_btnBmpStartRec;
    wxBitmap            *m_btnBmpStopRec;
    wxMenuItem          *m_btnReset;
    wxBitmap            *m_btnBmpReset;
    wxMenuItem          *m_btnResetSep;
    wxMenuItem          *m_btnResetAll;
    wxBitmap            *m_btnBmpResetAll;

    callbackFunction     m_fPushSatHere;
    wxString             m_fPushSatUUID;
    double               m_Sat;
    callbackFunction     m_fPushLonHere;
    wxString             m_fPushLonUUID;
    double               m_Lon;
    callbackFunction     m_fPushLatHere;
    wxString             m_fPushLatUUID;
    double               m_Lat;
    callbackFunction     m_fPushLogHere;
    wxString             m_fPushLogUUID;
    double               m_Log;
    
    void OnClose( wxCloseEvent& event );
    void Draw(wxGCDC* dc);
    void DrawBackground(wxGCDC* dc);
    void DrawForeground(wxGCDC* dc);
    void SetMinMaxLogScale();
    void DrawDistanceScale(wxGCDC* dc);
    void OnLogDataButtonPressed(wxCommandEvent& event);
    void OnStartStopPressed(wxCommandEvent& event);
    void OnResetPressed(wxCommandEvent& event);
    void OnResetAllPressed(wxCommandEvent& event);
    void ExportData(void);

};


#endif // __ODOGRAPH_H__

