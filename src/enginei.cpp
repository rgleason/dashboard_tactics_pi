/******************************************************************************
* $Id: enginei.cpp, v1.0 2019/11/30 VaderDarth Exp $
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

#include <algorithm>
#include <functional>

#include "dashboard_pi.h"
#include "enginei.h"
#include "plugin_ids.h"

// --- the following is probably needed only for demonstration and testing! ---
extern int GetRandomNumber(int, int);

wxBEGIN_EVENT_TABLE (DashboardInstrument_EngineI, DashboardInstrument_Single)
   EVT_TIMER (myID_TICK_ENGINEI, DashboardInstrument_EngineI::OnThreadTimerTick)
   EVT_CLOSE (DashboardInstrument_EngineI::OnClose)
wxEND_EVENT_TABLE ()
//************************************************************************************************************************
// Numerical instrument for engine monitoring data
//************************************************************************************************************************

DashboardInstrument_EngineI::DashboardInstrument_EngineI(
                             DashboardWindow *pparent, wxWindowID id, sigPathLangVector *sigPaths, wxString format ) :
DashboardInstrument_Single(pparent, id, "---", 0LL, format )
{
    SetDrawSoloInPane(false);

    m_data = L"---";
    m_pparent = pparent;
    m_path = wxEmptyString;
    m_sigPathLangVector = sigPaths;
    m_pushHereUUID = wxEmptyString;
    m_threadRunning = false;
    // Start the instrument thread
    m_threadEngineITimer = new wxTimer( this, myID_TICK_ENGINEI );
    m_threadEngineITimer->Start(1000, wxTIMER_CONTINUOUS);
}
DashboardInstrument_EngineI::~DashboardInstrument_EngineI(void)
{
    this->m_threadEngineITimer->Stop();
    delete this->m_threadEngineITimer;
    if ( !m_pushHereUUID.IsEmpty() ) // if parent window itself is Delete()d
        this->m_pparent->unsubscribeFrom( m_pushHereUUID );
    return;
}
void DashboardInstrument_EngineI::OnClose( wxCloseEvent &event )
{
    if ( !m_pushHereUUID.IsEmpty() ) { // civilized parent window informs: Close()
        m_pparent->unsubscribeFrom( m_pushHereUUID );
        m_pushHereUUID = wxEmptyString;
    }
    event.Skip(); // Destroy() must be called
}

void DashboardInstrument_EngineI::SetData(
    unsigned long long st, double data, wxString unit, long long timestamp)
{
    return; // this derived class gets its data from the multiplexer through a callback PushData()
}

void DashboardInstrument_EngineI::PushData( // for demo/testing purposes in this simple instrument
    double data, wxString unit, long long timestamp)
{
    if( !std::isnan(data) && (data < 9999.9) ) {
        setTimestamp( timestamp );                               // Triggers also base class' watchdog
        m_data = wxString::Format(m_format, data) + L" " + unit; // FYI, m_data is the string base class will draw
    } // then valid datea 
}

void DashboardInstrument_EngineI::derivedTimeoutEvent()
{
    m_data = L"---"; // No data seems to come in (anymore)
}

void DashboardInstrument_EngineI::OnThreadTimerTick( wxTimerEvent &event )
{
    m_threadRunning = true;
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
            m_sigPathLangVector->begin(), m_sigPathLangVector->end(),
            [this](const sigPathLangTuple& e){return std::get<0>(e) == m_path;});
        if ( it != m_sigPathLangVector->end() ) {
            sigPathLangTuple sigPathWithLangFeatures = *it;
            // the window title is changed in the base class, see instrument.h
            m_title = std::get<1>(sigPathWithLangFeatures);
            // Subscribe to the signal path data with this object's method to call back
            m_pushHere = std::bind(&DashboardInstrument_EngineI::PushData,
                                   this, _1, _2, _3 );
            m_pushHereUUID = m_pparent->subscribeTo ( m_path, m_pushHere );
        } // then found user selection from the available signal paths for subsribtion
    } // then no subscription to a signal path
}
