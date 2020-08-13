/***************************************************************************
* $Id: TacticsConfig, v1.0 2016/06/07 tomBigSpeedy Exp $
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

using namespace std;

#include "tactics_pi.h"

#include "tactics_pi_ext.h"


bool tactics_pi::TacticsLoadConfig()
{
    wxFileConfig *pConf = static_cast <wxFileConfig *>(m_hostplugin_pconfig);
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
    /* Unlike the tactics_pi plugin, Dashboard  not absolutely require
       to have polar-file - it may be that the user is not
       interested in performance part. Yet. We can ask that later. */
    BoatPolar = new Polar(this);
    if ( g_path_to_PolarFile != _T("NULL") ) {
        BoatPolar->loadPolar( g_path_to_PolarFile );
        if ( g_path_to_PolarLookupOutputFile != _T("NULL") ) {
            BoatPolar->saveLookupTable( g_path_to_PolarLookupOutputFile );
        }
    }
    else {
        BoatPolar->loadPolar(_T("NULL"));
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
        _("Import existing Tactics plug-in settings into DashT? (Cancel=later)"));
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
    pConf->Read(_T("MinLaylineWidth"), &g_iMinLaylineWidth, 2);
    pConf->Read(_T("MaxLaylineWidth"), &g_iMaxLaylineWidth, 30);
    pConf->Read(_T("LaylineWidthDampingFactor"), &g_dalphaDeltCoG, 0.25);
    pConf->Read(_T("ShowLaylinesOnChart"), &g_bDisplayLaylinesOnChart, false);
    m_bLaylinesIsVisible = g_bDisplayLaylinesOnChart;
    pConf->Read(_T("ShowCurrentOnChart"), &g_bDisplayCurrentOnChart, false);
    m_bDisplayCurrentOnChart = g_bDisplayCurrentOnChart;
    pConf->Read(_T("CMGSynonym"), &g_sCMGSynonym, _T("CMG"));
    pConf->Read(_T("VMGSynonym"), &g_sVMGSynonym, _T("VMG"));
    pConf->Read(_T("DataExportSeparator"), &g_sDataExportSeparator, _(";"));
    pConf->Read(_T("TacticsImportChecked"), &g_bTacticsImportChecked, false);
    pConf->Read(_T("DataExportUTC-ISO8601"), &g_bDataExportUTC, 0);
    pConf->Read(_T("DataExportClockticks"), &g_bDataExportClockticks,0);
}
/*
  Import tactics_pi settings from the Tactics Performance Group,
  underneath the group given in pConf object (you must set it)
*/
void tactics_pi::LoadTacticsPluginPerformancePart ( wxFileConfig *pConf )
{
    pConf->Read(_T("PolarFile"), &g_path_to_PolarFile, _T("NULL"));
    pConf->Read(_T("PolarLookupTableOutputFile"), &g_path_to_PolarLookupOutputFile, _T("NULL"));
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
    pConf->Read(_T("PersistentChartPolarAnimation"), &g_bPersistentChartPolarAnimation, true);
    m_bPersistentChartPolarAnimation = g_bPersistentChartPolarAnimation;
    pConf->Read(_T("UseSOGforTWCalc"), &g_bUseSOGforTWCalc, true);
    pConf->Read(_T("ExpPolarSpeed"), &g_bExpPerfData01, false);
    pConf->Read(_T("ExpCourseOtherTack"), &g_bExpPerfData02, false);
    pConf->Read(_T("ExpTargetVMG"), &g_bExpPerfData03, false);
    pConf->Read(_T("ExpVMG_CMG_Diff_Gain"), &g_bExpPerfData04, false);
    pConf->Read(_T("ExpCurrent"), &g_bExpPerfData05, false);
    pConf->Read(_T("NKE_TrueWindTableBug"), &g_bNKE_TrueWindTableBug, false);
    m_bNKE_TrueWindTableBug = g_bNKE_TrueWindTableBug;
}
void tactics_pi::TacticsApplyConfig(void)
{

    if (!(BoatPolar == NULL)) {
        if (g_path_to_PolarFile != _T("NULL")) {
            BoatPolar->loadPolar(g_path_to_PolarFile);
            if ( g_path_to_PolarLookupOutputFile != _T("NULL") ) {
                BoatPolar->saveLookupTable( g_path_to_PolarLookupOutputFile );
            }
        }
        else {
            BoatPolar->loadPolar(_T("NULL"));
        }
    }
    m_bDisplayCurrentOnChart = g_bDisplayCurrentOnChart;
    m_bShowWindbarbOnChart = g_bShowWindbarbOnChart;
    m_bShowPolarOnChart = g_bShowPolarOnChart;
    m_bPersistentChartPolarAnimation = g_bPersistentChartPolarAnimation;
    return;
}

bool tactics_pi::TacticsSaveConfig()
{
    wxFileConfig *pConf = static_cast <wxFileConfig *>(m_hostplugin_pconfig);
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
    pConf->Write(_T("ShowLaylinesOnChart"), g_bDisplayLaylinesOnChart);
    pConf->Write(_T("ShowCurrentOnChart"), g_bDisplayCurrentOnChart);
    pConf->Write(_T("CMGSynonym"), g_sCMGSynonym);
    pConf->Write(_T("VMGSynonym"), g_sVMGSynonym);
    pConf->Write(_T("DataExportSeparator"), g_sDataExportSeparator);
    pConf->Write(_T("DataExportUTC-ISO8601"), g_bDataExportUTC);
    pConf->Write(_T("DataExportClockticks"), g_bDataExportClockticks);
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
    pConf->Write(_T("PersistentChartPolarAnimation"), g_bPersistentChartPolarAnimation);
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
