/******************************************************************************
 * $Id: tactics_pi.h, v1.0 2016/06/27 tom_BigSpeedy Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Tactics Plugin
 * Author:   Thomas Rauch
 *   (Inspired by original work from Jean-Eudes Onfray)
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

#ifndef _TACTICSPI_H_
#define _TACTICSPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/notebook.h>
#include <wx/fileconf.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/spinctrl.h>
#include <wx/aui/aui.h>
#include <wx/fontpicker.h>
#include <wx/glcanvas.h>

#include "performance.h"
#include "bearingcompass.h"
#include "avg_wind.h"
#include "polarcompass.h"

class Polar;

#define aws_watchdog_timeout_ticks 10
#define brg_watchdog_timeout_ticks 10
#define twd_watchdog_timeout_ticks 10
#define tws_watchdog_timeout_ticks 10
#define vmg_watchdog_timeout_ticks 10
#define CURR_RECORD_COUNT 20
#define COGRANGE 60



//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#include "ocpn_plugin.h"
// #include "nmea0183/nmea0183.h"
#include "instrument.h"
// #include "speedometer.h"
// #include "compass.h"
// #include "wind.h"
// #include "rudder_angle.h"
// #include "gps.h"
// #include "depth.h"
// #include "clock.h"
// #include "wind_history.h"
// #include "baro_history.h"
// #include "from_ownship.h"
// #include "iirfilter.h"



class tactics_pi
{
public:
    tactics_pi(void);
    ~tactics_pi(void);
    virtual int Init(void) = 0;
    virtual int TacticsInit( opencpn_plugin *hostplugin, wxFileConfig *pConf ) final;
    virtual bool DeInit(void) = 0;
    virtual bool TacticsDeInit(void) final;
    virtual void Notify(void) = 0;
    virtual void TacticsNotify(void) final;
    virtual bool LoadConfig(void) = 0;
    virtual bool TacticsLoadConfig(void) final;
    virtual void ApplyConfig(void) = 0;
    virtual void TacticsApplyConfig(void) final;
    virtual bool SaveConfig(void) = 0;
    virtual bool TacticsSaveConfig(void) final;
    virtual bool RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp) = 0;
    virtual void UpdateAuiStatus(void) = 0;
    virtual void SetToggledStateVisible(bool isvisible) final;
    virtual bool TacticsRenderOverlay(
        wxDC &dc, PlugIn_ViewPort *vp) final;
    virtual bool RenderGLOverlay(
        wxGLContext *pcontext, PlugIn_ViewPort *vp) = 0;
    virtual bool TacticsRenderGLOverlay(
        wxGLContext *pcontext, PlugIn_ViewPort *vp) final;
    void DoRenderLaylineGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
    void DoRenderCurrentGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
    virtual void SetCursorLatLon(double lat, double lon) = 0;
    virtual void TacticsSetCursorLatLon(double lat, double lon) final;
    void DrawWindBarb(wxPoint pp, PlugIn_ViewPort *vp);
    void DrawPolar(PlugIn_ViewPort *vp, wxPoint pp, double PolarAngle );
    void DrawTargetAngle(PlugIn_ViewPort *vp, wxPoint pp, double Angle, wxString color, int size, double rad);
    void ToggleLaylineRender(void);
    void ToggleCurrentRender(void);
    void TogglePolarRender(void);
    void ToggleWindbarbRender(void);
    bool GetLaylineVisibility(void);
    bool GetWindbarbVisibility(void);
    bool GetCurrentVisibility(void);
    bool GetPolarVisibility(void);

    virtual void SetNMEASentence(wxString &sentence) = 0;
    void SetNMEASentence_Arm_AWS_Watchdog(void){mAWS_Watchdog = aws_watchdog_timeout_ticks;}
    void SetNMEASentence_Arm_BRG_Watchdog(void){mBRG_Watchdog = brg_watchdog_timeout_ticks;}
    void SetNMEASentence_Arm_TWD_Watchdog(void){mTWD_Watchdog = twd_watchdog_timeout_ticks;}
    void SetNMEASentence_Arm_TWS_Watchdog(void){mTWS_Watchdog = tws_watchdog_timeout_ticks;}
    void SetNMEASentence_Arm_VMG_Watchdog(void){mVMG_Watchdog = vmg_watchdog_timeout_ticks;}
    bool SetNMEASentenceMWD_NKEbug(double SentenceWindSpeedKnots);

    void CalculateCurrent(
        unsigned long long st, double value, wxString unit);
    void CalculateLeeway(
        unsigned long long st, double value, wxString unit);
    void CalculateTrueWind(
        unsigned long long st, double value, wxString unit);
    void CalculateLaylineDegreeRange(void);
    void CalculatePerformanceData(void);
    void CalculatePredictedCourse(void);
    void SetCalcVariables(
        unsigned long long st, double value, wxString unit);
    
    virtual void OnContextMenuItemCallback(int id) = 0;
    virtual void TacticsOnContextMenuItemCallback(int id) final;

    virtual void SendSentenceToAllInstruments(
        unsigned long long st, double value, wxString unit ) = 0;
    virtual bool SendSentenceToAllInstruments_PerformanceCorrections(
        unsigned long long st, double &value, wxString &unit ) final;
    virtual bool SendSentenceToAllInstruments_LaunchTrueWindCalculations(
        unsigned long long st, double value ) final;
    virtual bool SendSentenceToAllInstruments_GetCalculatedTrueWind(
        unsigned long long st, double value, wxString unit,
        unsigned long long &st_twa, double &value_twa, wxString &unit_twa,
        unsigned long long &st_tws, double &value_tws, wxString &unit_tws,
        unsigned long long &st_twd, double &value_twd, wxString &unit_twd
        ) final;
    virtual bool SendSentenceToAllInstruments_GetCalculatedLeeway(
        unsigned long long &st_leeway, double &value_leeway,
        wxString &unit_leeway) final;
    virtual bool SendSentenceToAllInstruments_GetCalculatedCurrent(
        unsigned long long st, double value, wxString unit,
        unsigned long long &st_currdir, double &value_currdir,
        wxString &unit_currdir,
        unsigned long long &st_currspd, double &value_currspd,
        wxString &unit_currspd) final;

    static wxString get_sCMGSynonym(void);
    static wxString get_sVMGSynonym(void);
    void set_m_bDisplayCurrentOnChart(bool value) {m_bDisplayCurrentOnChart = value;} 
private:
    opencpn_plugin      *m_hostplugin;
    wxFileConfig        *m_hostplugin_pconfig;
    wxString             m_hostplugin_config_path;
    wxString             m_this_config_path;

    int                  mBRG_Watchdog;
    int                  mTWD_Watchdog;
    int                  mTWS_Watchdog;
    int                  mAWS_Watchdog;
    int                  mVMG_Watchdog;
    // Bearing compass + TWA/TWD calculation
    wxMenu               *m_pmenu;
    double               mHdt;
    double               mStW;
    double               mSOG;
    double               mCOG;
    double               mlat;
    double               mlon;
    double               mheel;
    double               msensorheel;
    double               mLeeway;
    double               m_calcTWS;
    double               m_calcTWA;
    double               m_calcTWD; //temp testing for Windbarb display
    wxString             mHeelUnit;
    wxString             mAWAUnit;
    wxString             mAWSUnit;
    double               mAWA;
    double               mAWS;
    double               mTWA;
    double               mTWD;
    double               mTWS;
    bool                 m_bTrueWind_available;
    bool                 m_bLaylinesIsVisible;
    bool                 m_bLaylinesIsVisibleSavedState;
    bool                 m_bDisplayCurrentOnChart;
    bool                 m_bDisplayCurrentOnChartSavedState;
    bool                 m_bShowWindbarbOnChart;
    bool                 m_bShowWindbarbOnChartSavedState;
    bool                 m_bShowPolarOnChart;
    bool                 m_bShowPolarOnChartSavedState;
    bool                 m_bPersistentChartPolarAnimation;
    bool                 m_LeewayOK;
    bool                 m_bNKE_TrueWindTableBug;
    double               m_VWR_AWA;
    double               alpha_currspd, alpha_CogHdt;
    double               m_ExpSmoothCurrSpd;
    double               m_ExpSmoothCurrDir;
    double               m_ExpSmoothSog;
    double               m_ExpSmoothSinCurrDir;
    double               m_ExpSmoothCosCurrDir;
    double               m_ExpSmoothDiffCogHdt;
    double               m_LaylineDegRange;
    double               m_COGRange[COGRANGE];
    double               m_ExpSmoothDegRange;
    double               m_LaylineSmoothedCog;
    double               m_ExpSmoothSinCog;
    double               m_ExpSmoothCosCog;
    //  double                m_alphaLaylineCog;
    //Performance variables
    double               mPolarTargetSpeed;
    double               mPredictedHdG;
    double               mPredictedCoG;
    double               mPredictedSoG;
    double               mPercentTargetVMGupwind;
    double               mPercentTargetVMGdownwind;
    TargetxMG            tvmg;
    TargetxMG            tcmg;
    double               mVMGGain;
    double               mCMGGain;
    double               mVMGoptAngle;
    double               mCMGoptAngle;
    double               mBRG;
    wxPoint              vpoints[3];
    wxPoint              tackpoints[3];
    double               m_CurrentDirection;

    DoubleExpSmooth     *mSinCurrDir;
    DoubleExpSmooth     *mCosCurrDir;
    ExpSmooth           *mExpSmoothCurrSpd;
    DoubleExpSmooth     *mExpSmoothSog;
    DoubleExpSmooth     *mExpSmSinCog;
    DoubleExpSmooth     *mExpSmCosCog;
    ExpSmooth           *mExpSmDegRange;
    ExpSmooth           *mExpSmDiffCogHdt;
    
    bool                 b_tactics_dc_message_shown = false;
    bool                 m_bToggledStateVisible;
    bool                 m_bToggledStateVisibleDefined;

    bool LoadConfig_CheckTacticsPlugin( wxFileConfig *pConf );
    void ImportStandaloneTacticsSettings ( wxFileConfig *pConf );
    bool StandaloneTacticsSettingsExists ( wxFileConfig *pConf );
    void LoadTacticsPluginBasePart ( wxFileConfig *pConf );
    void LoadTacticsPluginPerformancePart ( wxFileConfig *pConf );
    void SaveTacticsPluginBasePart ( wxFileConfig *pConf );
    void SaveTacticsPluginPerformancePart ( wxFileConfig *pConf );
    void ExportPerformanceData(void);
    void createPNKEP_NMEA(
        int sentence, double data1, double data2,
        double data3, double data4 );
    void SendNMEASentence( wxString sentence );
    wxString ComputeChecksum(wxString sentence);
};

class TacticsPreferencesDialog : public wxDialog
{
public:
    TacticsPreferencesDialog(
        wxWindow *pparent, wxWindowID id, const wxString derivtitle );
    ~TacticsPreferencesDialog() {}

    virtual void TacticsPreferencesInit(
        wxNotebook *itemNotebook, int border_size) final;
    virtual void TacticsPreferencesPanel(void) final;
    virtual void SaveTacticsConfig(void) final;

    void SelectPolarFile(wxCommandEvent& event);
    void OnAWSAWACorrectionUpdated(wxCommandEvent& event);
    void OnManualHeelUpdate(wxCommandEvent& event);

    wxNotebook                   *m_itemNotebook;
    int                           m_border_size;
    
    wxSpinCtrlDouble             *m_alphaDeltCoG; //TR
    wxSpinCtrlDouble             *m_alphaLaylineDampFactor;//TR
    wxSpinCtrl                   *m_minLayLineWidth;//TR
    wxSpinCtrl                   *m_maxLayLineWidth;//TR
    wxSpinCtrlDouble             *m_LeewayFactor;//TR
    wxSpinCtrl                   *m_AlphaCurrDir; //TR
    wxSpinCtrlDouble             *m_fixedLeeway;//TR
    wxButton                     *m_buttonLoadPolar;//TR
    wxButton                     *m_buttonPrefsApply;//TR
    wxButton                     *m_buttonPrefOK;//TR
    wxTextCtrl                   *m_pTextCtrlPolar; //TR
    wxSpinCtrlDouble             *m_pLaylineLength; //TR
    wxSpinCtrlDouble             *m_heel5_45;
    wxSpinCtrlDouble             *m_heel5_90;
    wxSpinCtrlDouble             *m_heel5_135;
    wxSpinCtrlDouble             *m_heel10_45;
    wxSpinCtrlDouble             *m_heel10_90;
    wxSpinCtrlDouble             *m_heel10_135;
    wxSpinCtrlDouble             *m_heel15_45;
    wxSpinCtrlDouble             *m_heel15_90;
    wxSpinCtrlDouble             *m_heel15_135;
    wxSpinCtrlDouble             *m_heel20_45;
    wxSpinCtrlDouble             *m_heel20_90;
    wxSpinCtrlDouble             *m_heel20_135;
    wxSpinCtrlDouble             *m_heel25_45;
    wxSpinCtrlDouble             *m_heel25_90;
    wxSpinCtrlDouble             *m_heel25_135;
    wxTextCtrl                   *m_UseHeelSensor;
    wxCheckBox                   *m_CurrentOnChart;
    wxRadioButton                *m_ButtonLeewayFactor;
    wxRadioButton                *m_ButtonFixedLeeway;
    wxRadioButton                *m_ButtonHeelInput;
    wxRadioButton                *m_ButtonUseHeelSensor;
    wxCheckBox                   *m_CorrectSTWwithLeeway;
    wxCheckBox                   *m_CorrectAWwithHeel;
    wxCheckBox                   *m_ForceTrueWindCalculation;
    wxCheckBox                   *m_UseSOGforTWCalc;
    wxCheckBox                   *m_ShowWindbarbOnChart;
    wxCheckBox                   *m_ShowPolarOnChart;
    wxRadioButton                *m_ButtonExpNKE;
    wxCheckBox                   *m_ExpPerfData01;
    wxCheckBox                   *m_ExpPerfData02;
    wxCheckBox                   *m_ExpPerfData03;
    wxCheckBox                   *m_ExpPerfData04;
    wxCheckBox                   *m_ExpPerfData05;
    wxCheckBox                   *m_PersistentChartPolarAnimation;
private:
    void UpdateTacticsButtonsState(void);
    void UpdateButtonsState(void);
    wxFileConfig     *m_pconfig;

    int                           curSel;
    wxListCtrl                   *m_pListCtrlTacticss;
    wxBitmapButton               *m_pButtonAddTactics;
    wxBitmapButton               *m_pButtonDeleteTactics;
    wxPanel                      *m_pPanelTactics;
    wxTextCtrl                   *m_pTextCtrlCaption;
    wxCheckBox                   *m_pCheckBoxIsVisible;
    wxChoice                     *m_pChoiceOrientation;
    wxListCtrl                   *m_pListCtrlInstruments;
    wxButton                     *m_pButtonAdd;
    wxButton                     *m_pButtonEdit;
    wxButton                     *m_pButtonDelete;
    wxButton                     *m_pButtonUp;
    wxButton                     *m_pButtonDown;
};

enum eIdDashTacticsContextMenu {
    ID_DASH_TACTICS_PREFS_START = 10000,
    ID_DASH_LAYLINE,
    ID_DASH_CURRENT,
    ID_DASH_POLAR,
    ID_DASH_WINDBARB,
    ID_DASH_TACTICS_PREFS_END
};

class TacticsWindow : public wxWindow
{
public:
    TacticsWindow(
        wxWindow *pparent, wxWindowID id,
        tactics_pi *tactics, const wxString derivtitle );
    ~TacticsWindow();

    virtual void InsertTacticsIntoContextMenu (
        wxMenu *contextMenu ) final;
    virtual void TacticsInContextMenuAction (
        const int eventId ) final;


private:

    tactics_pi*         m_plugin;

};

#endif
