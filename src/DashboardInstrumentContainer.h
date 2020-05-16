/******************************************************************************
 * $Id: DashboardInstrumentContainer.h, v1.0 v1.0 2019/11/30 VaderDarth Exp $
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

#ifndef _DASHBOARDINSTRUMENTCONTAINER_H_
#define _DASHBOARDINTSRUMENTCONTAINER_H_

#include "instrument.h"

class DashboardInstrumentContainer
{
public:
    DashboardInstrumentContainer(int id, DashboardInstrument *instrument,
                                 unsigned long long capa, wxString ids = _T("") )
        {
            m_ID = id; m_pInstrument = instrument; m_cap_flag = capa; m_IDs = ids;
        };
    ~DashboardInstrumentContainer(){ delete m_pInstrument; };
    DashboardInstrument    *m_pInstrument;
    int                     m_ID;
    unsigned long long      m_cap_flag;
    wxString                m_IDs;
};

WX_DEFINE_ARRAY(DashboardInstrumentContainer *, wxArrayOfInstrument);

#endif // _DASHBOARDINSTRUMENTCONTAINER_H_
