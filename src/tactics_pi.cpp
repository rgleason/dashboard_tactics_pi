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
        wxString perfPath = _T("/Performance");
        wxConfigPathChanger tempConfigPath( (wxConfigBase *)pConf, basePath + perfPath);
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
    wxString message(
        _("Import existing Tactics plugin settings into Dashboard's integrated Tactics settings? (Cancel=later)"));
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
            return false;
        } // else not sure (cancel): not now, but will ask again
    } // else no or cancel
}
/*
 This is the actual load method, it may be used twice:
 once, initially, for the importing the original tactics_pi plugin settings and then,
 after that, to import the same values, but from the dashboard_pi plugin's
 new Performance group where we keep the settings, separeted from the tactics_pi plugin
 settings, for more freedom for the future development of dashboard_pi but also 
 to avoid confusion in case the two plugins are used in same installation.
*/
bool tactics_pi::LoadConfigTacticsPlugin( wxFileConfig *pConf )
{
    if (!pConf)
        return false;

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
