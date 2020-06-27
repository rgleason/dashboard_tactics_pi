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
}

void DashboardInstrument_RaceStart::RenderGLStartLine(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    if ( !(m_startStbdWp) || !(m_startPortWp) )
        return;
    return;
    wxPoint stbd;
    GetCanvasPixLL( vp, &stbd, m_startStbdWp->m_lat, m_startStbdWp->m_lon );
    wxPoint port;
    GetCanvasPixLL( vp, &stbd, m_startPortWp->m_lat, m_startPortWp->m_lon );
    glColor4ub(255, 128, 0, 168); //orange
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2d( stbd.x, stbd.y );
    glVertex2d( port.x, port.y );
    glEnd();
}

void DashboardInstrument_RaceStart::RenderGLWindBias(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
}

void DashboardInstrument_RaceStart::RenderGLLaylines(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
}

void DashboardInstrument_RaceStart::RenderGLGrid(
    wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
}
