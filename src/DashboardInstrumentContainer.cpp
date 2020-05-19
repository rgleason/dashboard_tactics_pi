/***************************************************************************
 * $Id: DashboardInstrumentContainer.cpp, v1.0 2010/08/05 SethDart Exp $
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

class dashboard_pi;


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/listctrl.h>

#include "DashboardInstrumentContainer.h"

#include "tactics_pi.h"

// Global helper functions to work with the enumerated instrument types

// The enumerated list does not allow us build with inheritage but let's use
// this function when we need to make separation of enumerated list between
// the base class (Tactics) and the derived class (Dashboard)
bool IsTacticsInstrument( int id ) {
    if ( id > ID_DPB_PERF_FIRST && id < ID_DPB_PERF_LAST )
        return true;
    return false;
}

bool IsObsolete( int id ) {
    switch( id ) {
    case ID_DBP_D_AWA:
        /* Please see above, placeholders for eventual new dashboard_pi insteuments shall go
           before tactics_pi instruments: the eInstruments enumeration and this
           "obsolence" list, a placeholder between the two instrument lists must match. */
    case ID_DBP_R_AAAA:
    case ID_DBP_R_AAAB:
    case ID_DBP_R_AAAC:
    case ID_DBP_R_AAAD:
    case ID_DBP_R_AAAE:
    case ID_DBP_R_AAAF:
    case ID_DBP_R_AABA:
    case ID_DBP_R_AABB:
    case ID_DBP_R_AABC:
    case ID_DBP_R_AABD:
    case ID_DBP_R_AABE:
    case ID_DBP_R_AABF:
    case ID_DBP_R_EAAA:
    case ID_DBP_R_EAAB:
    case ID_DBP_R_EAAC:
    case ID_DBP_R_EAAD:
    case ID_DBP_R_EAAE:
    case ID_DBP_R_EAAF:
    case ID_DBP_R_EABA:
    case ID_DBP_R_EABB:
    case ID_DBP_R_EABC:
    case ID_DBP_R_EABD:
    case ID_DBP_R_EABE:
    case ID_DBP_R_EABF:
        return true;
    default:
        return false;
    }
}

wxString getInstrumentCaption( unsigned int id )
{
    switch( id ){
    case ID_DBP_I_POS:
        return _("Position");
    case ID_DBP_I_SOG:
        return _("SOG");
    case ID_DBP_D_SOG:
        return _("Speedometer");
    case ID_DBP_I_COG:
        return _("COG");
    case ID_DBP_M_COG:
        return _("Mag COG");
    case ID_DBP_D_COG:
        return _("GPS Compass");
    case ID_DBP_D_HDT:
        return _("True Compass");
    case ID_DBP_I_STW:
        return _("STW");
    case ID_DBP_I_HDT:
        return _("True HDG");
    case ID_DBP_I_HDM:
        return _("Mag HDG");
    case ID_DBP_D_AW:
    case ID_DBP_D_AWA:
        return _("App. Wind Angle & Speed");
    case ID_DBP_D_AWA_TWA:
        return _("App & True Wind Angle");
    case ID_DBP_I_AWS:
        return _("App. Wind Speed");
    case ID_DBP_D_AWS:
        return _("App. Wind Speed");
    case ID_DBP_D_TW:
        return _("True Wind Angle & Speed");
    case ID_DBP_I_DPT:
        return _("Depth");
    case ID_DBP_D_DPT:
        return _("Depth");
    case ID_DBP_D_MDA:
        return _("Barometric pressure");
    case ID_DBP_I_MDA:
        return _("Barometric pressure");
    case ID_DBP_I_TMP:
        return _("Water Temp.");
    case ID_DBP_I_ATMP:
        return _("Air Temp.");
    case ID_DBP_I_AWA:
        return _("App. Wind Angle");
    case ID_DBP_I_TWA:
        return _("True Wind Angle");
    case ID_DBP_I_TWD:
        return _("True Wind Direction");
    case ID_DBP_I_TWS:
        return _("True Wind Speed");
    case ID_DBP_D_TWD:
        return _("True Wind Dir. & Speed");
    case ID_DBP_I_VMG:
        return _("VMG");
    case ID_DBP_D_VMG:
        return _("VMG");
    case ID_DBP_I_RSA:
        return _("Rudder Angle");
    case ID_DBP_D_RSA:
        return _("Rudder Angle");
    case ID_DBP_I_SAT:
       return _("GPS in View");
    case ID_DBP_D_GPS:
        return _("GPS Status");
    case ID_DBP_I_PTR:
        return _("Cursor");
    case ID_DBP_I_GPSUTC:
        return _("GPS Clock");
    case ID_DBP_I_SUN:
        return _("Sunrise/Sunset");
    case ID_DBP_D_MON:
        return _("Moon phase");
    case ID_DBP_D_WDH:
        return _("Wind history");
    case ID_DBP_D_BPH:
        return  _("Barometric history");
    case ID_DBP_I_VLW1:
        return _("Trip Log");
    case ID_DBP_I_VLW2:
        return _("Sum Log");
    case ID_DBP_I_FOS:
        return _("From Ownship");
    case ID_DBP_I_PITCH:
        return _("Pitch");
    case ID_DBP_I_HEEL:
        return _("Heel");
    case ID_DBP_I_GPSLCL:
        return _( "Local GPS Clock" );
    case ID_DBP_I_CPULCL:
        return _( "Local CPU Clock" );
    case ID_DBP_I_SUNLCL:
        return _( "Local Sunrise/Sunset" );
	case  ID_DBP_I_LEEWAY:
		return _(L"\u2191Leeway");
    case ID_DBP_I_TWAMARK:
        return _(L"\u2191TWA to Waypoint");
	case ID_DBP_I_CURRDIR:
		return _(L"\u2191Current Direction");
	case ID_DBP_I_CURRSPD:
		return _(L"\u2191Current Speed");
	case ID_DBP_D_BRG:
		return _(L"\u2191Bearing Compass");
	case	ID_DBP_I_POLSPD:
		return _(L"\u2191Polar Speed");
	case	ID_DBP_I_POLVMG:
		return _(L"\u2191Actual ") + tactics_pi::get_sVMGSynonym();
	case	ID_DBP_I_POLTVMG:
		return _(L"\u2191Target ") + tactics_pi::get_sVMGSynonym();
	case	ID_DBP_I_POLTVMGANGLE:
		return _(L"\u2191Target ") +
            tactics_pi::get_sVMGSynonym() + _("-Angle");
	case	ID_DBP_I_POLCMG:
		return _(L"\u2191Actual ") + tactics_pi::get_sCMGSynonym();
	case	ID_DBP_I_POLTCMG:
		return _(L"\u2191Target ") + tactics_pi::get_sCMGSynonym();
	case	ID_DBP_I_POLTCMGANGLE:
		return _(L"\u2191Target ") +
            tactics_pi::get_sCMGSynonym() + _("-Angle");
	case ID_DBP_D_POLPERF:
		return _(L"\u2191Polar Performance");
	case ID_DBP_D_AVGWIND:
		return _(L"\u2191Average Wind Direction");
	case ID_DBP_D_POLCOMP:
		return _(L"\u2191Polar Compass");
    case ID_DBP_V_IFLX:
		return _(L"\u2191InfluxDB Out");
    case ID_DBP_V_INSK:
		return _(L"\u2191Signal K In");
    case ID_DBP_D_ENGDJG: 
		return _(L"\u2b24 DashT E-Dial");
    case ID_DBP_D_TSETUI: 
		return _(L"\u2b24 DashT Line Chart");
    }
    return _T("");
}

bool getListItemForInstrument( wxListItem &item, unsigned int id )
{
    wxString sCapt = getInstrumentCaption( id );
    if ( sCapt.IsEmpty() || sCapt.IsNull() || sCapt.Cmp(_T("")) == 0 )
        return false;
    item.SetData( id );
    item.SetText( sCapt );
    switch( id ){
    case ID_DBP_I_POS:
    case ID_DBP_I_SOG:
    case ID_DBP_I_COG:
    case ID_DBP_M_COG:
    case ID_DBP_I_STW:
    case ID_DBP_I_HDT:
    case ID_DBP_I_HDM:
    case ID_DBP_I_AWS:
    case ID_DBP_I_DPT:
    case ID_DBP_I_MDA:
    case ID_DBP_I_TMP:
    case ID_DBP_I_ATMP:
    case ID_DBP_I_TWA:
    case ID_DBP_I_TWD:
    case ID_DBP_I_TWS:
    case ID_DBP_I_AWA:
    case ID_DBP_I_VMG:
    case ID_DBP_I_RSA:
    case ID_DBP_I_SAT:
    case ID_DBP_I_PTR:
    case ID_DBP_I_GPSUTC:
    case ID_DBP_I_GPSLCL:
    case ID_DBP_I_CPULCL:
    case ID_DBP_I_SUN:
    case ID_DBP_I_SUNLCL:
    case ID_DBP_I_VLW1:
    case ID_DBP_I_VLW2:
    case ID_DBP_I_FOS:
    case ID_DBP_I_PITCH:
    case ID_DBP_I_HEEL:
	case ID_DBP_I_LEEWAY:
    case ID_DBP_I_TWAMARK:
	case ID_DBP_I_CURRDIR:
	case ID_DBP_I_CURRSPD:
	case ID_DBP_I_POLSPD:
	case ID_DBP_I_POLVMG:
	case ID_DBP_I_POLTVMG:
	case ID_DBP_I_POLTVMGANGLE:
	case ID_DBP_I_POLCMG:
	case ID_DBP_I_POLTCMG:
	case ID_DBP_I_POLTCMGANGLE:
    case ID_DBP_V_IFLX:
    case ID_DBP_V_INSK:
        item.SetImage( 0 );
        break;
    case ID_DBP_D_SOG:
    case ID_DBP_D_COG:
    case ID_DBP_D_AW:
    case ID_DBP_D_AWA:
    case ID_DBP_D_AWS:
    case ID_DBP_D_TW:
    case ID_DBP_D_AWA_TWA:
    case ID_DBP_D_TWD:
    case ID_DBP_D_DPT:
    case ID_DBP_D_MDA:
    case ID_DBP_D_VMG:
    case ID_DBP_D_RSA:
    case ID_DBP_D_GPS:
    case ID_DBP_D_HDT:
    case ID_DBP_D_MON:
    case ID_DBP_D_WDH:
    case ID_DBP_D_BPH:
	case ID_DBP_D_BRG:
	case ID_DBP_D_POLPERF:
	case ID_DBP_D_AVGWIND:
	case ID_DBP_D_POLCOMP:
    case ID_DBP_D_ENGDJG:
    case ID_DBP_D_TSETUI:
        item.SetImage( 1 );
        break;
    }
    return true;
}

