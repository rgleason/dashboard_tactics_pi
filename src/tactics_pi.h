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

#include <mutex>

#include "ocpn_plugin.h"

#include "TacticsStructs.h"
#include "ExpSmooth.h"
#include "DoubleExpSmooth.h"

#include "SkData.h"

#define aws_watchdog_timeout_ticks 10
#define brg_watchdog_timeout_ticks 10
#define twd_watchdog_timeout_ticks 10
#define tws_watchdog_timeout_ticks 10
#define vmg_watchdog_timeout_ticks 10
#define CURR_RECORD_COUNT 20
#define COGRANGE 60

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
    virtual void ApplyConfig(bool init = false) = 0;
    virtual void TacticsApplyConfig(void) final;
    virtual bool SaveConfig(void) = 0;
    virtual bool TacticsSaveConfig(void) final;
    virtual bool RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp) = 0;
    virtual void UpdateAuiStatus(void) = 0;
    virtual void SetToggledStateVisible(bool isvisible) final;
    virtual void callAllRegisteredGLRenderers(
        wxGLContext* pcontext, PlugIn_ViewPort* vp,
        wxString className = wxEmptyString ) = 0;
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
    bool IsConfigSetToForcedTrueWindCalculation(void);

    virtual void SetNMEASentence(
        wxString& sentence, wxString* type=NULL, wxString* sentenceId=NULL, wxString* talker=NULL,
        wxString* src=NULL, int pgn=0, wxString* path=NULL, double value=NAN, wxString* valStr=NULL,
        long long timestamp=0LL, wxString* key=NULL) = 0;
    virtual void SetUpdateSignalK(
        wxString* type, wxString* sentenceId, wxString* talker, wxString* src, int pgn,
        wxString* path, double value, wxString* valStr, long long timestamp, wxString* key=NULL ) final;
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
        unsigned long long st, double value, wxString unit, long long timestamp=0LL ) = 0;
    virtual void pSendSentenceToAllInstruments(
        unsigned long long st, double value, wxString unit, long long timestamp=0LL) = 0;
    virtual bool getIsUntrustedLocalTime(void) = 0;
    virtual wxDateTime getGNSSuTCDateTime(void) = 0;
    virtual wxLongLong getGNSSreceivedAtLocalMs(void) = 0;
    virtual long long getmUTCRealGpsEpoch(void) = 0;
    virtual void SendPerfSentenceToAllInstruments(
        unsigned long long st, double value, wxString unit, long long timestamp ) final;
    virtual bool SendSentenceToAllInstruments_PerformanceCorrections(
        unsigned long long st, double &value, wxString &unit ) final;
    virtual bool SendSentenceToAllInstruments_LaunchTrueWindCalculations(
        unsigned long long st, double value ) final;
    virtual bool SendSentenceToAllInstruments_GetCalculatedTrueWind(
        unsigned long long st, double value, wxString unit,
        unsigned long long &st_twa, double &value_twa, wxString &unit_twa,
        unsigned long long &st_tws, unsigned long long &st_tws2, double &value_tws, wxString &unit_tws,
        unsigned long long &st_twd, double &value_twd, wxString &unit_twd,
        long long &calctimestamp
        ) final;
    virtual bool SendSentenceToAllInstruments_GetCalculatedLeeway(
        unsigned long long &st_leeway, double &value_leeway,
        wxString &unit_leeway, long long &calctimestamp ) final;
    virtual bool SendSentenceToAllInstruments_GetCalculatedCurrent(
        unsigned long long st, double value, wxString unit,
        unsigned long long &st_currdir, double &value_currdir,
        wxString &unit_currdir,
        unsigned long long &st_currspd, double &value_currspd,
        wxString &unit_currspd, long long &calctimestamp) final;
    virtual wxString GetActiveRouteName() = 0;
    virtual wxString GetActiveRouteGUID() = 0;
    virtual wxString GetWpActivatedName() = 0;
    virtual wxString GetWpActivatedGUID() = 0;
    virtual bool GetWpArrivedIsSkipped() = 0;
    virtual wxString GetWpArrivedName() = 0;
    virtual wxString GetWpArrivedGUID() = 0;
    virtual wxString GetWpArrivedNextName() = 0;
    virtual wxString GetWpArrivedNextGUID() = 0;
    virtual Plugin_Active_Leg_Info* GetActiveLegInfoPtr() = 0;

    virtual void OnAvgWindUpdTimer_Tactics(void) final;

    static wxString get_sCMGSynonym(void);
    static wxString get_sVMGSynonym(void);
    void set_m_bDisplayCurrentOnChart(bool value) {m_bDisplayCurrentOnChart = value;}

    bool getTacticsDCmsgShown(void) { return b_tactics_dc_message_shown; };
    void setTacticsDCmsgShownTrue(void) { b_tactics_dc_message_shown = true; };
    
protected:
    SkData              *m_pSkData;
    
private:
    opencpn_plugin      *m_hostplugin;
    wxFileConfig        *m_hostplugin_pconfig;
    wxString             m_hostplugin_config_path;
    wxString             m_this_config_path;

    int                  mBRG_Watchdog;
    int                  mTWD_Watchdog;
    std::mutex           mtxTWD;
    int                  mTWS_Watchdog;
    std::mutex           mtxTWS; // both mTWA and mTWS
    int                  mAWS_Watchdog;
    std::mutex           mtxAWS;
    int                  mVMG_Watchdog;
    std::mutex           mtxHdt;
    std::mutex           mtxBRG;
    // Bearing compass + TWA/TWD calculation
    double               mHdt;
    double               mStW;
    double               mStWnocorr;
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
    double               mAWAnocorr;
    double               mAWS;
    double               mAWSnocorr;
    double               mTWA;
    double               mTWD;
    double               mTWS;
    bool                 m_bTrueWindAngle_available;
    bool                 m_bTrueWindSpeed_available;
    bool                 m_bTrueWindDirection_available;
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
    double               alpha_currspd;
    double               alpha_CogHdt;
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
    double               m_SmoothedpredCog;
    double               m_ExpSmoothSinpredCog;
    double               m_ExpSmoothCospredCog;
    double               m_ExpSmcur_tacklinedir;
    double               m_ExpSmtarget_tacklinedir;
    double               m_ExpSmoothSincur_tacklinedir;
    double               m_ExpSmoothCoscur_tacklinedir;
    double               m_ExpSmoothSintarget_tacklinedir;
    double               m_ExpSmoothCostarget_tacklinedir;
    
    //Performance variables
    double               mPolarTargetSpeed;
    double               mPredictedHdG;
    double               mPredictedCoG;
    double               mPredictedSoG;
    double               mPercentTargetVMGupwind;
    double               mPercentTargetVMGdownwind;
    double               mPercentUserTargetSpeed;
    TargetxMG            tvmg;
    TargetxMG            tcmg;
    double               mVMGGain;
    double               mCMGGain;
    double               mVMGoptAngle;
    double               mCMGoptAngle;
    double               mBRG;
    double               mBRGnocorr;
    wxPoint              vpoints[3];
    wxPoint              tackpoints[3];
    double               m_CurrentDirection;

    DoubleExpSmooth     *mSinCurrDir;
    DoubleExpSmooth     *mCosCurrDir;
    ExpSmooth           *mExpSmoothCurrSpd;
    DoubleExpSmooth     *mExpSmoothSog;
    DoubleExpSmooth     *mExpSmSinCog;
    DoubleExpSmooth     *mExpSmCosCog;
    //double exp.smoothing of predicted Cog for boat laylines
    DoubleExpSmooth     *mExpSmSinpredCog;
    DoubleExpSmooth     *mExpSmCospredCog;
    //double exp.smoothing of predicted Cog for Tactics WP laylines
    DoubleExpSmooth     *mExpSmSincur_tacklinedir;
    DoubleExpSmooth     *mExpSmCoscur_tacklinedir;
    DoubleExpSmooth     *mExpSmSintarget_tacklinedir;
    DoubleExpSmooth     *mExpSmCostarget_tacklinedir;
    
    ExpSmooth           *mExpSmDegRange;
    ExpSmooth           *mExpSmDiffCogHdt;

    bool                 b_tactics_dc_message_shown;
    bool                 m_bToggledStateVisible;
    bool                 m_bToggledStateVisibleDefined;
    int                  m_iDbgRes_TW_Calc_TW_Available;
    int                  m_iDbgRes_TW_Calc_AWS_STC;
    int                  m_iDbgRes_TW_Calc_AWS;
    int                  m_iDbgRes_TW_Calc_Force;
    int                  m_iDbgRes_TW_Calc_AWA;
    int                  m_iDbgRes_TW_Calc_AWAUnit;
    int                  m_iDbgRes_TW_Calc_Hdt;
    int                  m_iDbgRes_TW_Calc_SOG;
    int                  m_iDbgRes_TW_Calc_StW;
    int                  m_iDbgRes_TW_Calc_Lau;
    int                  m_iDbgRes_TW_Calc_Exe;

    wxMenu              *m_pmenu;

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

enum eIdDashTacticsContextMenu {
    ID_DASH_TACTICS_PREFS_START = 10000,
    ID_DASH_LAYLINE,
    ID_DASH_CURRENT,
    ID_DASH_POLAR,
    ID_DASH_WINDBARB,
    ID_DASH_TACTICS_PREFS_END
};

#endif
