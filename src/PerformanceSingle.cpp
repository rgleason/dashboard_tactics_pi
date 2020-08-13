/**************************************************************************
* $Id: PerformanceSingle.cpp, v1.0 2016/06/07 tom_BigSpeedy Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Thomas Rauch
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
#include <map>
#include <cmath>
using namespace std;

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/math.h>
#include <wx/stdpaths.h>
#include <wx/progdlg.h>
#include <wx/gdicmn.h>
#include <wx/fileconf.h>

#include "dashboard_pi.h"

#include "nmea0183/nmea0183.h"

#include "TacticsEnums.h"
#include "TacticsFunctions.h"
#include "PerformanceSingle.h"

#include "DashboardWindow.h"

#include "dashboard_pi_ext.h"
#include "tactics_pi_ext.h"

// ----------------------------------------------------------------
//
//    TacticsInstrument_Simple Implementation
//
//----------------------------------------------------------------
TacticsInstrument_PerformanceSingle::TacticsInstrument_PerformanceSingle(DashboardWindow *pparent, wxWindowID id, wxString title, unsigned long long cap_flag, wxString format)
	: DashboardInstrument( (wxWindow *) pparent, id, title, cap_flag )
{
    m_pparent = pparent;
    mTWS = std::nan("1");
    mTWA = std::nan("1");
    mSTW = std::nan("1");
    mCMG = std::nan("1");
    mSOG = std::nan("1");
    mCOG = std::nan("1");
    mBRG = -1;
    mHDT = std::nan("1");
    mTWD = std::nan("1");
    m_lat = std::nan("1");
    m_lon = std::nan("1");
    stwunit = _T("");
    m_displaytype = 0;
    m_data = _T("---");
    m_format = format;
    m_DataHeight = 0;
    m_pconfig = GetOCPNConfigObject();
    (void) LoadConfig();
}
/***********************************************************************************

************************************************************************************/
wxSize TacticsInstrument_PerformanceSingle::GetSize(int orient, wxSize hint)
{
	wxClientDC dc(this);
	int w;
	dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
	dc.GetTextExtent(_T("000"), &w, &m_DataHeight, 0, 0, g_pFontData);

	if (orient == wxHORIZONTAL) {
		return wxSize(DefaultWidth, wxMax(hint.y, m_TitleHeight + m_DataHeight));
	}
	else {
		return wxSize(wxMax(hint.x, DefaultWidth), m_TitleHeight + m_DataHeight);
	}
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_PerformanceSingle::Draw(wxGCDC* dc)
{
	wxColour cl;
#ifdef __WXMSW__
	wxBitmap tbm(dc->GetSize().x, m_DataHeight, -1);
	wxMemoryDC tdc(tbm);
	wxColour c2;
	GetGlobalColor( g_sDialColorBackground, &c2 );
	tdc.SetBackground(c2);
	tdc.Clear();

	tdc.SetFont(*g_pFontData);
	GetGlobalColor( g_sDialColorForeground, &cl) ;
	tdc.SetTextForeground(cl);

	tdc.DrawText(m_data, 10, 0);

	tdc.SelectObject(wxNullBitmap);

	dc->DrawBitmap(tbm, 0, m_TitleHeight, false);
#else
	dc->SetFont(*g_pFontData);
	GetGlobalColor( g_sDialColorForeground, &cl );
	dc->SetTextForeground(cl);

	dc->DrawText(m_data, 10, m_TitleHeight);

#endif

}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_PerformanceSingle::SetDisplayType(int type){
	m_displaytype = type;
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_PerformanceSingle::SetData(
    unsigned long long st, double data, wxString unit, long long timestamp)
{

    if ( timestamp != 0LL )
        setTimestamp( timestamp );

    if (st == OCPN_DBP_STC_STW){
        if (std::isnan(data))
            mSTW = std::nan("1");
        else
            //convert to knots first
            mSTW = fromUsrSpeed_Plugin(data, g_iDashSpeedUnit);
        stwunit = unit;
    }
    else if (st == OCPN_DBP_STC_TWA){
        mTWA = data;
    }
    else if (st == OCPN_DBP_STC_COG){
        mCOG = data;
    }
    else if (st == OCPN_DBP_STC_SOG){
        if (std::isnan(data))
            mSOG = std::nan("1");
        else
            //convert to knots first
            mSOG = fromUsrSpeed_Plugin(data, g_iDashSpeedUnit);
    }
    else if (st == OCPN_DBP_STC_LAT) {
        m_lat = data;
    }
    else if (st == OCPN_DBP_STC_LON) {
        m_lon = data;
    }
    else if (st == OCPN_DBP_STC_BRG){
        mBRG = data;
    }
    else if (st == OCPN_DBP_STC_TWS){
        if (std::isnan(data))
            mTWS = std::nan("1");
        else
            //convert to knots
            mTWS = fromUsrSpeed_Plugin(data, g_iDashWindSpeedUnit);
    }
    else if (st == OCPN_DBP_STC_HDT){
        mHDT = data;
    }
    else if (st == OCPN_DBP_STC_TWD){
        mTWD = data;
    }
    if (!GetSingleWaypoint(g_sMarkGUID, m_pMark))
        m_pMark = NULL;
    if (m_pMark && m_lat > 0 && m_lon > 0) {
        double dist;
        DistanceBearingMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, m_lat, m_lon, &mBRG, &dist);
    }
    if (!std::isnan(mSTW) && !std::isnan(mTWA) && !std::isnan(mTWS)){
    
        if (m_displaytype == POLARSPEED){
            if ( !BoatPolar->isValid() ) {
                m_data = _T("no polar data");
            }
            else {
                double targetspeed = BoatPolar->GetPolarSpeed(mTWA, mTWS);
                //double avgtargetspeed = BoatPolar->GetAvgPolarSpeed(mTWA, mTWS);
      
                if (std::isnan(targetspeed) || mSTW == 0) {
                    m_data = _T("no polar targetspeed");
                }
                else {
                    double percent = mSTW / targetspeed * 100;
                    double user_targetSpeed = toUsrSpeed_Plugin(targetspeed, g_iDashSpeedUnit);
                    m_data = wxString::Format("%d", wxRound(percent)) + _T(" % / ") + wxString::Format("%.2f ", user_targetSpeed) + stwunit;
                    wxLongLong wxllNowMs = wxGetUTCTimeMillis();
                    m_pparent->SendPerfSentenceToAllInstruments(
                        OCPN_DBP_STC_POLPERF,
                        percent,
                        _T("%"),
                        wxllNowMs.GetValue() );
                    m_pparent->SendPerfSentenceToAllInstruments(
                        OCPN_DBP_STC_POLSPD,
                        user_targetSpeed,
                        stwunit,
                        wxllNowMs.GetValue() );
                }
            }
        }
        else if (m_displaytype == POLARVMG){
            if ( !BoatPolar->isValid() ) {
                m_data = _T("no polar data");
            }
            else {
                double VMG = BoatPolar->Calc_VMG(mTWA,mSTW);
                double user_VMG = toUsrSpeed_Plugin(VMG, g_iDashSpeedUnit);
                m_data = wxString::Format("%.2f", user_VMG) + _T(" ") + stwunit;
                wxLongLong wxllNowMs = wxGetUTCTimeMillis();
                m_pparent->SendPerfSentenceToAllInstruments(
                    OCPN_DBP_STC_POLVMG,
                    user_VMG,
                    stwunit,
                    wxllNowMs.GetValue() );
            }
      
        }
        else if (m_displaytype == POLARTARGETVMG){
            if ( !BoatPolar->isValid() ) {
                m_data = _T("no polar data");
            }
            else {
                TargetxMG targetVMG = BoatPolar->Calc_TargetVMG(mTWA, mTWS);
                if (targetVMG.TargetSpeed > 0) {
                    double VMG = BoatPolar->Calc_VMG(mTWA, mSTW);
                    double percent = fabs(VMG / targetVMG.TargetSpeed * 100.);
                    targetVMG.TargetSpeed = toUsrSpeed_Plugin(targetVMG.TargetSpeed, g_iDashSpeedUnit);
                    m_data = wxString::Format("%d", wxRound(percent)) + _T(" % / ") +
                        wxString::Format("%.2f", targetVMG.TargetSpeed) + _T(" ") + stwunit;
                    wxLongLong wxllNowMs = wxGetUTCTimeMillis();
                    m_pparent->SendPerfSentenceToAllInstruments(
                        OCPN_DBP_STC_POLTVMG,
                        targetVMG.TargetSpeed,
                        stwunit,
                        wxllNowMs.GetValue() );
                }
                else
                    m_data =  _T("--- % / --- ") + stwunit;
            }
        }
        else if (m_displaytype == POLARTARGETVMGANGLE){
            if ( !BoatPolar->isValid() ) {
                m_data = _T("no polar data");
            }
            else {
                TargetxMG targetVMG = BoatPolar->Calc_TargetVMG(mTWA, mTWS);
                if (!std::isnan(targetVMG.TargetAngle)) {
                    m_data = wxString::Format("%.0f", targetVMG.TargetAngle) + _T("\u00B0");
                    wxLongLong wxllNowMs = wxGetUTCTimeMillis();
                    m_pparent->SendPerfSentenceToAllInstruments(
                        OCPN_DBP_STC_POLTVMGANGLE,
                        targetVMG.TargetAngle,
                        _T("\u00B0"),
                        wxllNowMs.GetValue() );
                }
                else {
                    m_data = _T("no polar data");
                }
            }
        }
    }
    else {
        m_data = _T("---");
    }
  
    if (m_displaytype == POLARCMG){
        if ( !BoatPolar->isValid() ) {
            m_data = _T("no polar data");
        }
        else {
            if (!std::isnan(mSOG) && !std::isnan(mCOG) && mBRG>=0) {
                mCMG = BoatPolar->Calc_CMG(mCOG, mSOG, mBRG);
                double user_CMG = toUsrSpeed_Plugin(mCMG, g_iDashSpeedUnit);
                m_data = wxString::Format("%.2f", user_CMG) + _T(" ") + stwunit;
                wxLongLong wxllNowMs = wxGetUTCTimeMillis();
                m_pparent->SendPerfSentenceToAllInstruments(
                    OCPN_DBP_STC_POLTCMG,
                    user_CMG,
                    stwunit,
                    wxllNowMs.GetValue() );
            }
            else {
                m_data = _T("no bearing");
            }
        }
    }
    else if (m_displaytype == POLARTARGETCMG){
        if ( !BoatPolar->isValid() ) {
            m_data = _T("no polar data");
        }
        else {
            //TargetxMG targetCMG = BoatPolar->Calc_TargetCMG(mTWS, mTWD, mBRG);
            TargetxMG TCMGMax, TCMGMin;
            TCMGMax.TargetSpeed = std::nan("1");
            if (!std::isnan(mTWS) && !std::isnan(mTWD) && mBRG>=0)
                BoatPolar->Calc_TargetCMG2 (mTWS, mTWD, mBRG, &TCMGMax, &TCMGMin);
            //if (!std::isnan(targetCMG.TargetSpeed) && targetCMG.TargetSpeed > 0) {
            if (!std::isnan(TCMGMax.TargetSpeed) && TCMGMax.TargetSpeed > 0 && !std::isnan(mHDT) && !std::isnan(mSTW)) {
                double cmg = BoatPolar->Calc_CMG(mHDT, mSTW, mBRG);
                if (!std::isnan(cmg) )//&& cmg >=0)
                {
                    //double percent = fabs(cmg / targetCMG.TargetSpeed * 100.);
                    double percent = cmg / TCMGMax.TargetSpeed * 100.;
                    TCMGMax.TargetSpeed = toUsrSpeed_Plugin(TCMGMax.TargetSpeed, g_iDashSpeedUnit);
                    m_data = wxString::Format("%d", wxRound(percent)) + _T(" % / ") +
                        wxString::Format("%.2f", TCMGMax.TargetSpeed) + _T(" ") + stwunit;
                    wxLongLong wxllNowMs = wxGetUTCTimeMillis();
                    m_pparent->SendPerfSentenceToAllInstruments(
                        OCPN_DBP_STC_POLTCMG,
                        TCMGMax.TargetSpeed,
                        stwunit,
                        wxllNowMs.GetValue() );
                }
                else {
                    m_data = _T("--- % / --- ") + stwunit;
                }
            }
            else {
                m_data = _T("--- % / --- ") + stwunit;
            }
        }
    }
    else if (m_displaytype == POLARTARGETCMGANGLE){
        if ( !BoatPolar->isValid() ) {
            m_data = _T("no polar data");
        }
        else {
            if (!std::isnan(mSTW) && mBRG >= 0 && !std::isnan(mHDT)) {
                TargetxMG TCMGMax, TCMGMin;
                TCMGMax.TargetAngle = std::nan("1");
                if (!std::isnan(mTWS) && !std::isnan(mTWD) && mBRG >= 0) {
                    BoatPolar->Calc_TargetCMG2(mTWS, mTWD, mBRG, &TCMGMax, &TCMGMin);
                }
                if (!std::isnan(TCMGMax.TargetAngle)) {
                    m_data = wxString::Format("%.0f", TCMGMax.TargetAngle) + _T("\u00B0");
                    wxLongLong wxllNowMs = wxGetUTCTimeMillis();
                    m_pparent->SendPerfSentenceToAllInstruments(
                        OCPN_DBP_STC_POLTCMGANGLE,
                        TCMGMax.TargetAngle,
                        _T("\u00B0"),
                        wxllNowMs.GetValue() );
                }
                else {
                    m_data = _T("no polar data");
                }
            }
            else {
                m_data = _T("no data");
            }
        }
    }
    else if (m_displaytype == TWAMARK) {
        if ( !AverageWind ) {
            wxLogMessage (
                "dashboard_tactics_pi: TacticsInstrument_PerformanceSingle: "
                "No AverageWind service available. Cannot start instr. TWAMARK.");
            return;
        } // then sanity check for out-of tactics_pi usage
        bool useShortAvg = m_twamarkUseShortAvg;
        if (  useShortAvg && ( AverageWind->GetShortAvgTime() == 0 ) )
            useShortAvg = false; // then defined but not available
        double avWnd = ( useShortAvg ?
                         AverageWind->GetShortAvgWindDir() :
                         AverageWind->GetAvgWindDir() );
        while (avWnd > 360)
            avWnd -= 360;
        while (avWnd < 0)
            avWnd += 360;
        double port = avWnd +
            ( useShortAvg ?
              AverageWind->GetShortDegRangePort() :
              AverageWind->GetDegRangePort() );
        while (port > 360)
            port -= 360;
        while (port < 0)
            port += 360;
        double stb = avWnd +
             ( useShortAvg ?
               AverageWind->GetShortDegRangeStb() :
               AverageWind->GetDegRangeStb() );
        while (stb > 360)
            stb -= 360;
        while (stb < 0)
            stb += 360;
        if (mBRG >= 0 && !std::isnan(mTWD) && !std::isnan(avWnd) ) {
            // do the rounding inside the function to keep it
            // in sync with the AvgWind instrument ...
            double AvgMarkBrG = getDegRange( mBRG, wxRound( avWnd ));
            double leftMarkBrG = getDegRange( mBRG, wxRound( port ) );
            double rightMarkBrG = getDegRange( mBRG, wxRound( stb ) );
                
            if (leftMarkBrG > rightMarkBrG) {
                double tmp = leftMarkBrG;
                leftMarkBrG = rightMarkBrG;
                rightMarkBrG = tmp;
            }
            m_data = wxString::Format("%.0f", (double)leftMarkBrG) + _T("\u00B0") +
                wxString::Format(" - %.0f", (double)AvgMarkBrG) + _T("\u00B0") +
                wxString::Format(" - %.0f", (double)rightMarkBrG) + _T("\u00B0");

            wxLongLong wxllNowMs = wxGetUTCTimeMillis();
            m_pparent->SendPerfSentenceToAllInstruments(
                OCPN_DBP_STC_TWAMARK,
                AvgMarkBrG,
                _T("\u00B0"),
                wxllNowMs.GetValue() );
        }
        else {
                m_data = _T("---");
        }
    }
}

void TacticsInstrument_PerformanceSingle::timeoutEvent()
{
    m_data = _T("---");
}

bool TacticsInstrument_PerformanceSingle::LoadConfig()
{
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return false;

    pConf->SetPath( _T("/PlugIns/DashT/Tactics/Performance/") );
    pConf->Read( _T("TwaMarkUseShortAvgWind"), &m_twamarkUseShortAvg, true );
    pConf->Write( _T("TwaMarkUseShortAvgWind"), m_twamarkUseShortAvg );

    return true;
}
