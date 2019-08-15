/**************************************************************************
* $Id: streamout.cpp, v1.0 2019/08/08 DarthVader Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Petri Makijarvi
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
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
using namespace std;
#include <mutex>

// #include <wx/dir.h>
// #include <wx/filefn.h>
// #include <wx/textfile.h>
// #include <wx/tokenzr.h>
#include <wx/wfstream.h> 
// #include <wx/txtstrm.h> 
// #include <wx/math.h>
// #include <wx/stdpaths.h>
// #include <wx/progdlg.h>
// #include <wx/gdicmn.h>
#include <wx/fileconf.h>
// #include "nmea0183/nmea0183.h"

#include "streamout.h"
// #include <map>
// #include <cmath>
#include "ocpn_plugin.h"
#include "wx/jsonreader.h"

extern wxString g_path_to_PolarFile;
extern int g_iDashWindSpeedUnit;
extern int g_iDashSpeedUnit;
extern int g_iSpeedFormat;

// ----------------------------------------------------------------
//
//    TacticsInstrument_StreamoutSingle
//
//----------------------------------------------------------------
TacticsInstrument_StreamoutSingle::TacticsInstrument_StreamoutSingle(
    wxWindow *pparent, wxWindowID id, wxString title, unsigned long long cap_flag, wxString format,
    std::mutex &mtxNofStreamOut, int &nofStreamOut, wxString &echoStreamerShow, wxString confdir)
	:DashboardInstrument(pparent, id, title, cap_flag)
{
    std::unique_lock<std::mutex> lck_mtxNofStreamOut( mtxNofStreamOut);
    nofStreamOut++;
    m_mtxNofStreamOut = &mtxNofStreamOut;
    m_nofStreamOut = &nofStreamOut;
    m_echoStreamerShow = &echoStreamerShow;
    m_state = SSSM_STATE_UNKNOWN;
    if ( nofStreamOut > 1) {
        m_state = SSSM_STATE_DISPLAYRELAY;
        m_data = echoStreamerShow;
        return;
    }
    m_state = SSSM_STATE_INIT;
    m_data = _T("---");
    echoStreamerShow = m_data;
    m_format = format;
    m_DataHeight = 0;
    m_confdir = confdir;
    m_pconfig = GetOCPNConfigObject();
    m_configFileName = wxEmptyString;
    m_verbosity = 0;

    wxString emptyStr = wxEmptyString;
    emptyStr = emptyStr.wc_str();
    m_apiURL = emptyStr;
    m_apiHdr = emptyStr;
    m_connectionRetry = 0;
    m_timestamps = emptyStr;

    m_configured = LoadConfig();
}
/***********************************************************************************

************************************************************************************/
TacticsInstrument_StreamoutSingle::~TacticsInstrument_StreamoutSingle()
{
    std::unique_lock<std::mutex> lck_mtxNofStreamOut( *m_mtxNofStreamOut);
    SaveConfig();
    *m_nofStreamOut--;
}
/***********************************************************************************

************************************************************************************/
wxSize TacticsInstrument_StreamoutSingle::GetSize(int orient, wxSize hint)
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
void TacticsInstrument_StreamoutSingle::Draw(wxGCDC* dc)
{
	wxColour cl;
#ifdef __WXMSW__
	wxBitmap tbm(dc->GetSize().x, m_DataHeight, -1);
	wxMemoryDC tdc(tbm);
	wxColour c2;
	GetGlobalColor(_T("DASHB"), &c2);
	tdc.SetBackground(c2);
	tdc.Clear();

	tdc.SetFont(*g_pFontData);
	GetGlobalColor(_T("DASHF"), &cl);
	tdc.SetTextForeground(cl);

	tdc.DrawText(m_data, 10, 0);

	tdc.SelectObject(wxNullBitmap);

	dc->DrawBitmap(tbm, 0, m_TitleHeight, false);
#else
	dc->SetFont(*g_pFontData);
	GetGlobalColor(_T("DASHF"), &cl);
	dc->SetTextForeground(cl);

	dc->DrawText(m_data, 10, m_TitleHeight);

#endif

}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamoutSingle::SetData(unsigned long long st, double data, wxString unit)
{
    if (std::isnan(data))
        return;

    if ( m_state == SSSM_STATE_DISPLAYRELAY ) {
        m_data = *m_echoStreamerShow;
        return;
    }

    /*
    
    if (st == OCPN_DBP_STC_STW){
    
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
        //convert to knots first
        //mSOG = data;
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
        //mTWS = data;
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
                    //m_data = wxString::Format("%.2f / ", avgtargetspeed) + wxString::Format("%.2f", user_targetSpeed) + _T(" ") + stwunit;
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
	
                    m_data = wxString::Format("%d", wxRound(percent)) + _T(" % / ") + wxString::Format("%.2f", targetVMG.TargetSpeed) + _T(" ") + stwunit;
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
                if (!std::isnan(targetVMG.TargetAngle))
                    m_data = wxString::Format("%.0f", targetVMG.TargetAngle) + _T("\u00B0");
                else
                    m_data = _T("no polar data");
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
            TCMGMax.TargetSpeed = NAN;
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
                TCMGMax.TargetAngle = NAN;
                if (!std::isnan(mTWS) && !std::isnan(mTWD) && mBRG >= 0) {
                    BoatPolar->Calc_TargetCMG2(mTWS, mTWD, mBRG, &TCMGMax, &TCMGMin);
                }
                if (!std::isnan(TCMGMax.TargetAngle)) {
                    m_data = wxString::Format("%.0f", TCMGMax.TargetAngle) + _T("\u00B0");
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
    else if (m_displaytype == TWAMARK){
        if (mBRG>=0 && !std::isnan(mTWD)) {
            double markBrG = getDegRange(mBRG, mTWD);
            m_data = wxString::Format("%.0f",(double) markBrG) + _T("\u00B0");
        }
        else {
                m_data = _T("---");
        }
    }
    */
 
}
/***********************************************************************************

************************************************************************************/
bool TacticsInstrument_StreamoutSingle::LoadConfig()
{
    if ( *m_nofStreamOut > 1 )
        return true;
    
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return false;
    pConf->SetPath(_T("/PlugIns/Dashboard/Tactics/Streamout/"));
    pConf->Read(_T("ConfigFile"), &m_configFileName, "streamout.json");
    wxString s = wxFileName::GetPathSeparator();
    wxString confPath = m_confdir + m_configFileName;
    if ( !wxFileExists( confPath ) ) {
        wxString tmplPath  = *GetpSharedDataLocation();
        tmplPath += _T("plugins") + s + _T("dashboard_tactics_pi") + s + _T("streamout_template.json");
        if ( !wxFileExists( tmplPath ) ) {
            wxLogMessage ("dashboard_tactics_pi: ERROR - missing template %s", tmplPath);
            return false;
        }
        bool ret = wxCopyFile ( tmplPath, confPath ); 
        if ( !ret ) {
            wxLogMessage ("dashboard_tactics_pi: ERROR - cannot copy template %s to %s", tmplPath, confPath);
            return false;
        }
    }
    wxFileInputStream jsonStream( confPath );
    wxJSONValue  root;
    wxJSONReader reader;
    int numErrors = reader.Parse( jsonStream, &root );
    if ( numErrors > 0 )  {
        const wxArrayString& errors = reader.GetErrors();
        wxMessageBox(_("InfluxDB Steamer configuration file parsing error, see log file."));
        for (int i = 0; ( ((size_t) i < errors.GetCount()) && ( i < 10 ) ); i++) {
            wxLogMessage ("dashboard_tactics_pi: ERROR - parsing errors in the configuration file: %s", errors.Item(i) );
        }
        return false;
    }
    m_apiURL += root["influxdb"]["serverurl"].AsString();
    m_apiURL += root["influxdb"]["api"].AsString();
    m_apiURL += root["influxdb"]["org"].AsString();
    m_apiURL += root["influxdb"]["bucket"].AsString();
    m_apiURL += root["influxdb"]["precision"].AsString();

    m_apiHdr += root["influxdb"]["tokenprfx"].AsString();
    m_apiHdr += root["influxdb"]["token"].AsString();

    m_connectionRetry = root["streamer"]["connectionretry"].AsInt();
    m_timestamps += root["streamer"]["timestamps"].AsString();
    m_verbosity = root["streamer"]["verbosity"].AsInt();

    if ( m_verbosity > 1 ) {
        wxLogMessage( "dashboard_tactics_pi: InfluxDB API URL = \"%s\"", m_apiURL );
        wxLogMessage( "dashboard_tactics_pi: InfluxDB API Hdr = \"%s\"", m_apiHdr );
        wxLogMessage( "dashboard_tactics_pi: InfluxDB connection retries every %d seconds", m_connectionRetry );
        wxLogMessage( "dashboard_tactics_pi: InfluxDB timestamps = %s", m_timestamps );
    }

    return true;
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamoutSingle::SaveConfig()
{
    if ( *m_nofStreamOut > 1 )
        return;
    
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return;

    pConf->SetPath(_T("/PlugIns/Dashboard/Tactics/Streamout/"));
    pConf->Write(_T("ConfigFile"),m_configFileName);

    return;
}
