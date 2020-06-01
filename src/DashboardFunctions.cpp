/***************************************************************************
 * $Id: DashboardFunctions.cpp, v1.0 2010/08/05 SethDart Exp $
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

using namespace std;

#include <cmath>
#include <random>

#include "DashboardFunctions.h"

#include "dashboard_pi_ext.h"


int GetRandomNumber( int range_min, int range_max )
{
    // C++2011 and greater, instead of rand() of orignal Dashboard - it gives non-uniform results w/ float
    std::random_device rd; // pseudo or real random device (non-cryptographic)
    std::mt19937 mt( rd() ); // Fast and cross-platform consistent; Matsumoto and Nishimura, 1998
    std::uniform_int_distribution<int> iRandomRange(range_min, range_max); // Park, Miller, and Stockmeyer, 1988, 1993
    return iRandomRange( mt );
}


// RFC4122 version 4 compliant random UUIDs generator. Note requires C+2011 and the above GetRandomNumber().
wxString GetUUID( void )
{
    wxString str;
    struct {
        int time_low;
        int time_mid;
        int time_hi_and_version;
        int clock_seq_hi_and_rsv;
        int clock_seq_low;
        int node_hi;
        int node_low;
    } uuid;

    uuid.time_low = GetRandomNumber(0, INT_MAX);
    uuid.time_mid = GetRandomNumber(0, USHRT_MAX);
    uuid.time_hi_and_version = GetRandomNumber(0, USHRT_MAX);
    uuid.clock_seq_hi_and_rsv = GetRandomNumber(0,UCHAR_MAX);
    uuid.clock_seq_low = GetRandomNumber(0, UCHAR_MAX);
    uuid.node_hi = GetRandomNumber(0, USHRT_MAX);
    uuid.node_low = GetRandomNumber(0, INT_MAX);

    /* Set the two most significant bits (bits 6 and 7) of the
     * clock_seq_hi_and_rsv to zero and one, respectively. */
    uuid.clock_seq_hi_and_rsv = (uuid.clock_seq_hi_and_rsv & 0x3F) | 0x80;

    /* Set the four most significant bits (bits 12 through 15) of the
     * time_hi_and_version field to 4 */
    uuid.time_hi_and_version = (uuid.time_hi_and_version & 0x0fff) | 0x4000;

    str.Printf(_T("%08x-%04x-%04x-%02x%02x-%04x%08x"),
               uuid.time_low,
               uuid.time_mid,
               uuid.time_hi_and_version,
               uuid.clock_seq_hi_and_rsv,
               uuid.clock_seq_low,
               uuid.node_hi,
               uuid.node_low);

    return str;
}

wxString MakeName()
{
    return _T("DASHT-") + GetUUID();
}

// implement rule for *TMP and *ATMP sentences according g_iDashTemperatureUnit selection (C=0, F=1)
void checkNMEATemperatureDataAndUnit(double &TemperatureValue, wxString &TemperatureUnitOfMeasurement)
{
    if ( (TemperatureUnitOfMeasurement == _T("C")) && (g_iDashTemperatureUnit == 0) )
        return;
    if ( (TemperatureUnitOfMeasurement == _T("F")) && (g_iDashTemperatureUnit == 1) )
        return;
    if ( TemperatureUnitOfMeasurement == _T("C") ) {
        TemperatureUnitOfMeasurement = _T("F");
        TemperatureValue = TemperatureValue * 1.8 + 32.0;
        return;
    } // then convert Celcius to Fahrenheit
    // otherwise, convert Fahrenheit to Celcius
    TemperatureUnitOfMeasurement = _T("C");
    TemperatureValue = (TemperatureValue - 32.0) / 1.8;
    return;
}
