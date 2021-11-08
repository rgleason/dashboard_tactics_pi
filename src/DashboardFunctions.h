/***************************************************************************
 * $Id: DashboardFunctions.h, v1.0 2010/08/05 SethDart Exp $
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

#ifndef __DASHBOARDFUNCTIONS_H__
#define __DASHBOARDFUNCTIONS_H__

#include <wx/string.h>
#include <wx/font.h>

#include "ocpn_plugin.h"

int GetRandomNumber( int range_min, int range_max );
wxString GetUUID( void );
wxString MakeName( void );
void checkNMEATemperatureDataAndUnit(
    double& TemperatureValue, wxString& TemperatureUnitOfMeasurement );
void CopyPlugInWaypointWithoutHyperlinks(
    PlugIn_Waypoint *src, PlugIn_Waypoint* dst );
wxFontFamily GetFontFamily( wxString postfix );
wxFontStyle GetFontStyle( wxString postfix );
wxFontWeight GetFontWeight( wxString postfix );
wxDateTime parseRfc3359UTC( wxString* rfc3359UTCmsStr, bool& parseError, int verbosity = 0 );

#endif // __DASHBOARDFUNCTIONS_H__
