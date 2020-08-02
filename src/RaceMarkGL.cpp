/******************************************************************************
* $Id: RaceMarkGL.cpp, v1.0 2019/11/30 VaderDarth Exp $
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
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
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

#include "RaceMark.h"
#include "tactics_pi_ext.h"
using namespace std::placeholders;

void DashboardInstrument_RaceMark::ClearRendererCalcs()
{
    m_renAvgWindDir = std::nan("1");
    m_renAvgWindDirWindward = std::nan("1");
    m_renShortAvgWindDir = std::nan("1");
    m_renShortAvgWindDirWindward = std::nan("1");
    m_renTargetPoint_lat = std::nan("1");
    m_renTargetPoint_lon = std::nan("1");
    m_renAvgWindRightPlaneDir = std::nan("1");
    m_renShortAvgWindRightPlaneDir = std::nan("1");
    m_renAvgWindLeftPlaneDir = std::nan("1");
    m_renShortAvgWindLeftPlaneDir = std::nan("1");
    m_renWindwardLeg = true;
    m_renReachingLeg = false;
    m_renLLStbdDir = std::nan("1");
    m_renShortLLStbdDir = std::nan("1");
    m_renLLPortDir = std::nan("1");
    m_renShortLLPortDir = std::nan("1");
    m_renLLlen = std::nan("1");
}

void DashboardInstrument_RaceMark::DoRenderGLOverLay(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    ClearRendererCalcs();
    if ( !(m_raceAsRoute) )
        return;
    // if ( !m_dataRequestOn )
    //     return;
    if ( m_overlayPauseRequestOn )
        return;
    if ( !(m_targetWp) )
        return;
    if ( !(m_previousWp) )
        return;
    if ( !IsAllMeasurementDataValid() )
        return;
    if ( !(AverageWind) )
        return;
    /*
      Collect common wisdom for this module
    */
    // Average Wind direction
    m_renAvgWindDir = AverageWind->GetAvgWindDir();
    m_renAvgWindDirWindward = m_renAvgWindDir + 180.;
    if ( m_renAvgWindDirWindward > 360. )
        m_renAvgWindDirWindward -= 360.;
    m_renShortAvgWindDir = AverageWind->GetShortAvgWindDir();
    m_renShortAvgWindDirWindward = m_renShortAvgWindDir + 180.;
    if ( m_renShortAvgWindDirWindward > 360. )
        m_renShortAvgWindDirWindward -= 360.;
    /*
      Windward or Leeward leg?
      If wind and back mark are on the same plane: leeward;
      If wind and back mark are on different planes: windward.
    */
    if ( std::isnan( m_previousWpBearing ) )
        return;
    
    m_renAvgWindRightPlaneDir = m_renAvgWindDir + 90.;
    if ( m_renAvgWindRightPlaneDir > 360. )
        m_renAvgWindRightPlaneDir -= 360.;
    m_renAvgWindLeftPlaneDir = m_renAvgWindDir - 90.;
    if ( m_renAvgWindLeftPlaneDir < 0. )
        m_renAvgWindLeftPlaneDir += 360.;

    m_renShortAvgWindRightPlaneDir = m_renShortAvgWindDir + 90.;
    if ( m_renShortAvgWindRightPlaneDir > 360. )
        m_renShortAvgWindRightPlaneDir -= 360.;
    m_renShortAvgWindLeftPlaneDir = m_renShortAvgWindDir - 90.;
    if ( m_renShortAvgWindLeftPlaneDir < 0. )
        m_renShortAvgWindLeftPlaneDir += 360.;

    m_renWindwardLeg = true;
    double backbearingSignedOffsetRight = getSignedDegRange(
        m_renAvgWindRightPlaneDir, m_previousWpBearing);
    if ( backbearingSignedOffsetRight < 0.0) {
        m_renWindwardLeg = false;
    }  // then back mark is in sector 270 - 90 degrees relative to wind

    this->RenderGLAvgWindToWp( pcontext, vp );
    this->RenderGLLaylinesOnTargetWp( pcontext, vp );
    this->RenderGLAvgWindLadderRungs( pcontext, vp );
    this->RenderGLShortAvgWindLadderRungs( pcontext, vp );

}

bool DashboardInstrument_RaceMark::WindRenderingConditions()
{
    if ( std::isnan( m_previousWpDistance ) )
        return false;
    if ( std::isnan( m_previousWpBearing ) )
        return false;
    if ( std::isnan( m_renAvgWindDir ) )
        return false;
    return true;
}

void DashboardInstrument_RaceMark::RenderGLAvgWindToWp(
        wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    if ( !WindRenderingConditions() )
        return;

    GetCanvasPixLL(
        vp, &m_renTargetPoint,
        m_targetWp->m_lat, m_targetWp->m_lon );
    m_renTargetPoint_lat = m_targetWp->m_lat;
    m_renTargetPoint_lon = m_targetWp->m_lon;
    double avgWindLineEnd_lat;
    double avgWindLineEnd_lon;
    PositionBearingDistanceMercator_Plugin(
        m_targetWp->m_lat, m_targetWp->m_lon,
        ( m_renWindwardLeg ? m_renAvgWindDirWindward : m_renAvgWindDir ),
        m_previousWpDistance,
        &avgWindLineEnd_lat, &avgWindLineEnd_lon );

    GetCanvasPixLL(
        vp, &m_renAvgWindLineEndPoint,
        avgWindLineEnd_lat, avgWindLineEnd_lon );

    double shortAvgWindLineEnd_lat;
    double shortAvgWindLineEnd_lon;
    PositionBearingDistanceMercator_Plugin(
        m_targetWp->m_lat, m_targetWp->m_lon,
        ( m_renWindwardLeg ?
          m_renShortAvgWindDirWindward : m_renShortAvgWindDir ),
        m_previousWpDistance,
        &shortAvgWindLineEnd_lat, &shortAvgWindLineEnd_lon );

    GetCanvasPixLL(
        vp, &m_renShortAvgWindLineEndPoint,
        shortAvgWindLineEnd_lat, shortAvgWindLineEnd_lon );
    
    if ( !m_renDrawAvgWind )
        return;

    // Draw only "normal" average, short is just calculated for other methods
    glEnable(GL_LINE_STIPPLE); // discontinuing line, stipple
    glLineWidth( m_renAvgWindLineWidth );
    glColor4ub(0, 0, 0, 168); // black, somwwhat opaque
    glLineStipple(5, 0xAAAA);  /* long dash */
    // glLineStipple(5, 0x0101);  /*  dotted  */
    // glLineStipple(5, 0x00FF);  /*  dashed  */
    // glLineStipple(5, 0x1C47);  /*  dash/dot/dash */
    glBegin(GL_LINES);
    glVertex2d( m_renTargetPoint.x, m_renTargetPoint.y );
    glVertex2d( m_renAvgWindLineEndPoint.x, m_renAvgWindLineEndPoint.y );
    glEnd();
    glDisable(GL_LINE_STIPPLE); //Disabling the Line Type.

}

void DashboardInstrument_RaceMark::RenderGLLaylinesOnTargetWp(
        wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    if ( !WindRenderingConditions() )
        return;

    if ( m_renWindwardLeg ) {
        m_renLLStbdDir = m_renAvgWindDir + 135.;
        if ( m_renLLStbdDir > 360. )
            m_renLLStbdDir -= 360.;
        m_renShortLLStbdDir = m_renShortAvgWindDir + 135.;
        if ( m_renShortLLStbdDir > 360. )
            m_renShortLLStbdDir -= 360.;
        m_renLLPortDir = m_renAvgWindDir - 135.;
        if ( m_renLLPortDir < 0. )
            m_renLLPortDir += 360.0;
        m_renShortLLPortDir = m_renShortAvgWindDir - 135.;
        if ( m_renShortLLPortDir < 0. )
            m_renShortLLPortDir += 360.0;
    } // then boat is on windward leg
    else {
        m_renLLStbdDir = m_renAvgWindDir - 45.;
        if ( m_renLLStbdDir < 0. )
            m_renLLStbdDir += 360.0;
        m_renShortLLStbdDir = m_renShortAvgWindDir - 45.;
        if ( m_renShortLLStbdDir < 0. )
            m_renShortLLStbdDir += 360.0;
        m_renLLPortDir = m_renAvgWindDir + 45.;
        if ( m_renLLPortDir > 360. )
            m_renLLPortDir -= 360.;
        m_renShortLLPortDir = m_renShortAvgWindDir + 45.;
        if ( m_renShortLLPortDir > 360. )
            m_renShortLLPortDir -= 360.;
    } // else boat is on leeward leg

    double backBearingDeltaToLLStbdDir = getSignedDegRange(
        m_renLLStbdDir, m_previousWpBearing );
    double backBearingDeltaToLLPortDir = getSignedDegRange(
        m_renLLPortDir, m_previousWpBearing );
    if ( !( (backBearingDeltaToLLStbdDir > 0.0 ) &&
            (backBearingDeltaToLLPortDir < 0.0) ) )
        m_renReachingLeg = true;

    // make sure that ladder rungs will cross the laylines
    m_renLLlen = m_previousWpDistance / 0.7071; // cos(45)

    double renLLStbdEndPoint_lat;
    double renLLStbdEndPoint_lon;
    PositionBearingDistanceMercator_Plugin(
        m_targetWp->m_lat, m_targetWp->m_lon,
        m_renLLStbdDir, m_renLLlen,
        &renLLStbdEndPoint_lat, &renLLStbdEndPoint_lon );
    GetCanvasPixLL(
        vp, &m_renLLStbdEndPoint,
        renLLStbdEndPoint_lat, renLLStbdEndPoint_lon );

    double renShortLLStbdEndPoint_lat;
    double renShortLLStbdEndPoint_lon;
    PositionBearingDistanceMercator_Plugin(
        m_targetWp->m_lat, m_targetWp->m_lon,
        m_renShortLLStbdDir, m_renLLlen,
        &renShortLLStbdEndPoint_lat, &renShortLLStbdEndPoint_lon );
    GetCanvasPixLL(
        vp, &m_renShortLLStbdEndPoint,
        renShortLLStbdEndPoint_lat, renShortLLStbdEndPoint_lon );

    double renLLPortEndPoint_lat;
    double renLLPortEndPoint_lon;
    PositionBearingDistanceMercator_Plugin(
        m_targetWp->m_lat, m_targetWp->m_lon,
        m_renLLPortDir, m_renLLlen,
        &renLLPortEndPoint_lat, &renLLPortEndPoint_lon );
    GetCanvasPixLL(
        vp, &m_renLLPortEndPoint,
        renLLPortEndPoint_lat, renLLPortEndPoint_lon );

    double renShortLLPortEndPoint_lat;
    double renShortLLPortEndPoint_lon;
    PositionBearingDistanceMercator_Plugin(
        m_targetWp->m_lat, m_targetWp->m_lon,
        m_renShortLLPortDir, m_renLLlen,
        &renShortLLPortEndPoint_lat, &renShortLLPortEndPoint_lon );
    GetCanvasPixLL(
        vp, &m_renShortLLPortEndPoint,
        renShortLLPortEndPoint_lat, renShortLLPortEndPoint_lon );

    if ( !m_renDrawLaylines )
        return;

    if ( m_renReachingLeg )
        return;

    glColor4ub(0, 200, 0, 188); // green
    glLineWidth( m_renLaylineWidth );
    glBegin(GL_LINES);
    glVertex2d( m_renTargetPoint.x, m_renTargetPoint.y );
    glVertex2d( m_renLLStbdEndPoint.x, m_renLLStbdEndPoint.y );
    glEnd();

    glColor4ub(204, 41, 41, 138); // red
    glBegin(GL_LINES);
    glVertex2d( m_renTargetPoint.x, m_renTargetPoint.y );
    glVertex2d( m_renLLPortEndPoint.x, m_renLLPortEndPoint.y );
    glEnd();

}

void DashboardInstrument_RaceMark::RenderGLAvgWindLadderRungs(
        wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    if ( !WindRenderingConditions() )
        return;

    if  ( !m_renDrawRungs )
        return;

    if ( m_renReachingLeg )
        return;

    double thisPoint_lat = m_renTargetPoint_lat;
    double thisPoint_lon = m_renTargetPoint_lon;

    while ( 1 ) {
        // Calculate next point's position on the imaginary endless line
        PositionBearingDistanceMercator_Plugin(
            thisPoint_lat, thisPoint_lon,
            ( m_renWindwardLeg ? m_renAvgWindDirWindward : m_renAvgWindDir ),
            m_renRungStep,
            &thisPoint_lat, &thisPoint_lon );
        double distanceToStart;
        double brg;
        DistanceBearingMercator_Plugin(
            thisPoint_lat, thisPoint_lon,
            m_renTargetPoint_lat, m_renTargetPoint_lon,
            &brg, &distanceToStart );
        if ( distanceToStart > m_previousWpDistance ) {
            break;
        } // then now you know the reason why we use a break statement in a loop
        wxPoint thisPoint;
        GetCanvasPixLL(
            vp, &thisPoint,
            thisPoint_lat, thisPoint_lon );
        double rightLaylineX_lat;
        double rightLaylineX_lon;
        PositionBearingDistanceMercator_Plugin(
            thisPoint_lat, thisPoint_lon,
            m_renAvgWindRightPlaneDir, distanceToStart,
            &rightLaylineX_lat, &rightLaylineX_lon );
        wxPoint righLaylineXPoint;
        GetCanvasPixLL(
            vp, &righLaylineXPoint,
            rightLaylineX_lat, rightLaylineX_lon );
        double leftLaylineX_lat;
        double leftLaylineX_lon;
        PositionBearingDistanceMercator_Plugin(
            thisPoint_lat, thisPoint_lon,
            m_renAvgWindLeftPlaneDir, distanceToStart,
            &leftLaylineX_lat, &leftLaylineX_lon );
        wxPoint leftLaylineXPoint;
        GetCanvasPixLL(
            vp, &leftLaylineXPoint,
            leftLaylineX_lat, leftLaylineX_lon );

        glLineWidth( m_renRungLineWidth );
        glColor4ub(128, 128, 128, 158); // light gray, somewhat opaque

        glBegin(GL_LINES);
        glVertex2d( thisPoint.x, thisPoint.y );
        glVertex2d( leftLaylineXPoint.x, leftLaylineXPoint.y );
        glEnd();

        glBegin(GL_LINES);
        glVertex2d( thisPoint.x, thisPoint.y );
        glVertex2d( righLaylineXPoint.x, righLaylineXPoint.y );
        glEnd();

    } // while lengthy calculations do not prove that we're at back mark

}    

void DashboardInstrument_RaceMark::RenderGLShortAvgWindLadderRungs(
        wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    if ( !WindRenderingConditions() )
        return;

    if ( m_renReachingLeg )
        return;

    if  ( !m_renDrawShortAvgWindRungs )
        return;

    /* In contrary to the method name, we do not intend to redraw
       _all_ the ladder rungs, that would be too cumbersome, perhaps.
       Instead, we anchor one short average wind based ludder rung
       to the boat so that the strategist can see if they are on
       a header or on a lifter by comparing its angle to the nearby
       long term average wind ladder rungs.
    */

    // Define the triangle in which the boat shall be located
    wxRealPoint m_renTargetRealPoint( m_renTargetPoint );
    wxRealPoint m_renLLStbdEndRealPoint( m_renLLStbdEndPoint );
    wxRealPoint m_renLLPortEndRealPoint( m_renLLPortEndPoint );

    wxPoint boatPoint;
    GetCanvasPixLL( vp, &boatPoint, m_Lat, m_Lon );
    wxRealPoint boatPointReal( boatPoint );

    // Build  a line from the boat to the "left" and check for a crossing
    double lendLeft_lat;
    double lendLeft_lon;
    PositionBearingDistanceMercator_Plugin(
        m_Lat, m_Lon,
        m_renShortAvgWindLeftPlaneDir, m_renLLlen,
        &lendLeft_lat, &lendLeft_lon );
    wxPoint lendLeft;
    GetCanvasPixLL( vp, &lendLeft, lendLeft_lat, lendLeft_lon );
    wxRealPoint lendLeftReal( lendLeft );
    wxRealPoint crossLeftReal = GetLineIntersection(
        boatPointReal, lendLeftReal,
        m_renTargetRealPoint, m_renLLPortEndRealPoint );
    if ( ( crossLeftReal.x == -999. ) ||
         ( crossLeftReal.y == -999. ) ) {
        crossLeftReal = GetLineIntersection(
            boatPointReal, lendLeftReal,
            m_renLLPortEndRealPoint, m_renLLStbdEndRealPoint );
        if ( ( crossLeftReal.x == -999. ) ||
             ( crossLeftReal.y == -999. ) ) {
            crossLeftReal = GetLineIntersection(
                boatPointReal, lendLeftReal,
                m_renLLStbdEndRealPoint, m_renTargetRealPoint );
            if ( ( crossLeftReal.x == -999. ) ||
                 ( crossLeftReal.y == -999. ) ) {
                return;
            } // then not crossing any sides of the triangle
        } // then try #2 failed
    } // then try #1 failed
    m_renShortAvgWindLeftCrossPoint = crossLeftReal;

    // Build  a line from the boat to the "right" and check for a crossing
    double lendRight_lat;
    double lendRight_lon;
    PositionBearingDistanceMercator_Plugin(
        m_Lat, m_Lon,
        m_renShortAvgWindRightPlaneDir, m_renLLlen,
        &lendRight_lat, &lendRight_lon );
    wxPoint lendRight;
    GetCanvasPixLL( vp, &lendRight, lendRight_lat, lendRight_lon );
    wxRealPoint lendRightReal( lendRight );
    wxRealPoint crossRightReal = GetLineIntersection(
        boatPointReal, lendRightReal,
        m_renTargetRealPoint, m_renLLPortEndRealPoint );
    if ( ( crossRightReal.x == -999. ) ||
         ( crossRightReal.y == -999. ) ) {
        crossRightReal = GetLineIntersection(
            boatPointReal, lendRightReal,
            m_renLLPortEndRealPoint, m_renLLStbdEndRealPoint );
        if ( ( crossRightReal.x == -999. ) ||
             ( crossRightReal.y == -999. ) ) {
            crossRightReal = GetLineIntersection(
                boatPointReal, lendRightReal,
                m_renLLStbdEndRealPoint, m_renTargetRealPoint );
            if ( ( crossRightReal.x == -999. ) ||
                 ( crossRightReal.y == -999. ) ) {
                return;
            } // then not crossing any sides of the triangle
        } // then try #2 failed
    } // then try #1 failed
    m_renShortAvgWindRightCrossPoint = crossRightReal;

    /*
      Arriving here meeans tha the boat is inside the layline triangle, that
      of the "normal" average wind. Now, let's draw a single ladder rung,
      this time for the "short" integration average wind, passing through
      the boat but not going out of the "normal" layline triangle. Job is easy,
      since we have collected above all the necessary points for drawing.
    */
    
    glLineWidth( m_renShortAvgWindRungLineWidth );
    glColor4ub(166, 166, 166, 148); // lighter gray,  opaqueness

    glBegin(GL_LINES);
    glVertex2d( boatPoint.x, boatPoint.y );
    glVertex2d( m_renShortAvgWindLeftCrossPoint.x,
                m_renShortAvgWindLeftCrossPoint.y );
    glEnd();

    glBegin(GL_LINES);
    glVertex2d( boatPoint.x, boatPoint.y );
    glVertex2d( m_renShortAvgWindRightCrossPoint.x,
                m_renShortAvgWindRightCrossPoint.y );
    glEnd();
    
}
