/******************************************************************************
* $Id: RaceStart.cpp, v1.0 2019/11/30 VaderDarth Exp $
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
#include "RaceStart.h"
using namespace std::placeholders;
#include "plugin_ids.h"
#include "TacticsFunctions.h"

extern int GetRandomNumber(int, int);

wxBEGIN_EVENT_TABLE (DashboardInstrument_RaceStart, InstruJS)
   EVT_TIMER (myID_TICK_RACESTART, DashboardInstrument_RaceStart::OnThreadTimerTick)
   EVT_CLOSE (DashboardInstrument_RaceStart::OnClose)
wxEND_EVENT_TABLE ()
//************************************************************************************************************************
// Class providing assistance for race start line tactics
//************************************************************************************************************************

DashboardInstrument_RaceStart::DashboardInstrument_RaceStart(
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
    m_renStartLineDrawn = false;
    m_renWindBiasDrawn = false;
    m_dataRequestOn = false;
    ClearRoutesAndWPs();

    /*
      Subcscribe to some interesting data, for compatibility reasons
      we use OpenCPN Dashboard paths, so if there is Signal K paths
      they are considered having passing first through OpenCPN. This way,
      this instrument can be used both with OpenCPN and Signal K with no mods.
    */
    m_Twa = std::nan("1");
    m_pushTwaHere = std::bind(
        &DashboardInstrument_RaceStart::PushTwaHere, this, _1, _2, _3 );
    m_pushTwaUUID = m_pparent->subscribeTo ( _T("OCPN_DBP_STC_TWA"), m_pushTwaHere );
    m_Tws = std::nan("1");
    m_pushTwsHere = std::bind(
        &DashboardInstrument_RaceStart::PushTwsHere, this, _1, _2, _3 );
    m_pushTwsUUID = m_pparent->subscribeTo ( _T("OCPN_DBP_STC_TWS"), m_pushTwsHere );
    m_Cog = std::nan("1");
    m_pushCogHere = std::bind(
        &DashboardInstrument_RaceStart::PushCogHere, this, _1, _2, _3 );
    m_pushCogUUID = m_pparent->subscribeTo ( _T("OCPN_DBP_STC_COG"), m_pushCogHere );
    m_Lat = std::nan("1");
    m_pushLatHere = std::bind(
        &DashboardInstrument_RaceStart::PushLatHere, this, _1, _2, _3 );
    m_pushLatUUID = m_pparent->subscribeTo ( _T("OCPN_DBP_STC_LAT"), m_pushLatHere );
    m_Lon = std::nan("1");
    m_pushLonHere = std::bind(
        &DashboardInstrument_RaceStart::PushLonHere, this, _1, _2, _3 );
    m_pushLonUUID = m_pparent->subscribeTo ( _T("OCPN_DBP_STC_LON"), m_pushLonHere );
    
    /* 
       Startline set by us as a "route" with two waypoints, it is persistant,
       check if it is there right now when this instrument is started.
    */
    m_renDistanceToStartLine = std::nan("1");
    m_renDistanceCogToStartLine = std::nan("1");
    (void) CheckForValidStartLineGUID (
        _T(RACESTART_GUID_STARTLINE_AS_ROUTE), _T(RACESTART_NAME_STARTLINE_AS_ROUTE),
        _T(RACESTART_NAME_WP_STARTPORT), _T(RACESTART_NAME_WP_STARTSTBD) );

    if ( !LoadConfig() )
        return;

    /*
      We subscribe as sub-renderer to Tactics' OpenGL rendering services
    */
    m_rendererIsHere = std::bind(
        &DashboardInstrument_RaceStart::DoRenderGLOverLay, this, _1, _2 );
    m_sRendererCallbackUUID = m_pparent->registerGLRenderer(
        _T("DashboardInstrument_RaceStart"), m_rendererIsHere );

    if ( !m_fullPathHTML.IsEmpty() ) {
        m_pThreadRaceStartTimer = new wxTimer( this, myID_TICK_RACESTART );
        wxMilliSleep( GetRandomNumber( 800,1600 ) ); // avoid start loading all instruments simultaneously
        m_pThreadRaceStartTimer->Start(1000, wxTIMER_CONTINUOUS);
    } // then a reason to launch InstruJS, there is a page to load
}
DashboardInstrument_RaceStart::~DashboardInstrument_RaceStart(void)
{
    this->m_pThreadRaceStartTimer->Stop();
    delete this->m_pThreadRaceStartTimer;
    if ( !this->m_sRendererCallbackUUID.IsEmpty() )
        this->m_pparent->unregisterGLRenderer( this->m_sRendererCallbackUUID );
    if ( !this->m_pushTwaUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_pushTwaUUID );
    if ( !this->m_pushTwsUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_pushTwsUUID );
    if ( !this->m_pushCogUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_pushCogUUID );
    if ( !this->m_pushLatUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_pushLatUUID ); 
    if ( !this->m_pushLonUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_pushLonUUID ); 
    SaveConfig();
    return;
}
void DashboardInstrument_RaceStart::OnClose( wxCloseEvent &event )
{
    this->m_pThreadRaceStartTimer->Stop();
    this->stopScript(); // base class implements, we are first to be called
    if ( !this->m_sRendererCallbackUUID.IsEmpty() )
        this->m_pparent->unregisterGLRenderer( this->m_sRendererCallbackUUID );
    this->m_sRendererCallbackUUID = wxEmptyString;
    if ( !this->m_pushTwaUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_pushTwaUUID );
    this->m_pushTwaUUID = wxEmptyString;
    if ( !this->m_pushTwsUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_pushTwsUUID );
    this->m_pushTwsUUID = wxEmptyString;
    if ( !this->m_pushCogUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_pushCogUUID );
    this->m_pushCogUUID = wxEmptyString;
    if ( !this->m_pushLatUUID.IsEmpty() )
        this->m_pparent->unsubscribeFrom( m_pushLatUUID );
    this->m_pushLatUUID = wxEmptyString;
    event.Skip(); // Destroy() must be called
}

void DashboardInstrument_RaceStart::derivedTimeoutEvent()
{
    m_data = L"0.0";
    m_Twa = std::nan("1");
    m_Tws = std::nan("1");
    m_Cog = std::nan("1");
    m_Lat = std::nan("1");
    m_Lon = std::nan("1");
    m_renDistanceToStartLine = std::nan("1");
    m_renDistanceCogToStartLine = std::nan("1");
    derived2TimeoutEvent();
}

bool DashboardInstrument_RaceStart::IsAllMeasurementDataValid()
{
    if ( !std::isnan(m_Twa) && !std::isnan(m_Tws) && !std::isnan(m_Cog) &&
         !std::isnan(m_Lat) && !std::isnan(m_Lon) )
        return true;
    return false;
}

void DashboardInstrument_RaceStart::PushTwaHere(
    double data, wxString unit, long long timestamp)
{
    if ( !std::isnan(data) )
        setTimestamp( timestamp );
    m_Twa = data;
}

void DashboardInstrument_RaceStart::PushTwsHere(
    double data, wxString unit, long long timestamp)
{
    if ( !std::isnan(data) )
        setTimestamp( timestamp );
    m_Tws = data;
}

void DashboardInstrument_RaceStart::PushCogHere(
    double data, wxString unit, long long timestamp)
{
    if ( !std::isnan(data) )
        setTimestamp( timestamp );
    m_Cog = data;
}

void DashboardInstrument_RaceStart::PushLatHere(
    double data, wxString unit, long long timestamp)
{
    if ( !std::isnan(data) )
        setTimestamp( timestamp );
    m_Lat = data;
}

void DashboardInstrument_RaceStart::PushLonHere(
    double data, wxString unit, long long timestamp)
{
    if ( !std::isnan(data) )
        setTimestamp( timestamp );
    m_Lon = data;
}

void DashboardInstrument_RaceStart::ClearRoutesAndWPs()
{
    m_startWestWp =  nullptr;
    m_startEastWp = nullptr;
    m_sStartLineAsRouteGuid = wxEmptyString;
    m_startLineAsRoute = nullptr;
    m_sStartStbdWpGuid = wxEmptyString;
    m_startStbdWp = nullptr;
    m_sStartPortWpGuid = wxEmptyString;
    m_startPortWp = nullptr;
    m_startPointMoveDelayCount = 0;
    m_startWestWp = nullptr;
    m_startEastWp = nullptr;
    m_renDistanceToStartLine = std::nan("1");
    m_renDistanceCogToStartLine = std::nan("1");
}

// This method checks if the candiate for persistent startline is valid
bool DashboardInstrument_RaceStart::CheckForValidStartLineGUID( wxString sGUID, wxString lineName, wxString portName, wxString stbdName)
{
    if ( sGUID.IsEmpty() )
        return false;
    bool startlineAsRouteValid = true;
    m_sStartLineAsRouteGuid = sGUID;
    std::unique_ptr<PlugIn_Route> rte = GetRoute_Plugin( m_sStartLineAsRouteGuid );
    m_startLineAsRoute = rte.get();
    if ( m_startLineAsRoute ) {
        if ( !(m_startLineAsRoute->pWaypointList) )
            startlineAsRouteValid = false;
        else {
            if ( m_startLineAsRoute->m_NameString.CmpNoCase( lineName ) != 0 )
                startlineAsRouteValid = false;
            else {
                if ( m_startLineAsRoute->pWaypointList->GetCount() != 2 )
                    startlineAsRouteValid = false;
                else {
                    wxPlugin_WaypointListNode *firstWpNode =
                        m_startLineAsRoute->pWaypointList->GetFirst();
                    PlugIn_Waypoint *firstWp = firstWpNode->GetData();
                    if ( firstWp->m_MarkName.CmpNoCase( stbdName ) == 0 ) {
                        m_sStartStbdWpGuid = firstWp->m_GUID;
                        m_startStbdWp = firstWp;
                    } // then starts w/ starboard side waypoint
                    else if ( firstWp->m_MarkName.CmpNoCase( portName) == 0 ) {
                        m_sStartPortWpGuid = firstWp->m_GUID;
                        m_startPortWp = firstWp;
                    } // else then starts with port side waypoint
                    else
                        startlineAsRouteValid = false;
                    if ( startlineAsRouteValid ) {
                        wxPlugin_WaypointListNode *secondWpNode =
                            m_startLineAsRoute->pWaypointList->GetLast();
                        PlugIn_Waypoint *secondWp = secondWpNode->GetData();
                        if ( secondWp->m_MarkName.CmpNoCase( stbdName ) == 0 ) {
                            if ( m_sStartStbdWpGuid.IsEmpty() ) {
                                m_sStartStbdWpGuid = secondWp->m_GUID;
                                m_startStbdWp = secondWp;
                            } // then starboard marker not yet found
                            else
                                startlineAsRouteValid = false;
                        } // then starborad side waypoint
                        else if ( secondWp->m_MarkName.CmpNoCase( portName ) == 0 ) {
                            if ( m_sStartPortWpGuid.IsEmpty() ) {
                                m_sStartPortWpGuid = secondWp->m_GUID;
                                m_startPortWp = secondWp;
                            } // then port side marker not yet found
                            else
                                startlineAsRouteValid = false;
                        } // else then port side waypoint
                        else
                            startlineAsRouteValid = false;
                    } // then there is at least first point OK
                    // in case bad names, same names, etc. gone wrong:
                    if ( !(m_startStbdWp) || !(m_startPortWp) )
                        startlineAsRouteValid = false;
                } // else valid number of waypoints
            } // else a valid name for the startline route
        } // else there is a pointer to waypoint list
    } // then a route has been returned
    else
        startlineAsRouteValid = false;
    m_startWestWp =  nullptr;
    m_startEastWp = nullptr;
    if ( !startlineAsRouteValid ) {
        ClearRoutesAndWPs();
    } // then  some points given by routing, possibly, but can't map them to start
    return startlineAsRouteValid;
}

bool DashboardInstrument_RaceStart::CheckForValidUserSetStartLine()
{
    wxString activeRouteGUID = m_pparent->GetActiveRouteGUID();
    if ( activeRouteGUID.IsEmpty() )
        return false;
    wxString activeRouteName = m_pparent->GetActiveRouteName();
    if ( activeRouteName.IsEmpty() )
        return false;
    if ( activeRouteName.CmpNoCase( _T(RACESTART_NAME_STARTLINE_AS_ROUTE_USER) ) != 0 )
        return false;
    // There is an active route named with the matching user name nomenclature, let's study it
    return CheckForValidStartLineGUID(
        activeRouteGUID, activeRouteName,
        _T(RACESTART_NAME_WP_STARTPORT_USER), _T(RACESTART_NAME_WP_STARTSTBD_USER) );
}

// The startline is a route and can be killed in route manager of OpenCPN
bool DashboardInstrument_RaceStart::CheckStartLineStillValid()
{
    std::unique_ptr<PlugIn_Route> rte = GetRoute_Plugin( m_sStartLineAsRouteGuid );
    PlugIn_Route *selectedRouteAsStartLineStillThere = rte.get();
    if ( selectedRouteAsStartLineStillThere )
        return true;
    return false;
}

// The JavaScript part asks if we are ready: with no data, we are not
bool DashboardInstrument_RaceStart::instruIsReady()
{
    return IsAllMeasurementDataValid();
}

// The JavaScript part asks do we have a startline?
bool DashboardInstrument_RaceStart::userHasStartline()
{
    if ( (m_startStbdWp != nullptr) && (m_startPortWp != nullptr) )
        return true;
    return false;
}

// User has pressed the Starboard button to drop a mark
bool DashboardInstrument_RaceStart::dropStarboardMark()
{
    if ( !IsAllMeasurementDataValid() )
        return false;
    wxString wpGUID = _T(RACESTART_GUID_WP_STARTSTBD);
    wxString wpName = _T(RACESTART_NAME_WP_STARTSTBD);
    (void) DeleteSingleWaypoint ( wpGUID );
    m_startStbdWp = new PlugIn_Waypoint(
        m_Lat, m_Lon, _T("Symbol-Diamond-Green"),
        wpName, wpGUID );
    AddSingleWaypoint(m_startStbdWp, true);
    return true;
}
// User has pressed the Port button to drop a mark
bool DashboardInstrument_RaceStart::dropPortMark()
{
    if ( !IsAllMeasurementDataValid() )
        return false;
    wxString wpGUID = _T(RACESTART_GUID_WP_STARTPORT);
    wxString wpName = _T(RACESTART_NAME_WP_STARTPORT);
    (void) DeleteSingleWaypoint (wpGUID );
    m_startPortWp = new PlugIn_Waypoint(
        m_Lat, m_Lon, _T("Symbol-Diamond-Red"),
        wpName, wpGUID );
    AddSingleWaypoint(m_startPortWp, true);
    return true;
}

bool DashboardInstrument_RaceStart::CheckForPreviouslyDroppedStartLine()
{
    if ( CheckStartLineStillValid() )
        return false; // route based, fixed startline

    if ( (m_startStbdWp != nullptr) && (m_startPortWp != nullptr) )
        return false; // there is an on-going startline business

    bool thereAreValidPoints = true;
    
    wxString wpPortGUID = _T(RACESTART_GUID_WP_STARTPORT);
    wxString wpPortName = _T(RACESTART_NAME_WP_STARTPORT);
    m_startPortWp = new PlugIn_Waypoint();
    if ( !GetSingleWaypoint( wpPortGUID, m_startPortWp ) )
        thereAreValidPoints = false;
    wxString wpStbdGUID = _T(RACESTART_GUID_WP_STARTSTBD);
    wxString wpStbdName = _T(RACESTART_NAME_WP_STARTSTBD);
    m_startStbdWp = new PlugIn_Waypoint();
    if ( !GetSingleWaypoint( wpStbdGUID, m_startStbdWp ) )
        thereAreValidPoints = false;

    if ( !thereAreValidPoints )
        return false;

    wxString message(
        _("There is an existing startline from previous race or start. ") + "\n" +
        _("Do you want to keep it?") + "\n"
        );
    wxString msgButtonYes( _("Yes - Keep") );
    wxString msgButtonNo(  _("No - I will drop new marks") );
    wxMessageDialog *dlg = new wxMessageDialog(
        GetOCPNCanvasWindow(), message, _T("DashT Race Start"),
        wxYES_NO);
    if ( !dlg->SetYesNoLabels( ("&" + msgButtonYes),
                               ("&" + msgButtonNo) ) ) {
        wxString messageExtended(
            _("Yes: Keep existing | No: I will drop new marks")
            );
        dlg->SetExtendedMessage( messageExtended );
    } // then cannot change button lables on this platform
    int choice =  dlg->ShowModal();

    if ( choice == wxID_NO ) {
        (void) DeleteSingleWaypoint( wpStbdGUID );
        (void) DeleteSingleWaypoint( wpPortGUID );
        ClearRoutesAndWPs();
        return false;
    }
    return true;
}

bool DashboardInstrument_RaceStart::CheckForMovedUserDroppedWaypoints()
{
    if ( CheckStartLineStillValid() )
        return false; // route based, fixed startline

    if ( (m_startStbdWp == nullptr) || (m_startPortWp == nullptr) )
        return false; // there is no user dropped waypoints

    bool thereIsValidChange = false;

    wxString wpPortGUID = _T(RACESTART_GUID_WP_STARTPORT);
    wxString wpPortName = _T(RACESTART_NAME_WP_STARTPORT);
    PlugIn_Waypoint *cmpStartPortWp = new PlugIn_Waypoint();
    cmpStartPortWp->m_lat = -999.0;
    cmpStartPortWp->m_lon = -999.0;
    if ( GetSingleWaypoint( wpPortGUID, cmpStartPortWp ) ) {
        if ( cmpStartPortWp->m_lat != -999.0 ) {
            if ( m_startPortWp->m_lat != cmpStartPortWp->m_lat  )
                thereIsValidChange = true;
        }
        if ( cmpStartPortWp->m_lon != -999.0 ) {
            if ( m_startPortWp->m_lon != cmpStartPortWp->m_lon  )
                thereIsValidChange = true;
        }
    } // then there is a dropped waypoint for port side

    wxString wpStbdGUID = _T(RACESTART_GUID_WP_STARTSTBD);
    wxString wpStbdName = _T(RACESTART_NAME_WP_STARTSTBD);
    PlugIn_Waypoint *cmpStartStbdWp = new PlugIn_Waypoint();
    cmpStartStbdWp->m_lat = -999.0;
    cmpStartStbdWp->m_lon = -999.0;
    if ( GetSingleWaypoint( wpStbdGUID, cmpStartStbdWp ) ) {
        if ( cmpStartStbdWp->m_lat != -999.0 ) {
            if ( m_startStbdWp->m_lat != cmpStartStbdWp->m_lat  )
                thereIsValidChange = true;
        }
        if ( cmpStartStbdWp->m_lon != -999.0 ) {
            if ( m_startStbdWp->m_lon != cmpStartStbdWp->m_lon  )
                thereIsValidChange = true;
        }
    } // then there is a dropped waypoint for port side

    if ( thereIsValidChange ) {
        m_startPointMoveDelayCount++;
        if ( m_startPointMoveDelayCount < RACESTART_USER_MOVING_WP_GRACETIME_CNT )
            return false;
    } // else tracking the change with a delay
    else {
        m_startPointMoveDelayCount = 0;
        return false;
    } // no change until now

    wxString message(
        _("Startline waypoint markers have been moved. ") + "\n" +
        _("Do you want to move startline to the new location?")
        );
    wxString msgButtonYes( _("Yes - Move") );
    wxString msgButtonNo(  _("No - Keep old") );
    wxString msgButtonCancel(  _("Cancel - continue") );
    wxMessageDialog *dlg = new wxMessageDialog(
        GetOCPNCanvasWindow(), message, _T("DashT Race Start"),
        wxYES_NO|wxCANCEL|wxICON_EXCLAMATION);
    if ( !dlg->SetYesNoCancelLabels( ("&" + msgButtonYes),
                                     ("&" + msgButtonNo),
                                     ("&" + msgButtonCancel) ) ) {
        wxString messageExtended(
            _("Yes: move startline | No: keep previous | Cancel: keep on moving")
            );
        dlg->SetExtendedMessage( messageExtended );
    } // then cannot change button lables on this platform
    int choice =  dlg->ShowModal();

    if ( choice == wxID_YES ) {
        m_startStbdWp->m_lat = cmpStartStbdWp->m_lat;
        m_startStbdWp->m_lon = cmpStartStbdWp->m_lon;
        UpdateSingleWaypoint( m_startStbdWp );
        m_startPortWp->m_lat = cmpStartPortWp->m_lat;
        m_startPortWp->m_lon = cmpStartPortWp->m_lon;
        UpdateSingleWaypoint( m_startPortWp );
        m_startPointMoveDelayCount = 0;
        return true;
    }

    if ( choice == wxID_NO ) {
        UpdateSingleWaypoint( m_startStbdWp );
        UpdateSingleWaypoint( m_startPortWp );
    }

    m_startPointMoveDelayCount = 0;
    return false;
}

void DashboardInstrument_RaceStart::OnThreadTimerTick( wxTimerEvent &event )
{
    m_pThreadRaceStartTimer->Stop();
    
    if ( !(m_startLineAsRoute) ) {
        if ( !CheckForValidUserSetStartLine() ) {
            (void) CheckForPreviouslyDroppedStartLine();
            (void) CheckForMovedUserDroppedWaypoints();
        } // then give priority to user set route based startline
    } // then no points, maybe a user route points, or dropped points
    else {
        if ( !CheckStartLineStillValid() )
            ClearRoutesAndWPs();
    } // else user route points, but maybe killed, meanwhile

    if ( !m_htmlLoaded) {
        if ( testHTTPServer( m_httpServer ) ) {
            if ( (m_goodHttpServerDetects == -1) ||
                 (m_goodHttpServerDetects >= RACESTART_WAIT_NEW_HTTP_SERVER_TICKS) ) {
                wxSize thisSize = wxControl::GetSize();
                wxSize thisFrameInitSize = GetSize( m_orient, thisSize );
                SetInitialSize ( thisFrameInitSize );
                wxSize webViewInitSize = thisFrameInitSize;
                this->loadHTML( m_fullPathHTML, webViewInitSize );
                // No more threaded jobs, InstruJS is working now
                m_htmlLoaded= true;
                m_pThreadRaceStartTimer->Start( GetRandomNumber( 800,1100 ),
                                                wxTIMER_CONTINUOUS);
            } // then either a straigth start with server or detected a stable server
            else {
                m_goodHttpServerDetects += 1;
                m_pThreadRaceStartTimer->Start( GetRandomNumber( 800,1100 ),
                                                wxTIMER_CONTINUOUS);
            }
        } // then there is a server serving the page, can ask content to be loaded
        else {
            m_goodHttpServerDetects = 0;
            m_pThreadRaceStartTimer->Start( GetRandomNumber( 8000,11000 ),
                                            wxTIMER_CONTINUOUS);
        }  // else need to wait until a server appears
    } // then there is not page loaded keep on trying
    else {
        m_pThreadRaceStartTimer->Start( GetRandomNumber( 800,1100 ),
                                        wxTIMER_CONTINUOUS);
    } //else keep on polling for the startline changes

}

wxSize DashboardInstrument_RaceStart::GetSize( int orient, wxSize hint )
{
    int x,y;
    m_orient = orient;
    if( m_orient == wxHORIZONTAL ) {
        x = RACESTART_H_MIN_WIDTH;
        y = wxMax( hint.y, RACESTART_H_MIN_HEIGHT );
    }
    else {
        x = wxMax( hint.x, RACESTART_V_MIN_WIDTH );
        y = RACESTART_V_MIN_HEIGHT;
    }
    return wxSize( x, y );
}

bool DashboardInstrument_RaceStart::LoadConfig()
{
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return false;
    
    // Make a proposal for the defaul path _and_ the protocool, which user can then override in the file:
    wxString sFullPathHTML = "http://127.0.0.1:8088/racedashstart/";

    pConf->SetPath( _T("/PlugIns/DashT/WebView/RaceStart/") );
    pConf->Read( _T("instrujsURL"), &m_fullPathHTML, sFullPathHTML );

    m_httpServer = this->testURLretHost( m_fullPathHTML );
    
    if ( m_httpServer.IsEmpty() ) {
        wxString message(
            _("Malformed URL string in WebView/RaceStart ini-file entry: ") + "\n" );
        message += m_fullPathHTML;
        wxMessageDialog *dlg = new wxMessageDialog(
            GetOCPNCanvasWindow(), message, _T("DashT Race Start"), wxOK|wxICON_ERROR);
        (void) dlg->ShowModal();
        m_fullPathHTML = wxEmptyString;
    }

    pConf->SetPath( _T("/PlugIns/DashT/Race/RaceStart/") );
    pConf->Read( _T("LaylineWidth"), &m_renLaylineWidth, RACESTART_LAYL_LINE_WIDTH );
    m_renDrawLaylines = true;
    if ( m_renLaylineWidth <= 0 ) {
        m_renLaylineWidth = 0;
        m_renDrawLaylines = false;
    }
    pConf->Read( _T("GridSize"), &m_renGridSize, RACESTART_GRID_SIZE );
    m_renGridSize = abs(m_renGridSize);
    if ( m_renGridSize == 0. )
        m_renGridSize = RACESTART_GRID_SIZE;
    pConf->Read( _T("GridStep"), &m_renGridStep, RACESTART_GRID_STEP );
    m_renGridStep = abs(m_renGridStep);
    if ( m_renGridStep == 0. )
        m_renGridStep = RACESTART_GRID_STEP;
    pConf->Read( _T("GridLineWidth"), &m_renGridLineWidth, RACESTART_GRID_LINE_WIDTH );
    if ( m_renGridLineWidth < 0 )
        m_renGridLineWidth = 0;
    m_renDrawGrid = true;
    if ( m_renGridLineWidth == 0 )
        m_renDrawGrid = false;
     pConf->Read( _T("GridBoldInterval"), &m_renGridBoldInterval, RACESTART_GRID_BOLD_INTERVAL );
    if ( m_renGridBoldInterval < 1 )
        m_renGridBoldInterval = 1;
    pConf->Read( _T("ZeroBurnSeconds"), &m_renZeroBurnSeconds, RACESTART_ZERO_BURN_BY_POLAR_SECONDS );
    m_renZeroBurnSeconds = abs(m_renZeroBurnSeconds);
    if ( m_renZeroBurnSeconds > RACESTART_ZERO_BURN_BY_POLAR_SECONDS_UPPER_LIMIT )
        m_renZeroBurnSeconds = RACESTART_ZERO_BURN_BY_POLAR_SECONDS_UPPER_LIMIT;
    if ( m_renZeroBurnSeconds < RACESTART_ZERO_BURN_BY_POLAR_SECONDS_LOWER_LIMIT )
        m_renZeroBurnSeconds = 0;
       
    return true;
}

void DashboardInstrument_RaceStart::SaveConfig()
{
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return;

    pConf->SetPath(_T("/PlugIns/DashT/WebView/RaceStart/"));
    pConf->Write(_T("instrujsURL"), m_fullPathHTML );
    pConf->SetPath( _T("/PlugIns/DashT/Race/RaceStart/") );
    pConf->Write( _T("LaylineWidth"), m_renLaylineWidth ); 
    pConf->Write( _T("GridSize"), m_renGridSize );
    pConf->Write( _T("GridStep"), m_renGridStep );
    pConf->Write( _T("GridLineWidth"), m_renGridLineWidth );
    pConf->Write( _T("GridBoldInterval"), m_renGridBoldInterval );
    pConf->Write( _T("ZeroBurnSeconds"), m_renZeroBurnSeconds );
    
    return;
}
