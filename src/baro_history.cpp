/******************************************************************************
 * $Id: baro_history.cpp, v1.0 2014/02/10 tom-r Exp $
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

#include <climits>
#include <cstdint>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/fileconf.h>

#include "baro_history.h"

#include "tactics_pi_ext.h"

#define SETDRAWSOLOINPANE true

#include "plugin_ids.h"

wxBEGIN_EVENT_TABLE (DashboardInstrument_BaroHistory, DashboardInstrument)
   EVT_TIMER (myID_THREAD_BAROHISTORY, DashboardInstrument_BaroHistory::OnBaroHistUpdTimer)
wxEND_EVENT_TABLE ()

//************************************************************************************************************************
// History of barometic pressure
//************************************************************************************************************************

DashboardInstrument_BaroHistory::DashboardInstrument_BaroHistory( wxWindow *parent, wxWindowID id, wxString title) :
    DashboardInstrument(parent, id, title, OCPN_DBP_STC_MDA, SETDRAWSOLOINPANE)
{

    m_LastReceivedPressure = 0.0;
    m_LastReceivedTime = wxDateTime::Now().GetTm();
    m_PressRecCnt=0;
    for ( int i = 0; i < BARO_START_AVG_CNT; i++ )
        m_PressStartVal[i] = - 1;

    m_pconfig = GetOCPNConfigObject();
    m_BaroHistUpdTimer = new wxTimer( this, myID_THREAD_BAROHISTORY );


    for (int idx = 0; idx < BARO_RECORD_COUNT; idx++) {
        m_ArrayPressHistory[idx] = -1;
        m_ArrayRecTime[idx]=wxDateTime::UNow().GetTm();
        m_ArrayRecTime[idx].year=999;
    }

    m_MaxPress = 0.0;
    m_MinPress = 1200.0;
    m_TotalMaxPress = 0.0;
    m_TotalMinPress= 1200.0;
    m_Press = 0.0;
    m_PressScale = NAN;
    m_MaxPressScale = NAN;
    m_ratioW = NAN;

    m_IsRunning = false;
    m_SampleCount = 0;
    
    m_WindowRect=GetClientRect();
    m_DrawAreaRect=GetClientRect();
    m_DrawAreaRect.SetHeight(m_WindowRect.height-m_TopLineHeight-m_TitleHeight);
    m_TopLineHeight = 35;
    m_LeftLegend = 3;
    m_RightLegend = 3;
    m_TitleHeight = 10;

    m_logfile = wxEmptyString;
    m_ostreamlogfile = new wxFile();
    m_exportInterval = 0;

    m_BaroHistUpdTimer->Start(1000, wxTIMER_CONTINUOUS);

    //data export
    m_isExporting = false;
    wxPoint pos;
    pos.x = pos.y = 0;
    m_LogButton = new wxButton(this, wxID_ANY, _(">"), pos, wxDefaultSize,
                               wxBU_TOP | wxBU_EXACTFIT | wxFULL_REPAINT_ON_RESIZE | wxBORDER_NONE);
    m_LogButton->SetToolTip(
        _("'>' starts data export. Create a new, or append "
          "to an existing file,\n'X' stops data export") );
    m_LogButton->Connect(
        wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(
            DashboardInstrument_BaroHistory::OnLogDataButtonPressed),
        NULL,
        this);
    if (LoadConfig() == false) {
        m_exportInterval = 10;
        SaveConfig();
    }
    m_pExportmenu = new wxMenu();
    // this is a dummy menu required by Windows as parent to item created
    //wxMenuItem *pmi = new wxMenuItem(m_pExportmenu, -1, _T("Data Export"));
    btn10Sec = m_pExportmenu->AppendRadioItem(myID_BH_EXPORTRATE_10, _("Export rate 10 Seconds"));
    btn20Sec = m_pExportmenu->AppendRadioItem(myID_BH_EXPORTRATE_20, _("Export rate 20 Seconds"));
    btn60Sec = m_pExportmenu->AppendRadioItem(myID_BH_EXPORTRATE_60, _("Export rate 60 Seconds"));
    
    if (m_exportInterval == 10) btn10Sec->Check(true);
    if (m_exportInterval == 20) btn20Sec->Check(true);
    if (m_exportInterval == 60) btn60Sec->Check(true);
}
DashboardInstrument_BaroHistory::~DashboardInstrument_BaroHistory(void) {
    this->m_BaroHistUpdTimer->Stop();
    delete this->m_BaroHistUpdTimer;
    if (m_isExporting)
        m_ostreamlogfile->Close();
    delete this->m_ostreamlogfile;
    m_LogButton->Disconnect(
        wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(DashboardInstrument_BaroHistory::OnLogDataButtonPressed),
        NULL,
        this);
}

wxSize DashboardInstrument_BaroHistory::GetSize( int orient, wxSize hint )
{
    wxClientDC dc(this);
    int w;
    dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
    if( orient == wxHORIZONTAL ) {
        return wxSize( DefaultWidth, wxMax(m_TitleHeight+140, hint.y) );
    }
    else {
        return wxSize( wxMax(hint.x, DefaultWidth), wxMax(m_TitleHeight+140, hint.y) );
    }
}
void DashboardInstrument_BaroHistory::SetData(
    unsigned long long st,
    double data, wxString unit
    , long long timestamp
    )
{
    if (st == OCPN_DBP_STC_MDA) {
        m_Press = data;
        if ( (m_PressRecCnt >= 0) && (m_PressRecCnt < BARO_START_AVG_CNT) ) {
            m_PressStartVal[m_PressRecCnt] = m_Press;
            m_PressRecCnt++;
        } // then wait for having enough data for averaging
        else {
            m_LastReceivedTime = wxDateTime::UNow().GetTm();
        }
    }
}

// once every 5 seconds tick to collect pressure data received by SetData()
void DashboardInstrument_BaroHistory::OnBaroHistUpdTimer(wxTimerEvent &event)
{
    if ( m_BaroHistUpdTimer->GetInterval() == 1000 ) {
        
        if ( wxDateTime::Now().GetSecond() % 10 != 0) {
            return;
        } // then, no sync possible in ExportData()
        else {
            m_BaroHistUpdTimer->Stop();
            m_BaroHistUpdTimer->Start(5000, wxTIMER_CONTINUOUS);
        } // else can make ExportData() to sync from now on slow down the tick to 1/2 of min.recording
    }  // then we're still looking for divable by 10 time spot to start the actual 5s tick
    wxDateTime timeNow = wxDateTime::Now().GetTm();
    m_LastReceivedTime = wxDateTime::Now().GetTm();

    //start working after we collected 5 records each, as start values for the smoothed curves
    if ( m_PressRecCnt >= BARO_START_AVG_CNT) {
        int lowest = INT_MAX;
        int highest = 0;
        int sum = 0;
        for ( int i = 0; i < BARO_START_AVG_CNT; i++ ) {
            sum += m_PressStartVal[i];
            if ( m_PressStartVal[i] < lowest )
                lowest = m_PressStartVal[i];
            if ( m_PressStartVal[i] > highest )
                highest = m_PressStartVal[i];
        } // make weighted averaging
        m_LastReceivedPressure = (sum - lowest - highest) / ( BARO_START_AVG_CNT - 2);
        m_IsRunning = true;
        m_PressRecCnt = -1; // make sure that we don't come back here here
    } // then can start by averaging the start point

    if ( !m_IsRunning )
        return;

    if (m_Press > 0.0 ) {

        m_LastReceivedPressure = m_Press;
        m_Press = 0.0; // avoid that, with slow talking devices, we keep coming here with obsolete data
        
        m_SampleCount = m_SampleCount < BARO_RECORD_COUNT ? m_SampleCount + 1 : BARO_RECORD_COUNT;
        m_MaxPress = 0;
        //data shifting
        for (int idx = 1; idx < BARO_RECORD_COUNT; idx++) {
            if (BARO_RECORD_COUNT - m_SampleCount <= idx)
                m_MaxPress = wxMax(m_ArrayPressHistory[idx - 1], m_MaxPress);
            m_MinPress = wxMin(m_ArrayPressHistory[idx - 1], m_MinPress);
            m_ArrayPressHistory[idx - 1] = m_ArrayPressHistory[idx];
            m_ArrayRecTime[idx - 1] = m_ArrayRecTime[idx];
        }
        m_ArrayPressHistory[BARO_RECORD_COUNT - 1] = m_LastReceivedPressure;
        if (m_SampleCount < 2) {
            m_ArrayPressHistory[BARO_RECORD_COUNT - 2] = m_LastReceivedPressure;

        }
        m_ArrayRecTime[BARO_RECORD_COUNT - 1] = m_LastReceivedTime;
        m_MaxPress = wxMax(m_LastReceivedPressure, m_MaxPress);

        m_MinPress = wxMin(m_MinPress, m_LastReceivedPressure);
        if (wxMin(m_LastReceivedPressure, m_MinPress) == -1) {
            m_MinPress = wxMin(m_LastReceivedPressure, 1200); // to make a OK inital value
        }
        //get the overall max min pressure
        m_TotalMaxPress = wxMax(m_LastReceivedPressure, m_TotalMaxPress);
        m_TotalMinPress = wxMin(m_LastReceivedPressure, m_TotalMinPress);

        if ( timeNow.GetSecond() % m_exportInterval == 0 )
            ExportData();

    } // then pressure > 0
}

void DashboardInstrument_BaroHistory::Draw(wxGCDC* dc)
{
   m_WindowRect = GetClientRect();
   m_DrawAreaRect=GetClientRect();
   m_DrawAreaRect.SetHeight(m_WindowRect.height-m_TopLineHeight-m_TitleHeight);
   m_DrawAreaRect.SetX (m_LeftLegend+3);
   DrawBackground(dc);
   DrawForeground(dc);
}



//*********************************************************************************
// draw pressure scale
//*********************************************************************************
void  DashboardInstrument_BaroHistory::DrawPressureScale(wxGCDC* dc)
{
  wxString label1,label2,label3,label4,label5;
  wxColour cl;
  int width, height;
  cl=wxColour(61,61,204,255);
  dc->SetTextForeground(cl);
  dc->SetFont(*g_pFontSmall);
  //round m_MaxPress up to the next hpa ...
  if (m_MaxPress > 1100)
  m_MaxPress=1100;

  if (m_TotalMinPress < 930)
  m_TotalMinPress=930;


  m_MaxPressScale= (int)((m_MaxPress+15)-(m_TotalMinPress-15));

  if(!m_IsRunning) {
    label1=_T("-- hPa");
    label2=_T("-- hPa");
    label3=_T("-- hPa");
    label4=_T("-- hPa");
    label5=_T("-- hPa");
  }
  else {
/*
 The goal is to draw the legend with decimals only, if we really have them !
*/
    // top legend for max press
    label1.Printf(_T("%.0f hPa"), m_MaxPressScale +(m_TotalMinPress-18)  );

    // 3/4 legend

      label2.Printf(_T("%.0f hPa"), m_MaxPressScale *3./4 + (m_TotalMinPress-18)  );

    // center legend

      label3.Printf(_T("%.0f hPa"), m_MaxPressScale /2 +(m_TotalMinPress-18));

    // 1/4 legend

      label4.Printf(_T("%.0f hPa"), m_MaxPressScale /4 +(m_TotalMinPress-18)  );

    //bottom legend for min pressure
    label5.Printf(_T("%.0f hPa"), (m_TotalMinPress-18));
  }
  dc->GetTextExtent(label1, &m_LeftLegend, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label1, 4, (int)(m_TopLineHeight + 7 - height/2));
  dc->GetTextExtent(label2, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label2, 4, (int)(m_TopLineHeight+m_DrawAreaRect.height/4-height/2));
  m_LeftLegend = wxMax(width,m_LeftLegend);
  dc->GetTextExtent(label3, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label3, 4, (int)(m_TopLineHeight+m_DrawAreaRect.height/2-height/2));
  m_LeftLegend = wxMax(width,m_LeftLegend);
  dc->GetTextExtent(label4, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label4, 4, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.75-height/2));
  m_LeftLegend = wxMax(width,m_LeftLegend);
  dc->GetTextExtent(label5, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label5, 4,  (int)(m_TopLineHeight+m_DrawAreaRect.height-height/2));
  m_LeftLegend = wxMax(width,m_LeftLegend);
  m_LeftLegend+=4;
}

//*********************************************************************************
//draw background
//*********************************************************************************
void DashboardInstrument_BaroHistory::DrawBackground(wxGCDC* dc)
{
  wxString label,label1,label2,label3,label4,label5;
  wxColour cl;
  wxPen pen;
  //---------------------------------------------------------------------------------
  // draw legend for pressure
  //---------------------------------------------------------------------------------

  DrawPressureScale(dc);

  //---------------------------------------------------------------------------------
  // horizontal lines
  //---------------------------------------------------------------------------------
  GetGlobalColor(_T("UBLCK"), &cl);
  pen.SetColour(cl);
  dc->SetPen(pen);
  dc->DrawLine(m_LeftLegend+3, m_TopLineHeight, m_WindowRect.width-3-m_RightLegend, m_TopLineHeight); // the upper line
  dc->DrawLine(m_LeftLegend+3, (int)(m_TopLineHeight+m_DrawAreaRect.height), m_WindowRect.width-3-m_RightLegend, (int)(m_TopLineHeight+m_DrawAreaRect.height));
  pen.SetStyle(wxPENSTYLE_DOT);
  dc->SetPen(pen);
  dc->DrawLine(m_LeftLegend+3, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.25), m_WindowRect.width-3-m_RightLegend, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.25));
  dc->DrawLine(m_LeftLegend+3, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.75), m_WindowRect.width-3-m_RightLegend, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.75));
#ifdef __WXMSW__  
  pen.SetStyle(wxPENSTYLE_SHORT_DASH);
  dc->SetPen(pen);
#endif  
  dc->DrawLine(m_LeftLegend+3, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.5), m_WindowRect.width-3-m_RightLegend, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.5));
}


//*********************************************************************************
//draw foreground
//*********************************************************************************
void DashboardInstrument_BaroHistory::DrawForeground(wxGCDC* dc)
{
    wxColour col;
    double ratioH;
    int degw,degh;
    int width,height,min,hour;
    wxString BaroPressure;
    wxPen pen;
    wxString label;


    //---------------------------------------------------------------------------------
    // Pressure
    //---------------------------------------------------------------------------------
    col=wxColour(61,61,204,255); //blue, opaque
    dc->SetFont(*g_pFontData);
    dc->SetTextForeground(col);
    BaroPressure=wxString::Format(_T("hPa %4.1f  "),
                                  (m_LastReceivedPressure>0.0?m_LastReceivedPressure:m_Press));
    dc->GetTextExtent(BaroPressure, &degw, &degh, 0, 0, g_pFontData);
    dc->DrawText(BaroPressure, m_LeftLegend+3, m_TopLineHeight-degh);
    dc->SetFont(*g_pFontLabel);
    //determine the time range of the available data (=oldest data value)
    int i=0;
    while( (i < (BARO_RECORD_COUNT - 1)) && (m_ArrayRecTime[i].year == 999) ) i++;
    if ( i == (BARO_RECORD_COUNT - 1) ) {  min=0;
        hour=0;
    }
    else {
        min=m_ArrayRecTime[i].min;
        hour=m_ArrayRecTime[i].hour;
    }
    // Single text var to facilitate correct translations:
    wxString s_Max = _("Max");
    wxString s_Since = _("since");
    wxString s_OMax = _("Overall Max");
    wxString s_Min = _("Min");
    dc->DrawText(wxString::Format(_T(" %s %.1f %s %02d:%02d  %s %.1f %s %.1f "), s_Max, m_MaxPress, s_Since, hour, min, s_OMax,
                                  m_TotalMaxPress, s_Min, m_TotalMinPress), m_LeftLegend + 3 + 2 + degw, m_TopLineHeight - degh + 2);
    pen.SetStyle(wxPENSTYLE_SOLID);
    pen.SetColour(wxColour(61,61,204,255)); //blue, opaque
    pen.SetWidth(1);

    ratioH = (double)m_DrawAreaRect.height / (double)m_MaxPressScale ;
    m_DrawAreaRect.SetWidth(m_WindowRect.width - 6 - m_LeftLegend - m_RightLegend);
    m_ratioW = double(m_DrawAreaRect.width) / (BARO_RECORD_COUNT-1);
    
    wxPoint  pointPressure[BARO_RECORD_COUNT+2],pointPressure_old;
    pointPressure_old.x=m_LeftLegend+3;
    pointPressure_old.y = m_TopLineHeight+m_DrawAreaRect.height - (m_ArrayPressHistory[0] - (double)m_TotalMinPress + 18) * ratioH;

    //---------------------------------------------------------------------------------
    // live pressure data
    //---------------------------------------------------------------------------------
    if ( m_IsRunning) {
        for (int idx = 1; idx < BARO_RECORD_COUNT; idx++) {
            pointPressure[idx].y = m_TopLineHeight+m_DrawAreaRect.height - ((m_ArrayPressHistory[idx]-(double)m_TotalMinPress+18) * ratioH);
            pointPressure[idx].x = m_LeftLegend + 3 + idx * m_ratioW;
            if(BARO_RECORD_COUNT-m_SampleCount <= idx && pointPressure[idx].y > m_TopLineHeight && pointPressure_old.y > m_TopLineHeight &&
               pointPressure[idx].y <=m_TopLineHeight+m_DrawAreaRect.height && pointPressure_old.y<=m_TopLineHeight+m_DrawAreaRect.height)
                dc->DrawLine( pointPressure_old.x, pointPressure_old.y, pointPressure[idx].x,pointPressure[idx].y );
            pointPressure_old.x=pointPressure[idx].x;
            pointPressure_old.y=pointPressure[idx].y;
        } // for number of point
    } // then there is some data, no need to draw while we're collecting in the beginning slow data

    //---------------------------------------------------------------------------------
    //draw vertical timelines every 15 minutes
    //---------------------------------------------------------------------------------
    GetGlobalColor(_T("UBLCK"), &col);
    pen.SetColour(col);
    pen.SetStyle(wxPENSTYLE_DOT);
    dc->SetPen(pen);
    dc->SetTextForeground(col);
    dc->SetFont(*g_pFontSmall);
    int done=-1;
    wxPoint pointTime;
    int prevfiverfit = -15;
    for (int idx = 0; idx < BARO_RECORD_COUNT; idx++) {
        if (m_ArrayRecTime[idx].year != 999) {
            wxDateTime localTime( m_ArrayRecTime[idx] );
            min      = localTime.GetMinute( );
            hour     = localTime.GetHour( );
            if ( (hour*100+min) != done && (min % 15 == 0 ) ) {
                if ( min != prevfiverfit ) {
                    pointTime.x = idx * m_ratioW + 3 + m_LeftLegend;
                    dc->DrawLine( pointTime.x, m_TopLineHeight+1,
                                  pointTime.x,(
                                      m_TopLineHeight +
                                      m_DrawAreaRect.height +1 ) );
                    label.Printf(_T("%02d:%02d"), hour,min);
                    dc->GetTextExtent(label, &width, &height, 0, 0,
                                      g_pFontSmall);
                    dc->DrawText(label, pointTime.x-width / 2,
                                 m_WindowRect.height-height);
                    done=hour*100+min;
                    prevfiverfit = min;
                } // then avoid double printing in faster devices
            }
        }
    }
}

void DashboardInstrument_BaroHistory::OnLogDataButtonPressed(
    wxCommandEvent& event)
{

    if (m_isExporting == false) {
        wxPoint pos;
        m_LogButton->GetSize(&pos.x, &pos.y);
        pos.x = 0;
        this->PopupMenu(m_pExportmenu, pos);
        if (btn10Sec->IsChecked()) m_exportInterval = 10;
        if (btn20Sec->IsChecked()) m_exportInterval = 20;
        if (btn60Sec->IsChecked()) m_exportInterval = 60;

        wxFileDialog fdlg(GetOCPNCanvasWindow(), _("Choose a new or existing file"), wxT(""), m_logfile, wxT("*.*"), wxFD_SAVE);
        if (fdlg.ShowModal() != wxID_OK) {
            return;
        }
        m_logfile.Clear();
        m_logfile = fdlg.GetPath();
        bool exists = m_ostreamlogfile->Exists(m_logfile);
        m_ostreamlogfile->Open(m_logfile, wxFile::write_append);
        if (!exists) {
            wxString str_ticks =
                g_bDataExportClockticks ? wxString::Format(
                    _("   ClockTicks%s"), g_sDataExportSeparator) :
                _T("");
            wxString str_utc =
                g_bDataExportUTC ? wxString::Format(
                    _("         UTC-ISO8601%s"), g_sDataExportSeparator) :
                _T("");
            wxString str = wxString::Format(
                _T(
                    "%s"    // ticks
                    "%s"    // utc
                    "%s%s"  // local date, separator
                    "%s%s"  // local time, separator
                    "%s\n"),// pressure
                str_ticks,
                str_utc,
                "      Date", g_sDataExportSeparator,
                " Local Time", g_sDataExportSeparator,
                "Press.");
            m_ostreamlogfile->Write(str);
        }
        SaveConfig(); //save the new export-rate &filename to opencpn.ini
        m_isExporting = true; // note: this allows the ExportData to write at the next DAQ tick
        m_LogButton->SetLabel(_("X"));
        m_LogButton->Refresh();
    }
    else if (m_isExporting == true) {
        m_isExporting = false;
        m_ostreamlogfile->Close();
        m_LogButton->SetLabel(_(">"));
        m_LogButton->Refresh();
    }
}
/***************************************************************************************
****************************************************************************************/
bool DashboardInstrument_BaroHistory::LoadConfig(void)
{
    wxFileConfig *pConf = m_pconfig;

    if (pConf) {
        pConf->SetPath(_T("/PlugIns/DashT/Tactics/BaroHistory"));
        pConf->Read(_T("Exportrate"), &m_exportInterval, 60);
        pConf->Read(_T("BaroHistoryExportfile"), &m_logfile, wxEmptyString);
        return true;
    }
    else
        return false;
}
/***************************************************************************************
****************************************************************************************/
bool DashboardInstrument_BaroHistory::SaveConfig(void)
{
    wxFileConfig *pConf = m_pconfig;

    if (pConf)
    {
        pConf->SetPath(_T("/PlugIns/DashT/Tactics/BaroHistory"));
        pConf->Write(_T("Exportrate"), m_exportInterval);
        pConf->Write(_T("BaroHistoryExportfile"), m_logfile);
        return true;
    }
    else
        return false;
}
void DashboardInstrument_BaroHistory::ExportData()
{
    if ( !m_IsRunning || !m_isExporting )
        return;

    wxDateTime localTime(m_ArrayRecTime[BARO_RECORD_COUNT - 1]);
    wxString str_utc, ticks, sBuffer, ticksString;
    if (g_bDataExportUTC) {
        wxDateTime utc = localTime.ToUTC();
        str_utc = wxString::Format(_T("%sZ%s"), utc.FormatISOCombined('T'),
                                   g_sDataExportSeparator);
    }
    else
        str_utc = _T("");
    if (g_bDataExportClockticks) {
        wxLongLong ti = wxGetUTCTimeMillis();
        sBuffer = ti.ToString();
        ticksString = sBuffer.wc_str();
        ticks = wxString::Format(_T("%s%s"), ticksString,
                                 g_sDataExportSeparator);
    }
    else
        ticks = _T("");
    wxString str = wxString::Format(
        _T(
            "%s"       // ticks
            "%s"       // utc
            "%s%s"     // local date, separator
            "%11s%s"   // local time, separator
            "%6.1f\n" // barometric pressure [hPa]
            ),
        ticks,
        str_utc,
        localTime.FormatDate(), g_sDataExportSeparator,
        localTime.FormatTime(), g_sDataExportSeparator,
        m_LastReceivedPressure);
    m_ostreamlogfile->Write(str);
}
