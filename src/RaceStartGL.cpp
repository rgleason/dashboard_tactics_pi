/******************************************************************************
* $Id: RaceStartGL.cpp, v1.0 2019/11/30 VaderDarth Exp $
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
#include <wx/glcanvas.h>

#include "RaceStart.h"
#include "tactics_pi_ext.h"
using namespace std::placeholders;


void DashboardInstrument_RaceStart::DoRenderGLOverLay(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    this->RenderGLStartLine( pcontext, vp );
    this->RenderGLWindBias( pcontext, vp );
    this->RenderGLLaylines( pcontext, vp );
    this->RenderGLGrid( pcontext, vp );
    this->RenderGLZeroBurn(  pcontext, vp );
}

void DashboardInstrument_RaceStart::RenderGLStartLine(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    if ( !(m_startStbdWp) || !(m_startPortWp) ) {
        m_renStartLineDrawn = false;
        return;
    }
    GetCanvasPixLL(
        vp, &m_renPointStbd, m_startStbdWp->m_lat, m_startStbdWp->m_lon );
    GetCanvasPixLL(
        vp, &m_renPointPort, m_startPortWp->m_lat, m_startPortWp->m_lon );
    glColor4ub(255, 128, 0, 168); //orange
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2d( m_renPointStbd.x, m_renPointStbd.y );
    glVertex2d( m_renPointPort.x, m_renPointPort.y );
    glEnd();
    m_renStartLineDrawn = true;
}

void DashboardInstrument_RaceStart::RenderGLWindBias(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    double AvgWind = AverageWind->GetAvgWindDir();
    if ( !( m_renStartLineDrawn && !std::isnan( AvgWind )) ) {
        m_renWindBiasDrawn = false;
        m_startWestWp = nullptr;
        m_startEastWp = nullptr;
        m_renSlineLength = std::nan("1");
        m_renSlineDir = std::nan("1");
        m_renBiasSlineDir = std::nan("1");
        m_renWindBias = std::nan("1");
        m_renWindBiasAdvDist = std::nan("1");
        m_renWindBiasAdvDir = std::nan("1");
        m_renWindBiasLineDir = std::nan("1");
        return;
    }
    if ( (AvgWind >= 90.0) && (AvgWind <= 270.0) ) {
        m_startWestWp = m_startStbdWp;
        m_renPointWest = m_renPointStbd;
        m_startEastWp = m_startPortWp;
        m_renPointEast = m_renPointPort;
        m_renbNorthSector = false;
    } // then wind from southern segment
    else {
        m_startWestWp = m_startPortWp;
        m_renPointWest = m_renPointPort;
        m_startEastWp = m_startStbdWp;
        m_renPointEast = m_renPointStbd;
        m_renbNorthSector = true;
    } // else wind from northern segment
    DistanceBearingMercator_Plugin(
        m_startEastWp->m_lat, m_startEastWp->m_lon, // "to"
        m_startWestWp->m_lat, m_startWestWp->m_lon, // "from"
        &m_renSlineDir, &m_renSlineLength ); // result
    double zeroBiasWindDir;
    if ( m_renbNorthSector ) {
        zeroBiasWindDir = m_renSlineDir - 90.0;
        if ( zeroBiasWindDir < 0. ) {
            zeroBiasWindDir = 360. + zeroBiasWindDir;
        }
    }
    else {
        zeroBiasWindDir = m_renSlineDir + 90.0;
    }
    m_renWindBias = getSignedDegRange( zeroBiasWindDir, AvgWind );
    PlugIn_Waypoint *startPointBiasLine;
    PlugIn_Waypoint *endRefPointBiasLine;
    if ( m_renWindBias < 0. ) {
        startPointBiasLine = m_startPortWp;
        endRefPointBiasLine = m_startStbdWp;
        if ( m_renbNorthSector )
            m_renWindBiasLineDir = m_renSlineDir + m_renWindBias;
        else
            m_renWindBiasLineDir = m_renSlineDir -180. + m_renWindBias;
    } // then wind veers
    else {
        startPointBiasLine = m_startStbdWp;
        endRefPointBiasLine = m_startPortWp;
        if ( m_renbNorthSector )
            m_renWindBiasLineDir = m_renSlineDir - 180. + m_renWindBias;
        else
            m_renWindBiasLineDir = m_renSlineDir + m_renWindBias;
    } // else wind backs
    if ( m_renWindBiasLineDir > 360. )
        m_renWindBiasLineDir -= 360.;
    if ( m_renWindBiasLineDir < 0. )
        m_renWindBiasLineDir = 360. + m_renWindBiasLineDir;
    double startPointBiasEnd_lat;
    double startPointBiasEnd_lon;
    PositionBearingDistanceMercator_Plugin(
        startPointBiasLine->m_lat, startPointBiasLine->m_lon,
        m_renWindBiasLineDir, m_renSlineLength,
        &startPointBiasEnd_lat, &startPointBiasEnd_lon );
    DistanceBearingMercator_Plugin(
        startPointBiasEnd_lat, startPointBiasEnd_lon, // "to"
        endRefPointBiasLine->m_lat, endRefPointBiasLine->m_lon, // "from"
        &m_renWindBiasAdvDir, &m_renWindBiasAdvDist ); // result = "advantage" dist+direction
     GetCanvasPixLL(
        vp, &m_renPointBiasStart,
        startPointBiasLine->m_lat, startPointBiasLine->m_lon );
    GetCanvasPixLL(
        vp, &m_renPointBiasStop, startPointBiasEnd_lat, startPointBiasEnd_lon );
    // draw the wind turning biased "ladder rungs" start, as dotted line
    glEnable(GL_LINE_STIPPLE); // discontinuing line, stipple
    glLineWidth(4);
    glColor4ub(0, 0, 0, 168); // black, somwwhat opaque
    glLineStipple(5, 0xAAAA);  /* long dash */
    // glLineStipple(5, 0x0101);  /*  dotted  */
    // glLineStipple(5, 0x00FF);  /*  dashed  */
    // glLineStipple(5, 0x1C47);  /*  dash/dot/dash */
    glBegin(GL_LINES);
    glVertex2d( m_renPointBiasStart.x, m_renPointBiasStart.y );
    glVertex2d( m_renPointBiasStop.x, m_renPointBiasStop.y );
    glEnd();
    glDisable(GL_LINE_STIPPLE); //Disabling the Line Type.
    m_renWindBiasDrawn = true;
}

void DashboardInstrument_RaceStart::RenderGLLaylines(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    if ( !( m_renStartLineDrawn && m_renWindBiasDrawn &&
            !std::isnan(m_renSlineDir) && !std::isnan(m_renWindBias) ) ) {
        m_renLLPortDir = std::nan("1");
        m_renLLStbdDir = std::nan("1");
        m_renLaylinesDrawn = false;
        return;
    }
    if ( m_renbNorthSector ) {
        m_renLLStbdDir = m_renSlineDir + 45.0 + m_renWindBias;
        m_renLLPortDir = m_renLLStbdDir + 90.;
    }
    else {
        m_renLLPortDir = m_renLLStbdDir - 45.0 - m_renWindBias;
        if ( m_renLLPortDir < 0. )
            m_renLLPortDir += 360.0;
        m_renLLStbdDir = m_renSlineDir - 90.0;
        if ( m_renLLStbdDir < 0. )
            m_renLLStbdDir += 360.0;
    }
    double llWestStbd_lat;
    double llWestStbd_lon;
    PositionBearingDistanceMercator_Plugin(
        m_startWestWp->m_lat, m_startWestWp->m_lon,
        m_renLLStbdDir, RACESTART_GRID_SIZE,
        &llWestStbd_lat, &llWestStbd_lon );
    wxPoint llWestEndStdb;
    GetCanvasPixLL( vp, &llWestEndStdb, llWestStbd_lat, llWestStbd_lon );
    glColor4ub(0, 200, 0, 128); // green
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2d( m_renPointWest.x, m_renPointWest.y );
    glVertex2d( llWestEndStdb.x, llWestEndStdb.y );
    glEnd();
    double llWestPort_lat;
    double llWestPort_lon;
    PositionBearingDistanceMercator_Plugin(
        m_startWestWp->m_lat, m_startWestWp->m_lon,
        m_renLLPortDir, RACESTART_GRID_SIZE,
        &llWestPort_lat, &llWestPort_lon );
    wxPoint llWestEndPort;
    GetCanvasPixLL( vp, &llWestEndPort, llWestPort_lat, llWestPort_lon );
    glColor4ub(204, 41, 41, 128); // red
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2d( m_renPointWest.x, m_renPointWest.y );
    glVertex2d( llWestEndPort.x, llWestEndPort.y );
    glEnd();
    double llEastStbd_lat;
    double llEastStbd_lon;
    PositionBearingDistanceMercator_Plugin(
        m_startEastWp->m_lat, m_startEastWp->m_lon,
        m_renLLStbdDir, RACESTART_GRID_SIZE,
        &llEastStbd_lat, &llEastStbd_lon );
    wxPoint llEastEndStdb;
    GetCanvasPixLL( vp, &llEastEndStdb, llEastStbd_lat, llEastStbd_lon );
    glColor4ub(0, 200, 0, 128); // green
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2d( m_renPointEast.x, m_renPointEast.y );
    glVertex2d( llEastEndStdb.x, llEastEndStdb.y );
    glEnd();
    double llEastPort_lat;
    double llEastPort_lon;
    PositionBearingDistanceMercator_Plugin(
        m_startEastWp->m_lat, m_startEastWp->m_lon,
        m_renLLPortDir, RACESTART_GRID_SIZE,
        &llEastPort_lat, &llEastPort_lon );
    wxPoint llEastEndPort;
    GetCanvasPixLL( vp, &llEastEndPort, llEastPort_lat, llEastPort_lon );
    glColor4ub(204, 41, 41, 128); // red
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2d( m_renPointEast.x, m_renPointEast.y );
    glVertex2d( llEastEndPort.x, llEastEndPort.y );
    glEnd();

    m_renLaylinesDrawn = true;
 }

void DashboardInstrument_RaceStart::RenderGLGrid(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
}

void DashboardInstrument_RaceStart::RenderGLZeroBurn(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
}
