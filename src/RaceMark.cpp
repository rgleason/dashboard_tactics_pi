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

#include "dashboard_pi.h"
#include "RaceMark.h"
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
    m_jsCallBackAsHeartBeat = false;
    ClearRoutesAndWPs( true ); // mark coming from constructor...

    /*
      Subcscribe to some interesting data, for compatibility reasons
      we use OpenCPN Dashboard paths, so if there is Signal K paths
      they are considered having passing first through OpenCPN. This way,
      this instrument can be used both with OpenCPN and Signal K with no
      modifications.
    */
    m_Twa = std::nan("1");
    m_fPushTwaHere = std::bind(
        &DashboardInstrument_RaceMark::PushTwaHere, this, _1, _2, _3 );
    m_fPushTwaUUID = m_pparent->subscribeTo (
        _T("OCPN_DBP_STC_TWA"),m_fPushTwaHere );
    m_Tws = std::nan("1");
    m_fPushTwsHere = std::bind(
        &DashboardInstrument_RaceMark::PushTwsHere, this, _1, _2, _3 );
    m_fPushTwsUUID = m_pparent->subscribeTo (
        _T("OCPN_DBP_STC_TWS"), m_fPushTwsHere );
    m_Cog = std::nan("1");
    m_fPushCogHere = std::bind(
        &DashboardInstrument_RaceMark::PushCogHere, this, _1, _2, _3 );
    m_fPushCogUUID = m_pparent->subscribeTo (
        _T("OCPN_DBP_STC_COG"), m_fPushCogHere );
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
    if ( !this->m_fPushTwaUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushTwaUUID );
    if ( !this->m_fPushTwsUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushTwsUUID );
    if ( !this->m_fPushCogUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushCogUUID );
    if ( !this->m_fPushLatUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushLatUUID ); 
    if ( !this->m_fPushLonUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushLonUUID );
    ClearRoutesAndWPs();
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
    if ( !this->m_fPushTwaUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushTwaUUID );
    this->m_fPushTwaUUID = wxEmptyString;
    if ( !this->m_fPushTwsUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushTwsUUID );
    this->m_fPushTwsUUID = wxEmptyString;
    if ( !this->m_fPushCogUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_fPushCogUUID );
    this->m_fPushCogUUID = wxEmptyString;
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
    m_Cog = std::nan("1");
    m_Tws = std::nan("1");
    m_Twa = std::nan("1");
    derived2TimeoutEvent();
}

bool DashboardInstrument_RaceMark::IsAllMeasurementDataValid()
{
    if ( !std::isnan(m_Lat) && !std::isnan(m_Lon) &&
         !std::isnan(m_Tws) && !std::isnan(m_Twa) && !std::isnan(m_Cog) )
        return true;
    return false;
}

#define __RACEMARK_CHECK_DATA__(__INSDATA__) if ( !std::isnan(data) ) \
    setTimestamp( timestamp ); \
m_##__INSDATA__ = data;


void DashboardInstrument_RaceMark::PushTwaHere(
    double data, wxString unit, long long timestamp)
{
    __RACEMARK_CHECK_DATA__(Twa)
}

void DashboardInstrument_RaceMark::PushTwsHere(
    double data, wxString unit, long long timestamp)
{
    __RACEMARK_CHECK_DATA__(Tws)
}

void DashboardInstrument_RaceMark::PushCogHere(
    double data, wxString unit, long long timestamp)
{
    __RACEMARK_CHECK_DATA__(Cog)
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
    m_previousWpName = wxEmptyString;
    m_previousWpGuid = wxEmptyString;
    if ( m_previousWp && !ctor )
        delete m_previousWp;
    m_previousWp = nullptr;
    m_previousWpBearing = std::nan("1");
    m_targetWpName = wxEmptyString;
    m_targetWpGuid = wxEmptyString;
    if ( m_targetWp && !ctor )
        delete m_targetWp;
    m_targetWp = nullptr;
    ClearNextAndNextNextWpsOnly();
}

void DashboardInstrument_RaceMark::ClearNextAndNextNextWpsOnly( bool ctor )
{
    m_nextWpName = wxEmptyString;
    m_nextWpGuid = wxEmptyString;
    if ( m_nextWp && !ctor )
        delete m_nextWp;
    m_nextWp = nullptr;
    m_nextNextWpName = wxEmptyString;
    m_nextNextWpGuid = wxEmptyString;
    if ( m_nextNextWp && !ctor )
        delete m_nextNextWp;
    m_nextNextWp = nullptr;
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
    if ( selectedRouteAsRaceRouteStillThere )
        return true;
    return false;
}


// It is the route manager not us who activate the next waypoint
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

// Poll for a change in the active waypoint, being moved to next by route mgr.
int DashboardInstrument_RaceMark::PollForNextActiveRouteWp()
{
    /*
      Note: one needs to use Name, not GUID for "Arrived" WP, see dashboard_pi,
      which contains the reverse engineered interpretation of routemanp.cpp
      (of O) messages.
    */
    wxString arrivedWpName = m_pparent->GetWpArrivedName();
    if ( arrivedWpName.IsEmpty() )
        return 0;
    if ( arrivedWpName == m_targetWpName ) {
        wxString arrivedNextWpName = m_pparent->GetWpArrivedNextName();
        if ( arrivedNextWpName.IsEmpty() )
            return -1;
        return 1;
    } // then we have reached the next mark
    else {
        return 0;
    } // else require that we turn around marks in right order
}

// Reorganize the waypoints upon arrival to the mark
bool DashboardInstrument_RaceMark::ChangeToNextTargetMark()
{
    // previous mark
    m_previousWpName    = m_targetWpName;
    m_previousWpGuid    = m_targetWpGuid;
    m_previousWp        = m_targetWp;
    m_previousWpBearing = std::nan("1");
    // new target
    m_targetWpGuid = m_pparent->GetWpArrivedNextGUID();
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

// Analyze the route a bit further and update the next and the "next next" legs
void  DashboardInstrument_RaceMark::ChangeNextAndNextNextMarks()
{
    if ( !(m_raceAsRoute) )
        return; // then abandon if an active routing has not been found yet
    // find the active route again, in a local copy, keep marker updated
    std::unique_ptr<PlugIn_Route> rte = GetRoute_Plugin(
        m_raceAsRouteGuid );
    m_raceAsRoute = rte.get();
    if ( !(m_raceAsRoute) )
        return ; // then, meanwhile, the route has disappeared?

    ClearNextAndNextNextWpsOnly();
    Plugin_WaypointList::iterator iter;
    for ( iter = m_raceAsRoute->pWaypointList->begin();
          iter != m_raceAsRoute->pWaypointList->end(); ++iter ) {

        PlugIn_Waypoint *wpNode = *iter;
        if ( wpNode ) {
            if ( wpNode->m_GUID == m_targetWpGuid ) {
                ++iter;
                PlugIn_Waypoint *wpNext = *iter;
                if ( wpNext ) { // this pointer we do not entirely trust
                    m_nextWpGuid = wpNext->m_GUID;
                    std::unique_ptr<PlugIn_Waypoint> rte = GetWaypoint_Plugin(
                        m_nextWpGuid );
                    PlugIn_Waypoint *newWp = rte.get();
                    if ( m_nextWp )
                        delete m_nextWp;
                    if ( newWp ) {
                        m_nextWp = new PlugIn_Waypoint();
                        CopyPlugInWaypointWithoutHyperlinks( newWp, m_nextWp );
                        m_nextWpName = m_nextWp->m_MarkName;
                        ++iter;
                        PlugIn_Waypoint *wpNextNext = *iter;
                        if ( wpNextNext ) {
                            m_nextNextWpGuid = wpNextNext->m_GUID;
                            std::unique_ptr<PlugIn_Waypoint> rte =
                                GetWaypoint_Plugin( m_nextNextWpGuid );
                            newWp = rte.get();
                            if ( m_nextNextWp )
                                delete m_nextNextWp;
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
void DashboardInstrument_RaceMark::getRmData(
    wxString &nextLegTwaAvg, wxString &nextLegTwaShortAvg,
    wxString &nextNextLegTwaAvg, wxString &nextNextLegTwaShortAvg )
{
    nextLegTwaAvg = wxEmptyString;
    nextLegTwaShortAvg = wxEmptyString;
    nextNextLegTwaAvg = wxEmptyString;
    nextNextLegTwaShortAvg = wxEmptyString;
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
                CheckForActivatedWp();
            } // then we do not have yet the next mark as target
            if ( m_targetWp ) {
                if ( !CheckWpStillValid( m_targetWpGuid ) ) {
                    ClearRoutesAndWPs();
                } // then the waypoint we thing as next target has disappeared
                int targetArrivalStatus = PollForNextActiveRouteWp();
                if ( targetArrivalStatus < 0 ) {
                    ClearRoutesAndWPs();
                } // then arrived or an issue with the arrival to the mark
                else {
                    if ( targetArrivalStatus > 0 ) {
                        if ( ChangeToNextTargetMark() ) {
                            ChangeNextAndNextNextMarks();
                        } // then successfull change to the next target mark
                        else {
                            ClearRoutesAndWPs();
                            wxLogMessage(
                                "dashboard_tactics_pi: "
                                "DashboardInstrument_RaceMark:: - "
                                "timer thread - Error: "
                                "routing status reports inconsistency in arrived / "
                                "next route WP GUIDs/names - please report details "
                                "of your route." );
                            wxString message(
                                _("Sorry, cannot determine what is the next mark") +
                                "\n" + _("Check that all waypoints have a name.") );
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
    } //else keep on polling for the startline changes

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
    
    return;
}
