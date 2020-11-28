/******************************************************************************
 * $Id: Odograph.cpp, v1.0 2020/11/30 VaderDarth Exp $
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

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/fileconf.h>
#include "Odograph.h"
#include "tactics_pi_ext.h"
using namespace std::placeholders;

#define SETDRAWSOLOINPANE true

#include "icons.h"

#include "plugin_ids.h"
wxBEGIN_EVENT_TABLE (DashboardInstrument_Odograph, DashboardInstrument)
EVT_TIMER (myID_THREAD_ODOGRAPH,
           DashboardInstrument_Odograph::OnLogHistUpdTimer)
EVT_BUTTON (
    myID_OG_BUTTON, DashboardInstrument_Odograph::OnLogDataButtonPressed )
EVT_MENU (
    myID_OG_SAVESTOP, DashboardInstrument_Odograph::OnStartStopPressed )
EVT_MENU (
    myID_OG_RESET, DashboardInstrument_Odograph::OnResetPressed )
EVT_MENU (
    myID_OG_RESETALL, DashboardInstrument_Odograph::OnResetAllPressed )
EVT_CLOSE (DashboardInstrument_Odograph::OnClose)
wxEND_EVENT_TABLE ()

//****************************************************************************
// Odograph - registering distance and position
//****************************************************************************

DashboardInstrument_Odograph::DashboardInstrument_Odograph(
    TacticsWindow *pparent, wxWindowID id, wxString title) :
DashboardInstrument(pparent, id, "---", 0LL, SETDRAWSOLOINPANE)
{
    m_pparent = pparent;
    
    EmptyVectors();
    
    m_ratioW = std::nan("1");
    m_IsRunning = false;

    m_OdographUpdTimer = new wxTimer( this, myID_THREAD_ODOGRAPH );

    m_WindowRect = GetClientRect();
    m_DrawAreaRect = GetClientRect();
    m_DrawAreaRect.SetHeight(
        m_WindowRect.height - m_TopLineHeight -m_TitleHeight );
    m_TopLineHeight=35;
    m_TitleHeight = 10;
    m_width = 0;
    m_height = 0;
    m_LeftLegend = 3;
    m_RightLegend = 3;
    m_ostreamlogfile = new wxFile();
    m_isExporting = false;
    
    m_pconfig = GetOCPNConfigObject();
    if (LoadConfig() == false) {
        m_datapointInterval = ODOGRAPH_DATAPOINT_DEF_TIME;
        m_logfile = wxEmptyString;
        m_grandTotal = 0.0;
    }
    m_saveAllConf = true;

    /*
      Subcscribe to some interesting data, for compatibility reasons
      we use OpenCPN Dashboard paths, so if there is Signal K paths
      they are considered having passing first through OpenCPN. This way,
      this instrument can be used both with OpenCPN and Signal K with no
      modifications.
    */
    m_Log = std::nan("1");
    if ( m_useLog ) {
        m_fPushLogHere = std::bind(
            &DashboardInstrument_Odograph::PushLogHere, this, _1, _2, _3 );
        m_fPushLogUUID = m_pparent->subscribeTo (
            _T("OCPN_DBP_STC_VLW2"),m_fPushLogHere );
    }
    else {
        m_fPushLogHere = nullptr;
        m_fPushLogUUID = wxEmptyString;
    }

    m_Sat = std::nan("1");
    m_fPushSatHere = std::bind(
        &DashboardInstrument_Odograph::PushSatHere, this, _1, _2, _3 );
    m_fPushSatUUID = m_pparent->subscribeTo (
        _T("OCPN_DBP_STC_SAT"), m_fPushSatHere );

    m_Lat = std::nan("1");
    m_fPushLatHere = std::bind(
        &DashboardInstrument_Odograph::PushLatHere, this, _1, _2, _3 );
    m_fPushLatUUID = m_pparent->subscribeTo (
        _T("OCPN_DBP_STC_LAT"), m_fPushLatHere );

    m_Lon = std::nan("1");
    m_fPushLonHere = std::bind(
        &DashboardInstrument_Odograph::PushLonHere, this, _1, _2, _3 );
    m_fPushLonUUID = m_pparent->subscribeTo (
        _T("OCPN_DBP_STC_LON"), m_fPushLonHere );


    //data export
    wxPoint pos;
    pos.x = pos.y = 0;
    m_LogButton = new wxButton(
        this, myID_OG_BUTTON, _(">"), pos, wxDefaultSize,
        wxBU_TOP | wxBU_EXACTFIT |
        wxFULL_REPAINT_ON_RESIZE | wxBORDER_NONE);
    m_LogButton->SetToolTip(
        _("'>' starts data export. Create a new, or append "
          "to an existing file,\n'X': data export on-going.\n"
          "Reset counters." ) );
    m_pExportmenu = nullptr;
    m_btnStartStop = nullptr;
    m_btnStartStopSep = nullptr;
    m_btnBmpStartRec = new wxBitmap( wxBITMAP_PNG_FROM_DATA( btnstartrec ) );
    m_btnBmpStopRec = new wxBitmap( wxBITMAP_PNG_FROM_DATA( btnstoprec ) );
    m_btnReset = nullptr;
    m_btnBmpReset = new wxBitmap( wxBITMAP_PNG_FROM_DATA( btnbackstop ) );
    m_btnResetSep = nullptr;
    m_btnResetAll = nullptr;
    m_btnBmpResetAll = new wxBitmap( wxBITMAP_PNG_FROM_DATA( btneject ) );

    // Start thread
    m_OdographUpdTimer->Start(1000, wxTIMER_CONTINUOUS);
    
}

DashboardInstrument_Odograph::~DashboardInstrument_Odograph(void) {
    if ( m_OdographUpdTimer )
        m_OdographUpdTimer->Stop();
    if ( m_ostreamlogfile )
        delete m_ostreamlogfile;
    if ( !m_fPushLogUUID.IsEmpty() )
        m_pparent->unsubscribeFrom( m_fPushLogUUID );
    if ( !m_fPushSatUUID.IsEmpty() )
        m_pparent->unsubscribeFrom( m_fPushSatUUID );
    if ( !m_fPushLatUUID.IsEmpty() )
        m_pparent->unsubscribeFrom( m_fPushLatUUID );
    if ( !m_fPushLonUUID.IsEmpty() )
        m_pparent->unsubscribeFrom( m_fPushLonUUID );
}

void DashboardInstrument_Odograph::OnClose( wxCloseEvent &event )
{
    if ( m_OdographUpdTimer )
        m_OdographUpdTimer->Stop();
    if ( m_ostreamlogfile ) {
        if ( m_isExporting )
            m_ostreamlogfile->Close();
    }
    if ( !m_fPushLogUUID.IsEmpty() ) {
        m_pparent->unsubscribeFrom( m_fPushLogUUID );
        m_fPushLogUUID = wxEmptyString;
    }
    if ( !m_fPushSatUUID.IsEmpty() ) {
        m_pparent->unsubscribeFrom( m_fPushSatUUID );
        m_fPushSatUUID = wxEmptyString;
    }
    if ( !m_fPushLatUUID.IsEmpty() ) {
        m_pparent->unsubscribeFrom( m_fPushLatUUID );
        m_fPushLatUUID = wxEmptyString;
    }
    if ( !m_fPushLonUUID.IsEmpty() ) {
        m_pparent->unsubscribeFrom( m_fPushLonUUID );
        m_fPushLonUUID = wxEmptyString;
    }
    (void) SaveConfig();
}

void DashboardInstrument_Odograph::derivedTimeoutEvent()
{
    m_Log = std::nan("1");
    m_Sat = std::nan("1");
    m_Lat = std::nan("1");
    m_Lon = std::nan("1");
    m_IsRunning = false;
    derived2TimeoutEvent();
}

void DashboardInstrument_Odograph::EmptyVectors()
{
    m_SampleCount = 0;
    for (int idx = 0; idx < ODOM_RECORD_COUNT; idx++) {
        m_ArrayOdometer[idx] = -1.;
        m_ArrayDistance[idx] = -1.;
        m_ArrayDirection[idx] = -1;
        m_ArrayLat[idx] = -1;
        m_ArrayLon[idx] = -1;
        if ( m_useLog ) {
            m_ArrayLog[idx] = -1;
            m_ArrayLogOdom[idx] = -1;
            m_ArrayLogDist[idx] = -1;
        }
        m_ArrayRecTime[idx]=wxDateTime::UNow().GetTm();
        m_ArrayRecTime[idx].year=999;
    }
    if ( !m_useLog ) {
        m_ArrayLog[ ODOM_RECORD_COUNT - 1 ] = -1;
        m_ArrayLogOdom[ ODOM_RECORD_COUNT - 1 ] = -1;
        m_ArrayLogDist[ ODOM_RECORD_COUNT - 1 ] = -1;
    }
    m_gnssTotal = -1;
    m_logTotal = -1;
    m_logRangeLow = ODOGRAPH_DEF_LOG_GRAPH_RANGE_LOW_DEF;
    m_logRangeUp = ODOGRAPH_DEF_LOG_GRAPH_RANGE_UP_DEF;
    m_Log = std::nan("1");
    m_Sat = std::nan("1");
    m_Lat = std::nan("1");
    m_Lon = std::nan("1");

    SetMinMaxLogScale();
}

void DashboardInstrument_Odograph::ClearTotal()
{
    m_grandTotal = 0.0;
    (void) SaveGrandTotal();
    EmptyVectors();
}

#define __ODOGRAPH_CHECK_DATA__(__INSDATA__) if ( !std::isnan(data) ) \
    setTimestamp( timestamp ); \
m_##__INSDATA__ = data;

void DashboardInstrument_Odograph::PushLogHere(
    double data, wxString unit, long long timestamp)
{
    if ( !std::isnan(m_Log) && ( data <= m_Log ) )
        return; /* backward movement? - or just wrong, big negative
                   value like from Raymarine ST60+. Have to ignore. */
    __ODOGRAPH_CHECK_DATA__(Log)
}

void DashboardInstrument_Odograph::PushSatHere(
    double data, wxString unit, long long timestamp)
{
    __ODOGRAPH_CHECK_DATA__(Sat)
}

void DashboardInstrument_Odograph::PushLatHere(
    double data, wxString unit, long long timestamp)
{
    if ( std::isnan( m_Sat ) || ( m_Sat < 3. ) )
        return;
    __ODOGRAPH_CHECK_DATA__(Lat)
}

void DashboardInstrument_Odograph::PushLonHere(
    double data, wxString unit, long long timestamp)
{
    if ( std::isnan( m_Sat ) || ( m_Sat < 3. ) )
        return;
    __ODOGRAPH_CHECK_DATA__(Lon)
}

void DashboardInstrument_Odograph::OnLogHistUpdTimer(wxTimerEvent &event)
{
    if ( std::isnan(m_Sat) || std::isnan(m_Lat) || std::isnan(m_Lon) ) {
        m_IsRunning = false;
        return;
    }
    if ( m_Sat < 3. ) {
        m_IsRunning = false;
        return;
    }
    m_IsRunning = true;
    /*
      Check time when this tick occurred and only if match sampling rate
      then continue both with calucations and eventual exports
    */
    m_ArrayRecTime[ ODOM_RECORD_COUNT - 1] = wxDateTime::UNow().GetTm( );
    wxDateTime localTime( m_ArrayRecTime[ODOM_RECORD_COUNT - 1] );
    int secondNow = static_cast<int>( localTime.GetSecond() );
    if ( secondNow % m_datapointInterval != 0)
        return;
    /*
      We need of course at least two samples so the first one arriving
      has a special treatment - fast one!
    */
    if ( m_SampleCount < ODOM_RECORD_COUNT )
        m_SampleCount++;
    else
        m_SampleCount = ODOM_RECORD_COUNT;

    if ( m_SampleCount == 1 ) {
        m_gnssTotal = 0.0;
        m_ArrayOdometer[ ODOM_RECORD_COUNT - 1 ]     = 0.0;
        m_ArrayDistance[ ODOM_RECORD_COUNT - 1 ]     = 0.0;
        m_ArrayDirection[ ODOM_RECORD_COUNT - 1 ]    = 0.0;
        m_ArrayLat[ ODOM_RECORD_COUNT - 1 ]          = m_Lat;
        m_ArrayLon[ ODOM_RECORD_COUNT - 1 ]          = m_Lon;
        if ( m_useLog && !std::isnan(m_Log) ) {
            m_ArrayLog[ ODOM_RECORD_COUNT - 1 ]      = fabs( m_Log ); // yes!
            m_ArrayLogOdom[ ODOM_RECORD_COUNT - 1 ]  = 0.0;
            m_ArrayLogDist[ ODOM_RECORD_COUNT - 1 ]  = 0.0;
            m_logTotal = 0.0;
        }
        return;
    } // then first sample, store and skip the rest

    for (int idx = 1; idx < ODOM_RECORD_COUNT; idx++) {
        m_ArrayOdometer[ idx - 1 ]    = m_ArrayOdometer[ idx ];
        m_ArrayDistance[ idx - 1 ]    = m_ArrayDistance[ idx ];
        m_ArrayDirection[ idx - 1 ]   = m_ArrayDirection[ idx ];
        m_ArrayLat[ idx - 1 ]         = m_ArrayLat[ idx ];
        m_ArrayLon[ idx - 1 ]         = m_ArrayLon[ idx ];
        if ( m_useLog ) {
            m_ArrayLog[ idx - 1 ]     = m_ArrayLog[ idx ];
            m_ArrayLogOdom[ idx - 1 ] = m_ArrayLogOdom[ idx ];
            m_ArrayLogDist[ idx - 1 ] = m_ArrayLogDist[ idx ];
        }
        m_ArrayRecTime[ idx - 1 ]   = m_ArrayRecTime[ idx ];
    } // for shifting towards the right hand egde (drops out there)

    // Store the values as per the current state
    m_ArrayLat[ ODOM_RECORD_COUNT - 1] = m_Lat;
    m_ArrayLon[ ODOM_RECORD_COUNT - 1] = m_Lon;

    // Store and maintain the values coming from the total log
    if ( m_useLog ) {
        if ( !std::isnan(m_Log) ) {
            m_ArrayLog[ ODOM_RECORD_COUNT - 1] = fabs( m_Log ); // yes: neg.!
            if ( m_ArrayLog[ ODOM_RECORD_COUNT - 2 ] != -1. ) {
                double logDiff = (m_ArrayLog[ ODOM_RECORD_COUNT - 1] -
                                  m_ArrayLog[ ODOM_RECORD_COUNT - 2 ]);
                if ( m_logTotal < 0. )
                    m_logTotal = 0.0;
                if ( logDiff > 0. )
                    m_logTotal += logDiff;
                if ( logDiff >= 0. ) {
                    m_ArrayLogOdom[ ODOM_RECORD_COUNT - 1] = m_logTotal;
                    m_ArrayLogDist[ ODOM_RECORD_COUNT - 1] = logDiff;
                }
                else {
                    m_ArrayLogOdom[ ODOM_RECORD_COUNT - 1] = -1.;
                }
            } // then valid earlier log value, calculate trip
        } // then valid total log value
        else
            m_ArrayLog[ ODOM_RECORD_COUNT - 1]   = -1.;
    }
                           
    // Calculate distance and direction based on GPSS since last tick
    double distanceSinceLastTick = std::nan("1");
    double bearingFromLastTick = std::nan("1");
    DistanceBearingMercator_Plugin(
        m_ArrayLat[ ODOM_RECORD_COUNT - 1],
        m_ArrayLon[ ODOM_RECORD_COUNT - 1],
        m_ArrayLat[ ODOM_RECORD_COUNT - 2 ],
        m_ArrayLon[ ODOM_RECORD_COUNT - 2 ],
        &bearingFromLastTick, &distanceSinceLastTick );
    if ( !std::isnan(distanceSinceLastTick) ) {
        m_ArrayDistance[ ODOM_RECORD_COUNT - 1] = distanceSinceLastTick;
        m_gnssTotal  += distanceSinceLastTick;
        m_ArrayOdometer[ ODOM_RECORD_COUNT - 1] = m_gnssTotal;
        m_grandTotal += distanceSinceLastTick;
    }
    else
        m_ArrayOdometer[ ODOM_RECORD_COUNT - 1]   = -1.;
    if ( !std::isnan(bearingFromLastTick) )
        m_ArrayDirection[ ODOM_RECORD_COUNT - 1]  = bearingFromLastTick;
    else
        m_ArrayDirection[ ODOM_RECORD_COUNT - 1]  = -1.;

    // Data export  
    ExportData();

    // Set the new scale if needed
    SetMinMaxLogScale();

    // Save configuration, or at least the grand total counter
    if ( m_saveAllConf ) {
        (void) SaveConfig();
        m_saveAllConf = false;
    }
    else
        (void) SaveGrandTotal();
        
}

wxSize DashboardInstrument_Odograph::GetSize( int orient, wxSize hint )
{
      wxClientDC dc(this);
      int w;
      dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
      if( orient == wxHORIZONTAL ) {
        return wxSize( DefaultWidth, wxMax(m_TitleHeight+140, hint.y) );
      }
      else {
        return wxSize(
            wxMax(hint.x, DefaultWidth), wxMax(m_TitleHeight+140, hint.y) );
      }
}

void DashboardInstrument_Odograph::Draw( wxGCDC* dc )
{
   m_WindowRect = GetClientRect();
   m_DrawAreaRect=GetClientRect();
   m_DrawAreaRect.SetHeight(
       m_WindowRect.height-m_TopLineHeight -m_TitleHeight );
   m_DrawAreaRect.SetX( m_LeftLegend + 3 );
   DrawBackground( dc );
   DrawForeground( dc );
}

//***************************************************************************
// determine and set  min and max values based on the trip GPSS distance
//***************************************************************************

void DashboardInstrument_Odograph::SetMinMaxLogScale()
{
    if ( std::isnan( m_gnssTotal ) || (m_gnssTotal <= 0.) ||
         (m_SampleCount <= 1) ) {
        m_logRangeLow = ODOGRAPH_DEF_LOG_GRAPH_RANGE_LOW_DEF;
        m_logRangeUp = ODOGRAPH_DEF_LOG_GRAPH_RANGE_UP_DEF;
        return;
    } // then no data, or first tick

    int iMaxLogTotUp;
    double logTotalRef;
    if (  !m_useLog || std::isnan( m_logTotal ) || ( m_logTotal == -1 ) )
        logTotalRef = DBL_MIN;
    else
        logTotalRef = m_logTotal;

    double higherRef = ( (logTotalRef > m_gnssTotal) ?
                         logTotalRef : m_gnssTotal );
    if ( higherRef <= 1.0 ) {
        iMaxLogTotUp = 1;
    }
    else if ( higherRef <= 5.0 ) {
        iMaxLogTotUp = 5;
    }
    else {
        int topTenner = static_cast<int>( higherRef ) ;
        topTenner /= 10;
        topTenner += 1;
        topTenner *= 10;
        iMaxLogTotUp = topTenner;
    }
    m_logRangeUp = iMaxLogTotUp;

    int iMaxLogTotLow;
    double logLowRef;
    if ( !m_useLog || std::isnan( m_logTotal ) || ( m_logTotal == -1 ) )
        logLowRef = DBL_MAX;
    else {
        logLowRef = m_ArrayLogOdom[ODOM_RECORD_COUNT - m_SampleCount];
        if ( logLowRef <= 0 )
            logLowRef = DBL_MAX;
    }
    double gnssLowRef = m_ArrayOdometer[ODOM_RECORD_COUNT - m_SampleCount];
    double lowerRef = ( (logLowRef < gnssLowRef ) ?
                         logLowRef : gnssLowRef );
    if ( lowerRef == -1 )
        return; // a mishit?
    if ( lowerRef > higherRef )
        lowerRef = higherRef;
    
    if ( lowerRef <= 1.0 ) {
        iMaxLogTotLow = 0;
    }
    else if ( lowerRef <= 5.0 ) {
        iMaxLogTotLow = 5;
    }
    else {
        int topTenner = static_cast<int>( lowerRef ) ;
        topTenner /= 10;
        topTenner += 1;
        topTenner *= 10;
        iMaxLogTotLow = topTenner;
    }
    m_logRangeLow = iMaxLogTotLow;
}
//****************************************************************************
// distance legend
//****************************************************************************
void  DashboardInstrument_Odograph::DrawDistanceScale( wxGCDC* dc )
{
    wxString label1,label2,label3,label4,label5;
    wxColour cl;
    int width, height;
    cl = wxColour ( 61,61,204,255 );
    dc->SetTextForeground( cl );
    dc->SetFont( *g_pFontSmall );

    if( !m_IsRunning ) {
        label1=_T("-- nm");
        label2=_T("-- nm");
        label3=_T("-- nm");
        label4=_T("-- nm");
        label5=_T("-- nm");
    }
    else {
        double rangeUp  = static_cast<double>(m_logRangeUp);
        double rangeLow = static_cast<double>(m_logRangeLow);
        wxString formatDecStr = _T("%.1f nm");
        if ( m_logRangeUp < 10 )
            formatDecStr = _T("%.2f nm");
        /*
          Draw the legend with decimals only if we really have got them
        */
        // top legend for max distance
        label1.Printf( formatDecStr, rangeUp );
        // 3/4 legend
        label2.Printf( formatDecStr, (rangeUp - rangeLow) * 3. / 4. );
        // center legend
        label3.Printf( formatDecStr, (rangeUp - rangeLow) / 2. );
        // 1/4 legend
        label4.Printf( formatDecStr, (rangeUp - rangeLow) / 4. );
        // y origin legend
        label5.Printf( formatDecStr, rangeLow );
    }
    dc->GetTextExtent(
        label1, &m_LeftLegend, &height, 0, 0, g_pFontSmall );
    dc->DrawText(
        label1, 4, // this one, drop it slightly because of the button
        static_cast<int>( m_TopLineHeight + 7 - height/2 ) );
    dc->GetTextExtent(
        label2, &width, &height, 0, 0, g_pFontSmall );
    dc->DrawText(
        label2, 4,
        static_cast<int>( m_TopLineHeight +
                          m_DrawAreaRect.height/4 - height/2 ) );
    m_LeftLegend = wxMax( width, m_LeftLegend );
    dc->GetTextExtent(
        label3, &width, &height, 0, 0, g_pFontSmall );
    dc->DrawText(
        label3, 4,
        static_cast<int>( m_TopLineHeight +
                          m_DrawAreaRect.height/2 - height/2 ) );
    m_LeftLegend = wxMax( width, m_LeftLegend );
    dc->GetTextExtent(
        label4, &width, &height, 0, 0, g_pFontSmall );
    dc->DrawText(
        label4, 4,
        static_cast<int>( m_TopLineHeight +
                          m_DrawAreaRect.height*0.75 - height/2 ) );
    m_LeftLegend = wxMax( width, m_LeftLegend );
    dc->GetTextExtent(
        label5, &width, &height, 0, 0, g_pFontSmall );
    dc->DrawText(
        label5, 4,
        static_cast<int>( m_TopLineHeight +
                          m_DrawAreaRect.height - height/2 ) );
    m_LeftLegend = wxMax( width, m_LeftLegend );
    m_LeftLegend+=4;
}

//****************************************************************************
//draw background
//****************************************************************************
void DashboardInstrument_Odograph::DrawBackground( wxGCDC* dc )
{
    wxString label,label1,label2,label3,label4,label5;
    wxColour cl;
    wxPen pen;

    // draw legends
    DrawDistanceScale( dc );
    
    // horizontal lines
    GetGlobalColor( _T("UBLCK"), &cl );
    pen.SetColour( cl );
    dc->SetPen( pen );
    dc->DrawLine( m_LeftLegend + 3,
                  m_TopLineHeight,
                  m_WindowRect.width - 3 - m_RightLegend,
                  m_TopLineHeight); // the upper line
    dc->DrawLine( m_LeftLegend + 3,
                  (m_TopLineHeight + m_DrawAreaRect.height),
                  m_WindowRect.width - 3 - m_RightLegend,
                  (m_TopLineHeight + m_DrawAreaRect.height) );
    pen.SetStyle( wxPENSTYLE_DOT );
    dc->SetPen( pen );
    dc->DrawLine( m_LeftLegend + 3,
                  static_cast<int>(m_TopLineHeight +
                                   m_DrawAreaRect.height*0.25),
                  m_WindowRect.width - 3 - m_RightLegend,
                  static_cast<int>(m_TopLineHeight +
                                   m_DrawAreaRect.height*0.25) );
    dc->DrawLine( m_LeftLegend + 3,
                  static_cast<int>(m_TopLineHeight +
                                   m_DrawAreaRect.height*0.75),
                  m_WindowRect.width - 3 - m_RightLegend,
                  static_cast<int>(m_TopLineHeight +
                                   m_DrawAreaRect.height*0.75) );
#ifdef __WXMSW__
    pen.SetStyle( wxPENSTYLE_SHORT_DASH );
    dc->SetPen( pen );
#endif
    dc->DrawLine( m_LeftLegend + 3,
                  static_cast<int>(m_TopLineHeight +
                                   m_DrawAreaRect.height*0.5),
                  m_WindowRect.width - 3 - m_RightLegend,
                  static_cast<int>(m_TopLineHeight +
                                   m_DrawAreaRect.height*0.5) );
}

//****************************************************************************
//draw foreground
//****************************************************************************
void DashboardInstrument_Odograph::DrawForeground( wxGCDC* dc )
{
    wxColour col;
    double ratioH;
    int gnsw, gnsh;
    int width, height;
    wxString gnssTrip, logTrip;
    wxPen pen;
    wxString label;

    // GNSS trip
    dc->SetFont( *g_pFontData );
    col=wxColour( 204,41,41,255 ); //red, opaque
    dc->SetTextForeground( col );
    if( !m_IsRunning || std::isnan( m_gnssTotal ) )
        gnssTrip=_T("---- [----] (----) --");
    else {
        if ( m_gnssTotal < 0 )
            gnssTrip=_T("GNSS [----]|(----) nm");
        else
            gnssTrip = wxString::Format(
                _T("GNSS [%4.1f]|(%4.1f) nm"), m_gnssTotal, m_grandTotal);
    }
    dc->GetTextExtent( gnssTrip, &gnsw, &gnsh, 0, 0, g_pFontData );
    dc->DrawText( gnssTrip,
                  m_WindowRect.width -gnsw - m_RightLegend - 3,
                  m_TopLineHeight - gnsh );
    pen.SetStyle( wxPENSTYLE_SOLID );
    pen.SetColour( wxColour(204,41,41,255) ); //red, opaque
    pen.SetWidth( 1 );
    dc->SetPen( pen );
    ratioH = static_cast<double>(m_DrawAreaRect.height) /
        static_cast<double>(m_logRangeUp - m_logRangeLow);
    m_DrawAreaRect.SetWidth( m_WindowRect.width -
                             6 - m_LeftLegend - m_RightLegend );
    m_ratioW = static_cast<double>(m_DrawAreaRect.width) /
        static_cast<double>(ODOM_RECORD_COUNT - 1);

    // Log data header
    if ( m_useLog ) {
        col=wxColour( 61,61,204,255 ); //blue, opaque
        dc->SetFont( *g_pFontData );
        dc->SetTextForeground( col );
        if( !m_IsRunning || std::isnan( m_logTotal ) )
            logTrip=_T("             ");
        else {
            if ( m_logTotal < 0 )
                logTrip=_T("log [----] nm");
            else
                logTrip = wxString::Format(_T("log [%4.1f] nm"), m_logTotal);
        }
        dc->GetTextExtent( logTrip, &gnsw, &gnsh, 0, 0, g_pFontData );
        dc->DrawText( logTrip, m_LeftLegend + 3, m_TopLineHeight - gnsh );
        dc->SetFont( *g_pFontLabel );
    }

    // Do not use CPU time, if sampling rate is slow, it can take a while
    if ( m_SampleCount < 2 )
        return;

    int hasSampleIdx = ODOM_RECORD_COUNT - m_SampleCount;
    
    // GNSS data
    pen.SetWidth( 5 );
    dc->SetPen( pen );
    wxPoint points[ODOM_RECORD_COUNT + 2];
    wxPoint pointDist_old;
    pointDist_old.x = 3 + m_LeftLegend;
    pointDist_old.y = m_TopLineHeight + m_DrawAreaRect.height -
                           (m_ArrayOdometer[0] * ratioH);
    for ( int idx = (ODOM_RECORD_COUNT - 1);
          idx >= hasSampleIdx; idx-- ) {
        points[idx].x = idx * m_ratioW + 3 + m_LeftLegend;
        points[idx].y = m_TopLineHeight + m_DrawAreaRect.height -
            (m_ArrayOdometer[idx] * ratioH);
        if ( (points[idx].y > m_TopLineHeight) &&
             (pointDist_old.y > m_TopLineHeight) &&
             (points[idx].y <= (m_TopLineHeight +m_DrawAreaRect.height) ) &&
             (pointDist_old.y <= (m_TopLineHeight + m_DrawAreaRect.height) )
            )
            dc->DrawLine( pointDist_old.x, pointDist_old.y,
                          points[idx].x, points[idx].y );
        pointDist_old.x = points[idx].x;
        pointDist_old.y = points[idx].y;
    }

    if ( m_useLog && !std::isnan( m_logTotal ) && !( m_logTotal == -1 ) ) {
        // Log data
        pen.SetColour( wxColour(61,61,204,195) ); //blue, transparent
        pen.SetWidth( 3 );
        dc->SetPen( pen );
        pointDist_old.x = m_LeftLegend + 3;
        pointDist_old.y = m_TopLineHeight + m_DrawAreaRect.height -
            m_ArrayLogOdom[0] * ratioH;
        for ( int idx = (ODOM_RECORD_COUNT - 1);
              idx >= hasSampleIdx; idx-- ) {
            points[idx].x = idx * m_ratioW + 3 + m_LeftLegend;
            points[idx].y = m_TopLineHeight + m_DrawAreaRect.height -
                (m_ArrayLogOdom[idx] * ratioH);
            if ( (points[idx].y > m_TopLineHeight) &&
                 (pointDist_old.y > m_TopLineHeight) &&
                 (points[idx].y <= (m_TopLineHeight +
                                    m_DrawAreaRect.height)) &&
                 (pointDist_old.y <= (m_TopLineHeight +
                                      m_DrawAreaRect.height) )
                )
                dc->DrawLine( pointDist_old.x, pointDist_old.y,
                              points[idx].x, points[idx].y );
            pointDist_old.x=points[idx].x;
            pointDist_old.y=points[idx].y;
        }
    }

    //draw vertical timelines every 15 minutes
    GetGlobalColor( _T("UBLCK"), &col );
    pen.SetColour( col );
    pen.SetWidth( 1 );
    pen.SetStyle( wxPENSTYLE_DOT );
    dc->SetPen( pen );
    dc->SetTextForeground( col );
    dc->SetFont( *g_pFontSmall );
    int done = -1;
    wxPoint pointTime;
    int prevfiverfit = -15;
    for ( int idx = (ODOM_RECORD_COUNT - 1);
          idx >= hasSampleIdx; idx-- ) {
        if (m_ArrayRecTime[idx].year != 999) {
            wxDateTime localTime(
                m_ArrayRecTime[idx] );
            int min = localTime.GetMinute();
            int hour= localTime.GetHour();
            if ( ((hour * 100 + min) != done) &&
                 (min % 15 == 0) ) {
                if ( min != prevfiverfit ) {
                    pointTime.x = (idx * m_ratioW) + 3 + m_LeftLegend;
                    dc->DrawLine( pointTime.x, (m_TopLineHeight + 1),
                                  pointTime.x,(
                                      m_TopLineHeight +
                                      (m_DrawAreaRect.height + 1) ) );
                    label.Printf( _T("%02d:%02d"), hour,min );
                    dc->GetTextExtent(
                        label, &width, &height, 0, 0, g_pFontSmall) ;
                    dc->DrawText( label, pointTime.x-width/2,
                                  m_WindowRect.height-height);
                    done=hour*100+min;
                } // then avoid double printing
            }
        }
    }
}

void DashboardInstrument_Odograph::OnLogDataButtonPressed (
    wxCommandEvent& WXUNUSED(event))
{
    wxPoint pos;
    m_LogButton->GetSize(&pos.x, &pos.y);
    pos.x = 0;
    m_pExportmenu = new wxMenu();
    m_btnStartStop = new wxMenuItem(
        m_pExportmenu,
        myID_OG_SAVESTOP, ( m_isExporting ?
                            _("&Stop recording") :
                            _("&Start recording") ) );
    m_btnStartStop->SetBitmap( m_isExporting ?
                            *m_btnBmpStopRec :
                            *m_btnBmpStartRec );
    m_pExportmenu->Append( m_btnStartStop );
    m_btnStartStopSep = m_pExportmenu->AppendSeparator();
    m_btnReset = new wxMenuItem(
        m_pExportmenu,
        myID_OG_RESET, _("&Reset Trip") );
    m_btnReset->SetBitmap( *m_btnBmpReset );
    m_pExportmenu->Append( m_btnReset );
    m_btnResetSep = m_pExportmenu->AppendSeparator();
    m_btnResetAll = new wxMenuItem(
        m_pExportmenu,
        myID_OG_RESETALL, _("Reset Trip and &Total") );
    m_btnResetAll->SetBitmap( *m_btnBmpResetAll );
    m_pExportmenu->Append( m_btnResetAll );
    this->PopupMenu(m_pExportmenu, pos);
    if ( m_isExporting ) {
        m_pExportmenu->Check( myID_OG_SAVESTOP, true );
        m_btnStartStop->SetItemLabel( _("&Stop recording") );
    }
    else {
        m_pExportmenu->Check( myID_OG_SAVESTOP, false );
        m_btnStartStop->SetItemLabel( _("&Start recording") );
    }
}

void DashboardInstrument_Odograph::OnStartStopPressed (
    wxCommandEvent& WXUNUSED(event))
{
    if ( m_isExporting ) {
        m_isExporting = false;
        m_ostreamlogfile->Close();
        m_LogButton->SetLabel(_(">"));
        m_LogButton->Refresh();
    }
    else {
        wxPoint pos;
        m_LogButton->GetSize( &pos.x, &pos.y );
        pos.x = 0;
        wxFileDialog fdlg(
            GetOCPNCanvasWindow(),
            _("Choose a new or existing file"), wxT(""),
            m_logfile, wxT("*.*"), wxFD_SAVE);
        if ( fdlg.ShowModal() != wxID_OK ) {
            return; 
        }
        m_logfile.Clear();
        m_logfile = fdlg.GetPath();
        bool exists = m_ostreamlogfile->Exists( m_logfile );
        m_ostreamlogfile->Open( m_logfile, wxFile::write_append );
        if ( !exists ) {
            wxString str_ticks = g_bDataExportClockticks ?
                wxString::Format(_ ("   ClockTicks%s"),
                                 g_sDataExportSeparator) :
                _T("");
            wxString str_utc = g_bDataExportUTC ?
                wxString::Format(_("         UTC-ISO8601%s"),
                                 g_sDataExportSeparator) :
                _T("");
            wxString logHeaderPart;
            wxString columnFmt = _T(
                "%s"    // ticks
                "%s"    // utc
                "%s%s"  // local date, separator
                "%s%s"  // local time, separator
                "%s%s"  // lat, separator
                "%s%s"  // lon, separator
                "%s%s"  // tick distance, separator
                "%s%s"  // tick direction, separator
                "%s%s"  // gnss trip, separator
                "%s"    // gnss persistent log
                "%s\n");// boat loag header part if present
            if ( m_useLog )
                logHeaderPart = wxString::Format(
                    _T(
                        "%s"     // separator
                        "%s%s"   // log from boat's system, like based on STW
                        "%s%s"   // log distance from boat's log
                        "%s"),   // log trip from the boat's log
                    g_sDataExportSeparator,
                    "    Log", g_sDataExportSeparator,
                    "LogD", g_sDataExportSeparator,
                    "LogT" );
            else
                logHeaderPart = wxEmptyString;

            wxString str = wxString::Format(
                columnFmt,
                str_ticks,
                str_utc,
                "      Date", g_sDataExportSeparator,
                " Local Time", g_sDataExportSeparator,
                "      Lat", g_sDataExportSeparator,
                "      Lon", g_sDataExportSeparator,
                "Dist", g_sDataExportSeparator,
                "  Dir", g_sDataExportSeparator,
                " Trip", g_sDataExportSeparator,
                "  Total",
                logHeaderPart);
            m_ostreamlogfile->Write(str);
        }
        m_saveAllConf = true;
        m_isExporting = true;
        m_LogButton->SetLabel(_("X"));
        m_LogButton->Refresh();
    }
}

void DashboardInstrument_Odograph::OnResetPressed (
    wxCommandEvent& WXUNUSED(event))
{
    EmptyVectors();
    Refresh();
}

void DashboardInstrument_Odograph::OnResetAllPressed (
    wxCommandEvent& WXUNUSED(event))
{
    ClearTotal();
    Refresh();
}


/****************************************************************************

*****************************************************************************/

bool DashboardInstrument_Odograph::LoadConfig(void)
{
  wxFileConfig *pConf = m_pconfig;

  if (pConf) {
    pConf->SetPath(_T("/PlugIns/DashT/Tactics/Odograph"));
    pConf->Read(_T("DataPointInterval"),
                &m_datapointInterval,
                ODOGRAPH_DATAPOINT_DEF_TIME);
    if ( m_datapointInterval < ODOGRAPH_DATAPOINT_MIN_TIME )
        m_datapointInterval = ODOGRAPH_DATAPOINT_MIN_TIME;
    else {
        int tensofs = m_datapointInterval / 10;
        m_datapointInterval = tensofs * 10;
    } // else make a sanity check if not in tens of seconds
    pConf->Read(_T("OdographExportfile"), &m_logfile, wxEmptyString);
    pConf->Read(_T("GrandTotal"), &m_grandTotal, 0.0 );
    pConf->Read(_T("ShowBoatLog"), &m_useLog, true );
    return true;
  }
  else
    return false;
}
/****************************************************************************

*****************************************************************************/

bool DashboardInstrument_Odograph::SaveConfig(void)
{
  wxFileConfig *pConf = m_pconfig;

  if (pConf)
  {
    pConf->SetPath(_T("/PlugIns/DashT/Tactics/Odograph") );
    pConf->Write(_T("DataPointInterval"), m_datapointInterval );
    pConf->Write(_T("OdographExportfile"), m_logfile );
    pConf->Write(_T("GrandTotal"), m_grandTotal );
    pConf->Write(_T("ShowBoatLog"), m_useLog );
    return true;
  }
  else
    return false;
}

/****************************************************************************

*****************************************************************************/

bool DashboardInstrument_Odograph::SaveGrandTotal(void)
{
  wxFileConfig *pConf = m_pconfig;

  if (pConf)
  {
    pConf->SetPath(_T("/PlugIns/DashT/Tactics/Odograph"));
    pConf->Write(_T("GrandTotal"), m_grandTotal);
    return true;
  }
  else
    return false;
}
/****************************************************************************

*****************************************************************************/

void DashboardInstrument_Odograph::ExportData(void)
{
  if ( !m_isExporting )
      return;

  wxDateTime localTime( m_ArrayRecTime[ODOM_RECORD_COUNT - 1] );
  wxString str_utc, ticks; // either or
  if (g_bDataExportUTC) {
      wxDateTime utc = localTime.ToUTC();
      str_utc = wxString::Format(
          _T("%sZ%s"), utc.FormatISOCombined('T'),
          g_sDataExportSeparator);
  }
  else
      str_utc = _T("");
  if (g_bDataExportClockticks) {
      wxLongLong ti = wxGetUTCTimeMillis();
      wxString sBuffer = ti.ToString();
      wxString ticksString = sBuffer.wc_str();
      ticks = wxString::Format(
          _T("%s%s"), ticksString, g_sDataExportSeparator);
  }
  else
      ticks = _T("");
  wxString columnFmt = _T(
      "%s"       // ticks
      "%s"       // utc
      "%s%s"     // local date, separator
      "%11s%s"   // local time, separator
      "%9.5f%s"  // lat https://en.wikipedia.org/wiki/Decimal_degrees
      "%9.5f%s"  // lon, separator
      "%4.1f%s"  // tick distance, separator
      "%5.1f%s"  // tick direction, separator
      "%5.1f%s"  // gnss trip, separator
      "%7.1f");  // gnss persistent log
  if ( m_useLog )
      columnFmt += _T(
          "%s"         // separator
          "%7.1f%s"    // log from boat's system, like based on STW
          "%4.1f%s"    // log distance from boat's log
          "%4.1f\n");  // log trip from the boat's log
  else
      columnFmt += _T("\n");

  wxString str = wxString::Format(
      columnFmt,
      ticks,
      str_utc,
      localTime.FormatDate(), g_sDataExportSeparator,
      localTime.FormatTime(), g_sDataExportSeparator,
      m_ArrayLat[ODOM_RECORD_COUNT - 1], g_sDataExportSeparator,
      m_ArrayLon[ODOM_RECORD_COUNT - 1], g_sDataExportSeparator,
      m_ArrayDistance[ODOM_RECORD_COUNT - 1], g_sDataExportSeparator,
      m_ArrayDirection[ODOM_RECORD_COUNT - 1], g_sDataExportSeparator,
      m_gnssTotal, g_sDataExportSeparator,
      m_grandTotal, g_sDataExportSeparator,
      m_ArrayLog[ODOM_RECORD_COUNT - 1], g_sDataExportSeparator,
      m_ArrayLogDist[ODOM_RECORD_COUNT - 1], g_sDataExportSeparator,
      m_logTotal );
  m_ostreamlogfile->Write(str);

}
