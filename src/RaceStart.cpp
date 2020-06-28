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
    ClearRoutesAndWPs();
    
    // Startline set by us as a "route" with two waypoints, it is persistant, check if it is there
    (void) CheckForValidStartLineGUID (
        _T(RACESTART_GUID_STARTLINE_AS_ROUTE), _T(RACESTART_NAME_STARTLINE_AS_ROUTE),
        _T(RACESTART_NAME_WP_STARTP), _T(RACESTART_NAME_WP_STARTS) );

    if ( !LoadConfig() )
        return;

    m_rendererIsHere = std::bind(&DashboardInstrument_RaceStart::DoRenderGLOverLay,
                                               this, _1, _2 );
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
    event.Skip(); // Destroy() must be called
}

void DashboardInstrument_RaceStart::derivedTimeoutEvent()
{
    m_data = L"0.0";
    derived2TimeoutEvent();
}

void DashboardInstrument_RaceStart::SetData(
    unsigned long long st, double data, wxString unit, long long timestamp)
{
    return; // this derived class gets its data from the multiplexer through a callback PushData()
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
    m_startWestWp = nullptr;
    m_startEastWp = nullptr;
    m_renStartLineDrawn = false;
    m_renSlineLength = std::nan("1");
    m_renSlineDir = std::nan("1");
    m_renBiasSlineDir = std::nan("1");
    m_renWindBias = std::nan("1");
    m_renWindBiasAdvDist = std::nan("1");
    m_renWindBiasAdvDir = std::nan("1");
    m_renWindBiasLineDir = std::nan("1");
    m_renWindBiasDrawn = false;
    m_renLLPortDir = std::nan("1");
    m_renLLStbdDir = std::nan("1");
    m_renLaylinesDrawn = false;
}


// This method checks if the candiate for persistent startline is valid
bool DashboardInstrument_RaceStart::CheckForValidStartLineGUID( wxString sGUID, wxString lineName, wxString portName, wxString stbdName) {
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

// This functionmethod  is called while approaching start line area and there are _no_ marks yet
bool DashboardInstrument_RaceStart::CheckForValidUserSetStartLine() {
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
        _T(RACESTART_NAME_WP_STARTP_USER), _T(RACESTART_NAME_WP_STARTS_USER) );
}

// The startline is a route and can be killed in route manager of OpenCPN
bool DashboardInstrument_RaceStart::CheckStartLineStillValid() {
    std::unique_ptr<PlugIn_Route> rte = GetRoute_Plugin( m_sStartLineAsRouteGuid );
    PlugIn_Route *selectedRouteAsStartLineStillThere = rte.get();
    if ( selectedRouteAsStartLineStillThere )
        return true;
    return false;
}

void DashboardInstrument_RaceStart::OnThreadTimerTick( wxTimerEvent &event )
{
    
    if ( !(m_startLineAsRoute) )
        (void) CheckForValidUserSetStartLine();
    else {
        if ( !CheckStartLineStillValid() )
            ClearRoutesAndWPs();
    }

    // m_pThreadRaceStartTimer->Stop();
    // if ( !m_htmlLoaded) {
    //     if ( testHTTPServer( m_httpServer ) ) {
    //         if ( (m_goodHttpServerDetects == -1) ||
    //              (m_goodHttpServerDetects >= RACESTART_WAIT_NEW_HTTP_SERVER_TICKS) ) {
    //             wxSize thisSize = wxControl::GetSize();
    //             wxSize thisFrameInitSize = GetSize( m_orient, thisSize );
    //             SetInitialSize ( thisFrameInitSize );
    //             wxSize webViewInitSize = thisFrameInitSize;
    //             this->loadHTML( m_fullPathHTML, webViewInitSize );
    //             // No more threaded jobs, InstruJS is working now
    //             m_htmlLoaded= true;
    //         } // then either a straigth start with server or detected a stable server
    //         else {
    //             m_goodHttpServerDetects += 1;
    //             m_pThreadRaceStartTimer->Start( GetRandomNumber( 800,1100 ), wxTIMER_CONTINUOUS);
    //         }
    //     } // then there is a server serving the page, can ask content to be loaded
    //     else {
    //         m_goodHttpServerDetects = 0;
    //         m_pThreadRaceStartTimer->Start( GetRandomNumber( 8000,11000 ), wxTIMER_CONTINUOUS);
    //     }  // else need to wait until a server appears
    // } // else thread is not running (no JS instrument created in this frame, create one)

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

    pConf->SetPath(_T("/PlugIns/DashT/WebView/RaceStart/"));
    pConf->Read(_T("instrujsURL"), &m_fullPathHTML, sFullPathHTML );

    m_httpServer = this->testURLretHost( m_fullPathHTML );
    
    if ( m_httpServer.IsEmpty() ) {
        wxString message( _("Malformed URL string in WebView/RaceStart ini-file entry: ") + "\n" );
        message += m_fullPathHTML;
        wxMessageDialog *dlg = new wxMessageDialog(
            GetOCPNCanvasWindow(), message, _T("DashT Race Start"), wxOK|wxICON_ERROR);
        (void) dlg->ShowModal();
        m_fullPathHTML = wxEmptyString;
    }
    
    return true;
}

void DashboardInstrument_RaceStart::SaveConfig()
{
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return;

    pConf->SetPath(_T("/PlugIns/DashT/WebView/RaceStart/"));
    pConf->Write(_T("instrujsURL"), m_fullPathHTML );
    
    return;
}
