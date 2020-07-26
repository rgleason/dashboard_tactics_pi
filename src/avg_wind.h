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
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
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

#define AVG_WIND_DEFAULT_WIDTH   275
#define AVG_WIND_DEFAULT_HEIGHT  375

#define AVG_WIND_MIN_DEF_TIME 240   // seconds = 4 minutes
#define AVG_WIND_MAX_TIME 1800 // shall be <= AVG_WIND_RECORDS
#define SHORT_AVG_WIND_MIN_PERCENTAGE 10
#define SHORT_AVG_WIND_DEF_PERCENTAGE 25
#define SHORT_AVG_WIND_MAX_PERCENTAGE 50
#define SHORT_AVG_WIND_MAX_TIME AVG_WIND_MAX_TIME*SHORT_AVG_WIND_MAX_PERCENTAGE/100
#define AVG_WIND_RECORDS AVG_WIND_MAX_TIME // seconds = 30 min (div by 0 if count == 1)
#define SHORT_AVG_WIND_RECORDS SHORT_AVG_WIND_MAX_TIME
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
    int GetAvgTime( void );
    void SetShortAvgTime ( int time );
    int GetShortAvgTime( void );
    void DataClear( bool dataInterruption = true );
    double GetAvgWindDir();
    double GetDegRangePort();
    double GetDegRangeStb();
    double GetShortAvgWindDir();
    double GetShortDegRangePort();
    double GetShortDegRangeStb();
    double GetsignedWindDirArray( int idx );
    double GetExpSmoothSignedWindDirArray( int idx );
    int GetSampleCount();
    int GetShortSampleCount();

protected:
    int              m_SampleCount;
    int              m_ShortSampleCount;
    int              m_AvgTime; // [s]
    int              m_ShortAvgTime; // [s]
    double           m_AvgWindDir;
    double           m_DegRangeStb;
    double           m_DegRangePort; //live max-, min values
    double           m_ShortAvgWindDir;
    double           m_ShortDegRangeStb;
    double           m_ShortDegRangePort;
    double           m_WindDirArray[AVG_WIND_RECORDS];
    double           m_ShortWindDirArray[SHORT_AVG_WIND_RECORDS];
    double           m_signedWindDirArray[AVG_WIND_RECORDS];
    double           m_signedShortWindDirArray[SHORT_AVG_WIND_RECORDS];
    double           m_ExpSmoothSignedWindDirArray[AVG_WIND_RECORDS];
    double           m_ExpSmoothSignedShortWindDirArray[SHORT_AVG_WIND_RECORDS];
    double           m_ExpsinSmoothArrayWindDir[AVG_WIND_RECORDS];
    double           m_ExpcosSmoothArrayWindDir[AVG_WIND_RECORDS];
    double           m_ExpsinSmoothArrayShortWindDir[SHORT_AVG_WIND_RECORDS];
    double           m_ExpcosSmoothArrayShortWindDir[SHORT_AVG_WIND_RECORDS];
    DoubleExpSmooth *mDblsinExpSmoothWindDir, *mDblcosExpSmoothWindDir;
    DoubleExpSmooth *mDblsinExpSmoothShortWindDir, *mDblcosExpSmoothShortWindDir;

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
    void timeoutEvent(void) override;
#ifndef __DERIVEDTIMEOUT_OVERRIDE__
    virtual void derivedTimeoutEvent(void){};
#else
    virtual void derivedTimeoutEvent(void);
#endif // __DERIVEDTIMEOUT_OVERRIDE__
    wxSize GetSize(int orient, wxSize hint);
    virtual wxSize DoGetBestSize() const override;

private:
    int            m_soloInPane;

protected:
    double         m_WindDir;
    double         m_ratioW;
    double         m_ratioH;
    double         m_AvgWindDir;
    int            m_AvgTime; // [s]
    double         m_DegRangeStb;
    double         m_DegRangePort;
    double         m_ShortAvgWindDir;
    int            m_ShortAvgTime; // [s]
    int            m_ShortAvgTimePercentage;
    double         m_ShortDegRangeStb;
    double         m_ShortDegRangePort;
    bool           m_IsRunning;
    int            m_SampleCount;
    int            m_ShortSampleCount;
    wxSlider      *m_avgTimeSlider;
    wxTimer       *m_avgWindUpdTimer;
    int            m_TopLineHeight;
    int            m_avgSliderHeight;
    int            m_availableHeight;
    int            m_width;
    int            m_height;
    int            m_cx;
    wxSize         size;
    int            m_avgLegendW;
    int            m_cntNoData;
    wxFileConfig  *m_pconfig;

    wxDECLARE_EVENT_TABLE();

    void OnClose( wxCloseEvent& event );
    void Draw(wxGCDC* dc);
    void DrawBackground(wxGCDC* dc);
    void DrawForeground(wxGCDC* dc);
    double GetAvgWindDir();
    void DataClear();
    void OnAvgTimeSliderUpdated(wxCommandEvent& event);
    void CalcAvgWindDir(double CurWindDir);
    void OnAvgWindUpdTimer(wxTimerEvent & event);
    bool LoadConfig(void);
    void SaveConfig(void);

};

#endif // __AVG_WIND_H__

