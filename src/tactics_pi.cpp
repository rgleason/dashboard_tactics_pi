/***************************************************************************
* $Id: tactics_pi.cpp, v1.0 2016/06/07 tomBigSpeedy Exp $
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

#include "tactics_pi.h"

bool g_bTacticsImportChecked;
double g_dalphaDeltCoG;
double g_dalphaLaylinedDampFactor;
double g_dLeewayFactor;
double g_dfixedLeeway;
double g_dalpha_currdir;
int g_iMinLaylineWidth;
int g_iMaxLaylineWidth;
double g_dLaylineLengthonChart;
Polar* BoatPolar;
bool g_bDisplayCurrentOnChart;
wxString g_path_to_PolarFile;
PlugIn_Route *m_pRoute = NULL;
PlugIn_Waypoint *m_pMark = NULL;
double g_dmark_lat = NAN;
double g_dmark_lon = NAN;
double g_dcur_lat = NAN;
double g_dcur_lon = NAN;
double g_dheel[6][5];
bool g_bUseHeelSensor;
bool g_bUseFixedLeeway;
bool g_bManHeelInput;
bool g_bCorrectSTWwithLeeway;  //if true STW is corrected with Leeway (in case Leeway is available)
bool g_bCorrectAWwithHeel;    //if true, AWS/AWA will be corrected with Heel-Angle
bool g_bForceTrueWindCalculation;    //if true, NMEA Data for TWS,TWA,TWD is not used, but the plugin calculated data is used
bool g_bUseSOGforTWCalc; //if true, use SOG instead of STW to calculate TWS,TWA,TWD
bool g_bShowWindbarbOnChart;
bool g_bShowPolarOnChart;
bool g_bPersistentChartPolarAnimation; // If true, continue timer based functions to animate performance on the chart
bool g_bExpPerfData01;
bool g_bExpPerfData02;
bool g_bExpPerfData03;
bool g_bExpPerfData04;
bool g_bExpPerfData05;
bool g_bNKE_TrueWindTableBug;//variable for NKE TrueWindTable-Bugfix
wxString g_sCMGSynonym, g_sVMGSynonym;

//---------------------------------------------------------------------------------------------------------
//
//          Tactics Performance instruments and functions for Dashboard plug-in
//
//---------------------------------------------------------------------------------------------------------

tactics_pi::tactics_pi( void )
{
    m_bNKE_TrueWindTableBug = false;
    m_VWR_AWA = 10;
	alpha_currspd = 0.2;  //smoothing constant for current speed
	alpha_CogHdt = 0.1; // smoothing constant for diff. btw. Cog & Hdt
	m_ExpSmoothCurrSpd = NAN;
	m_ExpSmoothCurrDir = NAN;
	m_ExpSmoothSog = NAN;
	m_ExpSmoothSinCurrDir = NAN;
	m_ExpSmoothCosCurrDir = NAN;
	m_ExpSmoothSinCog = NAN;
	m_ExpSmoothCosCog = NAN;
	m_CurrentDirection = NAN;
	m_LaylineSmoothedCog = NAN;
	m_LaylineDegRange = 0;
	mSinCurrDir = new DoubleExpSmooth(g_dalpha_currdir);
	mCosCurrDir = new DoubleExpSmooth(g_dalpha_currdir);
	mExpSmoothCurrSpd = new ExpSmooth(alpha_currspd);
	mExpSmoothSog = new DoubleExpSmooth(0.4);
    mExpSmSinCog = new DoubleExpSmooth(
        g_dalphaLaylinedDampFactor);//prev. ExpSmooth(...
    mExpSmCosCog = new DoubleExpSmooth(
        g_dalphaLaylinedDampFactor);//prev. ExpSmooth(...
    m_ExpSmoothDegRange = 0;
	mExpSmDegRange = new ExpSmooth(g_dalphaDeltCoG);
	mExpSmDegRange->SetInitVal(g_iMinLaylineWidth);
	mExpSmDiffCogHdt = new ExpSmooth(alpha_CogHdt);
	mExpSmDiffCogHdt->SetInitVal(0);
	m_bShowPolarOnChart = false;
	m_bShowWindbarbOnChart = false;
	m_bDisplayCurrentOnChart = false;
	m_LeewayOK = false;
	mHdt = NAN;
	mStW = NAN;
	mTWA = NAN;
	mTWD = NAN;
	mTWS = NAN;
	m_calcTWA = NAN;
	m_calcTWD = NAN;
	m_calcTWS = NAN;
	mSOG = NAN;
	mCOG = NAN;
	mlat = NAN;
	mlon = NAN;
	mheel = NAN;
	mLeeway = NAN;
	mPolarTargetSpeed = NAN;
	mBRG = NAN;
	mVMGGain = mCMGGain = mVMGoptAngle = mCMGoptAngle = 0.0;
	mPredictedCoG = NAN;
	for (int i = 0; i < COGRANGE; i++) m_COGRange[i] = NAN;

	m_bTrueWind_available = false;

     b_tactics_dc_message_shown = false;

	BoatPolar = new Polar(this);
	if (g_path_to_PolarFile != _T("NULL"))
		BoatPolar->loadPolar(g_path_to_PolarFile);
	else
		BoatPolar->loadPolar(_T("NULL"));
	//    This PlugIn needs a toolbar icon
	wxString shareLocn = *GetpSharedDataLocation() +
		_T("plugins") + wxFileName::GetPathSeparator() +
		_T("tactics_pi") + wxFileName::GetPathSeparator()
		+ _T("data") + wxFileName::GetPathSeparator();

}

tactics_pi::~tactics_pi( void )
{

}

bool tactics_pi::LoadConfig( wxFileConfig *pConf )
{
	if (pConf) {

        wxString basePath = pConf->GetPath();
        wxstring perfPath = _T("/Performance");
        wxConfigPathChanger tempConfigPath( pConf, basePath + perfPath);
        if (this->LoadConfig_CheckTacticsPlugin( pConf )) {
            return true;
        } // then check if tactic_pi parameters exists and propose to import - if imported, that's it: config loaded
        // otherwise, if not imported from the original, standalone Tactics Plugin, we must have our own values now:
        return this->LoadConfigTacticsPlugin( pConf );
    }
    else
        return false;
}
/*
 Check and swap to original TacticsPlugin group if the user wish to import from there, return false if no import
*/
bool tactics_pi::LoadConfig_CheckTacticsPlugin( wxFileConfig *pConf )
{
    bool bCheckIt = false;
    if (!pConf->Exists(_T("TacticsImportChecked"))) {
            pConf->Read(_T("TacticsImportChecked"), &g_bTacticsImportChecked, true);
            bCheckIt = true;
        } // then check if tactic_pi parameters exists and propose to import
    else {
        pConf->Read(_T("TacticsImportChecked"), &g_bTacticsImportChecked, true);
        if (!g_bTacticsImportChecked) {
            bCheckIt = true;
            g_bTacticsImportChecked = true;
        }
    } // else check is done only once, normally, unless TacticsImportChecked is altered manually to false
    if (!bCheckIt)
        return false;
    if (!pConf->Exists(_T("/PlugIns/Tactics/Performance")))
        return false;
    wxString message(_("Do you want to import existing Tactics plugin settings into Dashboard's integrated Tactics settings? "));
    wxMessageDialog *dlg = new wxMessageDialog(
        GetOCPNCanvasWindow(), message, _T("Dashboard configuration choice"), wxYES_NO|wxCANCEL);
    int choice = dlg->ShowModal();
    if ( choice == wxID_YES ) { 
        wxConfigPathChanger tempConfigPath( pConf, "/PlugIns/Tactics/Performance");
        this->LoadConfigTacticsPlugin( pConf );
        return true;
    } // then import
    else {
        if (choice == wxID_NO ) { 
            return false;
        } // then do not import, attempt to import from local
        else {
            g_bTacticsImportChecked = false;
            return false
        } // else not sure (cancel): not now, but will ask again
    } // else no or cancel
}
/*
 This is the actual load method, it may be used twice:
 once for the importing the original tactics_pi plugin settings and, after that,
 to import the same values, but from the dashboard_pi plugins new Performance group
*/
bool tactics_pi::LoadConfigTacticsPlugin( wxFileConfig *pConf )
{
    if (!pConf)
        return false;

    pConf->SetPath(_T("/PlugIns/Tactics/Performance"));
    pConf->Read(_T("PolarFile"), &g_path_to_PolarFile, _T("NULL"));
    pConf->Read(_T("BoatLeewayFactor"), &g_dLeewayFactor, 10);
    pConf->Read(_T("fixedLeeway"), &g_dfixedLeeway, 30);
    pConf->Read(_T("UseHeelSensor"), &g_bUseHeelSensor, true);
    pConf->Read(_T("UseFixedLeeway"), &g_bUseFixedLeeway, false);
    pConf->Read(_T("UseManHeelInput"), &g_bManHeelInput, false);
    pConf->Read(_T("Heel_5kn_45Degree"), &g_dheel[1][1], 5);
    pConf->Read(_T("Heel_5kn_90Degree"), &g_dheel[1][2], 8);
    pConf->Read(_T("Heel_5kn_135Degree"), &g_dheel[1][3], 5);
    pConf->Read(_T("Heel_10kn_45Degree"), &g_dheel[2][1], 8);
    pConf->Read(_T("Heel_10kn_90Degree"), &g_dheel[2][2], 10);
    pConf->Read(_T("Heel_10kn_135Degree"), &g_dheel[2][3], 11);
    pConf->Read(_T("Heel_15kn_45Degree"), &g_dheel[3][1], 25);
    pConf->Read(_T("Heel_15kn_90Degree"), &g_dheel[3][2], 20);
    pConf->Read(_T("Heel_15kn_135Degree"), &g_dheel[3][3], 13);
    pConf->Read(_T("Heel_20kn_45Degree"), &g_dheel[4][1], 20);
    pConf->Read(_T("Heel_20kn_90Degree"), &g_dheel[4][2], 16);
    pConf->Read(_T("Heel_20kn_135Degree"), &g_dheel[4][3], 15);
    pConf->Read(_T("Heel_25kn_45Degree"), &g_dheel[5][1], 25);
    pConf->Read(_T("Heel_25kn_90Degree"), &g_dheel[5][2], 20);
    pConf->Read(_T("Heel_25kn_135Degree"), &g_dheel[5][3], 20);
    pConf->Read(_T("UseManHeelInput"), &g_bManHeelInput, false);
    pConf->Read(_T("CorrectSTWwithLeeway"), &g_bCorrectSTWwithLeeway, false);  //if true, STW is corrected with Leeway (in case Leeway is available)
    pConf->Read(_T("CorrectAWwithHeel"), &g_bCorrectAWwithHeel, false);    //if true, AWS/AWA are corrected with Heel-Angle
    pConf->Read(_T("ForceTrueWindCalculation"), &g_bForceTrueWindCalculation, false);    //if true, NMEA Data for TWS,TWA,TWD is not used, but the plugin calculated data is used
    pConf->Read(_T("ShowWindbarbOnChart"), &g_bShowWindbarbOnChart, false);
    m_bShowWindbarbOnChart = g_bShowWindbarbOnChart;
    pConf->Read(_T("ShowPolarOnChart"), &g_bShowPolarOnChart, false);
    m_bShowPolarOnChart = g_bShowPolarOnChart;
    pConf->Read(_T("UseSOGforTWCalc"), &g_bUseSOGforTWCalc, false);
    pConf->Read(_T("ExpPolarSpeed"), &g_bExpPerfData01, false);
    pConf->Read(_T("ExpCourseOtherTack"), &g_bExpPerfData02, false);
    pConf->Read(_T("ExpTargetVMG"), &g_bExpPerfData03, false);
    pConf->Read(_T("ExpVMG_CMG_Diff_Gain"), &g_bExpPerfData04, false);
    pConf->Read(_T("ExpCurrent"), &g_bExpPerfData05, false);
    pConf->Read(_T("NKE_TrueWindTableBug"), &g_bNKE_TrueWindTableBug, false);
    m_bNKE_TrueWindTableBug = g_bNKE_TrueWindTableBug;

    return true;
 
}



