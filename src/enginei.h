/******************************************************************************
* $Id: engine.h, v1.0 2019/11/30 VaderDarth Exp $
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

#ifndef __ENGINEI_H__
#define __ENGINEI_H__
using namespace std;

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#ifndef __DERIVEDTIMEOUT_OVERRIDE__
#define __DERIVEDTIMEOUT_OVERRIDE__
#endif // __DERIVEDTIMEOUT_OVERRIDE__
#include "instrument.h"
#include "tactics_pi.h"

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    DashboardInstrument_EngineI
//|
//| DESCRIPTION:
//|    This instrument provides numerical engine monitoring information
//+------------------------------------------------------------------------------

class DashboardInstrument_EngineI : public DashboardInstrument_Single
{
public:
    DashboardInstrument_EngineI(
        DashboardWindow *pparent, wxWindowID id, sigPathLangVector* sigPaths, wxString format = "%4.0f" );
    ~DashboardInstrument_EngineI(void);
    void SetData(unsigned long long, double, wxString, long long timestamp=0LL );
    void PushData(double, wxString, long long timestamp=0LL );
    void derivedTimeoutEvent(void) override;

protected:
    int                  m_soloInPane;
    DashboardWindow     *m_pparent;
    wxString             m_path;
    sigPathLangVector   *m_sigPathLangVector;
    wxTimer             *m_threadEngineITimer;
    bool                 m_threadRunning;
    callbackFunction     m_pushHere;
    wxString             m_pushHereUUID;

    wxDECLARE_EVENT_TABLE();

    void OnThreadTimerTick( wxTimerEvent& );
    void OnClose(wxCloseEvent& evt);

};

#endif // __ENGINEI_H__

