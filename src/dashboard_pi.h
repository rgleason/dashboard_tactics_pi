/******************************************************************************
 * $Id: dashboard_pi.h, v1.0 2010/08/05 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Jean-Eudes Onfray
 *
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

#ifndef _DASHBOARDPI_H_
#define _DASHBOARDPI_H_

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
#include <wx/unichar.h>
//wx2.9 #include <wx/wrapsizer.h>
#include "ocpn_plugin.h"

#ifdef _INCLUDE_TACTICS_PI_
#ifndef _TACTICSPI_H_
#include "tactics_pi.h"
#endif // _TACTICSPI_H_
#endif // _INCLUDE_TACTICS_PI_

#ifdef _TACTICSPI_H_
#include "version.h"
#else
#define     PLUGIN_VERSION_MAJOR    1
#define     PLUGIN_VERSION_MINOR    5
#endif // _TACTICSPI_H_

#define     MY_API_VERSION_MAJOR    1
#ifdef _TACTICSPI_H_
#define     MY_API_VERSION_MINOR    12
#else
#define     MY_API_VERSION_MINOR    6
#endif // _TACTICSPI_H_

#include "nmea0183/nmea0183.h"
#include "instrument.h"
#include "speedometer.h"
#include "compass.h"
#include "wind.h"
#include "rudder_angle.h"
#include "gps.h"
#include "depth.h"
#include "clock.h"
#include "wind_history.h"
#include "baro_history.h"
#include "from_ownship.h"
#include "iirfilter.h"
#ifdef _TACTICSPI_H_
#include "enginei.h"
#include "enginedjg.h"
#endif // _TACTICSPI_H_

class DashboardWindow;
class DashboardWindowContainer;
class DashboardInstrumentContainer;

#define DASHBOARD_TOOL_POSITION -1          // Request default positioning of toolbar tool

#define gps_watchdog_timeout_ticks  10

#ifdef _TACTICSPI_H_
// Incoming NMEA-2000 PGNs Dashboard inspects
#define PGN_ENG_PARAM_RAP 127488
#define PGN_ENG_PARAM_DYN 127489 
// Signal K conversions, see https://git.io/JeYry
#define DEG_IN_RAD 0.0174532925
#define RAD_IN_DEG 57.2957795
#define CELCIUS_IN_KELVIN 273.15
#define MS_IN_KNOTS 1.943844
#define KM_IN_NM 0.539956803
#define PA_IN_BAR 100000
#endif // _TACTICSPI_H_

class DashboardWindowContainer
{
public:
    DashboardWindowContainer(DashboardWindow *dashboard_window, wxString name, wxString caption, wxString orientation, wxArrayInt inst) {
        m_pDashboardWindow = dashboard_window; m_sName = name; m_sCaption = caption; m_sOrientation = orientation; m_aInstrumentList = inst; m_bIsVisible = false; m_bIsDeleted = false; m_bIsDocked = false; }

#ifdef _TACTICSPI_H_
    DashboardWindowContainer( DashboardWindowContainer *sourcecont ) {
            m_pDashboardWindow = sourcecont->m_pDashboardWindow;
            m_bIsVisible       = sourcecont->m_bIsVisible;
            m_bIsDeleted       = sourcecont->m_bIsDeleted;
            m_bIsDocked        = sourcecont->m_bIsDocked;
            m_bPersVisible     = sourcecont->m_bPersVisible;
            m_sName            = sourcecont->m_sName;
            m_sCaption         = sourcecont->m_sCaption;
            m_sOrientation     = sourcecont->m_sOrientation;
            m_aInstrumentList  = sourcecont->m_aInstrumentList;
    }
#endif // _TACTICSPI_H_
 
    ~DashboardWindowContainer(){}

DashboardWindow              *m_pDashboardWindow;
    bool                      m_bIsVisible;
    bool                      m_bIsDeleted;
    bool                      m_bPersVisible;  // Persists visibility, even when Dashboard tool is toggled off.
    bool                      m_bIsDocked;
    wxString                  m_sName;
    wxString                  m_sCaption;
    wxString                  m_sOrientation;
    wxArrayInt                m_aInstrumentList;
};

class DashboardInstrumentContainer
{
public:
#ifdef _TACTICSPI_H_
    DashboardInstrumentContainer(int id, DashboardInstrument *instrument,
                                 unsigned long long capa)
        {
            m_ID = id; m_pInstrument = instrument; m_cap_flag = capa;
        };
#else
    DashboardInstrumentContainer(int id, DashboardInstrument *instrument, int capa )
        {
            m_ID = id; m_pInstrument = instrument; m_cap_flag = capa;
        };
#endif // _TACTICSPI_H_
    ~DashboardInstrumentContainer(){ delete m_pInstrument; };
    DashboardInstrument    *m_pInstrument;
    int                     m_ID;
#ifdef _TACTICSPI_H_
    unsigned long long      m_cap_flag;
#else
    int                     m_cap_flag;
#endif // _TACTICSPI_H_
};

//    Dynamic arrays of pointers need explicit macros in wx261
#ifdef __WX261
WX_DEFINE_ARRAY_PTR(DashboardWindowContainer *, wxArrayOfDashboard);
WX_DEFINE_ARRAY_PTR(DashboardInstrumentContainer *, wxArrayOfInstrument);
#else
WX_DEFINE_ARRAY(DashboardWindowContainer *, wxArrayOfDashboard);
WX_DEFINE_ARRAY(DashboardInstrumentContainer *, wxArrayOfInstrument);
#endif

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------


class dashboard_pi : public
#ifdef _TACTICSPI_H_
    tactics_pi, wxTimer, opencpn_plugin_112
#else
    wxTimer, opencpn_plugin_16
#endif // _TACTICSPI_H_
{
public:
    dashboard_pi(void *ppimgr);
    ~dashboard_pi(void);

    //    The required PlugIn Methods
    int Init(void);
    bool DeInit(void);

    void Notify();

    int GetAPIVersionMajor();
    int GetAPIVersionMinor();
    int GetPlugInVersionMajor();
    int GetPlugInVersionMinor();
    wxBitmap *GetPlugInBitmap();
    wxString GetCommonName();
    wxString GetShortDescription();
    wxString GetLongDescription();

#ifdef _TACTICSPI_H_
    wxString GetStandardPath();
    // implementation of parent classes methods (w/ call-backs)
    void OnContextMenuItemCallback(int id);
    void SendSentenceToAllInstruments(
        unsigned long long st, double value, wxString unit, long long timestamp=0LL);
    void pSendSentenceToAllInstruments(
        unsigned long long st, double value, wxString unit, long long timestamp=0LL);
    void SendDataToAllPathSubscribers(
        wxString path, double value, wxString unit, long long timestamp );
#else
    void SendSentenceToAllInstruments(
        int st, double value, wxString unit);
#endif // _TACTICSPI_H_
#ifdef _TACTICSPI_H_
    bool RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp);
    bool RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
    void OnAvgWindUpdTimer(wxTimerEvent& event);
    void OnAuiRender( wxAuiManagerEvent& event );
#endif // _TACTICSPI_H_

    //    The optional method overrides
    void SetNMEASentence(wxString &sentence);
#ifdef _TACTICSPI_H_
    void SetNMEASentence(
        wxString& sentence, wxString* type=NULL, wxString* sentenceId=NULL, wxString* talker=NULL,
        wxString* src=NULL, int pgn=0, wxString* path=NULL, double value=NAN, wxString* valStr=NULL,
        long long timestamp=0LL, wxString* key=NULL);
#endif // _TACTICSPI_H_
    void SetPositionFix(PlugIn_Position_Fix &pfix);
    void SetCursorLatLon(double lat, double lon);
    int GetToolbarToolCount(void);
    void OnToolbarToolCallback(int id);
    void ShowPreferencesDialog( wxWindow* parent );
    void SetColorScheme(PI_ColorScheme cs);
    void OnPaneClose( wxAuiManagerEvent& event );
    void UpdateAuiStatus(void);
    bool SaveConfig(void);
    void PopulateContextMenu( wxMenu* menu );
    void ShowDashboard( size_t id, bool visible );
    int GetToolbarItemId(){ return m_toolbar_item_id; }
    int GetDashboardWindowShownCount();
    void SetPluginMessage(wxString &message_id, wxString &message_body);
#ifdef _TACTICSPI_H_
    wxWindow *pGetPluginFrame(void) { return m_pluginFrame; }
    void ApplyConfig( bool init=false );
    void SetApplySaveWinRequest(void) { mApS_Watchcat = 2; }
    bool ApplySaveWinRequested(void) { return (((mApS_Watchcat!=0)?true:false)); }
#endif // _TACTICSPI_H_

#ifdef _TACTICSPI_H_
    int                m_nofStreamOut;
    std::mutex         m_mtxNofStreamOut;
    wxString           m_echoStreamerShow;
    int                m_nofStreamInSk;
    std::mutex         m_mtxNofStreamInSk;
    wxString           m_echoStreamerInSkShow;
    sigPathLangVector  m_sigPathLangVector;
#endif // _TACTICSPI_H_
    
private:
    bool LoadConfig(void);
#ifndef _TACTICSPI_H_
    void ApplyConfig(void);
#endif // _TACTICSPI_H_
#ifdef _TACTICSPI_H_
    wxString GetCommonNameVersion(void);  
    wxString GetNameVersion(void);  
#endif // _TACTICSPI_H_ 

    void SendSatInfoToAllInstruments(int cnt, int seq, SAT_INFO sats[4]);
    void SendUtcTimeToAllInstruments( wxDateTime value );

#ifdef _TACTICSPI_H_
    bool                 m_bToggledStateVisible;
    int                  m_iPlugInRequirements;
    wxWindow            *m_pluginFrame;
    static const char   *s_common_name;
    wxTimer             *m_avgWindUpdTimer;
#endif // _TACTICSPI_H_
    wxFileConfig        *m_pconfig;
    wxAuiManager        *m_pauimgr;
    int                  m_toolbar_item_id;

    wxArrayOfDashboard   m_ArrayOfDashboardWindow;
    int                  m_show_id;
    int                  m_hide_id;

    NMEA0183             m_NMEA0183;                 // Used to parse NMEA Sentences
    short                mPriPosition;
    short                mPriCOGSOG;
    short                mPriHeadingM;
    short                mPriHeadingT;
    short                mPriVar;
    short                mPriDateTime;
    short                mPriAWA;
    short                mPriTWA;
    short                mPriDepth;
    double               mVar;
    // FFU
    double               mSatsInView;
    double               mHdm;
    wxDateTime           mUTCDateTime;
    int                  m_config_version;
    wxString             m_VDO_accumulator;
    int                  mHDx_Watchdog;
    int                  mHDT_Watchdog;
    int                  mGPS_Watchdog;
    int                  mVar_Watchdog;
#ifdef _TACTICSPI_H_
    int                  mStW_Watchdog;
    int                  mSiK_Watchdog;
    bool                 mSiK_DPT_environmentDepthBelowKeel;
    int                  mSiK_navigationGnssMethodQuality;
    int                  mApS_Watchcat;
#endif // _TACTICSPI_H_

    iirfilter            mSOGFilter;
    iirfilter            mCOGFilter;

protected:
#ifdef _TACTICSPI_H_
    DECLARE_EVENT_TABLE();
#endif // _TACTICSPI_H_
};
  
class DashboardPreferencesDialog : public
#ifdef _TACTICSPI_H_
    TacticsPreferencesDialog
#else
    wxDialog
#endif // _TACTICSPI_H_
{
public:
    DashboardPreferencesDialog( wxWindow *pparent, wxWindowID id, wxArrayOfDashboard config
#ifdef _TACTICSPI_H_
                                , wxString commonName, wxString nameVersion, wxPoint pos = wxDefaultPosition
#endif // _TACTICSPI_H_
        );
    ~DashboardPreferencesDialog() {}

    void OnCloseDialog(wxCloseEvent& event);
    void OnDashboardSelected(wxListEvent& event);
    void OnDashboardAdd(wxCommandEvent& event);
    void OnDashboardDelete(wxCommandEvent& event);
    void OnInstrumentSelected(wxListEvent& event);
    void OnInstrumentAdd(wxCommandEvent& event);
    void OnInstrumentEdit(wxCommandEvent& event);
    void OnInstrumentDelete(wxCommandEvent& event);
    void OnInstrumentUp(wxCommandEvent& event);
    void OnInstrumentDown(wxCommandEvent& event);
    void SaveDashboardConfig();

    wxArrayOfDashboard            m_Config;
#ifdef _TACTICSPI_H_
    wxNotebook                   *m_itemNotebook;
#endif // _TACTICSPI_H_
    wxFontPickerCtrl             *m_pFontPickerTitle;
    wxFontPickerCtrl             *m_pFontPickerData;
    wxFontPickerCtrl             *m_pFontPickerLabel;
    wxFontPickerCtrl             *m_pFontPickerSmall;
    wxSpinCtrl                   *m_pSpinSpeedMax;
    wxSpinCtrl                   *m_pSpinCOGDamp;
    wxSpinCtrl                   *m_pSpinSOGDamp;
    wxChoice                     *m_pChoiceUTCOffset;
    wxChoice                     *m_pChoiceSpeedUnit;
    wxChoice                     *m_pChoiceDepthUnit;
    wxSpinCtrlDouble             *m_pSpinDBTOffset;
    wxChoice                     *m_pChoiceDistanceUnit;
    wxChoice                     *m_pChoiceWindSpeedUnit;
#ifdef _TACTICSPI_H_
    wxChoice                     *m_pChoiceTemperatureUnit;
#endif // _TACTICSPI_H_

private:
    void UpdateDashboardButtonsState(void);
    void UpdateButtonsState(void);
    int                           curSel;
    wxListCtrl                   *m_pListCtrlDashboards;
    wxBitmapButton               *m_pButtonAddDashboard;
    wxBitmapButton               *m_pButtonDeleteDashboard;
    wxPanel                      *m_pPanelDashboard;
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

class AddInstrumentDlg : public wxDialog
{
public:
    AddInstrumentDlg(wxWindow *pparent, wxWindowID id);
    ~AddInstrumentDlg() {}

    unsigned int GetInstrumentAdded();

private:
    wxListCtrl*                   m_pListCtrlInstruments;

};

enum
{
    ID_DASHBOARD_WINDOW
};

enum
{
    ID_DASH_PREFS = 999,
    ID_DASH_VERTICAL,
    ID_DASH_HORIZONTAL,
    ID_DASH_UNDOCK
};

class DashboardWindow : public
#ifdef _TACTICSPI_H_
    TacticsWindow
#else
    wxWindow
#endif // _TACTICSPI_H_
{
public:
    DashboardWindow(
        wxWindow *pparent, wxWindowID id, wxAuiManager *auimgr,
        dashboard_pi* plugin,
        int orient, DashboardWindowContainer* mycont
#ifdef _TACTICSPI_H_
        , wxString commonName
#endif // _TACTICSPI_H_
        );
    ~DashboardWindow();

    void SetColorScheme( PI_ColorScheme cs );
    void SetSizerOrientation( int orient );
    int GetSizerOrientation();
    void OnSize( wxSizeEvent& evt );
    void OnContextMenu( wxContextMenuEvent& evt );
    void OnContextMenuSelect( wxCommandEvent& evt );
    bool isInstrumentListEqual( const wxArrayInt& list );
    void SetInstrumentList( wxArrayInt list );
    void SendSentenceToAllInstruments(
#ifdef _TACTICSPI_H_
        unsigned long long st,
#else
        int st,
#endif // _TACTICSPI_H_
       double value, wxString unit
#ifdef _TACTICSPI_H_
        , long long timestamp=0LL
#endif // _TACTICSPI_H_
        );
    void SendSatInfoToAllInstruments( int cnt, int seq, SAT_INFO sats[4] );
    void SendUtcTimeToAllInstruments( wxDateTime value );
    void ChangePaneOrientation( int orient, bool updateAUImgr );
    /*TODO: OnKeyPress pass event to main window or disable focus*/

    DashboardWindowContainer* m_Container;

#ifdef _TACTICSPI_H_
protected:
    wxDECLARE_EVENT_TABLE();
    void OnClose(wxCloseEvent& evt);
#endif // _TACTICSPI_H_

private:
    wxAuiManager         *m_pauimgr;
    dashboard_pi*         m_plugin;

    //wx2.9      wxWrapSizer*          itemBoxSizer;
    wxBoxSizer*          itemBoxSizer;
    wxArrayOfInstrument  m_ArrayOfInstrument;
};

#endif
