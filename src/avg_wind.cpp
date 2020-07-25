/******************************************************************************
* $Id: avg_wind.cpp, v1.0 2010/08/30 tom_BigSpeedy Exp $
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
#include <cmath>

#include "avg_wind.h"

#include "TacticsFunctions.h"

#include "tactics_pi_ext.h"
#include "dashboard_pi_ext.h"

#include "plugin_ids.h"


// wxBEGIN_EVENT_TABLE (TacticsInstrument_AvgWindDir, DashboardInstrument)
wxBEGIN_EVENT_TABLE (TacticsInstrument_AvgWindDir, wxControl)
EVT_TIMER (myID_TICK_AVGWIND, TacticsInstrument_AvgWindDir::OnAvgWindUpdTimer)
EVT_CLOSE (TacticsInstrument_AvgWindDir::OnClose)
wxEND_EVENT_TABLE ()
/* ****************************************************************************
Class for Average Wind Calculation

Derived and used by  AvgWindDir instrument but not only, when this instrument
is active, the onject created by this class is available for other classes
as well, like tactics_pi, RaceStart classes, etc. to share the information
across the application.

For this calss to work, it shall be fed constantly by TWD. The class deals with
the exponential smmothing calculations. Retrieval of the calculated wind +
port & stb limits is provided with a set of  getter-methods.

Please note that, as object we get the TWD data from tactics_pi (which is
likely to calculate it, if not available or if forced calculation selected).
However, if the display instrument gets starvated by the same TWD data, it
need to clear the object's calculations, otherwise funny historical skew
will occur if data is coming back.

Therefore, the object cannot exist without the average wind instrument which
controls also the sampling time which can be selected with a slider it provides.
***************************************************************************** */

AvgWind::AvgWind()
{
    m_AvgTime = AVG_WIND_MIN_DEF_TIME;
    m_ShortAvgTime = SHORT_AVG_WIND_MAX_TIME;
    DataClear( false ); // fresh start, not a data loss
}

AvgWind::~AvgWind()
{
    delete mDblsinExpSmoothWindDir;
    delete mDblcosExpSmoothWindDir;
    delete mDblsinExpSmoothShortWindDir;
    delete mDblcosExpSmoothShortWindDir;
}

void AvgWind::DataClear( bool dataInterruption )
{
    m_SampleCount = 0;
    m_ShortSampleCount = 0;
    m_AvgWindDir = std::nan("1");
    m_DegRangePort = 0.0;
    m_DegRangeStb = 0.0;
    m_ShortAvgWindDir = std::nan("1");
    m_ShortDegRangePort = 0.0;
    m_ShortDegRangeStb = 0.0;
    for (int i = 0; i < AVG_WIND_RECORDS; i++) {
        m_WindDirArray[i] = std::nan("1");
        m_ShortWindDirArray[i] = std::nan("1");
        m_signedWindDirArray[i] = std::nan("1");
        m_signedShortWindDirArray[i] = std::nan("1");
        m_ExpsinSmoothArrayWindDir[i] = std::nan("1");
        m_ExpcosSmoothArrayWindDir[i] = std::nan("1");
        m_ExpSmoothSignedWindDirArray[i] = std::nan("1");
        m_ExpSmoothSignedShortWindDirArray[i] = std::nan("1");
    }
    if ( dataInterruption ) {
        delete mDblsinExpSmoothWindDir;
        delete mDblcosExpSmoothWindDir;
        delete mDblsinExpSmoothShortWindDir;
        delete mDblcosExpSmoothShortWindDir;
    }
    mDblsinExpSmoothWindDir = new DoubleExpSmooth(0.09);
    mDblcosExpSmoothWindDir = new DoubleExpSmooth(0.09);
    mDblsinExpSmoothShortWindDir = new DoubleExpSmooth(0.098);
    mDblcosExpSmoothShortWindDir = new DoubleExpSmooth(0.098);
}

void AvgWind::CalcAvgWindDir( double CurWindDir )
{
    if ( std::isnan( CurWindDir ) )
        return;

    if ( CurWindDir > 360.0 )
        return;

    if ( CurWindDir == 360.0 )
        CurWindDir = 0.0;

    m_SampleCount = ( (m_SampleCount < AVG_WIND_RECORDS) ?
                      (m_SampleCount + 1) : AVG_WIND_RECORDS );
    m_ShortSampleCount = ( (m_ShortSampleCount < SHORT_AVG_WIND_RECORDS) ?
                      (m_ShortSampleCount + 1) : SHORT_AVG_WIND_RECORDS );
    /*
      Fill in the array, perform data shifting in case the array is
      completely filled. Always fill the whole array, independent of which
      time average is set. Once the array is filled up, we can dynamically
      change the time average w/o the need to wait for another full set
      of data.
    */
    for ( int i = (AVG_WIND_RECORDS - 1); i > 0; i-- ) {
        m_WindDirArray[i] = m_WindDirArray[i - 1];
        m_ExpsinSmoothArrayWindDir[i] = m_ExpsinSmoothArrayWindDir[i - 1];
        m_ExpcosSmoothArrayWindDir[i] = m_ExpcosSmoothArrayWindDir[i - 1];
    }
    m_WindDirArray[0] = CurWindDir;
    double rad = (90 - CurWindDir) * M_PI / 180.;
    if (m_SampleCount == 1) {
        mDblsinExpSmoothWindDir->SetInitVal(sin(rad));
        mDblcosExpSmoothWindDir->SetInitVal(cos(rad));
    }
    m_ExpsinSmoothArrayWindDir[0] =
        mDblsinExpSmoothWindDir->GetSmoothVal(sin(rad));
    m_ExpcosSmoothArrayWindDir[0] =
        mDblcosExpSmoothWindDir->GetSmoothVal(cos(rad));

    for ( int i = (SHORT_AVG_WIND_RECORDS - 1); i > 0; i-- ) {
        m_ShortWindDirArray[i] = m_WindDirArray[i - 1];
        m_ExpsinSmoothArrayShortWindDir[i] = m_ExpsinSmoothArrayShortWindDir[i - 1];
        m_ExpcosSmoothArrayShortWindDir[i] = m_ExpcosSmoothArrayShortWindDir[i - 1];
    }
    m_ShortWindDirArray[0] = CurWindDir;
    if (m_ShortSampleCount == 1) {
        mDblsinExpSmoothShortWindDir->SetInitVal(sin(rad));
        mDblcosExpSmoothShortWindDir->SetInitVal(cos(rad));
    }
    m_ExpsinSmoothArrayShortWindDir[0] =
        mDblsinExpSmoothShortWindDir->GetSmoothVal(sin(rad));
    m_ExpcosSmoothArrayShortWindDir[0] =
        mDblcosExpSmoothShortWindDir->GetSmoothVal(cos(rad));

    // Problem of north directions: 355deg - 10deg :
    //     resolved with atan2 function...
    // Calculation of arithmetical mean value:
    double sinAvgDir = 0.0;
    double cosAvgDir = 0.0;
    double sinShortAvgDir = 0.0;
    double cosShortAvgDir = 0.0;
    rad = 0.0;
    int samples =
        ( (m_SampleCount < m_AvgTime) ? m_SampleCount : m_AvgTime );
    int shortsamples =
        ( (m_SampleCount < m_ShortAvgTime) ? m_SampleCount : m_ShortAvgTime );
    for (int i = 0; i < samples; i++) {
        rad = (90. - m_WindDirArray[i]) * M_PI / 180.;
        double sinrad = sin( rad );
        double cosrad = cos( rad );
        sinAvgDir += sinrad;
        cosAvgDir += cosrad;
        if ( i < shortsamples ) {
            sinShortAvgDir += sinrad;
            cosShortAvgDir += cosrad;
        }
    }
    m_AvgWindDir =
        (90. - (atan2( sinAvgDir, cosAvgDir )* 180. / M_PI) + 360.);
    while (m_AvgWindDir >= 360.)
        m_AvgWindDir -= 360.;
    m_ShortAvgWindDir =
        (90. - (atan2( sinShortAvgDir, cosShortAvgDir )* 180. / M_PI) + 360.);
    while (m_ShortAvgWindDir >= 360.)
        m_ShortAvgWindDir -= 360.;


    //m_AvgDegRange definition
    m_DegRangePort = 360;
    m_DegRangeStb = -360;
    for ( int i = 0;
          ( (i < samples) && !std::isnan( m_WindDirArray[i] ) );
          i++ ) {
        double val = getSignedDegRange( m_AvgWindDir, m_WindDirArray[i] );
        m_signedWindDirArray[i] = val;
        double smWDir =
            (90. - (atan2( m_ExpsinSmoothArrayWindDir[i],
                           m_ExpcosSmoothArrayWindDir[i]) * 180. / M_PI) +
             360.);
        while (smWDir >= 360)
            smWDir -= 360;
        double smval = getSignedDegRange(m_AvgWindDir, smWDir);
        m_ExpSmoothSignedWindDirArray[i] = smval;
        if (val < m_DegRangePort)
            m_DegRangePort = val;
        if (val > m_DegRangeStb)
            m_DegRangeStb = val;
    }

    m_ShortDegRangePort = 360;
    m_ShortDegRangeStb = -360;
    for ( int i = 0;
          ( (i < shortsamples) && !std::isnan( m_WindDirArray[i] ) );
          i++ ) {
        double val = getSignedDegRange( m_AvgWindDir, m_WindDirArray[i] );
        m_signedShortWindDirArray[i] = val;
        double smWDir =
            (90. - (atan2( m_ExpsinSmoothArrayShortWindDir[i],
                           m_ExpcosSmoothArrayShortWindDir[i]) * 180. / M_PI) +
             360.);
        while (smWDir >= 360)
            smWDir -= 360;
        double smval = getSignedDegRange(m_AvgWindDir, smWDir);
        m_ExpSmoothSignedShortWindDirArray[i] = smval;
        if (val < m_ShortDegRangePort)
            m_ShortDegRangePort = val;
        if (val > m_ShortDegRangeStb)
            m_ShortDegRangeStb = val;
    }
}

double AvgWind::GetAvgWindDir()
{
    return m_AvgWindDir;
}

double AvgWind::GetDegRangePort()
{
    return m_DegRangePort;
}

double AvgWind::GetDegRangeStb()
{
    return m_DegRangeStb;
}

double AvgWind::GetShortAvgWindDir()
{
    return m_ShortAvgWindDir;
}

double AvgWind::GetShortDegRangePort()
{
    return m_ShortDegRangePort;
}

double AvgWind::GetShortDegRangeStb()
{
    return m_ShortDegRangeStb;
}

void AvgWind::SetAvgTime(int time)
{
    m_AvgTime = time; //seconds !
}

int AvgWind::GetAvgTime()
{
    return m_AvgTime;
}

void AvgWind::SetShortAvgTime(int time)
{
    m_ShortAvgTime = time; //seconds !
}

int AvgWind::GetShortAvgTime()
{
    return m_ShortAvgTime;
}

double AvgWind::GetsignedWindDirArray(int idx)
{
    return m_signedWindDirArray[idx];
}

double AvgWind::GetExpSmoothSignedWindDirArray(int idx)
{
    return m_ExpSmoothSignedWindDirArray[idx];
}

int  AvgWind::GetSampleCount()
{
    return m_SampleCount;
}

int  AvgWind::GetShortSampleCount()
{
    return m_ShortSampleCount;
}

/* *************************************************************************
Instrument History of wind direction 
**************************************************************************** */

TacticsInstrument_AvgWindDir::TacticsInstrument_AvgWindDir(wxWindow *parent, wxWindowID id, wxString title) :
DashboardInstrument(parent, id, title, OCPN_DBP_STC_TWD)
{
    SetDrawSoloInPane(true);
    if ( !AverageWind ) {
        wxLogMessage (
            "dashboard_tactics_pi: TacticsInstrument_AvgWindDir: "
            "No AverageWind service available. Cannot start.");
        return;
    } // then sanity check for out-of tactics_pi usage
    m_pconfig = GetOCPNConfigObject();
    m_avgWindUpdTimer = nullptr;
    m_WindDir = std::nan("1");
    m_cntNoData = 0;
    m_AvgWindDir = std::nan("1");
    m_TopLineHeight = 30;
    m_TitleHeight = 10;
    m_avgSliderHeight = 0;
    m_availableHeight = 0;
    m_width = 0;
    m_height = 0;
    m_cx = 0;
    m_ratioW = std::nan("1");
    m_ratioH = std::nan("1");
    m_IsRunning = false;
    m_SampleCount = 0;
    m_ShortSampleCount = 0;
    m_avgLegendW = 3;
    m_AvgTime = AVG_WIND_MIN_DEF_TIME;
    m_ShortAvgTimePercentage = SHORT_AVG_WIND_DEF_PERCENTAGE;
    m_ShortAvgTime = AVG_WIND_MIN_DEF_TIME * SHORT_AVG_WIND_DEF_PERCENTAGE / 100;
    m_DegRangePort = 0.0;
    m_DegRangeStb = 0.0;
    wxSize size = GetClientSize();
    m_cx = size.x / 2;

    (void) LoadConfig();
    AverageWind->SetAvgTime( m_AvgTime );
    AverageWind->SetShortAvgTime( m_ShortAvgTime );
       
    m_avgTimeSlider = new wxSlider(
        this, wxID_ANY, m_AvgTime / 60,
        AVG_WIND_MIN_DEF_TIME / 60,
        AVG_WIND_MAX_TIME / 60,
        wxPoint(0,0), wxSize(100,100),
        wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_BOTTOM |
        wxFULL_REPAINT_ON_RESIZE | wxSL_LABELS );
    m_avgTimeSlider->SetPageSize(2);
    m_avgTimeSlider->SetLineSize(2);
    m_avgTimeSlider->SetTickFreq(2);
    m_avgTimeSlider->SetValue( m_AvgTime / 60 );
    m_avgTimeSlider->Connect(
        wxEVT_COMMAND_SLIDER_UPDATED,
        wxCommandEventHandler(
            TacticsInstrument_AvgWindDir::OnAvgTimeSliderUpdated),
        NULL, this );

    int w = 0;
    m_avgTimeSlider->GetSize(&w, &m_avgSliderHeight);

    // Start the processing thread
    m_avgWindUpdTimer = new wxTimer( this, myID_TICK_AVGWIND );
    m_avgWindUpdTimer->Start(1000, wxTIMER_CONTINUOUS);
}

TacticsInstrument_AvgWindDir::~TacticsInstrument_AvgWindDir(void)
{
    if ( this->m_avgWindUpdTimer ) {
        this->m_avgWindUpdTimer->Stop();
        delete this->m_avgWindUpdTimer;
    }
}

void TacticsInstrument_AvgWindDir::OnClose( wxCloseEvent &event )
{
    if ( this->m_avgWindUpdTimer )
        this->m_avgWindUpdTimer->Stop();
    SaveConfig();
}

void TacticsInstrument_AvgWindDir::OnAvgWindUpdTimer(wxTimerEvent & event)
{
    if ( !std::isnan(m_WindDir) ) {
        m_cntNoData = 0;
        m_AvgWindDir = AverageWind->GetAvgWindDir();
        m_DegRangePort = AverageWind->GetDegRangePort();
        m_DegRangeStb = AverageWind->GetDegRangeStb();
        m_SampleCount = AverageWind->GetSampleCount();
        m_ShortAvgWindDir = AverageWind->GetShortAvgWindDir();
        m_ShortDegRangePort = AverageWind->GetShortDegRangePort();
        m_ShortDegRangeStb = AverageWind->GetShortDegRangeStb();
        m_ShortSampleCount = AverageWind->GetShortSampleCount();
    }
    else {
        if ( m_cntNoData >= AVG_WIND_CLEAR_NO_DATA_CNT ) {
            AverageWind->DataClear();
            m_cntNoData = -1;
        }
        else if ( m_cntNoData >= 0 )
            m_cntNoData++;
    }
}

void TacticsInstrument_AvgWindDir::OnAvgTimeSliderUpdated(
    wxCommandEvent& event)
{ /*
    Note : the slider also updates the calculation in tactics_pi and all
    other instruments subscribed to the shared AverageWind class object
  */
    m_AvgTime = m_avgTimeSlider->GetValue() * 60;
    AverageWind->SetAvgTime( m_AvgTime );
    m_ShortAvgTime = m_AvgTime * m_ShortAvgTimePercentage / 100;
    AverageWind->SetShortAvgTime( m_ShortAvgTime );
}

wxSize TacticsInstrument_AvgWindDir::DoGetBestSize() const
{
    return wxSize(AVG_WIND_DEFAULT_WIDTH, AVG_WIND_DEFAULT_HEIGHT);
}

wxSize TacticsInstrument_AvgWindDir::GetSize(int orient, wxSize hint)
{
    wxClientDC dc( this );
    int w;
    dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
    if (orient == wxHORIZONTAL) {
        return wxSize( DefaultWidth, wxMax( m_TitleHeight + 140, hint.y ) );
    }
    else {
        return wxSize( wxMax( hint.x, DefaultWidth),
                       wxMax( m_TitleHeight + 140, hint.y ) );
    }
}
void TacticsInstrument_AvgWindDir::SetData(
    unsigned long long st, double data, wxString unit, long long timestamp)
{
    if (st == OCPN_DBP_STC_TWD ) { 
        m_WindDir = data; // Live wind
    }
    m_IsRunning = ( std::isnan(m_WindDir) ? false : true );
}

void TacticsInstrument_AvgWindDir::Draw(wxGCDC* dc)
{
  wxColour c1;
  GetGlobalColor(_T("DASHB"), &c1);
  wxBrush b1(c1);
  dc->SetBackground(b1);
  dc->Clear();

  wxSize size = GetClientSize();
  m_cx = size.x / 2;
  m_height = size.y;

  m_avgTimeSlider->SetSize( 10, 0, size.x - 20, 5 );
  int w;
  m_avgTimeSlider->GetSize( &w, &m_avgSliderHeight );

  int h;
  // "99" is not necessarily printed out, just the maximum value of legend
  dc->GetTextExtent( _T("99"), &w, &h, 0, 0, g_pFontSmall );
  m_avgLegendW = w;
  m_width = size.x - (2 * m_avgLegendW) - 2;

  m_availableHeight = size.y - m_TopLineHeight -
      m_avgSliderHeight - 1 - h;
  DrawBackground( dc );
  DrawForeground( dc );

}

// *****************************************************************************
// Draw background
// *****************************************************************************
void TacticsInstrument_AvgWindDir::DrawBackground(wxGCDC* dc)
{
  wxString label;
  wxColour cl;
  wxPen pen;
  //----------------------------------------------------------------------------
  // Draw lines
  //----------------------------------------------------------------------------
  GetGlobalColor( _T("UBLCK"), &cl );
  pen.SetColour( cl );
  pen.SetStyle( wxPENSTYLE_SOLID );
  dc->SetPen( pen );
  int width, height;
  int time = m_AvgTime / 60;
  cl = wxColour( 0, 0, 0, 255 ); //black, solid

  dc->SetTextForeground( cl );
  dc->SetFont( *g_pFontSmall );

  // vertical center line
  dc->DrawLine( m_cx, m_TopLineHeight + m_avgSliderHeight, m_cx,
                (int)(m_TopLineHeight + m_avgSliderHeight + m_availableHeight) );

  // top horizontal line
  label = wxString::Format( _T("%2d"), 0 );
  dc->GetTextExtent( label, &width, &height, 0, 0, g_pFontSmall );
  dc->DrawText( label, 1,
                (int)(m_TopLineHeight + m_avgSliderHeight - height / 2.) );
  dc->DrawLine( m_avgLegendW + 1, m_TopLineHeight + m_avgSliderHeight,
                m_avgLegendW + 1 + m_width,
                m_TopLineHeight + m_avgSliderHeight );
  // bottom line + legend
  label = wxString::Format( _T("%2d"), time );
  dc->GetTextExtent( label, &width, &height, 0, 0, g_pFontSmall );
  dc->DrawText( label, 1, (int)(m_TopLineHeight + m_avgSliderHeight +
                                m_availableHeight - height / 2.) ); 
  dc->DrawLine(
      m_avgLegendW + 1, (int)(m_TopLineHeight + m_avgSliderHeight +
                              m_availableHeight),
      (m_avgLegendW + 1 +m_width),
      static_cast<int>( (m_TopLineHeight + m_avgSliderHeight +
                         m_availableHeight) ) );
  int x1, x2;
  for ( int i = 1; i < time; i++ ) {
      if ( (i % 5) == 0 ) {
          x1 = m_cx;
          x2 = m_cx;
          label = wxString::Format( _T("%2d"), i );
          dc->DrawText(
              label, 1,
              (int)(m_TopLineHeight + m_avgSliderHeight - height / 2. +
                    m_availableHeight /
                    static_cast<double>( time ) * i) );
      }
      else {
          x1 = m_avgLegendW + 11;
          x2 = m_avgLegendW + 1 + m_width - 10;
      }
      dc->DrawLine( m_avgLegendW + 1 ,
                    (int)(m_TopLineHeight + m_avgSliderHeight +
                          m_availableHeight / (double)time * i), x1,
                    (int)(m_TopLineHeight + m_avgSliderHeight +
                          m_availableHeight / (double)time * i));
      dc->DrawLine( x2,
                    (int)(m_TopLineHeight + m_avgSliderHeight +
                          m_availableHeight / (double)time * i),
                    (m_avgLegendW + 1 + m_width),
                    (int)(m_TopLineHeight + m_avgSliderHeight +
                          m_availableHeight / (double)time * i));
  } // for time range
}

// *****************************************************************************
// Draw foreround
// *****************************************************************************
void TacticsInstrument_AvgWindDir::DrawForeground(wxGCDC* dc)
{
    wxColour col;
    int degw, degh;
    wxString avgWindAngle, minAngle, maxAngle;
    wxPen pen;
    wxString label;

    wxColour cl;
    GetGlobalColor( _T("UBLCK"), &cl );

  //----------------------------------------------------------------------------
  // Average wind direction
  //----------------------------------------------------------------------------
    dc->SetFont( *g_pFontData );
    col = wxColour( 255, 0, 0, 255 ); //red, solid
    dc->SetTextForeground( col );
    if ( !m_IsRunning || std::isnan( m_WindDir ) )
        avgWindAngle = _T("---");
    else {
        double dir = wxRound( m_AvgWindDir );
        while (dir > 360.)
            dir -= 360.;
        while (dir < 0.)
            dir += 360.;
        avgWindAngle = wxString::Format( _T("%3.0f" ), dir ) + DEGREE_SIGN;
    }
    dc->GetTextExtent( avgWindAngle, &degw, &degh, 0, 0, g_pFontData );
    dc->DrawText( avgWindAngle, (m_avgLegendW + 1 + m_width / 2 - degw / 2),
                  m_TopLineHeight + m_avgSliderHeight - degh );

    /* WindDirArray direction
       Actual values as variation from AverageWindDir, w/ +/- direction.
       - min and max directions from the array (0 .. m_AvgTime of the service
       class) resulting in the horizontal zoom factor m_ratioW.
       - Average time to be shown m_AvgTime results in the vertical zoom factor
       m_ratioH.
    */
    // height by number of seconds
    m_ratioH =
        static_cast<double>( m_availableHeight ) /
        static_cast<double>( m_AvgTime );
    // Take the biggest varition and double upl the average is always centered
    double maxDegRange = 2.* wxMax( m_DegRangeStb,
                                    abs( m_DegRangePort ) );

    m_ratioW = static_cast<double>( m_width ) / maxDegRange;
    pen.SetStyle( wxPENSTYLE_SOLID );

  //----------------------------------------------------------------------------
  // Live direction data
  //----------------------------------------------------------------------------
  
    wxPoint points, pointAngle_old;
    pointAngle_old.x = m_width / 2. +
        AverageWind->GetsignedWindDirArray( 0 ) * m_ratioW + m_avgLegendW + 1;
    pointAngle_old.y = m_TopLineHeight + m_avgSliderHeight + 1;
    pen.SetColour( wxColour( 0, 0, 255, 60 ) ); //blue, opaque
    dc->SetPen( pen );
    int samples = ( (m_SampleCount < m_AvgTime) ? m_SampleCount : m_AvgTime );

    for ( int idx = 1; idx < samples; idx++ ) {
        points.x = m_width / 2. +
            AverageWind->GetsignedWindDirArray(idx) * m_ratioW + m_avgLegendW + 1;
        points.y = static_cast<int>( static_cast<double>( m_TopLineHeight ) +
                                     m_avgSliderHeight + 1. +
                                     static_cast<double>( idx ) * m_ratioH );
        dc->DrawLine( pointAngle_old, points );
        pointAngle_old = points;
    }

    //----------------------------------------------------------------------------
    // Short term average and its min / max positions
    //----------------------------------------------------------------------------
  
    int shortsamples = ( (m_ShortSampleCount < m_ShortAvgTime) ?
                         0 : m_ShortAvgTime );
    int samplesRequired = samples * m_ShortAvgTimePercentage / 100;
    if ( shortsamples < samplesRequired )
        shortsamples = 0;
    if ( shortsamples != 0 ) {
        wxPoint pointShortPort, pointShortPort_old;
        wxPoint pointShortAvg, pointShortAvg_old;
        wxPoint pointShortStb, pointShortStb_old;
        pointShortPort_old.x = m_width / 2. +
            m_ShortDegRangePort * m_ratioW + m_avgLegendW + 1;
        if ( pointShortPort_old.x <= m_avgLegendW )
            pointShortPort_old.x = m_avgLegendW;
        pointShortPort_old.y = m_TopLineHeight + m_avgSliderHeight + 1;
        pointShortAvg_old.x = m_width / 2. +
            getSignedDegRange(
                m_AvgWindDir, m_ShortAvgWindDir ) * m_ratioW + m_avgLegendW + 1;
        if ( pointShortAvg_old.x <= m_avgLegendW )
            pointShortAvg_old.x = m_avgLegendW;
        if ( pointShortAvg_old.x >= (m_width - m_avgLegendW) )
            pointShortAvg_old.x = m_width - m_avgLegendW - 1;
        pointShortAvg_old.y = pointShortPort_old.y;
        pointShortStb_old.x = m_width / 2. +
            m_ShortDegRangeStb * m_ratioW + m_avgLegendW + 1;
        if ( pointShortStb_old.x > (m_width + m_avgLegendW + 1) )
            pointShortStb_old.x = m_width + m_avgLegendW + 1;
        pointShortStb_old.y = pointShortPort_old.y;

        dc->SetPen( pen );
        int shortsamples = ( (m_ShortSampleCount < m_ShortAvgTime) ?
                             m_ShortSampleCount : m_ShortAvgTime );

        for ( int idx = 1; idx < shortsamples; idx++ ) {
            pointShortPort.x = pointShortPort_old.x;
            pointShortPort.y = static_cast<int>(
                static_cast<double>( m_TopLineHeight ) +
                m_avgSliderHeight + 1. +
                static_cast<double>( idx ) * m_ratioH );
            pen.SetColour( wxColour( 128, 128, 128, 178) ); // light gray
            pen.SetWidth( 1 );
            dc->SetPen( pen );
            dc->DrawLine( pointShortPort_old, pointShortPort );
            pointShortPort_old = pointShortPort;
            pointShortAvg.x = pointShortAvg_old.x;
            pointShortAvg.y = static_cast<int>(
                static_cast<double>( m_TopLineHeight ) +
                m_avgSliderHeight + 1. +
                static_cast<double>( idx ) * m_ratioH );
            pen.SetColour( wxColour ( 255, 0, 0, 178 ) ); // red, opaqueness
            pen.SetWidth( 5 );
            dc->SetPen( pen );
            dc->DrawLine( pointShortAvg_old, pointShortAvg );
            pointShortAvg_old = pointShortAvg;
            pointShortStb.x = pointShortStb_old.x;
            pointShortStb.y = static_cast<int>(
                static_cast<double>( m_TopLineHeight ) +
                m_avgSliderHeight + 1. +
                static_cast<double>( idx ) * m_ratioH );
            pen.SetColour( wxColour( 128, 128, 128, 178) ); // light gray
            pen.SetWidth( 1 );
            dc->SetPen( pen );
            dc->DrawLine( pointShortStb_old, pointShortStb );
            pointShortStb_old = pointShortStb;
        }
    } // then start drawing short term statistics only if enough samples
    //----------------------------------------------------------------------------
    // Printing the exponential smoothed direction
    //----------------------------------------------------------------------------
    pen.SetStyle( wxPENSTYLE_SOLID );
    pen.SetColour( wxColour( 0, 0, 255, 128 ) );
    pen.SetWidth( 2 );
    dc->SetPen( pen );
    wxBrush greenbrush, redbrush;
    greenbrush.SetStyle( wxBRUSHSTYLE_SOLID );
    greenbrush.SetColour( wxColour( 0, 200, 0, 128) ); // green opaque
    redbrush.SetStyle( wxBRUSHSTYLE_SOLID );
    redbrush.SetColour( wxColour( 204, 41, 41, 128 ) ); // red opaque
    wxPoint fill[4];
    pointAngle_old.x = m_width / 2. +
        AverageWind->GetExpSmoothSignedWindDirArray(0) * m_ratioW +
        m_avgLegendW + 1;
    pointAngle_old.y = m_TopLineHeight + m_avgSliderHeight + 1;
  
    for ( int idx = 1; idx < samples; idx++ ) {
        points.x = m_width / 2. +
            AverageWind->GetExpSmoothSignedWindDirArray(idx) * m_ratioW +
            m_avgLegendW + 1;
        points.y = m_TopLineHeight + m_avgSliderHeight + 1 + idx * m_ratioH;
        fill[0].x = pointAngle_old.x;
        fill[0].y = pointAngle_old.y;
        fill[1].x = points.x;
        fill[1].y = points.y;
        fill[2].x = m_cx;
        fill[2].y = points.y;
        fill[3].x = m_cx;
        fill[3].y = pointAngle_old.y;
        dc->SetPen( pen );
        dc->DrawLine( pointAngle_old, points );
        dc->SetPen( *wxTRANSPARENT_PEN );
        if ( (points.x >= m_cx) && (pointAngle_old.x >= m_cx) )
            dc->SetBrush( greenbrush );
        else
            dc->SetBrush( redbrush );
        dc->DrawPolygon( 4, fill, 0, 0 );
        pointAngle_old.x = points.x;
        pointAngle_old.y = points.y;
    }
  
    //----------------------------------------------------------------------------
    // Live wind direction
    //----------------------------------------------------------------------------
    dc->SetFont( *g_pFontData );
    if ( !m_IsRunning || std::isnan( m_WindDir ) ) {
        minAngle = _T("---");
        maxAngle = _T("---");
    }
    else {
        double leftAngle = wxRound( m_AvgWindDir + m_DegRangePort );
        while (leftAngle > 360.)
            leftAngle -= 360.;
        while (leftAngle < 0.)
            leftAngle += 360.;

        minAngle = wxString::Format( _T("%3.0f"), leftAngle ) + DEGREE_SIGN;
        double rightAngle = wxRound( m_AvgWindDir + m_DegRangeStb );
        while (rightAngle > 360.)
            rightAngle -= 360;
        while (rightAngle < 0.)
            rightAngle += 360;
        maxAngle = wxString::Format( _T("%3.0f"), rightAngle ) + DEGREE_SIGN;
    }
    dc->GetTextExtent( minAngle, &degw, &degh, 0, 0, g_pFontData );
    col = wxColour(wxColour( 204, 41, 41, 128) ); //red, opaque
    dc->SetTextForeground( col );
    dc->DrawText( minAngle, m_avgLegendW + 1,
                  m_TopLineHeight + m_avgSliderHeight - degh );
    dc->GetTextExtent( maxAngle, &degw, &degh, 0, 0, g_pFontData );
    col = wxColour( wxColour( 0, 200, 0, 192 ) ); //green, opaque
    dc->SetTextForeground( col );
    dc->DrawText( maxAngle, m_width - degw + m_avgLegendW + 1,
                  m_TopLineHeight + m_avgSliderHeight - degh );
}

bool TacticsInstrument_AvgWindDir::LoadConfig()
{
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return false;
    
    pConf->SetPath( _T("/PlugIns/DashT/Tactics/AverageWind/") );
    pConf->Read( _T("AvgTime"), &m_AvgTime, AVG_WIND_MIN_DEF_TIME );
    if ( m_AvgTime < AVG_WIND_MIN_DEF_TIME )
        m_AvgTime = AVG_WIND_MIN_DEF_TIME;
    if ( m_AvgTime > AVG_WIND_MAX_TIME )
        m_AvgTime = AVG_WIND_MAX_TIME;
    int shortAvgTimePercentage;
    pConf->Read( _T("ShortAvgTimePercentage"), &shortAvgTimePercentage,
                 SHORT_AVG_WIND_DEF_PERCENTAGE );
    if ( shortAvgTimePercentage != 0 ) {
        if ( shortAvgTimePercentage < SHORT_AVG_WIND_MIN_PERCENTAGE )
            shortAvgTimePercentage = SHORT_AVG_WIND_MIN_PERCENTAGE;
        if ( shortAvgTimePercentage > SHORT_AVG_WIND_MAX_PERCENTAGE )
            shortAvgTimePercentage = SHORT_AVG_WIND_MAX_PERCENTAGE;
    }
    m_ShortAvgTimePercentage =  shortAvgTimePercentage;
    m_ShortAvgTime = m_AvgTime * m_ShortAvgTimePercentage / 100;
       
    return true;
}

void TacticsInstrument_AvgWindDir::SaveConfig()
{
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return;

    pConf->SetPath( _T("/PlugIns/DashT/Tactics/AverageWind/") );
    pConf->Write( _T("AvgTime"), m_AvgTime );
    pConf->Write( _T("ShortAvgTimePercentage"), m_ShortAvgTimePercentage );
    
    return;
}
