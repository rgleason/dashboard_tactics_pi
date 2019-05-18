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




