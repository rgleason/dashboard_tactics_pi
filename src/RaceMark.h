/******************************************************************************
* $Id: RaceMark.h, v1.0 2019/11/30 VaderDarth Exp $
*
* Project:  OpenCPN
* Purpose:  dahboard_tactics_pi plug-in
* Author:   Jean-Eudes Onfray
*
***************************************************************************
*   Copyright (C) 2010 by David S. Register   *
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
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
***************************************************************************
*/

#ifndef __RACEMARK_H__
#define __RACEMARK_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#define __DERIVEDTIMEOUTJS_OVERRIDE__
#include "InstruJS.h"

#include "ocpn_plugin.h"

/*
  The default window size value are depending both of the HTML-file
  and the JavaScript C++ instrument, here we have set the values to
  the JustGauge scalable, SVG-drawn instrument.
  See enginedjg.css: skpath, gauge and bottom classes for the ratios.
*/
#ifdef __WXMSW__
#define RACEMARK_V_TITLEH    18
#define RACEMARK_V_WIDTH    900
#define RACEMARK_V_HEIGHT   200
#define RACEMARK_V_BOTTOM    25
#define RACEMARK_H_TITLEH    19
#define RACEMARK_H_WIDTH    900
#define RACEMARK_H_HEIGHT   200
#define RACEMARK_H_BOTTOM    25
#else
#define RACEMARK_V_TITLEH    18
#define RACEMARK_V_WIDTH    900
#define RACEMARK_V_HEIGHT   200
#define RACEMARK_V_BOTTOM    25
#define RACEMARK_H_TITLEH    19
#define RACEMARK_H_WIDTH    900
#define RACEMARK_H_HEIGHT   200
#define RACEMARK_H_BOTTOM    25
#endif // ifdef __WXMSW__
#define RACEMARK_V_MIN_WIDTH  RACEMARK_V_WIDTH
#define RACEMARK_V_MIN_HEIGHT (RACEMARK_V_TITLEH + RACEMARK_V_HEIGHT + RACEMARK_V_BOTTOM)
#define RACEMARK_H_MIN_WIDTH  RACEMARK_H_WIDTH
#define RACEMARK_H_MIN_HEIGHT (RACEMARK_H_TITLEH + RACEMARK_H_HEIGHT + RACEMARK_H_BOTTOM)

#define RACEMARK_WAIT_NEW_HTTP_SERVER_TICKS 2 // if initially no server

// These values for first time start only
#define RACEMARK_RUNGLINE_WIDTH 2 // pen width
#define RACEMARK_LADDER_RUNG_STEP 0.053996 // 100 meters in nautical mile
#define RACEMARK_LADDER_RUNG_STEP_MINIMUM 0.00486 // 9 meters in nautical miles
#define RACEMARK_AVGWINDLINE_WIDTH 3 // pen width
#define RACEMARK_SHORTAVGWINDRUNGLINE_WIDTH 1 // pen width

//+------------------------------------------------------------------------
//|
//| CLASS:
//|    DashboardInstrument_RaceMark
//|
//| DESCRIPTION:
//|    This instrument provides assistance for race mark approach
//+------------------------------------------------------------------------

class DashboardInstrument_RaceMark : public InstruJS
{
public:
    DashboardInstrument_RaceMark(
        TacticsWindow *pparent, wxWindowID id, wxString ids,
        PI_ColorScheme cs, wxString format = "" );
    ~DashboardInstrument_RaceMark(void);
    void SetData(unsigned long long, double, wxString, long long timestamp=0LL ){;};
#ifndef __RACEMARK_DERIVEDTIMEOUT_OVERRIDE__
    virtual void derived2TimeoutEvent(void){};
#else
    virtual void derived2TimeoutEvent(void) = 0;
#endif // __RACEMARK_DERIVEDTIMEOUT_OVERRIDE__
    virtual void derivedTimeoutEvent(void);
    
    bool CheckForValidActiveRoute(void);
    bool CheckRouteStillValid(void);
    bool CheckForActivatedWp(void);
    bool UpdatePreviousWpFromNewTarget(void);
    int  PollForNextActiveRouteWp(void);
    bool ChangeToNextTargetMark(void);
    bool CalculateBearingFromNewTargetToPreviousMark(void);
    bool ChangeNextAndNextNextMarks(void);
    bool PeekTwaOnNextAndNextNextLegs(void);
    bool CheckWpStillValid( wxString sGUID );

    virtual bool instruIsReady(void) override;
    virtual bool userHasActiveRoute(void) override;
    virtual bool sendRmData(void) override;
    virtual bool stopRmData(void) override;
    virtual void getRmData(
        wxString& nextLegTwaAvg, wxString& nextLegTwaShortAvg,
        wxString& nextNextLegTwaAvg, wxString& nextNextLegTwaShortAvg )
        override;

    virtual wxSize GetSize( int orient, wxSize hint ) override;
    void DoRenderGLOverLay(wxGLContext* pcontext, PlugIn_ViewPort* vp );
    virtual void PushTwaHere(
        double data, wxString unit, long long timestamp=0LL);
    virtual void PushTwsHere(
        double data, wxString unit, long long timestamp=0LL);
    virtual void PushCogHere(
        double data, wxString unit, long long timestamp=0LL);
    virtual void PushLatHere(
        double data, wxString unit, long long timestamp=0LL);
    virtual void PushLonHere(
        double data, wxString unit, long long timestamp=0LL);
    
protected:
    TacticsWindow       *m_pparent;
    int                  m_orient;
    bool                 m_htmlLoaded;
    wxTimer             *m_pThreadRaceMarkTimer;
    int                  m_goodHttpServerDetects;
    wxFileConfig        *m_pconfig;
    wxString             m_fullPathHTML;
    wxString             m_httpServer;
    wxString             m_raceAsRouteGuid;
    wxString             m_raceAsRouteName;
    PlugIn_Route        *m_raceAsRoute; // per method ptr, tells a route exists
    wxString             m_targetWpName;
    wxString             m_targetWpGuid;
    PlugIn_Waypoint     *m_targetWp;
    wxString             m_previousWpName;
    wxString             m_previousWpGuid;
    PlugIn_Waypoint     *m_previousWp;
    double               m_previousWpBearing; // note: from target to previous
    double               m_previousWpDistance;
    wxString             m_nextWpName;
    wxString             m_nextWpGuid;
    PlugIn_Waypoint     *m_nextWp;
    double               m_nextWpLegBearing;
    double               m_nextWpLegDistance;
    double               m_nextWpLegAvgTwa;
    double               m_nextWpLegShortAvgTwa;
    wxString             m_nextNextWpName;
    wxString             m_nextNextWpGuid;
    PlugIn_Waypoint     *m_nextNextWp;
    double               m_nextNextWpLegBearing;
    double               m_nextNextWpLegDistance;
    double               m_nextNextWpLegAvgTwa;
    double               m_nextNextWpLegShortAvgTwa;
    bool                 m_dataRequestOn;
    bool                 m_overlayPauseRequestOn;
    bool                 m_jsCallBackAsHeartBeat;
    glRendererFunction   m_rendererIsHere;
    wxString             m_rendererCallbackUUID;

    bool                 m_renDrawLaylines;
    int                  m_renLaylineWidth;
    bool                 m_renDrawRungs;
    int                  m_renRungLineWidth;
    double               m_renRungStep;
    bool                 m_renDrawAvgWind;
    int                  m_renAvgWindLineWidth;
    bool                 m_renDrawShortAvgWindRungs;
    int                  m_renShortAvgWindRungLineWidth;
    double               m_renAvgWindDir;
    double               m_renAvgWindDirWindward;
    double               m_renShortAvgWindDir;
    double               m_renShortAvgWindDirWindward;
    double               m_renTargetPoint_lat;
    double               m_renTargetPoint_lon;
    wxPoint              m_renTargetPoint;
    wxPoint              m_renAvgWindLineEndPoint;
    wxPoint              m_renShortAvgWindLineEndPoint;
    double               m_renAvgWindRightPlaneDir;
    double               m_renShortAvgWindRightPlaneDir;
    double               m_renAvgWindLeftPlaneDir;
    double               m_renShortAvgWindLeftPlaneDir;
    double               m_renLLlen;
    double               m_renLLStbdDir;
    wxPoint              m_renLLStbdEndPoint;
    double               m_renShortLLStbdDir;
    wxPoint              m_renShortLLStbdEndPoint;
    double               m_renLLPortDir;
    wxPoint              m_renLLPortEndPoint;
    double               m_renShortLLPortDir;
    wxPoint              m_renShortLLPortEndPoint;
    wxPoint              m_renShortAvgWindLeftCrossPoint;
    wxPoint              m_renShortAvgWindRightCrossPoint;
    bool                 m_renWindwardLeg;
    bool                 m_renReachingLeg;
    
    callbackFunction     m_fPushLonHere;
    wxString             m_fPushLonUUID;
    double               m_Lon;
    callbackFunction     m_fPushLatHere;
    wxString             m_fPushLatUUID;
    double               m_Lat;
    callbackFunction     m_fPushCogHere;
    wxString             m_fPushCogUUID;
    double               m_Cog;
    callbackFunction     m_fPushTwsHere;
    wxString             m_fPushTwsUUID;
    double               m_Tws;
    callbackFunction     m_fPushTwaHere;
    wxString             m_fPushTwaUUID;
    double               m_Twa;


    wxDECLARE_EVENT_TABLE();

    void ClearRoutesAndWPs( bool ctor = false );
    void ClearNextAndNextNextWpsOnly( bool ctor = false );
    void ClearRendererCalcs(void);
    void OnThreadTimerTick(wxTimerEvent& event);
    void OnClose(wxCloseEvent& event);
    bool LoadConfig(void);
    void SaveConfig(void);
    bool IsAllMeasurementDataValid(void);
    bool WindRenderingConditions(void);
    void RenderGLAvgWindToWp( wxGLContext* pcontext, PlugIn_ViewPort* vp );
    void RenderGLLaylinesOnTargetWp(
        wxGLContext* pcontext, PlugIn_ViewPort* vp );
    void RenderGLAvgWindLadderRungs(
        wxGLContext* pcontext, PlugIn_ViewPort* vp );
    void RenderGLShortAvgWindLadderRungs(
        wxGLContext* pcontext, PlugIn_ViewPort* vp );

};

#endif // __RACEMARK_H__
