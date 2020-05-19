/**************************************************************************
* $Id: tactics_pi_ext.h, v1.0 2016/06/07 tom_BigSpeedy Exp $
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

#ifndef __TACTICSPIEXT_H__
#define __TACTICSPIEXT_H__


#include "ocpn_plugin.h"
#include "avg_wind.h"

class TacticsInstrument_PerformanceSingle;
#include "Polar.h"


extern bool g_bTacticsImportChecked;
extern double g_dalphaDeltCoG;
extern double g_dalphaLaylinedDampFactor;
extern double g_dLeewayFactor;
extern double g_dfixedLeeway;
extern double g_dalpha_currdir;
extern int g_iMinLaylineWidth;
extern int g_iMaxLaylineWidth;
extern double g_dLaylineLengthonChart;
extern Polar* BoatPolar;
extern bool g_bDisplayLaylinesOnChart;
extern bool g_bDisplayCurrentOnChart;
extern wxString g_path_to_PolarFile;
extern wxString g_path_to_PolarLookupOutputFile;
extern PlugIn_Route *m_pRoute;
extern PlugIn_Waypoint *m_pMark;
extern wxString g_sMarkGUID;
extern double g_dmark_lat;
extern double g_dmark_lon;
extern double g_dcur_lat;
extern double g_dcur_lon;
extern double g_dheel[6][5];
extern bool g_bUseHeelSensor;
extern bool g_bUseFixedLeeway;
extern bool g_bManHeelInput;
extern bool g_bCorrectSTWwithLeeway;  //if true STW is corrected with Leeway (in case Leeway is available)
extern bool g_bCorrectAWwithHeel;    //if true, AWS/AWA will be corrected with Heel-Angle
extern bool g_bForceTrueWindCalculation;    //if true, NMEA Data for TWS,TWA,TWD is not used, but the plugin calculated data is used
extern bool g_bUseSOGforTWCalc; //if true, use SOG instead of STW to calculate TWS,TWA,TWD
extern bool g_bShowWindbarbOnChart;
extern bool g_bShowPolarOnChart;
extern bool g_bPersistentChartPolarAnimation; // If true, continue timer based functions to animate performance on the chart
extern bool g_bExpPerfData01;
extern bool g_bExpPerfData02;
extern bool g_bExpPerfData03;
extern bool g_bExpPerfData04;
extern bool g_bExpPerfData05;
extern bool g_bNKE_TrueWindTableBug;//variable for NKE TrueWindTable-Bugfix
extern wxString g_sCMGSynonym, g_sVMGSynonym;
extern wxString g_sDataExportSeparator;
extern bool     g_bDataExportUTC;
extern bool     g_bDataExportClockticks;
extern AvgWind* AverageWind;
extern int g_iDbgRes_Polar_Status;


#endif // __TACTICSPIEXT_H__

