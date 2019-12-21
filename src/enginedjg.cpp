/******************************************************************************
* $Id: enginedjg.cpp, v1.0 2019/11/30 VaderDarth Exp $
*
* Project:  OpenCPN
* Purpose:  dahbooard_tactics_pi plug-in
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

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/version.h>

#include <algorithm>
#include <functional>

#include "dashboard_pi.h"
#include "enginedjg.h"
#include "plugin_ids.h"

// --- the following is probably needed only for demonstration and testing! ---
extern int GetRandomNumber(int, int);


wxBEGIN_EVENT_TABLE (DashboardInstrument_EngineDJG, InstruJS)
   EVT_TIMER (myID_TICK_ENGINEDJG, DashboardInstrument_EngineDJG::OnThreadTimerTick)
   EVT_CLOSE (DashboardInstrument_EngineDJG::OnClose)
wxEND_EVENT_TABLE ()
//************************************************************************************************************************
// Numerical instrument for engine monitoring data
//************************************************************************************************************************

DashboardInstrument_EngineDJG::DashboardInstrument_EngineDJG(
    TacticsWindow *pparent, wxWindowID id, sigPathLangVector *sigPaths, wxString format ) :
   InstruJS ( pparent, id )
{
    m_pparent = pparent;
    m_id = id;
    previousTimestamp = 0LL; // dashboard instru base class
    m_path = wxEmptyString;
    m_format = format;
    m_orient = wxVERTICAL;
    m_pSigPathLangVector = sigPaths;
    m_pushHereUUID = wxEmptyString;
    m_threadRunning = false;

    if ( !LoadConfig() )
        return;

    m_pThreadEngineDJGTimer = new wxTimer( this, myID_TICK_ENGINEDJG );
    m_pThreadEngineDJGTimer->Start(100, wxTIMER_CONTINUOUS);
}
DashboardInstrument_EngineDJG::~DashboardInstrument_EngineDJG(void)
{
    this->m_pThreadEngineDJGTimer->Stop();
    delete this->m_pThreadEngineDJGTimer;
    if ( !m_pushHereUUID.IsEmpty() ) // if parent window itself is going away
        this->m_pparent->unsubscribeFrom( m_pushHereUUID );
    return;
}
void DashboardInstrument_EngineDJG::OnClose( wxCloseEvent &event )
{
    this->m_pThreadEngineDJGTimer->Stop();
    this->stopScript(); // base class implements, we are first to be called
    if ( !m_pushHereUUID.IsEmpty() ) { // civilized parent window informs: Close()
        m_pparent->unsubscribeFrom( m_pushHereUUID );
        m_pushHereUUID = wxEmptyString;
    }
    event.Skip(); // Destroy() must be called
}

void DashboardInstrument_EngineDJG::SetData(
    unsigned long long st, double data, wxString unit, long long timestamp)
{
    return; // this derived class gets its data from the multiplexer through a callback PushData()
}

void DashboardInstrument_EngineDJG::PushData( // subscribed data is pushed here
    double data, wxString unit, long long timestamp)
{
    if( !std::isnan(data) && (data < 9999.9) ) {
        setTimestamp( timestamp );                               // Triggers also base class' watchdog
        m_data = wxString::Format(m_format, data) + L" " + unit; // FYI, m_data is the string base class will draw
    } // then valid datea 
}

void DashboardInstrument_EngineDJG::OnThreadTimerTick( wxTimerEvent &event )
{
    if ( !m_threadRunning ) {
        wxSize thisSize = wxControl::GetSize();
        wxSize thisFrameInitSize = GetSize( m_orient, thisSize );
        SetInitialSize ( thisFrameInitSize );
        wxSize webViewInitSize = thisFrameInitSize;
        webViewInitSize.x -= ENGINEDJG_BORDER;
        this->loadHTML( m_fullPathHTML, webViewInitSize );
        m_pThreadEngineDJGTimer->Stop();
        m_pThreadEngineDJGTimer->Start(1000, wxTIMER_CONTINUOUS);
        m_threadRunning = true;
    }

    if ( m_path.IsEmpty() ) {
        /*
          We will emulate in this event the right click event for a selection a signal path from a list,
          given by the hosting application in the constructor (see below how to use). We'll
          make a simple random simulator (not to implement any GUI features for now)
        */
        wxString sTestingOnly[3];
        sTestingOnly[0] = L"propulsion.port.revolutions";
        sTestingOnly[1] = L"propulsion.port.oilPressure";
        sTestingOnly[2] = L"propulsion.port.temperature";
        m_path = sTestingOnly[ GetRandomNumber(0,2) ]; // let Mme Fortuna to be the user, for testing!!!!!!!
        /*
          Get the titles, descriptions and user's language for his selection from the hosting application
        */
        sigPathLangVector::iterator it = std::find_if(
            m_pSigPathLangVector->begin(), m_pSigPathLangVector->end(),
            [this](const sigPathLangTuple& e){return std::get<0>(e) == m_path;});
        if ( it != m_pSigPathLangVector->end() ) {
            sigPathLangTuple sigPathWithLangFeatures = *it;
            // the window title is changed in the base class, see instrument.h
            m_title = std::get<1>(sigPathWithLangFeatures);
            // Subscribe to the signal path data with this object's method to call back
            m_pushHere = std::bind(&DashboardInstrument_EngineDJG::PushData,
                                   this, _1, _2, _3 );
            m_pushHereUUID = m_pparent->subscribeTo ( m_path, m_pushHere );
        } // then found user selection from the available signal paths for subsribtion
    } // then not subscribed to any path yet
}

wxSize DashboardInstrument_EngineDJG::GetSize( int orient, wxSize hint )
{
    int x,y;
    m_orient = orient;
    if( m_orient == wxHORIZONTAL ) {
        x = ENGINEDJG_MIN_WIDTH;
        y = wxMax( hint.y, ENGINEDJG_MIN_HEIGHT );
    }
    else {
        x = wxMax( hint.x, ENGINEDJG_MIN_WIDTH );
        y = ENGINEDJG_MIN_HEIGHT;
    }
    return wxSize( x, y );
}

bool DashboardInstrument_EngineDJG::LoadConfig()
{
    m_fullPathHTML = *GetpSharedDataLocation(); // provide by the plug-in API
    wxString s = wxFileName::GetPathSeparator();
    m_fullPathHTML += _T("plugins") + s + _T("dashboard_tactics_pi") + s + _T("data") + s + _T("enginedjg.html");
    return true;
}

