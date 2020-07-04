/******************************************************************************
* $Id: RaceStart.h, v1.0 2019/11/30 VaderDarth Exp $
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

#ifndef __RACESTART_H__
#define __RACESTART_H__

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
#define RACESTART_V_TITLEH    18
#define RACESTART_V_WIDTH    900
#define RACESTART_V_HEIGHT   200
#define RACESTART_V_BOTTOM    25
#define RACESTART_H_TITLEH    19
#define RACESTART_H_WIDTH    900
#define RACESTART_H_HEIGHT   200
#define RACESTART_H_BOTTOM    25
#else
#define RACESTART_V_TITLEH    18
#define RACESTART_V_WIDTH    900
#define RACESTART_V_HEIGHT   200
#define RACESTART_V_BOTTOM    25
#define RACESTART_H_TITLEH    19
#define RACESTART_H_WIDTH    900
#define RACESTART_H_HEIGHT   200
#define RACESTART_H_BOTTOM    25
#endif // ifdef __WXMSW__
#define RACESTART_V_MIN_WIDTH  RACESTART_V_WIDTH
#define RACESTART_V_MIN_HEIGHT (RACESTART_V_TITLEH + RACESTART_V_HEIGHT + RACESTART_V_BOTTOM)
#define RACESTART_H_MIN_WIDTH  RACESTART_H_WIDTH
#define RACESTART_H_MIN_HEIGHT (RACESTART_H_TITLEH + RACESTART_H_HEIGHT + RACESTART_H_BOTTOM)

#define RACESTART_WAIT_NEW_HTTP_SERVER_TICKS 2 // if initially no server, and it appears

#define RACESTART_GUID_STARTLINE_AS_ROUTE "DashT_RaceStart_0001_STARTLINE"
#define RACESTART_NAME_STARTLINE_AS_ROUTE "DashT Startline"
#define RACESTART_NAME_STARTLINE_AS_ROUTE_USER "My Startline"
#define RACESTART_GUID_WP_STARTS "DashT_RaceStart_0001_STARTS"
#define RACESTART_NAME_WP_STARTS "STARTS"
#define RACESTART_NAME_WP_STARTS_USER "MySTARTS"
#define RACESTART_GUID_WP_STARTP "DashT_RaceStart_0001_STARTP"
#define RACESTART_NAME_WP_STARTP "STARTP"
#define RACESTART_NAME_WP_STARTP_USER "MySTARTP"

// These values for first time start only
#define RACESTART_GRID_SIZE 2. // in nautical miles
#define RACESTART_GRID_STEP 0.0107991 // 20 meters in nautical miles
#define RACESTART_GRID_BOLD_INTERVAL 5 // 1 all bold, 2 every 2nd bold, etc.
#define RACESTART_ZERO_BURN_BY_POLAR_SECONDS 60
#define RACESTART_ZERO_BURN_BY_POLAR_SECONDS_UPPER_LIMIT 180
#define RACESTART_ZERO_BURN_BY_POLAR_SECONDS_LOWER_LIMIT 30 // below, will turn to zero = disable

template<typename PlugIn_Waypoint, typename Base, typename Del>
std::unique_ptr<PlugIn_Waypoint, Del> static_unique_ptr_cast_waypoint(
    std::unique_ptr<Base, Del>&& p )
{
    auto d = static_cast<PlugIn_Waypoint *>(p.release());
    return std::unique_ptr<PlugIn_Waypoint, Del>(d, std::move(p.get_deleter()));
}
template<typename PlugIn_Route, typename Base, typename Del>
std::unique_ptr<PlugIn_Route, Del> static_unique_ptr_cast_route(
    std::unique_ptr<Base, Del>&& p )
{
    auto d = static_cast<PlugIn_Route *>(p.release());
    return std::unique_ptr<PlugIn_Route, Del>(d, std::move(p.get_deleter()));
}
//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    DashboardInstrument_RaceStart
//|
//| DESCRIPTION:
//|    This instrument provides assistance for race start line tactics
//+------------------------------------------------------------------------------

class DashboardInstrument_RaceStart : public InstruJS
{
public:
    DashboardInstrument_RaceStart(
        TacticsWindow *pparent, wxWindowID id, wxString ids,
        PI_ColorScheme cs, wxString format = "" );
    ~DashboardInstrument_RaceStart(void);
    void SetData(unsigned long long, double, wxString, long long timestamp=0LL ){;};
#ifndef __RACESTART_DERIVEDTIMEOUT_OVERRIDE__
    virtual void derived2TimeoutEvent(void){};
#else
    virtual void derived2TimeoutEvent(void) = 0;
#endif // __RACESTART_DERIVEDTIMEOUT_OVERRIDE__
    virtual void derivedTimeoutEvent(void);
    bool CheckForValidStartLineGUID( wxString sGUID, wxString lineName,
                                     wxString portName, wxString stbdName);
    bool CheckForValidUserSetStartLine(void);
    bool CheckStartLineStillValid(void);
    virtual wxSize GetSize( int orient, wxSize hint ) override;
    void DoRenderGLOverLay(wxGLContext* pcontext, PlugIn_ViewPort* vp );
    virtual void PushTwaHere(double data, wxString unit, long long timestamp=0LL);
    virtual void PushTwsHere(double data, wxString unit, long long timestamp=0LL);
    virtual void PushCogHere(double data, wxString unit, long long timestamp=0LL);
    
protected:
    TacticsWindow       *m_pparent;
    int                  m_orient;
    bool                 m_htmlLoaded;
    wxTimer             *m_pThreadRaceStartTimer;
    int                  m_goodHttpServerDetects;
    wxFileConfig        *m_pconfig;
    wxString             m_fullPathHTML;
    wxString             m_httpServer;
    wxString             m_sStartLineAsRouteGuid;
    PlugIn_Route        *m_startLineAsRoute;
    wxString             m_sStartStbdWpGuid;
    PlugIn_Waypoint     *m_startStbdWp;
    wxString             m_sStartPortWpGuid;
    PlugIn_Waypoint     *m_startPortWp;
    wxString             m_sRendererCallbackUUID;
    glRendererFunction   m_rendererIsHere;
    bool                 m_renStartLineDrawn;
    wxPoint              m_renPointStbd;
    wxPoint              m_renPointPort;
    bool                 m_renWindBiasDrawn;
    PlugIn_Waypoint     *m_startWestWp;
    PlugIn_Waypoint     *m_startEastWp;
    wxPoint              m_renPointWest;
    wxRealPoint          m_renRealPointWest;
    wxPoint              m_renPointEast;
    wxRealPoint          m_renRealPointEast;
    bool                 m_renbNorthSector;
    double               m_renSlineLength;
    double               m_renSlineDir;
    double               m_renOppositeSlineDir;
    double               m_renBiasSlineDir;
    double               m_renWindBias;
    double               m_renWindBiasAdvDist;
    double               m_renWindBiasAdvDir;
    double               m_renWindBiasLineDir;
    wxPoint              m_renPointBiasStart;
    wxPoint              m_renPointBiasStop;
    bool                 m_renDrawLaylines;
    bool                 m_renLaylinesCalculated;
    double               m_renGridBoxDir;
    double               m_renGridDirEast;
    double               m_renGridDirWest;
    double               m_renGridEndOffset;
    double               m_renGridLineMaxLen;
    double               m_gridStepWestOnStartline;
    double               m_gridStepEastOnStartline;
    double               m_renGridEndPointStartlineWest_lat;
    double               m_renGridEndPointStartlineWest_lon;
    wxRealPoint          m_renGridEndRealPointStartlineWest;
    double               m_renGridEndPointStartlineEast_lat;
    double               m_renGridEndPointStartlineEast_lon;
    wxRealPoint          m_renGridEndRealPointStartlineEast;
    double               m_renGridEndPointOtherWest_lat;
    double               m_renGridEndPointOtherWest_lon;
    wxRealPoint          m_renGridEndRealPointOtherWest;
    double               m_renGridEndPointOtherEast_lat;
    double               m_renGridEndPointOtherEast_lon;
    wxRealPoint          m_renGridEndRealPointOtherEast;
    bool                 m_renGridBoxCalculated;
    double               m_renLLPortDir;
    double               m_renLLStbdDir;
    double               m_renGridSize;
    double               m_renGridStep;
    int                  m_renGridBoldInterval;
    bool                 m_renDrawGrid;
    bool                 m_renGridDrawn;
    int                  m_renZeroBurnSeconds;
    bool                 m_renZeroBurnDrawn;
    
    callbackFunction     m_pushTwaHere;
    wxString             m_pushTwaUUID;
    double               m_Twa;
    callbackFunction     m_pushTwsHere;
    wxString             m_pushTwsUUID;
    double               m_Tws;
    callbackFunction     m_pushCogHere;
    wxString             m_pushCogUUID;
    double               m_Cog;

    wxDECLARE_EVENT_TABLE();

    void ClearRoutesAndWPs(void);
    void ClearRendererCalcs(void);
    void OnThreadTimerTick(wxTimerEvent& event);
    void OnClose(wxCloseEvent& event);
    bool LoadConfig(void);
    void SaveConfig(void);
    void RenderGLStartLine(wxGLContext* pcontext, PlugIn_ViewPort* vp);
    void RenderGLWindBias(wxGLContext* pcontext, PlugIn_ViewPort* vp);
    void RenderGLLaylines(wxGLContext* pcontext, PlugIn_ViewPort* vp);
    bool CalculateGridBox(wxGLContext* pcontext, PlugIn_ViewPort* vp);
    void RenderGLGrid(wxGLContext* pcontext, PlugIn_ViewPort* vp);
    void RenderGLZeroBurn(wxGLContext* pcontext, PlugIn_ViewPort* vp);
};

#endif // __RACESTART_H__
