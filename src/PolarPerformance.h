/***************************************************************************
* $Id: PolarPerformance.h, v1.0 2016/06/07 tomBigSpeedy Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Thomas Rauch
*       (Inspired by original work from Jean-Eudes Onfray)
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
***************************************************************************
*/

#ifndef __POLARPERFORMANCE_H__
#define __POLARPERFORMANCE_H__

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/dynarray.h>
#include <wx/grid.h>
#include <wx/filename.h>
#include <map>

#include "instrument.h"
#include "plugin_ids.h"

class DoubleExpSmooth;
#include "avg_wind.h"


// Warn: div by 0 if count == 1
#define DATA_RECORD_COUNT 1500

class TacticsInstrument_PolarPerformance : public DashboardInstrument
{
public:
    TacticsInstrument_PolarPerformance(wxWindow *parent, wxWindowID id, wxString title);
    ~TacticsInstrument_PolarPerformance(void);
    void SetData(unsigned long long st, double data, wxString unit, long long timestamp=0LL);
    virtual void timeoutEvent(void){};
    wxSize GetSize(int orient, wxSize hint);
    void OnPolarPerfUpdTimer(wxTimerEvent & event);
    
private:
    wxFileConfig  *m_pconfig;
    bool LoadConfig(void);
    bool SaveConfig(void);

    wxDECLARE_EVENT_TABLE();

protected:
    double alpha;
    double m_ArrayPercentSpdHistory[DATA_RECORD_COUNT];
    double m_ArrayBoatSpdHistory[DATA_RECORD_COUNT];
    double m_ArrayTWAHistory[DATA_RECORD_COUNT];
    double m_ExpSmoothArrayBoatSpd[DATA_RECORD_COUNT];
    double m_ExpSmoothArrayPercentSpd[DATA_RECORD_COUNT];
    wxDateTime::Tm m_ArrayRecTime[DATA_RECORD_COUNT];
    
    wxTimer *m_PolarPerfUpdTimer;
    double m_MaxBoatSpd;
    double m_MinBoatSpd;
    double m_MaxPercent;  //...in array
    double m_AvgSpdPercent;
    DoubleExpSmooth  *mExpSmAvgSpdPercent;
    double m_AvgTWA;         //for data export
    DoubleExpSmooth  *mExpSmAvgTWA;//for data export
    double m_AvgTWS;         //for data export >100% 
    DoubleExpSmooth  *mExpSmAvgTWS;//for data export >100%
    
    double m_TWA;
    double m_TWS;
    double m_STW;
    double m_PolarSpeedPercent;
    double m_PolarSpeed;
    double m_MaxPercentScale;
    double m_MaxBoatSpdScale;
    int num_of_scales;
    double m_ratioW;
    bool m_IsRunning;
    int m_SampleCount;
    wxString m_STWUnit;
    wxString m_PercentUnit;

    wxRect m_WindowRect;
    wxRect m_DrawAreaRect; //the coordinates of the real darwing area
    int m_TopLineHeight;
    int m_LeftLegend;
    int m_RightLegend;

    wxString    m_logfile;        //for data export
    wxFile     *m_ostreamlogfile; //for data export
    bool        m_isExporting;      //for data export
    int         m_exportInterval; //for data export
    wxButton   *m_LogButton;     //for data export
    wxMenu     *m_pExportmenu;//for data export
    wxMenuItem *btn1Sec;//for data export
    wxMenuItem *btn5Sec;//for data export
    wxMenuItem *btn10Sec;//for data export
    wxMenuItem *btn20Sec;//for data export
    wxMenuItem *btn60Sec;//for data export
  struct pol
  {
    double   tmpwinddir[WINDDIR + 1];
    bool     ischanged[WINDDIR + 1]; 
  } tmpwindsp[WINDSPEED + 1];

  void Draw(wxGCDC* dc);
  void DrawBackground(wxGCDC* dc);
  void DrawForeground(wxGCDC* dc);
  void DrawBoatSpeedScale(wxGCDC* dc);
  void DrawPercentSpeedScale(wxGCDC* dc);
  void OnLogDataButtonPressed(wxCommandEvent& event);
  void ExportData(void);
};

#endif // __POLARPERFORMANCE_H__
