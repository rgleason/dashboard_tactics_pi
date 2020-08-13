/******************************************************************************
* $Id: RaceMark.cpp, v1.0 2019/11/30 VaderDarth Exp $
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
using namespace std;

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/version.h>

#include "RaceMark.h"
#include "tactics_pi_ext.h"
using namespace std::placeholders;

#include "DashboardFunctions.h"
#include "TacticsFunctions.h"

#include "plugin_ids.h"
wxBEGIN_EVENT_TABLE (DashboardInstrument_RaceMark, InstruJS)
   EVT_TIMER (myID_TICK_RACEMARK, DashboardInstrument_RaceMark::OnThreadTimerTick)
   EVT_CLOSE (DashboardInstrument_RaceMark::OnClose)
wxEND_EVENT_TABLE ()
//************************************************************************************************************************
// Class providing assistance for race start line tactics
//************************************************************************************************************************

DashboardInstrument_RaceMark::DashboardInstrument_RaceMark(
    TacticsWindow *pparent, wxWindowID id, wxString ids,
    PI_ColorScheme cs, wxString format ) : InstruJS (
        pparent, id, ids, cs, JSI_DS_EXTERNAL_DATABASE )
{
    m_pparent = pparent;
    m_orient = wxHORIZONTAL;
    previousTimestamp = 0LL; // see dashboard instru base class
    m_htmlLoaded= false;
    m_goodHttpServerDetects = -1; // default is: there is a server at startup
    m_httpServer = wxEmptyString;
    m_fullPathHTML = wxEmptyString;
    m_pconfig = GetOCPNConfigObject();
    m_dataRequestOn = false;
    m_overlayPauseRequestOn = false;
    m_jsCallBackAsHeartBeat = false;
    m_raceRouteJSData = new raceRouteJSData;
    ClearRoutesAndWPs( true ); // for constructor, do not delete objects.

    /*
      Subcscribe to some interesting data, for compatibility reasons
      we use OpenCPN Dashboard paths, so if there is Signal K paths
      they are considered having passing first through OpenCPN. This way,
      this instrument can be used both with OpenCPN and Signal K with no
      modifications.
    */
    m_Twd = std::nan("1");
    m_fPushTwdHere = std::bind(
        &DashboardInstrument_RaceMark::PushTwdHere, this, _1, _2, _3 );
    m_fPushTwdUUID = m_pparent->subscribeTo (
        _T("OCPN_DBP_STC_TWD"),m_fPushTwdHere );
    m_CurDir = std::nan("1");
    m_CurDirOpposite = std::nan("1");
    m_fPushCurDirHere = std::bind(
        &DashboardInstrument_RaceMark::PushCurDirHere, this, _1, _2, _3 );
    m_fPushCurDirUUID = m_pparent->subscribeTo (
        _T("OCPN_DBP_STC_CURRDIR"), m_fPushCurDirHere );
    m_Lat = std::nan("1");
    m_fPushLatHere = std::bind(
        &DashboardInstrument_RaceMark::PushLatHere, this, _1, _2, _3 );
    m_fPushLatUUID = m_pparent->subscribeTo (
        _T("OCPN_DBP_STC_LAT"), m_fPushLatHere );
    m_Lon = std::nan("1");
    m_fPushLonHere = std::bind(
        &DashboardInstrument_RaceMark::PushLonHere, this, _1, _2, _3 );
    m_fPushLonUUID = m_pparent->subscribeTo (
        _T("OCPN_DBP_STC_LON"), m_fPushLonHere );

    if ( !LoadConfig() )
        return;

    /*
      We subscribe as sub-renderer to Tactics' OpenGL rendering services
    */
    m_rendererIsHere = std::bind(
        &DashboardInstrument_RaceMark::DoRenderGLOverLay, this, _1, _2 );
    m_rendererCallbackUUID = m_pparent->registerGLRenderer(
        _T("DashboardInstrument_RaceMark"), m_rendererIsHere );

    if ( !m_fullPathHTML.IsEmpty() ) {
        m_pThreadRaceMarkTimer = new wxTimer( this, myID_TICK_RACEMARK );
        // avoid start loading all instruments simultaneously
        wxMilliSleep( GetRandomNumber( 800,1600 ) );
        m_pThreadRaceMarkTimer->Start(1000, wxTIMER_CONTINUOUS);
    } // then a reason to launch InstruJS, there is a page to load
}
DashboardInstrument_RaceMark::~DashboardInstrument_RaceMark(void)
{
    this->m_pThreadRaceMarkTimer->Stop();
    delete this->m_pThreadRaceMarkTimer;
    if ( !this->m_rendererCallbackUUID.IsEmpty() )
        this->m_pparent->unregisterGLRenderer(
            this->m_rendererCallbackUUID );
    if ( !this->m_fPushTwdUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushTwdUUID );
    if ( !this->m_fPushCurDirUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushCurDirUUID );
    if ( !this->m_fPushLatUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushLatUUID ); 
    if ( !this->m_fPushLonUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushLonUUID );
    ClearRoutesAndWPs();
    delete this->m_raceRouteJSData;
    return;
}
void DashboardInstrument_RaceMark::OnClose( wxCloseEvent &event )
{
    this->m_pThreadRaceMarkTimer->Stop();
    this->stopScript(); // base class implements, we are first to be called
    if ( !this->m_rendererCallbackUUID.IsEmpty() )
        this->m_pparent->unregisterGLRenderer(
            this->m_rendererCallbackUUID );
    this->m_rendererCallbackUUID = wxEmptyString;
    if ( !this->m_fPushTwdUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushTwdUUID );
    this->m_fPushTwdUUID = wxEmptyString;
    if ( !this->m_fPushCurDirUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushCurDirUUID );
    this->m_fPushCurDirUUID = wxEmptyString;
    if ( !this->m_fPushLatUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushLatUUID );
    this->m_fPushLatUUID = wxEmptyString;
    SaveConfig();
    event.Skip(); // Destroy() must be called
}

void DashboardInstrument_RaceMark::derivedTimeoutEvent()
{
    m_data = L"0.0";
    m_Lon = std::nan("1");
    m_Lat = std::nan("1");
    m_CurDir = std::nan("1");
    m_CurDirOpposite = std::nan("1");
    m_Twd = std::nan("1");
    derived2TimeoutEvent();
}

bool DashboardInstrument_RaceMark::IsAllMeasurementDataValid()
{
    if ( !std::isnan(m_Lat) && !std::isnan(m_Lon) &&
         !std::isnan(m_Twd) && !std::isnan(m_CurDir) )
        return true;
    return false;
}

#define __RACEMARK_CHECK_DATA__(__INSDATA__) if ( !std::isnan(data) ) \
    setTimestamp( timestamp ); \
m_##__INSDATA__ = data;


void DashboardInstrument_RaceMark::PushTwdHere(
    double data, wxString unit, long long timestamp)
{
    __RACEMARK_CHECK_DATA__(Twd)
}

void DashboardInstrument_RaceMark::PushCurDirHere(
    double data, wxString unit, long long timestamp)
{
    __RACEMARK_CHECK_DATA__(CurDir)
        if ( std::isnan( data ) )
            m_CurDirOpposite = std::nan("1");
        else {
            m_CurDirOpposite = data - 180.0;
            if ( m_CurDirOpposite < 0. )
                m_CurDirOpposite += 360.;
        }
}

void DashboardInstrument_RaceMark::PushLatHere(
    double data, wxString unit, long long timestamp)
{
    __RACEMARK_CHECK_DATA__(Lat)
}

void DashboardInstrument_RaceMark::PushLonHere(
    double data, wxString unit, long long timestamp)
{
    __RACEMARK_CHECK_DATA__(Lon)
}

void DashboardInstrument_RaceMark::ClearRoutesAndWPs( bool ctor )
{
    m_raceAsRouteGuid = wxEmptyString;
    m_raceAsRouteName = wxEmptyString;
    m_raceAsRoute = nullptr; // do not delete, is volatile anyway
    m_thisWpLegBearing = std::nan("1");
    m_thisWpLegDistance = std::nan("1");
    m_thisWpLegTwa = std::nan("1");
    m_thisWpLegAvgTwa = std::nan("1");
    m_thisWpLegShortAvgTwa = std::nan("1");
    m_thisWpLegCurrent = std::nan("1");
    m_previousWpName = wxEmptyString;
    m_previousWpGuid = wxEmptyString;
    if ( m_previousWp && !ctor )
        delete m_previousWp;
    m_previousWp = nullptr;
    m_previousWpBearing = std::nan("1");
    m_previousWpDistance = std::nan("1");
    m_targetWpName = wxEmptyString;
    m_targetWpGuid = wxEmptyString;
    if ( m_targetWp && !ctor )
        delete m_targetWp;
    m_targetWp = nullptr;

    m_raceRouteJSData->hasActiveRoute = "false";
    if ( ctor )
        m_raceRouteJSData->instruIsReady = "false";
    else
        m_raceRouteJSData->instruIsReady =
            IsAllMeasurementDataValid() ? "true" : "false";
    m_raceRouteJSData->mark1Name = "- - -";
    m_raceRouteJSData->mark2Name = "- - -";
    m_raceRouteJSData->mark3Name = "- - -";
    m_raceRouteJSData->thisLegTwa = "-999.0";
    m_raceRouteJSData->thisLegTwaShortAvg = "-999.0";
    m_raceRouteJSData->thisLegTwaAvg = "-999.0";
    m_raceRouteJSData->thisLegCurrent = "-999.0";
    m_raceRouteJSData->bearingBack = "-999.0";

    ClearNextAndNextNextWpsOnly( ctor );
}

void DashboardInstrument_RaceMark::ClearNextAndNextNextWpsOnly( bool ctor )
{
    m_nextWpName = wxEmptyString;
    m_nextWpGuid = wxEmptyString;
    if ( m_nextWp && !ctor )
        delete m_nextWp;
    m_nextWp = nullptr;
    m_nextWpLegBearing = std::nan("1");
    m_nextWpLegDistance = std::nan("1");
    m_nextWpLegTwa = std::nan("1");
    m_nextWpLegAvgTwa = std::nan("1");
    m_nextWpLegShortAvgTwa = std::nan("1");
    m_nextWpLegCurrent =  std::nan("1");
    m_nextNextWpLegBearing = std::nan("1");
    m_nextNextWpLegDistance = std::nan("1");
    m_nextNextWpLegTwa = std::nan("1");
    m_nextNextWpLegAvgTwa = std::nan("1");
    m_nextNextWpLegShortAvgTwa = std::nan("1");
    m_nextNextWpLegCurrent =  std::nan("1");
    m_nextNextWpName = wxEmptyString;
    m_nextNextWpGuid = wxEmptyString;
    if ( m_nextNextWp && !ctor )
        delete m_nextNextWp;
    m_nextNextWp = nullptr;

    m_raceRouteJSData->nextLegTwa = "-999.0";
    m_raceRouteJSData->nextLegTwaShortAvg = "-999.0";
    m_raceRouteJSData->nextLegTwaAvg = "-999.0";
    m_raceRouteJSData->nextLegCurrent = "-999.0";
    m_raceRouteJSData->nextNextLegTwa = "-999.0";
    m_raceRouteJSData->nextNextLegTwaShortAvg = "-999.0";
    m_raceRouteJSData->nextNextLegTwaAvg = "-999.0";
    m_raceRouteJSData->nextNextLegCurrent = "-999.0";
}

bool DashboardInstrument_RaceMark::CheckForValidActiveRoute()
{
    wxString activeRouteGUID = m_pparent->GetActiveRouteGUID();
    if ( activeRouteGUID.IsEmpty() )
        return false;
    wxString activeRouteName = m_pparent->GetActiveRouteName();
    if ( activeRouteName.IsEmpty() )
        return false;
    m_raceAsRouteGuid = activeRouteGUID;
    std::unique_ptr<PlugIn_Route> rte = GetRoute_Plugin(
        m_raceAsRouteGuid );
    m_raceAsRoute = rte.get(); // pointed object is volatile
    if ( !(m_raceAsRoute) )
        return false;
    if ( m_raceAsRoute->m_NameString != activeRouteName )
        return false;
    m_raceAsRouteName = activeRouteName;
    return true;
}

// The race is a route and can be killed in route manager of OpenCPN
bool DashboardInstrument_RaceMark::CheckRouteStillValid()
{
    std::unique_ptr<PlugIn_Route> rte = GetRoute_Plugin(
        m_raceAsRouteGuid );
    PlugIn_Route *selectedRouteAsRaceRouteStillThere = rte.get();
    if ( selectedRouteAsRaceRouteStillThere ) {
        wxString activeRouteGUID = m_pparent->GetActiveRouteGUID();
        if ( activeRouteGUID.IsEmpty() )
            return false;
        if ( selectedRouteAsRaceRouteStillThere->m_GUID != activeRouteGUID )
            return false;
        return true;
    } // then route is still there, is it active, or if it is, is the same?
    return false;
}

// It is the route manager not us who activates the next waypoint
bool DashboardInstrument_RaceMark::CheckForActivatedWp()
{
    wxString activatedWpGUID = m_pparent->GetWpActivatedGUID();
    if ( activatedWpGUID.IsEmpty() )
        return false;
    wxString activatedWpName = m_pparent->GetWpActivatedName();
    if ( activatedWpName.IsEmpty() )
        return false;
    m_targetWpGuid = activatedWpGUID;
    std::unique_ptr<PlugIn_Waypoint> wpt = GetWaypoint_Plugin(
        m_targetWpGuid );
    PlugIn_Waypoint *newWp = wpt.get();
    if ( !(newWp) )
        return false;
    if ( newWp->m_MarkName != activatedWpName )
        return false;
    if ( m_targetWp )
        delete m_targetWp;
    m_targetWp = new PlugIn_Waypoint();
    CopyPlugInWaypointWithoutHyperlinks( newWp, m_targetWp );
    m_targetWpName = activatedWpName;
    return true;
}

#define __UpdateActiveRtePtr__  if ( !(m_raceAsRoute) ) \
        return false; \
    std::unique_ptr<PlugIn_Route> rte = GetRoute_Plugin( \
        m_raceAsRouteGuid ); \
    m_raceAsRoute = rte.get(); \
    if ( !(m_raceAsRoute) ) \
        return false;

bool DashboardInstrument_RaceMark::UpdatePreviousWpFromNewTarget()
{
    __UpdateActiveRtePtr__

    Plugin_WaypointList::iterator iter;
    int targetPos = -1;
    for ( iter = m_raceAsRoute->pWaypointList->begin();
          iter != m_raceAsRoute->pWaypointList->end(); ++iter ) {

        PlugIn_Waypoint *wpNode = *iter;
        targetPos += 1;
        if ( wpNode ) {
            if ( wpNode->m_GUID == m_targetWpGuid ) {
                if ( targetPos == 0 )
                    return false;
                --iter;
                PlugIn_Waypoint *wpPrev = *iter;
                if ( wpPrev ) { // this pointer we do not entirely trust
                    m_previousWpGuid = wpPrev->m_GUID;
                    std::unique_ptr<PlugIn_Waypoint> rte = GetWaypoint_Plugin(
                        m_previousWpGuid );
                    PlugIn_Waypoint *newWp = rte.get();
                    if ( m_previousWp ) {
                        delete m_previousWp;
                        m_previousWp = nullptr;
                    }
                    if ( newWp ) {
                        m_previousWp = new PlugIn_Waypoint();
                        CopyPlugInWaypointWithoutHyperlinks( newWp, m_previousWp );
                        m_previousWpName = m_previousWp->m_MarkName;
                    } // then a valid "next" wp pointer
                    else {
                        m_previousWpName = wxEmptyString;
                        m_previousWpGuid = wxEmptyString;
                    } // else not a valid pointer do not continue
                } // then there is a "next" waypoint
                break;
            } // then current target mark found
        } // then valid waypoint node
    } // for search the next waypoint from the route waypoint list container
    return true;
}

// Poll for a change in the active waypoint, being moved to next by route mgr.
int DashboardInstrument_RaceMark::PollForNextActiveRouteWp()
{
    /*
      Note: one needs to use Name, not GUID for "Arrived" WP, see dashboard_pi,
      which contains the reverse engineered interpretation of routemanp.cpp
      (of O) messages. There is an issue with GUID overrwriting: in O <= v5.2
      the  arrived wp's GUID is overwritten by the next wp's GUID.
    */
    wxString arrivedWpName = m_pparent->GetWpArrivedName();
    if ( arrivedWpName.IsEmpty() )
        return 0;
    if ( arrivedWpName == m_targetWpName ) {
        wxString arrivedNextWpName = m_pparent->GetWpArrivedNextName();
        if ( arrivedNextWpName.IsEmpty() )
            return -1;
        if ( arrivedNextWpName == m_targetWpName )
            return 0; // wrong alert or maybe the message is not complete
        return 1;
    } // then we have reached the next mark
    else {
        if ( arrivedWpName == m_previousWpName )
            return 0;
        return 2;
    } /*  else, in polling by name, set by an event, we get the previous
          waypoint as arrived, until we arrive to the next target point */
}

// Reorganize the waypoints upon arrival to the mark
bool DashboardInstrument_RaceMark::ChangeToNextTargetMark()
{
    wxString arrivedNextGUID = m_pparent->GetWpArrivedNextGUID();
    if ( arrivedNextGUID == m_targetWpGuid )
        return false;
    
    // previous mark
    m_previousWpName     = m_targetWpName;
    m_previousWpGuid     = m_targetWpGuid;
    m_previousWp         = m_targetWp;
    m_previousWpBearing  = std::nan("1");
    m_previousWpDistance = std::nan("1");
    // new target
    m_targetWpGuid = arrivedNextGUID;
    std::unique_ptr<PlugIn_Waypoint> wpt = GetWaypoint_Plugin(
        m_targetWpGuid );
    PlugIn_Waypoint *newWp = wpt.get();
    if ( !(newWp) )
        return false; // then a bigger problem, indicate the disaster
    if ( newWp->m_MarkName.IsEmpty() )
        return false; // then quit on non-named waypoint - needed for detection
    if ( newWp->m_MarkName != m_pparent->GetWpArrivedNextName() )
        return false; // then quit on inconistencies
    m_targetWp = new PlugIn_Waypoint();
    CopyPlugInWaypointWithoutHyperlinks( newWp, m_targetWp );
    m_targetWpName = m_targetWp->m_MarkName;
    // Indicate a change return
    return true;
}

bool DashboardInstrument_RaceMark::CalculateBearingFromNewTargetToPreviousMark()
{
    if ( !(m_previousWp) || !(m_targetWp) )
        return false;
    DistanceBearingMercator_Plugin(
        m_targetWp->m_lat, m_targetWp->m_lon, // "to"
        m_previousWp->m_lat, m_previousWp->m_lon, // "from"
        &m_thisWpLegBearing, &m_thisWpLegDistance ); // result
    DistanceBearingMercator_Plugin(
        m_previousWp->m_lat, m_previousWp->m_lon, // "to"
        m_targetWp->m_lat, m_targetWp->m_lon, // "from"
        &m_previousWpBearing, &m_previousWpDistance ); // result
    return true;
}

// Analyze the route a bit further and update the next and the "next next" legs
bool  DashboardInstrument_RaceMark::ChangeNextAndNextNextMarks()
{
    __UpdateActiveRtePtr__

    ClearNextAndNextNextWpsOnly();
    int maxIdxWaypoints = ( m_raceAsRoute->pWaypointList->GetCount() - 1 );
    int idxWaypoint = -1;
    Plugin_WaypointList::iterator iter;
    for ( iter = m_raceAsRoute->pWaypointList->begin();
          iter != m_raceAsRoute->pWaypointList->end(); ++iter ) {

        idxWaypoint++;
        PlugIn_Waypoint *wpNode = *iter;
        if ( wpNode ) {
            if ( wpNode->m_GUID == m_targetWpGuid ) {
                if ( idxWaypoint >= maxIdxWaypoints )
                    break;
                ++iter;
                idxWaypoint++;
                PlugIn_Waypoint *wpNext = *iter;
                if ( wpNext ) { // this pointer we do not entirely trust
                    m_nextWpGuid = wpNext->m_GUID;
                    std::unique_ptr<PlugIn_Waypoint> rte = GetWaypoint_Plugin(
                        m_nextWpGuid );
                    PlugIn_Waypoint *newWp = rte.get();
                    if ( m_nextWp ) {
                        delete m_nextWp;
                        m_nextWp =  nullptr;
                    }
                    if ( newWp ) {
                        m_nextWp = new PlugIn_Waypoint();
                        CopyPlugInWaypointWithoutHyperlinks( newWp, m_nextWp );
                        m_nextWpName = m_nextWp->m_MarkName;
                        if ( idxWaypoint >= maxIdxWaypoints )
                            break;
                        ++iter;
                        // idxWaypoint++; // yes, codacy, I know it is useless!
                        PlugIn_Waypoint *wpNextNext = *iter;
                        if ( wpNextNext ) {
                            m_nextNextWpGuid = wpNextNext->m_GUID;
                            std::unique_ptr<PlugIn_Waypoint> rte =
                                GetWaypoint_Plugin( m_nextNextWpGuid );
                            newWp = rte.get();
                            if ( m_nextNextWp ) {
                                delete m_nextNextWp;
                                m_nextNextWp = nullptr;
                            }
                            if ( newWp ) {
                                m_nextNextWp = new PlugIn_Waypoint();
                                CopyPlugInWaypointWithoutHyperlinks(
                                    newWp, m_nextNextWp );
                                m_nextNextWpName = m_nextNextWp->m_MarkName;
                            } // then a valid pointer
                            else {
                                m_nextNextWp = nullptr;
                                m_nextNextWpGuid = wxEmptyString;
                                m_nextNextWpName = wxEmptyString;
                            } // else not a valid pointer do not continue
                        } // then there is a "next next" waypoint
                    } // then a valid "next" wp pointer
                    else {
                        ClearNextAndNextNextWpsOnly(); 
                    } // else not a valid pointer do not continue
                } // then there is a "next" waypoint
                break;
            } // then current target mark found
        } // then valid waypoint node
    } // for search the next waypoint from the route waypoint list container
    return true;
}

bool  DashboardInstrument_RaceMark::ThisLegDataUpdate()
{
    if ( !CalculateBearingFromNewTargetToPreviousMark() )
        return false;
    m_thisWpLegTwa = getDegRange( m_thisWpLegBearing, m_Twd );
    if ( !(AverageWind) )
        return false;
    double avgWindDir = AverageWind->GetAvgWindDir();
    m_thisWpLegAvgTwa = getDegRange(
        m_thisWpLegBearing, avgWindDir );
    double avgShortWindDir = AverageWind->GetShortAvgWindDir();
    m_thisWpLegShortAvgTwa = getDegRange(
        m_thisWpLegBearing, avgShortWindDir );
    m_thisWpLegCurrent = getDegRange(
        m_thisWpLegBearing, m_CurDirOpposite );
    return true;
}

bool  DashboardInstrument_RaceMark::PeekTwaOnNextAndNextNextLegs()
{
    if ( !(m_targetWp) || !(m_nextWp) )
        return false;
    // Waypoints can move
    DistanceBearingMercator_Plugin(
        m_nextWp->m_lat, m_nextWp->m_lon, // "to"
        m_targetWp->m_lat, m_targetWp->m_lon, // "from"
        &m_nextWpLegBearing, &m_nextWpLegDistance ); // result
    m_nextWpLegTwa = getDegRange( m_nextWpLegBearing, m_Twd );
    if ( !(AverageWind) )
        return false;
    double avgWindDir = AverageWind->GetAvgWindDir();
    m_nextWpLegAvgTwa = getDegRange(
        m_nextWpLegBearing, avgWindDir );
    double avgShortWindDir = AverageWind->GetShortAvgWindDir();
    m_nextWpLegShortAvgTwa = getDegRange(
        m_nextWpLegBearing, avgShortWindDir );
    m_nextWpLegCurrent = getDegRange(
        m_nextWpLegBearing, m_CurDirOpposite );

    if ( !(m_nextNextWp) )
        return true;
    
     DistanceBearingMercator_Plugin(
        m_nextNextWp->m_lat, m_nextNextWp->m_lon, // "to"
        m_nextWp->m_lat, m_nextWp->m_lon, // "from"
        &m_nextNextWpLegBearing, &m_nextNextWpLegDistance ); // result
    m_nextNextWpLegTwa = getDegRange( m_nextNextWpLegBearing, m_Twd );
    m_nextNextWpLegAvgTwa = getDegRange(
        m_nextNextWpLegBearing, avgWindDir );
    m_nextNextWpLegShortAvgTwa = getDegRange(
        m_nextNextWpLegBearing, avgShortWindDir );
    m_nextNextWpLegCurrent = getDegRange(
        m_nextNextWpLegBearing, m_CurDirOpposite );
   
    return true;
}

// The race is a route and can be killed in route manager of OpenCPN
bool DashboardInstrument_RaceMark::CheckWpStillValid( wxString sGUID )
{
    std::unique_ptr<PlugIn_Waypoint> rte = GetWaypoint_Plugin( sGUID );
    PlugIn_Waypoint *theWayPointIsStillThere = rte.get();
    if ( theWayPointIsStillThere )
        return true;
    return false;
}

// The JavaScript part asks if we are ready: with no data, we are not
bool DashboardInstrument_RaceMark::instruIsReady()
{
    return IsAllMeasurementDataValid();
}

// The JavaScript part asks do we have an Active route?
bool DashboardInstrument_RaceMark::userHasActiveRoute()
{
    if ( m_raceAsRoute )
        return true;
    return false;
}

bool DashboardInstrument_RaceMark::sendRmData()
{
    m_dataRequestOn = true;
    return true;
}
bool DashboardInstrument_RaceMark::stopRmData()
{
    m_dataRequestOn = false;
    m_jsCallBackAsHeartBeat = true;
    return true;
}
bool DashboardInstrument_RaceMark::hideChartOverlay()
{
    m_overlayPauseRequestOn = true;
    return true;
}
bool DashboardInstrument_RaceMark::showChartOverlay()
{
    m_overlayPauseRequestOn = false;
    m_jsCallBackAsHeartBeat = true;
    return true;
}
raceRouteJSData *DashboardInstrument_RaceMark::getRmDataPtr()
{
    m_raceRouteJSData->hasActiveRoute = m_raceAsRoute ? "true" : "false";
    m_raceRouteJSData->instruIsReady =
        IsAllMeasurementDataValid() ? "true" : "false";
    m_raceRouteJSData->mark1Name =
        m_previousWpName.IsEmpty() ? "- - -" : m_previousWpName;
    m_raceRouteJSData->mark2Name =
        m_targetWpName.IsEmpty() ? "- - -" : m_targetWpName;
    m_raceRouteJSData->mark3Name =
        m_nextWpName.IsEmpty() ? "- - -" : m_nextWpName;
    m_raceRouteJSData->thisLegTwa =
        std::isnan( m_thisWpLegTwa ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_thisWpLegTwa );
    m_raceRouteJSData->thisLegTwaShortAvg =
        std::isnan( m_thisWpLegShortAvgTwa ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_thisWpLegShortAvgTwa );
    m_raceRouteJSData->thisLegTwaAvg =
        std::isnan( m_thisWpLegAvgTwa ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_thisWpLegAvgTwa );
    m_raceRouteJSData->thisLegCurrent =
        std::isnan( m_thisWpLegCurrent ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_thisWpLegCurrent );
    m_raceRouteJSData->nextLegTwa =
        std::isnan( m_nextWpLegTwa ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_nextWpLegTwa );
    m_raceRouteJSData->nextLegTwaShortAvg =
        std::isnan( m_nextWpLegShortAvgTwa ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_nextWpLegShortAvgTwa );
    m_raceRouteJSData->nextLegTwaAvg =
        std::isnan( m_nextWpLegAvgTwa ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_nextWpLegAvgTwa );
    m_raceRouteJSData->nextLegCurrent =
        std::isnan( m_nextWpLegCurrent ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_nextWpLegCurrent );
    m_raceRouteJSData->nextNextLegTwa =
        std::isnan( m_nextNextWpLegTwa ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_nextNextWpLegTwa );
    m_raceRouteJSData->nextNextLegTwaShortAvg = std::isnan(
        m_nextNextWpLegShortAvgTwa ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_nextNextWpLegShortAvgTwa );
    m_raceRouteJSData->nextNextLegTwaAvg = std::isnan(
        m_nextNextWpLegAvgTwa ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_nextNextWpLegAvgTwa );
    m_raceRouteJSData->nextNextLegCurrent = std::isnan(
        m_nextNextWpLegCurrent ) ? "-999.0" :
        wxString::Format( wxT("%f"), m_nextNextWpLegCurrent );
    m_raceRouteJSData->bearingBack = std::isnan(
        m_previousWpBearing ) ? "-999.0" :
        wxString::Format( wxT("%f"),  m_previousWpBearing );

    return m_raceRouteJSData;
}

void DashboardInstrument_RaceMark::OnThreadTimerTick( wxTimerEvent &event )
{
    m_pThreadRaceMarkTimer->Stop();
    
    if ( !(m_raceAsRoute) )
        CheckForValidActiveRoute();
    if ( m_raceAsRoute ) {
        if ( !CheckRouteStillValid() ) {
            ClearRoutesAndWPs();
        } // then a route has disappeared, meanwhile
        else {
            if ( !(m_targetWp) ) {
                if ( CheckForActivatedWp() ) {
                    (void) UpdatePreviousWpFromNewTarget();
                    (void) CalculateBearingFromNewTargetToPreviousMark();
                } // then there is a new active waypoint
            } // then we do not have yet the next mark as target
            if ( m_targetWp ) {
                (void) ThisLegDataUpdate(); // keep updating
                (void) PeekTwaOnNextAndNextNextLegs(); // keep updating
                if ( !CheckWpStillValid( m_targetWpGuid ) ) {
                    ClearRoutesAndWPs();
                } // then the waypoint we thing as next target has disappeared
                int targetArrivalStatus = PollForNextActiveRouteWp();
                if ( targetArrivalStatus < 0 ) {
                    ClearRoutesAndWPs();
                } // then arrived to the end (or an issue with the arrival)
                else {
                    if ( targetArrivalStatus > 0 ) {
                        if ( ChangeToNextTargetMark() ) {
                            (void) CalculateBearingFromNewTargetToPreviousMark();
                            (void) ChangeNextAndNextNextMarks();
                            (void) PeekTwaOnNextAndNextNextLegs();
                        } // then successfull change to the next target mark
                        else {
                            ClearRoutesAndWPs();
                            wxLogMessage(
                                "dashboard_tactics_pi: "
                                "DashboardInstrument_RaceMark:: - "
                                "timer thread - Error: "
                                "routing status reports inconsistency in arrived / "
                                "next route WP GUIDs/names - please report details "
                                "of your route and at which point things went wrong. "
                                "Please check also that all your race waypoints "
                                "do have a unique name: unique GUIDs cannot be used "
                                "for detection of an arrived waypoint as for now "
                                "due to an issue in OpenCPN <= v5.2 (at least)"
                                );
                            wxString message(
                                _("Sorry, cannot determine what is the next mark.") +
                                "\n" +
                                _("Check that all waypoints have a unique name.") );
                            wxMessageDialog *dlg = new wxMessageDialog(
                                GetOCPNCanvasWindow(), message,
                                _T("DashT Race Mark"), wxOK|wxICON_ERROR);
                            (void) dlg->ShowModal();
                        } // else a failure to switch to the next target
                    } // then arrived to the mark, next mark set
                } // else either not yet arrived to mark or arrived to it
            } // then there is a next mark, activated by router
        } // else there is a route and it is still valid
    } // then there is an active route

    if ( !m_htmlLoaded) {
        if ( testHTTPServer( m_httpServer ) ) {
            if ( (m_goodHttpServerDetects == -1) ||
                 (m_goodHttpServerDetects >=
                  RACEMARK_WAIT_NEW_HTTP_SERVER_TICKS) ) {
                wxSize thisSize = wxControl::GetSize();
                wxSize thisFrameInitSize = GetSize( m_orient, thisSize );
                SetInitialSize ( thisFrameInitSize );
                wxSize webViewInitSize = thisFrameInitSize;
                this->loadHTML( m_fullPathHTML, webViewInitSize );
                // No more threaded jobs, InstruJS is working now
                m_htmlLoaded= true;
                m_pThreadRaceMarkTimer->Start( GetRandomNumber( 800,1100 ),
                                                wxTIMER_CONTINUOUS);
            } /* then either a straigth start with server or detected
                 a stable server */
            else {
                m_goodHttpServerDetects += 1;
                m_pThreadRaceMarkTimer->Start( GetRandomNumber( 800,1100 ),
                                                wxTIMER_CONTINUOUS);
            }
        } /* then there is a server serving the page, can ask content
             to be loaded */
        else {
            m_goodHttpServerDetects = 0;
            m_pThreadRaceMarkTimer->Start( GetRandomNumber( 8000,11000 ),
                                            wxTIMER_CONTINUOUS);
        }  // else need to wait until a server appears
    } // then there is not page loaded keep on trying
    else {
        m_pThreadRaceMarkTimer->Start( GetRandomNumber( 800,1100 ),
                                        wxTIMER_CONTINUOUS);
    } //else keep on polling for the route changes

}

wxSize DashboardInstrument_RaceMark::GetSize( int orient, wxSize hint )
{
    int x,y;
    m_orient = orient;
    if( m_orient == wxHORIZONTAL ) {
        x = RACEMARK_H_MIN_WIDTH;
        y = wxMax( hint.y, RACEMARK_H_MIN_HEIGHT );
    }
    else {
        x = wxMax( hint.x, RACEMARK_V_MIN_WIDTH );
        y = RACEMARK_V_MIN_HEIGHT;
    }
    return wxSize( x, y );
}

bool DashboardInstrument_RaceMark::LoadConfig()
{
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return false;
    
    // Make a proposal for the defaul path _and_ the protocool, which user can then override in the file:
    wxString sFullPathHTML = "http://127.0.0.1:8088/racedashmark/";

    pConf->SetPath( _T("/PlugIns/DashT/WebView/RaceMark/") );
    pConf->Read( _T("instrujsURL"), &m_fullPathHTML, sFullPathHTML );

    m_httpServer = this->testURLretHost( m_fullPathHTML );
    
    if ( m_httpServer.IsEmpty() ) {
        wxString message(
            _("Malformed URL string in WebView/RaceMark ini-file entry: ")
            + "\n" );
        message += m_fullPathHTML;
        wxMessageDialog *dlg = new wxMessageDialog(
            GetOCPNCanvasWindow(), message, _T("DashT Race Mark"),
            wxOK|wxICON_ERROR);
        (void) dlg->ShowModal();
        m_fullPathHTML = wxEmptyString;
    }

    pConf->SetPath( _T("/PlugIns/DashT/Race/RaceMark/") );

    pConf->Read( _T("LaylineWidth"), &m_renLaylineWidth,
                 RACEMARK_RUNGLINE_WIDTH );
    m_renDrawLaylines = true;
    if ( m_renLaylineWidth <= 0 ) {
        m_renLaylineWidth = 0;
        m_renDrawLaylines = false;
    }
    pConf->Read( _T("RungStep"), &m_renRungStep, RACEMARK_LADDER_RUNG_STEP );
    m_renRungStep = abs(m_renRungStep);
    if ( m_renRungStep < RACEMARK_LADDER_RUNG_STEP_MINIMUM )
        m_renRungStep = RACEMARK_LADDER_RUNG_STEP_MINIMUM;
    pConf->Read( _T("RungLineWidth"), &m_renRungLineWidth,
                 RACEMARK_RUNGLINE_WIDTH );
    if ( m_renRungLineWidth < 0 )
        m_renRungLineWidth = 0;
    m_renDrawRungs = true;
    if ( m_renRungLineWidth == 0 )
        m_renDrawRungs = false;
    pConf->Read( _T("AvgWindLineWidth"), &m_renAvgWindLineWidth,
                 RACEMARK_AVGWINDLINE_WIDTH );
    if ( m_renAvgWindLineWidth < 0 )
        m_renAvgWindLineWidth = 0;
    m_renDrawAvgWind = true;
    if ( m_renAvgWindLineWidth == 0 )
        m_renDrawAvgWind = false;
    pConf->Read( _T("ShortAvgWindRungLineWidth"), &m_renShortAvgWindRungLineWidth,
                 RACEMARK_SHORTAVGWINDRUNGLINE_WIDTH );
    if ( m_renShortAvgWindRungLineWidth < 0 )
        m_renShortAvgWindRungLineWidth = 0;
    m_renDrawShortAvgWindRungs = true;
    if ( m_renShortAvgWindRungLineWidth == 0 )
        m_renDrawShortAvgWindRungs = false;

    
    return true;
}

void DashboardInstrument_RaceMark::SaveConfig()
{
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return;

    pConf->SetPath(_T("/PlugIns/DashT/WebView/RaceMark/"));
    pConf->Write(_T("instrujsURL"), m_fullPathHTML );
    pConf->SetPath( _T("/PlugIns/DashT/Race/RaceMark/") );
    pConf->Write( _T("LaylineWidth"), m_renLaylineWidth );
    pConf->Write( _T("RungStep"), m_renRungStep ); 
    pConf->Write( _T("RungLineWidth"), m_renRungLineWidth ); 
    pConf->Write( _T("AvgWindLineWidth"), m_renAvgWindLineWidth );
    pConf->Write( _T("ShortAvgWindRungLineWidth"),
                  m_renShortAvgWindRungLineWidth );

    return;
}
