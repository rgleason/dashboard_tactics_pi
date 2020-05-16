/******************************************************************************
 * $Id: DashboardWindowContainer.h, v1.0 v1.0 2019/11/30 VaderDarth Exp $
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

#ifndef _DASHBOARDWINDOWCONTAINER_H_
#define _DASHBOARDWINDOWCONTAINER_H_

#include "instrument.h"

class DashboardWindowContainer
{
public:
#ifdef _TACTICSPI_H_
    DashboardWindowContainer(DashboardWindow *dashboard_window, wxString name, wxString caption, wxString orientation,
                             wxArrayInt inst, wxArrayString instID ) {
        m_pDashboardWindow = dashboard_window; m_sName = name; m_sCaption = caption; m_sOrientation = orientation;
        m_aInstrumentList = inst; m_aInstrumentIDs = instID;
        m_bIsVisible = false; m_bIsDeleted = false; m_bIsDocked = false; }
#else
    DashboardWindowContainer(DashboardWindow *dashboard_window, wxString name, wxString caption, wxString orientation, wxArrayInt inst) {
        m_pDashboardWindow = dashboard_window; m_sName = name; m_sCaption = caption; m_sOrientation = orientation; m_aInstrumentList = inst; m_bIsVisible = false; m_bIsDeleted = false; m_bIsDocked = false; }
#endif // _TACTICSPI_H_
#ifdef _TACTICSPI_H_
    DashboardWindowContainer( DashboardWindowContainer *sourcecont ) {
            m_pDashboardWindow = sourcecont->m_pDashboardWindow;
            m_bIsVisible       = sourcecont->m_bIsVisible;
            m_bIsDeleted       = sourcecont->m_bIsDeleted;
            m_bIsDocked        = sourcecont->m_bIsDocked;
            m_bPersVisible     = sourcecont->m_bPersVisible;
            m_sName            = sourcecont->m_sName;
            m_sCaption         = sourcecont->m_sCaption;
            m_sOrientation     = sourcecont->m_sOrientation;
            m_aInstrumentList  = sourcecont->m_aInstrumentList;
            m_aInstrumentIDs   = sourcecont->m_aInstrumentIDs;
    }
#endif // _TACTICSPI_H_
 
    ~DashboardWindowContainer(){}

    DashboardWindow          *m_pDashboardWindow;
    bool                      m_bIsVisible;
    bool                      m_bIsDeleted;
    bool                      m_bPersVisible;  // Persists visibility, even when Dashboard tool is toggled off.
    bool                      m_bIsDocked;
    wxString                  m_sName;
    wxString                  m_sCaption;
    wxString                  m_sOrientation;
    wxArrayInt                m_aInstrumentList;
#ifdef _TACTICSPI_H_
    wxArrayString             m_aInstrumentIDs;
#endif // _TACTICSPI_H_
};

WX_DEFINE_ARRAY(DashboardWindowContainer *, wxArrayOfDashboard);

#endif // _DASHBOARDWINDOWCONTAINER_H_
