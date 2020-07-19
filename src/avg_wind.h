/******************************************************************************
* $Id: avg_wind.h, v1.0 2010/08/30 tom-r Exp $
*
* Project:  OpenCPN
* Purpose:  Tactics_pi Plugin
* Author:   Thomas Rauch
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

#ifndef __AVG_WIND_H__
#define __AVG_WIND_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#define AVG_WIND_RECORDS 1800 // 30 min with 60 secs (warning: div by 0 if count == 1)
#define AVG_WIND_MIN_DEF_TIME 240   // 240 seconds = 4 minutes
#define AVG_WIND_MAX_TIME 1800 // 1800 seconds = 30 minutes, must be <= records
#define AVG_WIND_CLEAR_NO_DATA_CNT 5 // if data is not coming back, restart

#include "instrument.h"
#include "dial.h"

#include "DoubleExpSmooth.h"

// class for calculation of the average wind direction 
class AvgWind
{
public:
    AvgWind();
    //  AvgWind(tactics_pi *parent);
    ~AvgWind(void);
    void CalcAvgWindDir( double CurWindDir );
    void SetAvgTime( int time );
    void DataClear( bool dataInterruption = true );
    double GetAvgWindDir();
    double GetDegRangePort();
    double GetDegRangeStb();
    double GetsignedWindDirArray( int idx );
    double GetExpSmoothSignedWindDirArray( int idx );
    int GetSampleCount();

protected:
    int              m_SampleCount;
    double           m_DegRangeStb;
    double           m_DegRangePort; //live max-, min values
    int              m_AvgTime; // in [secs]
    double           m_AvgWindDir;
    double           m_WindDirArray[AVG_WIND_RECORDS];
    double           m_signedWindDirArray[AVG_WIND_RECORDS];
    double           m_ExpSmoothSignedWindDirArray[AVG_WIND_RECORDS];
    double           m_ExpsinSmoothArrayWindDir[AVG_WIND_RECORDS];
    double           m_ExpcosSmoothArrayWindDir[AVG_WIND_RECORDS]; //30 min with 60sec each
    DoubleExpSmooth *mDblsinExpSmoothWindDir, *mDblcosExpSmoothWindDir;

};

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    TacticsInstrument_AvgWindDir
//|
//| DESCRIPTION:
//|    This instrument  keeps track on the average wind direction
//+------------------------------------------------------------------------------

class TacticsInstrument_AvgWindDir : public DashboardInstrument 
{
public:
    TacticsInstrument_AvgWindDir(wxWindow *parent, wxWindowID id, wxString title);
    ~TacticsInstrument_AvgWindDir(void);
    void SetData(unsigned long long, double, wxString, long long timestamp=0LL );
    virtual void timeoutEvent(void){};
    wxSize GetSize(int orient, wxSize hint);

private:
    int m_soloInPane;

protected:
    double    m_WindDir;
    double    m_ratioW;
    double    m_ratioH;
    double    m_AvgWindDir;
    int       m_AvgTime; // in [secs]
    double    m_DegRangeStb;
    double    m_DegRangePort;
    bool      m_IsRunning;
    int       m_SampleCount;
    wxSlider *m_AvgTimeSlider;
    wxTimer  *m_avgWindUpdTimer;
    int       m_TopLineHeight;
    int       m_SliderHeight;
    int       m_availableHeight;
    int       m_width;
    int       m_height;
    int       m_cx;
    wxSize    size;
    int       m_Legend;
    int       m_cntNoData;

    wxDECLARE_EVENT_TABLE();

    void OnClose( wxCloseEvent& event );
    void Draw(wxGCDC* dc);
    void DrawBackground(wxGCDC* dc);
    void DrawForeground(wxGCDC* dc);
    double GetAvgWindDir();
    void OnAvgTimeSliderUpdated(wxCommandEvent& event);
    void CalcAvgWindDir(double CurWindDir);
    void OnAvgWindUpdTimer(wxTimerEvent & event);
};

#endif // __AVG_WIND_H__

