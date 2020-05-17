/******************************************************************************
 * $Id: DashboardWindow.h, v1.0 v1.0 2019/11/30 VaderDarth Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Jean-Eudes Onfray
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
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

#ifndef _DASHBOARDWINDOW_H_
#define _DASHBOARDWINDOW_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/aui/aui.h>

#include "TacticsWindow.h"
#include "DashboardInstrumentContainer.h"
#include "DashboardWindowContainer.h"
#include "dashboard_pi.h"
#include "ocpn_plugin.h"
#include "nmea0183/SatInfo.h"

class DashboardWindow : public TacticsWindow
{
public:
    DashboardWindow(
        wxWindow *pparent, wxWindowID id, wxAuiManager *auimgr,
        dashboard_pi* plugin, int orient, DashboardWindowContainer* mycont,
        wxString commonName, SkData* pSkData );
    ~DashboardWindow();

    void SetColorScheme( PI_ColorScheme cs );
    void SetSizerOrientation( int orient );
    int GetSizerOrientation();
    void OnSize( wxSizeEvent& evt );
    void OnContextMenu( wxContextMenuEvent& evt );
    void OnContextMenuSelect( wxCommandEvent& evt );
    bool isInstrumentListEqual( const wxArrayInt& list );
    void SetInstrumentList( wxArrayInt list, wxArrayString listIDs );
    void SetMinSizes( void );
    void RebuildPane( wxArrayInt list, wxArrayString listIDs);
    void SendSentenceToAllInstruments(
        unsigned long long st, double value, wxString unit,
        long long timestamp=0LL );
    void SendSatInfoToAllInstruments( int cnt, int seq, SAT_INFO sats[4] );
    void SendUtcTimeToAllInstruments( wxDateTime value );
    void SendColorSchemeToAllJSInstruments( PI_ColorScheme cs );
    void ChangePaneOrientation( int orient, bool updateAUImgr );
    /*TODO: OnKeyPress pass event to main window or disable focus*/

    DashboardWindowContainer* m_Container;

protected:
    wxDECLARE_EVENT_TABLE();
    void OnClose(wxCloseEvent& evt);

private:
    wxAuiManager         *m_pauimgr;
    dashboard_pi         *m_plugin;

    wxBoxSizer           *itemBoxSizer;
    wxArrayOfInstrument  m_ArrayOfInstrument;
};

#endif // _DASHBOARDWINDOW_H_
