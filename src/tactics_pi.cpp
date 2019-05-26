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
wxString tactics_pi::get_sCMGSynonym(void) {return g_sCMGSynonym;};
wxString tactics_pi::get_sVMGSynonym(void) {return g_sVMGSynonym;};


//---------------------------------------------------------------------------------------------------------
//
//          Tactics Performance instruments and functions for Dashboard plug-in
//
//---------------------------------------------------------------------------------------------------------

tactics_pi::tactics_pi( void )
{
    m_hostplugin = NULL;
    m_hostplugin_pconfig = NULL;
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

}

tactics_pi::~tactics_pi( void )
{
    return;
}

int tactics_pi::Init( opencpn_plugin *hostplugin, wxFileConfig *pConf )
{
    m_hostplugin = hostplugin;
    m_hostplugin_pconfig = pConf;
    m_hostplugin_config_path = pConf->GetPath();
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

    this->LoadConfig();
    this->ApplyConfig();

	// Context menue for making marks    
	m_pmenu = new wxMenu();
	// this is a dummy menu required by Windows as parent to item created
	wxMenuItem *pmi = new wxMenuItem(m_pmenu, -1, _T("Set Tactics Mark "));
	int miid = AddCanvasContextMenuItem(pmi, m_hostplugin);
	SetCanvasContextMenuItemViz(miid, true);

	return (WANTS_CURSOR_LATLON |
		WANTS_TOOLBAR_CALLBACK |
		INSTALLS_TOOLBAR_TOOL |
		WANTS_PREFERENCES |
		WANTS_CONFIG |
		WANTS_NMEA_SENTENCES |
		WANTS_NMEA_EVENTS |
		USES_AUI_MANAGER |
		WANTS_PLUGIN_MESSAGING |
		WANTS_OPENGL_OVERLAY_CALLBACK |
		WANTS_OVERLAY_CALLBACK
		);
}

bool tactics_pi::DeInit(opencpn_plugin *hostplugin )
{
	SaveConfig();

	return true;
}


bool tactics_pi::LoadConfig()
{
    wxFileConfig *pConf = (wxFileConfig *) m_hostplugin_pconfig;
	if (!pConf)
        return false;

    this->m_this_config_path =
        this->m_hostplugin_config_path + _T("/Tactics");
    pConf->SetPath( this->m_this_config_path );
    if (!this->LoadConfig_CheckTacticsPlugin( pConf )) {
        /* If not imported from the tactics_pi
         standalone Tactics Plugin, we must have our own
         settings in ini-file by now: */
        pConf->SetPath( this->m_this_config_path );
        bool bUserDecision = g_bTacticsImportChecked;
        this->LoadTacticsPluginBasePart( pConf );
        pConf->SetPath( this->m_this_config_path + _T("/Performance"));
        this->LoadTacticsPluginPerformancePart( pConf );
        g_bTacticsImportChecked = bUserDecision;
    } // then load from this plugin's sub-group
    else {
        bool bUserDecision = g_bTacticsImportChecked;
        this->ImportStandaloneTacticsSettings ( pConf );
        g_bTacticsImportChecked = bUserDecision;
    } // else load from the tactics_pi plugin's group
    BoatPolar = NULL;
    /* Unlike the tactics_pi plugin, Dashboard  not absolutely require
       to have polar-file - it may be that the user is not
       interested in performance part. Yet. We can ask that later. */
    if (g_path_to_PolarFile != _T("NULL")) {
        BoatPolar = new Polar(this);
        BoatPolar->loadPolar(g_path_to_PolarFile);
    }
    
    return true;
}
/*
 Check and swap to original TacticsPlugin group if the user wish to import from there, return false if no import
*/
bool tactics_pi::LoadConfig_CheckTacticsPlugin( wxFileConfig *pConf )
{
    bool bCheckIt = false;
    pConf->SetPath( this->m_this_config_path );
   if (!pConf->Exists(_T("TacticsImportChecked"))) {
        g_bTacticsImportChecked = false;
        bCheckIt = true;
    } // then this must be the first run...
    else {
        pConf->Read(_T("TacticsImportChecked"), &g_bTacticsImportChecked, false);
        if (!g_bTacticsImportChecked)
            bCheckIt = true;
    } /* else check is done only once, normally, unless
         TacticsImportChecked is altered manually to false */
    if (!bCheckIt)
        return false;
    if (!this->StandaloneTacticsSettingsExists ( pConf )) 
        return false;
    wxString message(
        _("Import existing Tactics plugin settings into Dashboard's integrated Tactics settings? (Cancel=later)"));
    wxMessageDialog *dlg = new wxMessageDialog(
        GetOCPNCanvasWindow(), message, _T("Dashboard configuration choice"), wxYES_NO|wxCANCEL);
    int choice = dlg->ShowModal();
    if ( choice == wxID_YES ) {
        
        g_bTacticsImportChecked = true;
        return true;
    } // then import
    else {
        if (choice == wxID_NO ) { 
            g_bTacticsImportChecked = true;
            return false;
        } // then do not import, attempt to import from local
        else {
            g_bTacticsImportChecked = false;
            return false;
        } // else not sure (cancel): not now, but will ask again
    } // else no or cancel
}
void tactics_pi::ImportStandaloneTacticsSettings ( wxFileConfig *pConf )
{
    // Here we suppose that both tactics_pi and this Dashboard implementation are identical
    pConf->SetPath( "/PlugIns/Tactics" );
    this->LoadTacticsPluginBasePart( pConf );
    pConf->SetPath( "/PlugIns/Tactics/Performance" );
    this->LoadTacticsPluginPerformancePart( pConf );
}

bool tactics_pi::StandaloneTacticsSettingsExists ( wxFileConfig *pConf )
{
    pConf->SetPath( "/PlugIns/Tactics/Performance" );
    if (pConf->Exists(_T("PolarFile")))
        return true;
    return false;
}

/*
  Load tactics_pi settings from the Tactics base group,
  underneath the group given in pConf object (you must set it).
*/
void tactics_pi::LoadTacticsPluginBasePart ( wxFileConfig *pConf )
{
    pConf->Read(_T("CurrentDampingFactor"), &g_dalpha_currdir, 0.008);
    pConf->Read(_T("LaylineDampingFactor"), &g_dalphaLaylinedDampFactor, 0.15);
    pConf->Read(_T("LaylineLenghtonChart"), &g_dLaylineLengthonChart, 10.0);
    pConf->Read(_T("MinLaylineWidth"), &g_iMinLaylineWidth, 4);
    pConf->Read(_T("MaxLaylineWidth"), &g_iMaxLaylineWidth, 30);
    pConf->Read(_T("LaylineWidthDampingFactor"), &g_dalphaDeltCoG, 0.25);
    pConf->Read(_T("ShowCurrentOnChart"), &g_bDisplayCurrentOnChart, false);
    m_bDisplayCurrentOnChart = g_bDisplayCurrentOnChart;
    pConf->Read(_T("CMGSynonym"), &g_sCMGSynonym, _T("CMG"));
    pConf->Read(_T("VMGSynonym"), &g_sVMGSynonym, _T("VMG"));
    pConf->Read(_T("TacticsImportChecked"), &g_bTacticsImportChecked, false);
}
/*
  Import tactics_pi settings from the Tactics Performance Group,
  underneath the group given in pConf object (you must set it)
*/
void tactics_pi::LoadTacticsPluginPerformancePart ( wxFileConfig *pConf )
{
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
}
void tactics_pi::ApplyConfig(void)
{
    return;
}

bool tactics_pi::SaveConfig()
{
    wxFileConfig *pConf = (wxFileConfig *) m_hostplugin_pconfig;
    if (!pConf)
        return false;
    pConf->SetPath( this->m_this_config_path );
    SaveTacticsPluginBasePart ( pConf );
    pConf->SetPath( this->m_this_config_path + _T("/Performance"));
    SaveTacticsPluginPerformancePart ( pConf );
    return true;
}
/*
  Save tactics_pi settings into the Tactics base group,
  underneath the group given in pConf object (you have to set it).
*/
void tactics_pi::SaveTacticsPluginBasePart ( wxFileConfig *pConf )
{
    pConf->Write(_T("CurrentDampingFactor"), g_dalpha_currdir);
    pConf->Write(_T("LaylineDampingFactor"), g_dalphaLaylinedDampFactor);
    pConf->Write(_T("LaylineLenghtonChart"), g_dLaylineLengthonChart);
    pConf->Write(_T("MinLaylineWidth"), g_iMinLaylineWidth);
    pConf->Write(_T("MaxLaylineWidth"), g_iMaxLaylineWidth);
    pConf->Write(_T("LaylineWidthDampingFactor"), g_dalphaDeltCoG);
    pConf->Write(_T("ShowCurrentOnChart"), g_bDisplayCurrentOnChart);
    pConf->Write(_T("CMGSynonym"), g_sCMGSynonym);
    pConf->Write(_T("VMGSynonym"), g_sVMGSynonym);
    pConf->Write(_T("TacticsImportChecked"), g_bTacticsImportChecked);
}
/*
  Save tactics_pi settings into the Tactics Performance Group,
  underneath the group given in pConf object (you have to set it).
*/
void tactics_pi::SaveTacticsPluginPerformancePart ( wxFileConfig *pConf )
{
    pConf->Write(_T("PolarFile"), g_path_to_PolarFile);
    pConf->Write(_T("BoatLeewayFactor"), g_dLeewayFactor);
    pConf->Write(_T("fixedLeeway"), g_dfixedLeeway);
    pConf->Write(_T("UseHeelSensor"), g_bUseHeelSensor);
    pConf->Write(_T("UseFixedLeeway"), g_bUseFixedLeeway);
    pConf->Write(_T("UseManHeelInput"), g_bManHeelInput);
    pConf->Write(_T("CorrectSTWwithLeeway"), g_bCorrectSTWwithLeeway);
    pConf->Write(_T("CorrectAWwithHeel"), g_bCorrectAWwithHeel);
    pConf->Write(_T("ForceTrueWindCalculation"), g_bForceTrueWindCalculation);
    pConf->Write(_T("UseSOGforTWCalc"), g_bUseSOGforTWCalc);
    pConf->Write(_T("ShowWindbarbOnChart"), g_bShowWindbarbOnChart);
    pConf->Write(_T("ShowPolarOnChart"), g_bShowPolarOnChart);
    pConf->Write(_T("Heel_5kn_45Degree"), g_dheel[1][1]);
    pConf->Write(_T("Heel_5kn_90Degree"), g_dheel[1][2]);
    pConf->Write(_T("Heel_5kn_135Degree"), g_dheel[1][3]);
    pConf->Write(_T("Heel_10kn_45Degree"), g_dheel[2][1]);
    pConf->Write(_T("Heel_10kn_90Degree"), g_dheel[2][2]);
    pConf->Write(_T("Heel_10kn_135Degree"), g_dheel[2][3]);
    pConf->Write(_T("Heel_15kn_45Degree"), g_dheel[3][1]);
    pConf->Write(_T("Heel_15kn_90Degree"), g_dheel[3][2]);
    pConf->Write(_T("Heel_15kn_135Degree"), g_dheel[3][3]);
    pConf->Write(_T("Heel_20kn_45Degree"), g_dheel[4][1]);
    pConf->Write(_T("Heel_20kn_90Degree"), g_dheel[4][2]);
    pConf->Write(_T("Heel_20kn_135Degree"), g_dheel[4][3]);
    pConf->Write(_T("Heel_25kn_45Degree"), g_dheel[5][1]);
    pConf->Write(_T("Heel_25kn_90Degree"), g_dheel[5][2]);
    pConf->Write(_T("Heel_25kn_135Degree"), g_dheel[5][3]);
    pConf->Write(_T("ExpPolarSpeed"), g_bExpPerfData01);
    pConf->Write(_T("ExpCourseOtherTack"), g_bExpPerfData02);
    pConf->Write(_T("ExpTargetVMG"), g_bExpPerfData03);
    pConf->Write(_T("ExpVMG_CMG_Diff_Gain"), g_bExpPerfData04);
    pConf->Write(_T("ExpCurrent"), g_bExpPerfData05);
    pConf->Write(_T("NKE_TrueWindTableBug"), g_bNKE_TrueWindTableBug);
}

/*********************************************************************************
Taken from cutil
**********************************************************************************/
inline int myCCW(wxRealPoint p0, wxRealPoint p1, wxRealPoint p2) {
	double dx1, dx2;
	double dy1, dy2;

	dx1 = p1.x - p0.x; dx2 = p2.x - p0.x;
	dy1 = p1.y - p0.y; dy2 = p2.y - p0.y;

	/* This is basically a slope comparison: we don't do divisions because

	* of divide by zero possibilities with pure horizontal and pure
	* vertical lines.
	*/
	return ((dx1 * dy2 > dy1 * dx2) ? 1 : -1);

}
/*********************************************************************************
returns true if we have a line intersection.
Taken from cutil, but with double variables
**********************************************************************************/
inline bool IsLineIntersect(wxRealPoint p1, wxRealPoint p2, wxRealPoint p3, wxRealPoint p4)
{
	return (((myCCW(p1, p2, p3) * myCCW(p1, p2, p4)) <= 0)
		&& ((myCCW(p3, p4, p1) * myCCW(p3, p4, p2) <= 0)));

}
/********************************************************************************
calculate Line intersection between 2 lines, each described by 2 points
return lat/lon of intersection point
basic calculation:
int p1[] = { -4,  5, 1 };
int p2[] = { -2, -5, 1 };
int p3[] = { -6,  2, 1 };
int p4[] = {  5,  4, 1 };
int l1[3], l2[3], s[3];
double sch[2];
l1[0] = p1[1] * p2[2] - p1[2] * p2[1];
l1[1] = p1[2] * p2[0] - p1[0] * p2[2];
l1[2] = p1[0] * p2[1] - p1[1] * p2[0];
l2[0] = p3[1] * p4[2] - p3[2] * p4[1];
l2[1] = p3[2] * p4[0] - p3[0] * p4[2];
l2[2] = p3[0] * p4[1] - p3[1] * p4[0];
s[0] = l1[1] * l2[2] - l1[2] * l2[1];
s[1] = l1[2] * l2[0] - l1[0] * l2[2];
s[2] = l1[0] * l2[1] - l1[1] * l2[0];
sch[0] = (double)s[0] / (double)s[2];
sch[1] = (double)s[1] / (double)s[2];
*********************************************************************************/
wxRealPoint GetLineIntersection(wxRealPoint line1point1, wxRealPoint line1point2, wxRealPoint line2point1, wxRealPoint line2point2)
{
	wxRealPoint intersect;
	intersect.x = -999.;
	intersect.y = -999.;
	if (IsLineIntersect(line1point1, line1point2, line2point1, line2point2)){
		double line1[3], line2[3], s[3];
		line1[0] = line1point1.y * 1. - 1. * line1point2.y;
		line1[1] = 1. * line1point2.x - line1point1.x * 1.;
		line1[2] = line1point1.x * line1point2.y - line1point1.y * line1point2.x;
		line2[0] = line2point1.y * 1. - 1. * line2point2.y;
		line2[1] = 1. * line2point2.x - line2point1.x * 1.;
		line2[2] = line2point1.x * line2point2.y - line2point1.y * line2point2.x;
		s[0] = line1[1] * line2[2] - line1[2] * line2[1];
		s[1] = line1[2] * line2[0] - line1[0] * line2[2];
		s[2] = line1[0] * line2[1] - line1[1] * line2[0];
		intersect.x = s[0] / s[2];
		intersect.y = s[1] / s[2];
	}
	return intersect;
}
/*********************************************************************************
Function calculates the time to sail for a given distance, TWA and TWS, based on
the polar data
**********************************************************************************/
double CalcPolarTimeToMark(double distance, double twa, double tws)
{
	double pspd = BoatPolar->GetPolarSpeed(twa, tws);
	return distance / pspd;
}
/*********************************************************************************
Function returns the (smaller) TWA of a given TWD and Course.
Used for Target-CMG calculation.
It covers the 359 - 0 degree problem
e.g. : TWD = 350, ctm = 10; the TWA is returned as 20 degrees
(and not 340 if we'd do a simple TWD - ctm)
**********************************************************************************/
double getMarkTWA(double twd, double ctm)
{
	double val, twa;
	if (twd > 180)
	{
		val = twd - 180;
		if (ctm < val)
			twa = 360 - twd + ctm;
		else
			twa = twd > ctm ? twd - ctm : ctm - twd;
	}
	else
	{
		val = twd + 180;
		if (ctm > val)
			twa = 360 - ctm + twd;
		else
			twa = twd > ctm ? twd - ctm : ctm - twd;
	}
	return twa;
}
/*********************************************************************************
Function returns the (smaller) degree range of 2 angular values
on the compass rose (without sign)
It covers the 359 - 0 degree problem
e.g. : max = 350, min = 10; the rage is returned as 20 degrees
(and not 340 if we'd do a simple max - min)
**********************************************************************************/
double getDegRange(double max, double min)
{
	double val, range;
	if (max > 180)
	{
		val = max - 180;
		if (min < val)
			range = 360 - max + min;
		else
			range = max > min ? max - min : min - max;
	}
	else
	{
		val = max + 180;
		if (min > val)
			range = 360 - min + max;
		else
			range = max > min ? max - min : min - max;
	}
	return range;
}
/*********************************************************************************
Function returns the (smaller) signed degree range of 2 angular values
on the compass rose (clockwise is +)
It covers the 359 - 0 degree problem
e.g. : fromAngle = 350, toAngle = 10; the range is returned as +20 degrees
(and not 340 if we'd do a simple fromAngle - toAngle)
**********************************************************************************/
double getSignedDegRange(double fromAngle, double toAngle)
{
	double val, range;
	if (fromAngle > 180)
	{
		val = fromAngle - 180;
		if (toAngle < val)
			range = 360 - fromAngle + toAngle;
		else
			range = toAngle - fromAngle;
	}
	else
	{
		val = fromAngle + 180;
		if (toAngle > val)
			range = -(360 - toAngle + fromAngle);
		else
			range = toAngle - fromAngle;
	}
	return range;
}
