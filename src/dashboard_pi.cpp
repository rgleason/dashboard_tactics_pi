/***************************************************************************
 * $Id: dashboard_pi.cpp, v1.0 2010/08/05 SethDart Exp $
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


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <cmath>

// xw 2.8
#include <wx/filename.h>

#include <wx/utils.h>

#include <typeinfo>
#include "dashboard_pi.h"

#ifdef _TACTICSPI_H_
#include <random>
using namespace std;
#endif // _TACTICSPI_H_


#include "icons.h"
#include "wx/jsonreader.h"
#include "wx/jsonwriter.h"

#include "ocpn_plugin.h"
#include <wx/glcanvas.h>


wxFont *g_pFontTitle;
wxFont *g_pFontData;
wxFont *g_pFontLabel;
wxFont *g_pFontSmall;
int g_iDashSpeedMax;
int g_iDashCOGDamp;
int g_iDashSpeedUnit;
int g_iDashSOGDamp;
int g_iDashDepthUnit;
int g_iDashDistanceUnit;
int g_iDashWindSpeedUnit;
#ifdef _TACTICSPI_H_
int g_iDashTemperatureUnit;
#endif // _TACTICSPI_H_
int g_iUTCOffset;
double g_dDashDBTOffset;

#ifdef _TACTICSPI_H_
#include "plugin_ids.h"
wxBEGIN_EVENT_TABLE (dashboard_pi, wxTimer)
EVT_TIMER (myID_THREAD_AVGWIND, dashboard_pi::OnAvgWindUpdTimer)
wxEND_EVENT_TABLE ()
#endif // _TACTICSPI_H_

#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double*)&lNaN)
#endif

#ifdef _TACTICSPI_H_
const char *dashboard_pi::s_common_name = _("Dashboard_Tactics");
#endif // _TACTICSPI_H_

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi( void *ppimgr )
{
    return (opencpn_plugin *) new dashboard_pi( ppimgr );
}

extern "C" DECL_EXP void destroy_pi( opencpn_plugin* p )
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    Dashboard PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------
// !!! WARNING !!!
// do not change the order, add new instruments at the end, before ID_DBP_LAST_ENTRY!
// otherwise, for users with an existing opencpn.ini file, their instruments are changing !
#ifndef _TACTICSPI_H_
enum {
#else
enum eInstruments {
#endif // _TACTICSPI_H_
    ID_DBP_I_POS, ID_DBP_I_SOG, ID_DBP_D_SOG, ID_DBP_I_COG, ID_DBP_D_COG, ID_DBP_I_STW,
    ID_DBP_I_HDT, ID_DBP_D_AW, ID_DBP_D_AWA, ID_DBP_I_AWS, ID_DBP_D_AWS, ID_DBP_D_TW,
    ID_DBP_I_DPT, ID_DBP_D_DPT, ID_DBP_I_TMP, ID_DBP_I_VMG, ID_DBP_D_VMG, ID_DBP_I_RSA,
    ID_DBP_D_RSA, ID_DBP_I_SAT, ID_DBP_D_GPS, ID_DBP_I_PTR, ID_DBP_I_GPSUTC, ID_DBP_I_SUN,
    ID_DBP_D_MON, ID_DBP_I_ATMP, ID_DBP_I_AWA, ID_DBP_I_TWA, ID_DBP_I_TWD, ID_DBP_I_TWS,
    ID_DBP_D_TWD, ID_DBP_I_HDM, ID_DBP_D_HDT, ID_DBP_D_WDH, ID_DBP_I_VLW1, ID_DBP_I_VLW2,
    ID_DBP_D_MDA,ID_DBP_I_MDA,ID_DBP_D_BPH, ID_DBP_I_FOS, ID_DBP_M_COG, ID_DBP_I_PITCH,
    ID_DBP_I_HEEL, ID_DBP_D_AWA_TWA, ID_DBP_I_GPSLCL, ID_DBP_I_CPULCL, ID_DBP_I_SUNLCL,
#ifdef _TACTICSPI_H_
    /* The below lines are allows the base dashboard code defining more instruments:
       If there will be new instruments (now after ID_DBP_I_SUNLCL), remove the same
       number of these "buffer" (between the Dashboard and Tactics)  enumeration values.
       This way, if there is opencpn.ini file with the Tactics instruments, they will not
       point to a wrong Tactics instrument!
    */
    ID_DBP_R_AAAA, ID_DBP_R_AAAB, ID_DBP_R_AAAC, ID_DBP_R_AAAD, ID_DBP_R_AAAE, ID_DBP_R_AAAF,
    ID_DBP_R_AABA, ID_DBP_R_AABB, ID_DBP_R_AABC, ID_DBP_R_AABD, ID_DBP_R_AABE, ID_DBP_R_AABF,
    /* These are the actual Tactics instrument enumerations, note _FIRST and _LAST markers;
       they are used to defined instrument belonging to "performance" category (i.e. Tactics).
       If you neednew perfomance instruments, put them between this andID_DPB_PERF_LAST. */
    ID_DPB_PERF_FIRST, ID_DBP_I_LEEWAY, ID_DBP_I_TWAMARK, ID_DBP_I_CURRDIR, ID_DBP_I_CURRSPD,
    ID_DBP_D_BRG, ID_DBP_I_POLSPD, ID_DBP_I_POLVMG, ID_DBP_I_POLTVMG, ID_DBP_I_POLTVMGANGLE,
    ID_DBP_I_POLCMG, ID_DBP_I_POLTCMG, ID_DBP_I_POLTCMGANGLE, ID_DBP_D_POLPERF, ID_DBP_D_AVGWIND,
    ID_DBP_D_POLCOMP, ID_DBP_V_IFLX, ID_DBP_V_INSK,
    /* More room between the sails and engines to allow sails to expand... */
    ID_DBP_R_EAAA, ID_DBP_R_EAAB, ID_DBP_R_EAAC, ID_DBP_R_EAAD, ID_DBP_R_EAAE, ID_DBP_R_EAAF,
    ID_DBP_R_EABA, ID_DBP_R_EABB, ID_DBP_R_EABC, ID_DBP_R_EABD, ID_DBP_R_EABE, ID_DBP_R_EABF,
    /* JavaScript/WebView based instruments, energy, engine, database and other utilities */
    ID_DBP_D_ENGDJG, ID_DBP_D_TSETUI,
    /* the section end marker, do not remove */
    ID_DPB_PERF_LAST,
#endif // _TACTICSPI_H_
    ID_DBP_LAST_ENTRY /* This has a reference in one of the routines; defining a "LAST_ENTRY" and
                         setting the reference to it, is one codeline less to change (and find)
                         when adding new instruments :-) */
};

#ifdef _TACTICSPI_H_
// This function can be used to separate the original dashboard instruments from the original dashboard instruments
bool IsTacticsInstrument( int id ) {
    if ( id > ID_DPB_PERF_FIRST && id < ID_DPB_PERF_LAST )
        return true;
    return false;
}
#endif // _TACTICSPI_H_

bool IsObsolete( int id ) {
    switch( id ) {
    case ID_DBP_D_AWA:
#ifdef _TACTICSPI_H_
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
#endif // _TACTICSPI_H_
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
#ifdef _TACTICSPI_H_
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
#endif // _TACTICSPI_H_
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
#ifdef _TACTICSPI_H_
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
#endif // _TACTICSPI_H_
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
#ifdef _TACTICSPI_H_
	case ID_DBP_D_BRG:
	case ID_DBP_D_POLPERF:
	case ID_DBP_D_AVGWIND:
	case ID_DBP_D_POLCOMP:
    case ID_DBP_D_ENGDJG:
    case ID_DBP_D_TSETUI:
#endif // _TACTICSPI_H_
        item.SetImage( 1 );
        break;
    }
    return true;
}

/*  These two function were taken from gpxdocument.cpp (route_pi) */
int GetRandomNumber(int range_min, int range_max)
{
#ifdef _TACTICSPI_H_
    // C++ 2011 and greater, instead of rand() which gives non-uniform results w/ float
    std::random_device rd; // pseudo or real random device (non-cryptographic)
    std::mt19937 mt( rd() ); // Fast and cross-platform consistent; Matsumoto and Nishimura, 1998
    std::uniform_int_distribution<int> iRandomRange(range_min, range_max); // Park, Miller, and Stockmeyer, 1988, 1993
    return iRandomRange( mt );
#else
    long u = (long)wxRound(((double)rand() / ((double)(RAND_MAX) + 1) * (range_max - range_min)) + range_min);
    return (int)u;
#endif // _TACTICSPI_H_
}


// RFC4122 version 4 compliant random UUIDs generator.
wxString GetUUID(void)
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
    return _T("DASH_") + GetUUID();
}

#ifdef _TACTICSPI_H_
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
#endif // _TACTICSPI_H_
//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

dashboard_pi::dashboard_pi( void *ppimgr ) :
#ifdef _TACTICSPI_H_
    tactics_pi(), wxTimer( this ), opencpn_plugin_112( ppimgr )
#else
     wxTimer( this ), opencpn_plugin_16( ppimgr )
#endif // _TACTICSPI_H_
{
#ifdef _TACTICSPI_H_
    m_nofStreamOut = 0;
    std::unique_lock<std::mutex> init_m_mtxNofStreamOut( m_mtxNofStreamOut, std::defer_lock );
    m_echoStreamerShow = wxEmptyString;
    m_nofStreamInSk = 0;
    std::unique_lock<std::mutex> init_m_mtxNofStreamInSk( m_mtxNofStreamInSk, std::defer_lock );
    m_echoStreamerInSkShow = wxEmptyString;

    m_bToggledStateVisible = false;
    m_iPlugInRequirements = 0;
    m_pluginFrame = NULL;
#endif // _TACTICSPI_H_
    m_NMEA0183 = new NMEA0183();
    mPriPosition = 99;
    mPriCOGSOG = 99;
    mPriHeadingT = 99; // True heading
    mPriHeadingM = 99; // Magnetic heading
    mPriVar = 99;
    mPriDateTime = 99;
    mPriAWA = 99; // Relative wind
    mPriTWA = 99; // True wind
    mPriDepth = 99;
    mVar = NAN;
    mSatsInView = 0.0;
    mHdm = 0.0;
    mUTCDateTime.Set( (time_t) -1 );
    m_config_version = -1;
    mHDx_Watchdog = 2;
    mHDT_Watchdog = 2;
    mGPS_Watchdog = 2;
    mVar_Watchdog = 2;
#ifdef _TACTICSPI_H_
    mStW_Watchdog = 2;
    mSiK_Watchdog = 0;
    mSiK_DPT_environmentDepthBelowKeel = false;
    mSiK_navigationGnssMethodQuality = 0;
    APPLYSAVEWININIT;
#endif // _TACTICSPI_H_

    // Create the PlugIn icons
    initialize_images();
}

dashboard_pi::~dashboard_pi( void )
{
    delete m_NMEA0183;
    delete _img_dashboard_tactics_pi;
    delete _img_dashboard_tactics;
    delete _img_dial;
    delete _img_instrument;
    delete _img_minus;
    delete _img_plus;
}

int dashboard_pi::Init( void )
{
#ifdef _TACTICSPI_H_
    AddLocaleCatalog( _T("opencpn-dashboard_tactics_pi") );
#else
    AddLocaleCatalog( _T("opencpn-dashboard_pi") );
#endif // _TACTICSPI_H_

    g_pFontTitle = new wxFont( 10, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL );
    g_pFontData = new wxFont( 14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
    g_pFontLabel = new wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
    g_pFontSmall = new wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );

    m_pauimgr = GetFrameAuiManager();
#ifdef _TACTICSPI_H_
    m_pluginFrame = m_pauimgr->GetManagedWindow();
    m_pauimgr->Connect( wxEVT_AUI_RENDER, wxAuiManagerEventHandler( dashboard_pi::OnAuiRender ),
                        NULL, this );
#else
    m_pauimgr->Connect( wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler( dashboard_pi::OnPaneClose ),
                        NULL, this );
#endif //  _TACTICSPI_H_

    //    Get a pointer to the opencpn configuration object
    m_pconfig = GetOCPNConfigObject();

    //    And load the configuration items

#ifdef _TACTICSPI_H_
    m_pconfig->SetPath( _T("/PlugIns/Dashboard") );
    int what_tactics_pi_wants = this->TacticsInit( this, m_pconfig );
    // Tick for average wind calculations is taken care in Tactics class, we host the timers  
    m_avgWindUpdTimer = new wxTimer ( this, myID_THREAD_AVGWIND );
    m_avgWindUpdTimer->Start(1000, wxTIMER_CONTINUOUS);
#endif //  _TACTICSPI_H_

    LoadConfig();

#ifdef OCPN_USE_SVG
    m_toolbar_item_id = InsertPlugInToolSVG(
        this->GetCommonName(),
#ifdef _TACTICSPI_H_
        _svg_dashboard_tactics, _svg_dashboard_tactics_rollover, _svg_dashboard_tactics_toggled,
#else
        _svg_dashboard, _svg_dashboard_rollover, _svg_dashboard_toggled,
#endif // _TACTICSPI_H_
        wxITEM_CHECK, this->GetShortDescription(), _T( "" ), NULL, DASHBOARD_TOOL_POSITION, 0, this);
#else
    // Use memory allocated PNG-images (icons.cpp)
    m_toolbar_item_id = InsertPlugInTool
        (_T(""),
#ifdef _TACTICSPI_H_
         _img_dashboard_tactics_pi, _img_dashboard_tactics_pi,
#else
         _img_dashboard_pi, _img_dashboard_pi,
#endif // _TACTICSPI_H_
         wxITEM_CHECK, this->GetCommonName(), _T(""), NULL,
         DASHBOARD_TOOL_POSITION, 0, this);
#endif // OCPN_USE_SVG

#ifndef _TACTICSPI_H_
    /* porting note: I reckon that this is obsolete in ov50, but since the code is there
       let's keep it in the non-Tactics-version compilation. */
    if(GetActiveStyleName().Lower() != _T("traditional")){
        wxString normalIcon = _T("");
        wxString toggledIcon = _T("");
        wxString rolloverIcon = _T("");
        m_toolbar_item_id = InsertPlugInToolSVG(
            _T(""), normalIcon, rolloverIcon, toggledIcon, wxITEM_CHECK,
            _("Dashboard"), _T(""), NULL,DASHBOARD_TOOL_POSITION, 0, this);
    }
#endif // _TACTICSPI_H_

#ifdef _TACTICSPI_H_
    bool init = true;
    ApplyConfig( init );
#else
    ApplyConfig();
#endif // _TACTICSPI_H_

    //  If we loaded a version 1 config setup, convert now to version 2
    if(m_config_version == 1) {
        SaveConfig();
    }

    Start( 1000, wxTIMER_CONTINUOUS );

#ifdef _TACTICSPI_H_
    m_iPlugInRequirements = // dashboard_pi requirements
        WANTS_CURSOR_LATLON | WANTS_TOOLBAR_CALLBACK | INSTALLS_TOOLBAR_TOOL |
        WANTS_PREFERENCES   | WANTS_CONFIG           | WANTS_NMEA_SENTENCES  |
        WANTS_NMEA_EVENTS   | USES_AUI_MANAGER       | WANTS_PLUGIN_MESSAGING;
    m_iPlugInRequirements = what_tactics_pi_wants | m_iPlugInRequirements;
    return m_iPlugInRequirements;
#else
    return (
        WANTS_CURSOR_LATLON |
        WANTS_TOOLBAR_CALLBACK |
        INSTALLS_TOOLBAR_TOOL |
        WANTS_PREFERENCES |
        WANTS_CONFIG |
        WANTS_NMEA_SENTENCES |
        WANTS_NMEA_EVENTS |
        USES_AUI_MANAGER |
        WANTS_PLUGIN_MESSAGING );
#endif //  _TACTICSPI_H_
}

bool dashboard_pi::DeInit( void )
{
    SaveConfig();
    if( IsRunning() ) // Timer started?
        Stop(); // Stop timer

    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindow *dashboard_window = m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow;
        if( dashboard_window ) {
            m_pauimgr->DetachPane( dashboard_window );
            dashboard_window->Close();
            dashboard_window->Destroy();
            m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow = NULL;
        }
    }

    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindowContainer *pdwc = m_ArrayOfDashboardWindow.Item( i );
        delete pdwc;
    }

    delete g_pFontTitle;
    delete g_pFontData;
    delete g_pFontLabel;
    delete g_pFontSmall;

#ifdef _TACTICSPI_H_
    this->m_avgWindUpdTimer->Stop();
    delete this->m_avgWindUpdTimer;
    return this->TacticsDeInit();
#endif //  _TACTICSPI_H_

    return true;
}

void dashboard_pi::Notify()
{

    SendUtcTimeToAllInstruments( mUTCDateTime );
    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindow *dashboard_window = m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow;
        if( dashboard_window ) dashboard_window->Refresh();
    }
    //  Manage the watchdogs
    mHDx_Watchdog--;
    if( mHDx_Watchdog <= 0 ) {
        mHdm = NAN;
        SendSentenceToAllInstruments( OCPN_DBP_STC_HDM, mHdm, _T("\u00B0") );
        mHDx_Watchdog = gps_watchdog_timeout_ticks;
    }

    mHDT_Watchdog--;
    if( mHDT_Watchdog <= 0 ) {
        SendSentenceToAllInstruments( OCPN_DBP_STC_HDT, NAN, _T("\u00B0T") );
        mHDT_Watchdog = gps_watchdog_timeout_ticks;
    }

    mVar_Watchdog--;
    if( mVar_Watchdog <= 0 ) {
        mVar = NAN;
        mPriVar = 99;
        SendSentenceToAllInstruments( OCPN_DBP_STC_HMV, NAN, _T("\u00B0T") );
        mVar_Watchdog = gps_watchdog_timeout_ticks;
    }

#ifdef _TACTICSPI_H_
    mStW_Watchdog--;
    if( mStW_Watchdog <= 0 ) {
        SendSentenceToAllInstruments( OCPN_DBP_STC_STW, NAN, "" );
        mStW_Watchdog = gps_watchdog_timeout_ticks;
    }
    if ( mSiK_Watchdog > 0)
        mSiK_Watchdog--;
#endif // _TACTICSPI_H_

    mGPS_Watchdog--;
    if( mGPS_Watchdog <= 0 ) {
        SAT_INFO sats[4];
        for(int i=0 ; i < 4 ; i++) {
            sats[i].SatNumber = 0;
            sats[i].SignalToNoiseRatio = 0;
        }
        SendSatInfoToAllInstruments( 0, 1, sats );
        SendSatInfoToAllInstruments( 0, 2, sats );
        SendSatInfoToAllInstruments( 0, 3, sats );

        mSatsInView = 0;
        SendSentenceToAllInstruments( OCPN_DBP_STC_SAT, 0, _T("") );
        mGPS_Watchdog = gps_watchdog_timeout_ticks;
    }
#ifdef _TACTICSPI_H_
    this->TacticsNotify();

    if ( APPLYSAVEWINREQUESTED ) {
        ApplyConfig();
        SaveConfig();
    }
    APPLYSAVEWINSERVED;
#endif //  _TACTICSPI_H_
    return;
}

#ifdef _TACTICSPI_H_
void dashboard_pi::OnAvgWindUpdTimer(wxTimerEvent &event)
{
    this->OnAvgWindUpdTimer_Tactics();
}

void dashboard_pi::OnAuiRender( wxAuiManagerEvent &event )
{
    event.Skip();
    if ( APPLYSAVEWINRUNNING )
        return;
    DashboardWindow *dashboard_window = NULL;
    wxAuiPaneInfo pane;
    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        dashboard_window =
            m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow;
        if( dashboard_window ) {
            pane = m_pauimgr->GetPane( dashboard_window );
            if ( pane.IsOk() ) {
                if ( pane.IsDocked() ) {
                    if ( !m_ArrayOfDashboardWindow.Item( i )->m_bIsDocked ) {
                        SetApplySaveWinRequest();
                        return;
                    } // then workaround missing Aui on-docking event
                } // then pane is docked
                else {
                    if ( m_ArrayOfDashboardWindow.Item( i )->m_bIsDocked ) {
                        SetApplySaveWinRequest();
                        return;
                    } // then workaround for missing AUI un-dock pane event
                } // else pane is floating
            } // then valid window pane of the dashboard window
        } // then valid dashboard window in the container
    } // for number of dashboard windows in the container
}
#endif //  _TACTICSPI_H_

int dashboard_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int dashboard_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int dashboard_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int dashboard_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxBitmap *dashboard_pi::GetPlugInBitmap()
{
#ifdef _TACTICSPI_H_
    return new wxBitmap(_img_dashboard_tactics_pi->ConvertToImage().Copy());
#else
    return new wxBitmap(_img_dashboard_pi->ConvertToImage().Copy());
#endif // _TACTICSPI_H_
}

#ifdef _TACTICSPI_H_
wxString dashboard_pi::GetNameVersion()
{
    char name_version[32];
    sprintf( name_version, "v%d.%d.%d",
             PLUGIN_VERSION_MAJOR, PLUGIN_VERSION_MINOR, PLUGIN_VERSION_PATCH );
    wxString retstr(name_version);
    return retstr;
}
wxString dashboard_pi::GetCommonNameVersion()
{
    wxString retstr ( GetCommonName() + " " + GetNameVersion() );
    return retstr;
}
#endif // _TACTICSPI_H_

wxString dashboard_pi::GetCommonName()
{
#ifdef _TACTICSPI_H_
    wxString retstr( s_common_name );
    return retstr;
#else
    return _("Dashboard");
#endif // _TACTICSPI_H_
}


wxString dashboard_pi::GetShortDescription()
{
#ifdef _TACTICSPI_H_
    return _("Dashboard and Tactics");
#else
    return _("Dashboard");
#endif // _TACTICSPI_H_
    
}

wxString dashboard_pi::GetLongDescription()
{
#ifdef _TACTICSPI_H_
    return _("Dashboard PlugIn with Tactics for OpenCPN\n\
Provides navigation instruments enhanced with performance functions and alternative input/output functions.");
#else
    return _("Dashboard PlugIn for OpenCPN\n\
Provides navigation instrument display from NMEA source.");
#endif // _TACTICSPI_H_

}

#ifdef _TACTICSPI_H_
wxString dashboard_pi::GetStandardPath()
{
    wxString s = wxFileName::GetPathSeparator();
    wxString stdPath  = *GetpPrivateApplicationDataLocation();

    stdPath += _T("plugins");
    if (!wxDirExists(stdPath))
      wxMkdir(stdPath);

    stdPath += s + _T("dashoard_tactics_pi");

    if (!wxDirExists(stdPath))
      wxMkdir(stdPath);

    stdPath += s;
    return stdPath;
}
#endif // _TACTICSPI_H_


#ifdef _TACTICSPI_H_
/* Porting note: this section is the cornerstone of the Tactics porting effort.
   The below private method is about the original dashboard's simple and real
   method: it merely passes to dashboard instruments the NMEA sentences. Now,
   the NMEA sentences are first fed to Tactic's different engines in
   the original method and optional, Tactics specific NMEA sentences are
   passed to instruments here as in the original Dashboard_pi. */
void dashboard_pi::pSendSentenceToAllInstruments(
    unsigned long long st, double value, wxString unit, long long timestamp  )
{
    if ( APPLYSAVEWINRUNNING )
        return;

    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindow *dashboard_window =
            m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow;
        if( dashboard_window )
            dashboard_window->SendSentenceToAllInstruments(
                st, value, unit, timestamp );
    }
}
/* Porting note: with Tactics new, virtual NMEA sentences are introduced, like
   the true wind calculations. Likewise, the bearing to the TacticsWP
   (if it exists) to performance instruments as a specific NMEA sentence having
   a special unit. Current speed and leeway are also virtual, calculated
   NMEA sentences. We can publish to the outside world target angle information
   and similar to be displayed to the helmsman on a performance instruments.
   This is not enough: it is also possible to make corrections to the
   NMEA sentences coming from the boat's instruments (you need to read
   the excellent documentation of Tactics plugin for the theory).
   For these calculations, Tactics class and its helpers are using the real
   NMEA sentences. The logic of these operations is strict and defined below.
   It is not recommend to alter or experiment with this logic without a good
   understanding of the Tactics class' internal variables and how they get
   their data. A good debugging tool is a must to visualize the actual value
   and unit versus the calculated value(s) and unit(s). */
void dashboard_pi::SendSentenceToAllInstruments(
    unsigned long long st, double value, wxString unit, long long timestamp )
{
    if ( this->SendSentenceToAllInstruments_LaunchTrueWindCalculations(
             st, value ) ) {
        // we have a valid AWS sentence here, it may require heel correction
        double distvalue = value;
        wxString distunit = unit;
        if ( this->SendSentenceToAllInstruments_PerformanceCorrections (
                 st, distvalue, distunit ) ) {
            this->SetCalcVariables(st, distvalue, distunit);
            pSendSentenceToAllInstruments( st, distvalue, distunit, timestamp );
        } // then send with corrections
        else {
            this->SetCalcVariables(st, value, unit);
            pSendSentenceToAllInstruments( st, value, unit, timestamp );
        } // else send the sentence as it is
        // AWS corrected or not, it is now sent, move to TW calculations
        unsigned long long st_twa, st_tws, st_tws2, st_twd;
        double value_twa, value_tws, value_twd;
        wxString unit_twa, unit_tws, unit_twd;
        long long calctimestamp;
        if (this->SendSentenceToAllInstruments_GetCalculatedTrueWind (
                st, value, unit,
                st_twa, value_twa, unit_twa,
                st_tws, st_tws2, value_tws, unit_tws,
                st_twd, value_twd, unit_twd,
                calctimestamp)) {
            pSendSentenceToAllInstruments( st_twa, value_twa, unit_twa, calctimestamp );
            pSendSentenceToAllInstruments( st_tws, value_tws, unit_tws, calctimestamp );
            pSendSentenceToAllInstruments( st_tws2, value_tws, unit_tws, calctimestamp );
            pSendSentenceToAllInstruments( st_twd, value_twd, unit_twd, calctimestamp );
            this->SetNMEASentence_Arm_TWD_Watchdog();
            this->SetNMEASentence_Arm_TWS_Watchdog();
        } // then calculated wind values required and need to be distributed
    } // then Tactics true wind calculations
    else {
        // we have sentence which may or may not require correction
        double distvalue = value;
        wxString distunit = unit;
        bool perfCorrections = false;
        if ( this->SendSentenceToAllInstruments_PerformanceCorrections (
                 st, distvalue, distunit ) ) {
            perfCorrections = true;
            this->SetCalcVariables(st, distvalue, distunit);
            pSendSentenceToAllInstruments( st, distvalue, distunit, timestamp );
        } // then send with corrections
        else {
            this->SetCalcVariables(st, value, unit);
            pSendSentenceToAllInstruments( st, value, unit, timestamp );
        } // else send the sentence as it is
        // Leeway
        unsigned long long st_leeway;
        double value_leeway;
        wxString unit_leeway;
        long long calctimestamp;
        if (this->SendSentenceToAllInstruments_GetCalculatedLeeway (
                st_leeway, value_leeway, unit_leeway, calctimestamp)) {
            pSendSentenceToAllInstruments( st_leeway, value_leeway,
                                           unit_leeway, calctimestamp );
        } // then calculated leeway required, is avalaible can be be distributed
        // Current
        unsigned long long st_currdir, st_currspd;
        double value_currdir, value_currspd;
        wxString unit_currdir, unit_currspd;
        if (this->SendSentenceToAllInstruments_GetCalculatedCurrent (
                st, (perfCorrections ? distvalue : value), (perfCorrections ? distunit : unit),
                st_currdir, value_currdir, unit_currdir,
                st_currspd, value_currspd, unit_currspd, calctimestamp)) {
            pSendSentenceToAllInstruments(
                st_currdir, value_currdir, unit_currdir, calctimestamp );
            pSendSentenceToAllInstruments(
                st_currspd, value_currspd, unit_currspd, calctimestamp );
        } // then calculated current required and need to be distributed
    } // else no true wind calculations
    // Take this opportunity to keep the Tactics performance enginge ticking for rendering
    this->CalculateLaylineDegreeRange();
    this->CalculatePerformanceData();
}

void dashboard_pi::SendDataToAllPathSubscribers (
    wxString path, double value, wxString unit, long long timestamp )
{
    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindow *dashboard_window =
            m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow;
        if( dashboard_window )
            dashboard_window->SendDataToAllPathSubscribers(
                path, value, unit, timestamp);
    }
}

#else

void dashboard_pi::SendSentenceToAllInstruments(
    int st, double value, wxString unit )
{
    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindow *dashboard_window =
            m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow;
        if( dashboard_window )
            dashboard_window->SendSentenceToAllInstruments(
                st, value, unit );
    }
}
#endif // _TACTICSPI_H_

void dashboard_pi::SendUtcTimeToAllInstruments( wxDateTime value )
{
    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindow *dashboard_window = m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow;
        if( dashboard_window ) dashboard_window->SendUtcTimeToAllInstruments( value );
    }
}

void dashboard_pi::SendSatInfoToAllInstruments( int cnt, int seq, SAT_INFO sats[4] )
{
    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindow *dashboard_window = m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow;
        if( dashboard_window ) dashboard_window->SendSatInfoToAllInstruments( cnt, seq, sats );
    }
}

#ifdef _TACTICSPI_H_
void dashboard_pi::SetNMEASentence(wxString &sentence)
{
    this->SetNMEASentence (sentence,
                           (wxString *)NULL, (wxString *)NULL,  (wxString *)NULL, (wxString *)NULL,
                           0, (wxString *)NULL, NAN, (wxString *)NULL, 0LL, (wxString *)NULL );
}

void dashboard_pi::SetNMEASentence( // NMEA0183-sentence either from O main, or from Signal K input
    wxString &sentence, wxString *type, wxString *sentenceId, wxString *talker, wxString *src,
    int pgn, wxString *path, double value, wxString *valStr, long long timestamp, wxString *key)
#else
void dashboard_pi::SetNMEASentence(wxString &sentence)
#endif // _TACTICSPI_H_
	
{

    
#ifdef _TACTICSPI_H_
    bool SignalK = false;
    // Select datasource: either O's NMEA event distribution or Signal K input stream
    if ( (type != NULL) && (sentenceId != NULL) && (talker != NULL) &&
         (src != NULL) && (path != NULL) && (!std::isnan(value)) ) {
        SignalK = true;
        mSiK_Watchdog = gps_watchdog_timeout_ticks;
    } // then Signal K input stream provided data
    else {
        if ( mSiK_Watchdog > 0 ) {
            (*m_NMEA0183) << sentence;  //  peek for exceptions (not sent as delta by Signal K)
            if ( m_NMEA0183->PreParse() ) { // list of NMEA-0183 sentences Signal K updates does not have
                if ( !(m_NMEA0183->LastSentenceIDReceived == _T("XDR")) &&
                     !(m_NMEA0183->LastSentenceIDReceived == _T("GSV")) &&
                     !(m_NMEA0183->LastSentenceIDReceived == _T("MTA")) &&
                     !(m_NMEA0183->LastSentenceIDReceived == _T("MWD")) &&
                     !(m_NMEA0183->LastSentenceIDReceived == _T("VWT")) &&
                     !(m_NMEA0183->LastSentenceIDReceived == _T("ZDA")) )
                    return; // no reason to interleave NMEA-0183 coming from OpenCPN with Signal K
            }
            else
                return; // failure in NMEA sentence
        } // else Signal K is active; this is an interelaving NMEA-0183 coming via OpenCPN
        else {
            (*m_NMEA0183) << sentence;
            if( !m_NMEA0183->PreParse() )
                return; // failure in NMEA sentence
        }  // else no Signal K, this is a normal cycle with NMEA-0183 coming from OpenCPN 
    } // else this is NMEA-0183 coming via OpenCPN

    if ( !SignalK ) {
#else
        (*m_NMEA0183) << sentence;
        if ( m_NMEA0183->PreParse() ) {
#endif // _TACTICSPI_H_

        if( m_NMEA0183->LastSentenceIDReceived == _T("DBT") ) {
            if( m_NMEA0183->Parse() ) {
                if( mPriDepth >= 2 ) {
                    mPriDepth = 2;

                    /*
                      double m_NMEA0183->Dbt.DepthFeet;
                      double m_NMEA0183->Dbt.DepthMeters;
                      double m_NMEA0183->Dbt.DepthFathoms;
                    */
                    double depth = 999.;
                    if( m_NMEA0183->Dbt.DepthMeters != 999. ) depth = m_NMEA0183->Dbt.DepthMeters;
                    else if( m_NMEA0183->Dbt.DepthFeet != 999. ) depth = m_NMEA0183->Dbt.DepthFeet
                                                                    * 0.3048;
                    else if( m_NMEA0183->Dbt.DepthFathoms != 999. ) depth =
                                                                       m_NMEA0183->Dbt.DepthFathoms * 1.82880;
                    depth += g_dDashDBTOffset;
                    SendSentenceToAllInstruments( OCPN_DBP_STC_DPT, toUsrDistance_Plugin( depth / 1852.0, g_iDashDepthUnit ), getUsrDistanceUnit_Plugin( g_iDashDepthUnit ) );
                }
            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("DPT") ) {
            if( m_NMEA0183->Parse() ) {
                if( mPriDepth >= 1 ) {
                    mPriDepth = 1;

                    /*
                      double m_NMEA0183->Dpt.DepthMeters
                      double m_NMEA0183->Dpt.OffsetFromTransducerMeters
                    */
                    double depth = 999.;
                    if( m_NMEA0183->Dpt.DepthMeters != 999. ) depth = m_NMEA0183->Dpt.DepthMeters;
                    if( m_NMEA0183->Dpt.OffsetFromTransducerMeters != 999. ) depth += m_NMEA0183->Dpt.OffsetFromTransducerMeters;
                    depth += g_dDashDBTOffset;
                    SendSentenceToAllInstruments( OCPN_DBP_STC_DPT, toUsrDistance_Plugin( depth / 1852.0, g_iDashDepthUnit ), getUsrDistanceUnit_Plugin( g_iDashDepthUnit ) );
                }
            }
        }
        // TODO: GBS - GPS Satellite fault detection
        else if( m_NMEA0183->LastSentenceIDReceived == _T("GGA") ) {
            if( m_NMEA0183->Parse() ) {
                if( m_NMEA0183->Gga.GPSQuality > 0 ) {
                    if( mPriPosition >= 3 ) {
                        mPriPosition = 3;
                        double lat, lon;
                        float llt = m_NMEA0183->Gga.Position.Latitude.Latitude;
                        int lat_deg_int = (int) ( llt / 100 );
                        float lat_deg = lat_deg_int;
                        float lat_min = llt - ( lat_deg * 100 );
                        lat = lat_deg + ( lat_min / 60. );
                        if( m_NMEA0183->Gga.Position.Latitude.Northing == South ) lat = -lat;
                        SendSentenceToAllInstruments( OCPN_DBP_STC_LAT, lat, _T("SDMM") );

                        float lln = m_NMEA0183->Gga.Position.Longitude.Longitude;
                        int lon_deg_int = (int) ( lln / 100 );
                        float lon_deg = lon_deg_int;
                        float lon_min = lln - ( lon_deg * 100 );
                        lon = lon_deg + ( lon_min / 60. );
                        if( m_NMEA0183->Gga.Position.Longitude.Easting == West ) lon = -lon;
                        SendSentenceToAllInstruments( OCPN_DBP_STC_LON, lon, _T("SDMM") );
                    }

                    if( mPriDateTime >= 4 ) {
                        // Not in use, we need the date too.
                        //mPriDateTime = 4;
                        //mUTCDateTime.ParseFormat( m_NMEA0183->Gga.UTCTime.c_str(), _T("%H%M%S") );
                    }

                    mSatsInView = m_NMEA0183->Gga.NumberOfSatellitesInUse;
                }
            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("GLL") ) {
            if( m_NMEA0183->Parse() ) {
                if( m_NMEA0183->Gll.IsDataValid == NTrue ) {
                    if( mPriPosition >= 1 ) {
                        mPriPosition = 1;
                        double lat, lon;
                        float llt = m_NMEA0183->Gll.Position.Latitude.Latitude;
                        int lat_deg_int = (int) ( llt / 100 );
                        float lat_deg = lat_deg_int;
                        float lat_min = llt - ( lat_deg * 100 );
                        lat = lat_deg + ( lat_min / 60. );
                        if( m_NMEA0183->Gll.Position.Latitude.Northing == South ) lat = -lat;
                        SendSentenceToAllInstruments( OCPN_DBP_STC_LAT, lat, _T("SDMM") );

                        float lln = m_NMEA0183->Gll.Position.Longitude.Longitude;
                        int lon_deg_int = (int) ( lln / 100 );
                        float lon_deg = lon_deg_int;
                        float lon_min = lln - ( lon_deg * 100 );
                        lon = lon_deg + ( lon_min / 60. );
                        if( m_NMEA0183->Gll.Position.Longitude.Easting == West ) lon = -lon;
                        SendSentenceToAllInstruments( OCPN_DBP_STC_LON, lon, _T("SDMM") );
                    }

                    if( mPriDateTime >= 5 ) {
                        // Not in use, we need the date too.
                        //mPriDateTime = 5;
                        //mUTCDateTime.ParseFormat( m_NMEA0183->Gll.UTCTime.c_str(), _T("%H%M%S") );
                    }
                }
            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("GSV") ) {
            if( m_NMEA0183->Parse() ) {
                mSatsInView = m_NMEA0183->Gsv.SatsInView;
                // m_NMEA0183->Gsv.NumberOfMessages;
                SendSentenceToAllInstruments( OCPN_DBP_STC_SAT, m_NMEA0183->Gsv.SatsInView, _T("") );
                SendSatInfoToAllInstruments( m_NMEA0183->Gsv.SatsInView,
                                             m_NMEA0183->Gsv.MessageNumber, m_NMEA0183->Gsv.SatInfo );

                mGPS_Watchdog = gps_watchdog_timeout_ticks;
            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("HDG") ) {
            if( m_NMEA0183->Parse() )
            {
                if( mPriVar >= 2 )
                {
                    // Any device sending VAR=0.0 can be assumed to not really know
                    // what the actual variation is, so in this case we use WMM if available
                    if( (!std::isnan( m_NMEA0183->Hdg.MagneticVariationDegrees )) &&
                        0.0 != m_NMEA0183->Hdg.MagneticVariationDegrees)
                    {
                        mPriVar = 2;
#ifdef _TACTICSPI_H_
                        /* Porting note: I am bit puzzled by this: here we override the mVar
                           which was initially provided by O, in Plugin_Position_Fix().
                           So, we trust the instrument. Fine. But Plugin_Position_Fix()
                           is called by some timer, probably every second like everything
                           in O. It will override mVar, which is used later on in
                           calculations for _STC_HDT. Which sets the mHdt in tactics_pi.
                           mHdt is used about in every algorithm. Now, if WMM-model value
                           and the instrument do not agree, the below leads to a toggling
                           mVar value, and as consequence to toggling mHdt value in Tactics.
                        */
#endif // _TACTICSPI_H_
                        if( m_NMEA0183->Hdg.MagneticVariationDirection == East )
                            mVar =  m_NMEA0183->Hdg.MagneticVariationDegrees;
                        else if( m_NMEA0183->Hdg.MagneticVariationDirection == West )
                            mVar = -m_NMEA0183->Hdg.MagneticVariationDegrees;
                        SendSentenceToAllInstruments( OCPN_DBP_STC_HMV, mVar, _T("\u00B0") );
#ifdef _TACTICSPI_H_
                        /* Porting note: Why not rearm the mVar_Watchdog?
                           Answer: it does not matter. We get anyway the mVar from O's 'fix'
                           which distributes it to all plugins including us. Here it
                           is the priority #1 mVar source, so the others do not really
                           never get used, unless O itself fails at some point to
                           interpret NMEA-sentences.
                        */
                        mVar_Watchdog = gps_watchdog_timeout_ticks;
#endif // _TACTICSPI_H_
                    }

                }
                if( mPriHeadingM >= 1 ) {
                    mPriHeadingM = 1;
                    mHdm = m_NMEA0183->Hdg.MagneticSensorHeadingDegrees;
                    SendSentenceToAllInstruments( OCPN_DBP_STC_HDM, mHdm, _T("\u00B0") );
                }
                if( !std::isnan(m_NMEA0183->Hdg.MagneticSensorHeadingDegrees) )
                    mHDx_Watchdog = gps_watchdog_timeout_ticks;

                //      If Variation is available, no higher priority HDT is available,
                //      then calculate and propagate calculated HDT
                if( !std::isnan(m_NMEA0183->Hdg.MagneticSensorHeadingDegrees) ) {
                    if( !std::isnan( mVar )  && (mPriHeadingT > 3) ){
                        mPriHeadingT = 4;
#ifdef _TACTICSPI_H_
                        /* Porting note: tactics_pi has contained the below corrections
                           since 2015, in "HDG" and in "HDM" with a class variable.Now, in  ov50,
                           the correction has been implemented in dashboard_pi, but with
                           a local variable. Since tactics_pi does not use the class
                           variable anywhere, we retain the solution of dashboard_pi.
                           (You can delete this note and the conditional compilation) */
#endif // _TACTICSPI_H_
                        double heading = mHdm + mVar;
                        if (heading < 0)
                            heading += 360;
                        else if (heading >= 360.0)
                            heading -= 360;
                        SendSentenceToAllInstruments(OCPN_DBP_STC_HDT, heading, _T("\u00B0"));
                        mHDT_Watchdog = gps_watchdog_timeout_ticks;
                    }
                }
            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("HDM") ) {
            if( m_NMEA0183->Parse() ) {
                if( mPriHeadingM >= 2 ) {
                    mPriHeadingM = 2;
                    mHdm = m_NMEA0183->Hdm.DegreesMagnetic;
                    SendSentenceToAllInstruments( OCPN_DBP_STC_HDM, mHdm, _T("\u00B0M") );
                }
                if( !std::isnan(m_NMEA0183->Hdm.DegreesMagnetic) )
                    mHDx_Watchdog = gps_watchdog_timeout_ticks;

                //      If Variation is available, no higher priority HDT is available,
                //      then calculate and propagate calculated HDT
                if( !std::isnan(m_NMEA0183->Hdm.DegreesMagnetic) ) {
                    if( !std::isnan( mVar )  && (mPriHeadingT > 2) ){
                        mPriHeadingT = 3;
                        double heading = mHdm + mVar;
                        if (heading < 0)
                            heading += 360;
                        else if (heading >= 360.0)
                            heading -= 360;
                        SendSentenceToAllInstruments(OCPN_DBP_STC_HDT, heading, _T("\u00B0"));
                        mHDT_Watchdog = gps_watchdog_timeout_ticks;
                    }
                }

            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("HDT") ) {
            if( m_NMEA0183->Parse() ) {
                if( mPriHeadingT >= 1 ) {
                    mPriHeadingT = 1;
                    if( m_NMEA0183->Hdt.DegreesTrue < 999. ) {
                        SendSentenceToAllInstruments( OCPN_DBP_STC_HDT, m_NMEA0183->Hdt.DegreesTrue,
                                                      _T("\u00B0T") );
                    }
                }
                if( !std::isnan(m_NMEA0183->Hdt.DegreesTrue) )
                    mHDT_Watchdog = gps_watchdog_timeout_ticks;
            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("MTA") ) {  //Air temperature
            if( m_NMEA0183->Parse() ) {
                /*
                  double   m_NMEA0183->Mta.Temperature;
                  wxString m_NMEA0183->Mta.UnitOfMeasurement;
                */
#ifdef _TACTICSPI_H_
                double TemperatureValue               = m_NMEA0183->Mta.Temperature;
                wxString TemperatureUnitOfMeasurement = m_NMEA0183->Mta.UnitOfMeasurement;
                checkNMEATemperatureDataAndUnit( TemperatureValue, TemperatureUnitOfMeasurement );
                SendSentenceToAllInstruments( OCPN_DBP_STC_ATMP, TemperatureValue, TemperatureUnitOfMeasurement );
#else
                SendSentenceToAllInstruments( OCPN_DBP_STC_ATMP, m_NMEA0183->Mta.Temperature,
                                              m_NMEA0183->Mta.UnitOfMeasurement );
#endif // _TACTICSPI_H_
            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("MDA") ) {  //Barometric pressure
            if( m_NMEA0183->Parse() ) {
                // TODO make posibilyti to select between Bar or InchHg
                /*

                  double   m_NMEA0183->Mda.Pressure;

                  wxString m_NMEA0183->Mda.UnitOfMeasurement;

                */

                if( m_NMEA0183->Mda.Pressure > .8 && m_NMEA0183->Mda.Pressure < 1.1 ) {
                    SendSentenceToAllInstruments( OCPN_DBP_STC_MDA, m_NMEA0183->Mda.Pressure *1000,
                                                  _T("hPa") ); //Convert to hpa befor sending to instruments.
                }

            }

        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("MTW") ) {
            if( m_NMEA0183->Parse() ) {
                /*
                  double   m_NMEA0183->Mtw.Temperature;
                  wxString m_NMEA0183->Mtw.UnitOfMeasurement;
                */
#ifdef _TACTICSPI_H_
                double TemperatureValue = m_NMEA0183->Mtw.Temperature;
                wxString TemperatureUnitOfMeasurement = m_NMEA0183->Mtw.UnitOfMeasurement;
                checkNMEATemperatureDataAndUnit( TemperatureValue, TemperatureUnitOfMeasurement );
                SendSentenceToAllInstruments( OCPN_DBP_STC_TMP, TemperatureValue, TemperatureUnitOfMeasurement );
#else
                SendSentenceToAllInstruments( OCPN_DBP_STC_TMP, m_NMEA0183->Mtw.Temperature,
                                              m_NMEA0183->Mtw.UnitOfMeasurement );
#endif // _TACTICSPI_H_
            }

        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("VLW") ) {
            if( m_NMEA0183->Parse() ) {
                /*
                  double   m_NMEA0183->Vlw.TotalMileage;
                  double   m_NMEA0183->Vlw.TripMileage;
                */
#ifdef _TACTICSPI_H_
                /* Porting note: tactics_pi has the below two sentence commented out,
                   for no obvious reason. dashboard_pi solution is retained.
                   (You can delete this note and the conditional compilation) */
#endif // _TACTICSPI_H_
                SendSentenceToAllInstruments( OCPN_DBP_STC_VLW1, toUsrDistance_Plugin( m_NMEA0183->Vlw.TripMileage, g_iDashDistanceUnit ),
                                              getUsrDistanceUnit_Plugin( g_iDashDistanceUnit ) );

                SendSentenceToAllInstruments( OCPN_DBP_STC_VLW2, toUsrDistance_Plugin( m_NMEA0183->Vlw.TotalMileage, g_iDashDistanceUnit ),
                                              getUsrDistanceUnit_Plugin( g_iDashDistanceUnit ) );
            }

        }
        // NMEA 0183 standard Wind Direction and Speed, with respect to north.
        else if( m_NMEA0183->LastSentenceIDReceived == _T("MWD") ) {
            if( m_NMEA0183->Parse() ) {
                // Option for True vs Magnetic
                wxString windunit;
                if( m_NMEA0183->Mwd.WindAngleTrue < 999. ) { //if WindAngleTrue is available, use it ...
                    SendSentenceToAllInstruments( OCPN_DBP_STC_TWD, m_NMEA0183->Mwd.WindAngleTrue,
                                                  _T("\u00B0T") );
#ifdef _TACTICSPI_H_
                    this->SetNMEASentence_Arm_TWD_Watchdog();
#endif // _TACTICSPI_H_
                } else if( m_NMEA0183->Mwd.WindAngleMagnetic < 999. ) { //otherwise try WindAngleMagnetic ...
                    SendSentenceToAllInstruments( OCPN_DBP_STC_TWD, m_NMEA0183->Mwd.WindAngleMagnetic,
                                                  _T("\u00B0M") );
#ifdef _TACTICSPI_H_
                    this->SetNMEASentence_Arm_TWD_Watchdog();
#endif // _TACTICSPI_H_
                }
#ifdef _TACTICSPI_H_
                if (!this->SetNMEASentenceMWD_NKEbug(m_NMEA0183->Mwd.WindSpeedKnots)) {
#endif // _TACTICSPI_H_
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_TWS, toUsrSpeed_Plugin( m_NMEA0183->Mwd.WindSpeedKnots, g_iDashWindSpeedUnit ),
                        getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ) );
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_TWS2, toUsrSpeed_Plugin( m_NMEA0183->Mwd.WindSpeedKnots, g_iDashWindSpeedUnit ),
                        getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ) );
#ifdef _TACTICSPI_H_
                } // then NKEbug hunt activated and condition for it detected, drop.
#endif // _TACTICSPI_H_
                //m_NMEA0183->Mwd.WindSpeedms
            }
        }
        // NMEA 0183 standard Wind Speed and Angle, in relation to the vessel's bow/centerline.
        else if( m_NMEA0183->LastSentenceIDReceived == _T("MWV") ) {
            if( m_NMEA0183->Parse() ) {
                if( m_NMEA0183->Mwv.IsDataValid == NTrue ) {
                    //MWV windspeed has different units. Form it to knots to fit "toUsrSpeed_Plugin()"
                    double m_wSpeedFactor = 1.0; //knots ("N")
                    if (m_NMEA0183->Mwv.WindSpeedUnits == _T("K") ) m_wSpeedFactor = 0.53995 ; //km/h > knots
                    if (m_NMEA0183->Mwv.WindSpeedUnits == _T("M") ) m_wSpeedFactor = 1.94384 ; //m/s > knots

                    if( m_NMEA0183->Mwv.Reference == _T("R") ) // Relative (apparent wind)
                    {
                        if( mPriAWA >= 1 ) {
                            mPriAWA = 1;
                            wxString m_awaunit;
                            double m_awaangle;
                            if (m_NMEA0183->Mwv.WindAngle >180) {
#ifdef _TACTICSPI_H_
                                m_awaunit = L"\u00B0lr"; // == wind arrow on port side
#else
                                m_awaunit = _T("\u00B0L");
#endif // _TACTICSPI_H_

                                
                                m_awaangle = 180.0 - (m_NMEA0183->Mwv.WindAngle - 180.0);
                            }
                            else {
#ifdef _TACTICSPI_H_
                                m_awaunit = L"\u00B0rl"; // == wind arrow on starboard side
#else
                                m_awaunit = _T("\u00B0R");
#endif // _TACTICSPI_H_
                                m_awaangle = m_NMEA0183->Mwv.WindAngle;
                            }
                            SendSentenceToAllInstruments( OCPN_DBP_STC_AWA,
                                                          m_awaangle, m_awaunit);
                            SendSentenceToAllInstruments( OCPN_DBP_STC_AWS,
                                                          toUsrSpeed_Plugin( m_NMEA0183->Mwv.WindSpeed * m_wSpeedFactor,
                                                                             g_iDashWindSpeedUnit ),
                                                          getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ) );
#ifdef _TACTICSPI_H_
                            this->SetNMEASentence_Arm_AWS_Watchdog();
#endif // _TACTICSPI_H_
                        }
                    }

                    else if( m_NMEA0183->Mwv.Reference == _T("T") ) { // Theoretical (aka True)
                        if( mPriTWA >= 1 ) {
                            mPriTWA = 1;
                            wxString m_twaunit;
                            double m_twaangle;
                            if (m_NMEA0183->Mwv.WindAngle >180) {
#ifdef _TACTICSPI_H_
                                m_twaunit = L"\u00B0lr";  // == wind arrow on port side
#else
                                m_twaunit = _T("\u00B0L");
#endif // _TACTICSPI_H_
                                m_twaangle = 180.0 - (m_NMEA0183->Mwv.WindAngle - 180.0);
                            }
                            else {
#ifdef _TACTICSPI_H_
                                m_twaunit = L"\u00B0rl";  // == wind arrow on starboard side
#else
                                m_twaunit = _T("\u00B0R");
#endif // _TACTICSPI_H_
                                m_twaangle = m_NMEA0183->Mwv.WindAngle;
                            }
                            SendSentenceToAllInstruments( OCPN_DBP_STC_TWA,
                                                          m_twaangle, m_twaunit);
                            SendSentenceToAllInstruments( OCPN_DBP_STC_TWS,
                                                          toUsrSpeed_Plugin( m_NMEA0183->Mwv.WindSpeed * m_wSpeedFactor,
                                                                             g_iDashWindSpeedUnit ),
                                                          getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ) );
                            SendSentenceToAllInstruments( OCPN_DBP_STC_TWS2,
                                                          toUsrSpeed_Plugin( m_NMEA0183->Mwv.WindSpeed * m_wSpeedFactor,
                                                                             g_iDashWindSpeedUnit ),
                                                          getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ) );
#ifdef _TACTICSPI_H_
                            this->SetNMEASentence_Arm_TWS_Watchdog();
#endif // _TACTICSPI_H_
                        }
                    }
                }
            }
        }

#ifdef _TACTICSPI_H_
        else if (m_NMEA0183->LastSentenceIDReceived == _T("RMB")) {
            if (m_NMEA0183->Parse()) {
                if (m_NMEA0183->Rmb.IsDataValid == NTrue) {
                    if ( !std::isnan(m_NMEA0183->Rmb.BearingToDestinationDegreesTrue) &&
                         (m_NMEA0183->Rmb.BearingToDestinationDegreesTrue < 999.) ) { // empty field
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_BRG, m_NMEA0183->Rmb.BearingToDestinationDegreesTrue, m_NMEA0183->Rmb.To);
                        this->SetNMEASentence_Arm_BRG_Watchdog();
                        
                    }
                    if ( !std::isnan(m_NMEA0183->Rmb.RangeToDestinationNauticalMiles) &&
                         (m_NMEA0183->Rmb.RangeToDestinationNauticalMiles < 999.) ) // empty field
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_DTW, m_NMEA0183->Rmb.RangeToDestinationNauticalMiles, _T("Nm"));
                    /*
                      Note: there are no consumers in Tactics functions for the below sentence but without it
                      the Dashboard's VMG-instruments remain silent when there is next active waypoint set
                      by the OpenCPN's routing functions. We capture here the sentence send by OpenCPN
                      to the autopilot and  other interested parties like. If the GitHub issue #1422 is
                      recognized and fixed in OpenCPN, this note and the below sentences can be removed */
                    if ( !std::isnan(m_NMEA0183->Rmb.DestinationClosingVelocityKnots) &&
                         (m_NMEA0183->Rmb.DestinationClosingVelocityKnots < 999.) ) { // empty field
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_VMG, toUsrSpeed_Plugin(
                                m_NMEA0183->Rmb.DestinationClosingVelocityKnots, g_iDashWindSpeedUnit ),
                            getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ) );
                        this->SetNMEASentence_Arm_VMG_Watchdog();
                    } // then valid sentence with VMG information received
                } // then valid data
            } // then sentence parse OK
        } // then last sentence is RMB
#endif // _TACTICSPI_H_

        else if( m_NMEA0183->LastSentenceIDReceived == _T("RMC") ) {
            if( m_NMEA0183->Parse() ) {
                if( m_NMEA0183->Rmc.IsDataValid == NTrue ) {
                    if( mPriPosition >= 4 ) {
                        mPriPosition = 4;
                        double lat, lon;
                        float llt = m_NMEA0183->Rmc.Position.Latitude.Latitude;
                        int lat_deg_int = (int) ( llt / 100 );
                        float lat_deg = lat_deg_int;
                        float lat_min = llt - ( lat_deg * 100 );
                        lat = lat_deg + ( lat_min / 60. );
                        if( m_NMEA0183->Rmc.Position.Latitude.Northing == South ) lat = -lat;
                        SendSentenceToAllInstruments( OCPN_DBP_STC_LAT, lat, _T("SDMM") );

                        float lln = m_NMEA0183->Rmc.Position.Longitude.Longitude;
                        int lon_deg_int = (int) ( lln / 100 );
                        float lon_deg = lon_deg_int;
                        float lon_min = lln - ( lon_deg * 100 );
                        lon = lon_deg + ( lon_min / 60. );
                        if( m_NMEA0183->Rmc.Position.Longitude.Easting == West ) lon = -lon;
                        SendSentenceToAllInstruments( OCPN_DBP_STC_LON, lon, _T("SDMM") );
                    }

                    if( mPriCOGSOG >= 3 ) {
                        mPriCOGSOG = 3;
                        if( m_NMEA0183->Rmc.SpeedOverGroundKnots < 999. ) {
                            SendSentenceToAllInstruments( OCPN_DBP_STC_SOG,
                                                          toUsrSpeed_Plugin(
                                                              mSOGFilter.filter(m_NMEA0183->Rmc.SpeedOverGroundKnots),
                                                              g_iDashSpeedUnit ),
                                                          getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ) );
                        } else {
                            //->SetData(_T("---"));
                        }
                        if( m_NMEA0183->Rmc.TrackMadeGoodDegreesTrue < 999. ) {
                            SendSentenceToAllInstruments(
                                OCPN_DBP_STC_COG, mCOGFilter.filter(
                                    m_NMEA0183->Rmc.TrackMadeGoodDegreesTrue), _T("\u00B0") );
                        } else {
                            //->SetData(_T("---"));
                        }
                        if( m_NMEA0183->Rmc.TrackMadeGoodDegreesTrue < 999. && m_NMEA0183->Rmc.MagneticVariation < 999.) {
                            double dMagneticCOG;
                            if (m_NMEA0183->Rmc.MagneticVariationDirection == East) {
                                dMagneticCOG = mCOGFilter.get() - m_NMEA0183->Rmc.MagneticVariation;
                                if ( dMagneticCOG < 0.0 ) dMagneticCOG = 360.0 + dMagneticCOG;
                            }
                            else {
                                dMagneticCOG = mCOGFilter.get() + m_NMEA0183->Rmc.MagneticVariation;
                                if ( dMagneticCOG > 360.0 ) dMagneticCOG = dMagneticCOG - 360.0;
                            }
                            SendSentenceToAllInstruments( OCPN_DBP_STC_MCOG,
                                                          dMagneticCOG, _T("\u00B0M") );
                        } else {
                            //->SetData(_T("---"));
                        }
                    }

                    if( mPriVar >= 3 )
                    {
                        // Any device sending VAR=0.0 can be assumed to not really know
                        // what the actual variation is, so in this case we use WMM if available
                        if( (!std::isnan( m_NMEA0183->Rmc.MagneticVariation)) &&
                            0.0 != m_NMEA0183->Rmc.MagneticVariation )
                        {
                            mPriVar = 3;
                            if (m_NMEA0183->Rmc.MagneticVariationDirection == East)
                                mVar = m_NMEA0183->Rmc.MagneticVariation;
                            else if (m_NMEA0183->Rmc.MagneticVariationDirection == West)
                                mVar = -m_NMEA0183->Rmc.MagneticVariation;
                            mVar_Watchdog = gps_watchdog_timeout_ticks;

                            SendSentenceToAllInstruments(OCPN_DBP_STC_HMV, mVar, _T("\u00B0"));
                        }
                    }

                    if( mPriDateTime >= 3 ) {
                        mPriDateTime = 3;
                        wxString dt = m_NMEA0183->Rmc.Date + m_NMEA0183->Rmc.UTCTime;
                        mUTCDateTime.ParseFormat( dt.c_str(), _T("%d%m%y%H%M%S") );
                    }
                }
            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("RSA") ) {
            if( m_NMEA0183->Parse() ) {
                if( m_NMEA0183->Rsa.IsStarboardDataValid == NTrue ) {
                    SendSentenceToAllInstruments( OCPN_DBP_STC_RSA, m_NMEA0183->Rsa.Starboard,
                                                  _T("\u00B0") );
                } else if( m_NMEA0183->Rsa.IsPortDataValid == NTrue ) {
                    SendSentenceToAllInstruments( OCPN_DBP_STC_RSA, -m_NMEA0183->Rsa.Port,
                                                  _T("\u00B0") );
                }
            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("VHW") ) {
            if( m_NMEA0183->Parse() ) {
                if( mPriHeadingT >= 2 ) {
                    if( m_NMEA0183->Vhw.DegreesTrue < 999. ) {
                        mPriHeadingT = 2;
                        SendSentenceToAllInstruments( OCPN_DBP_STC_HDT, m_NMEA0183->Vhw.DegreesTrue,
                                                      _T("\u00B0T") );
                    }
                }
                if( mPriHeadingM >= 3 ) {
                    mPriHeadingM = 3;
                    SendSentenceToAllInstruments( OCPN_DBP_STC_HDM, m_NMEA0183->Vhw.DegreesMagnetic,
                                                  _T("\u00B0M") );
                }
                if( m_NMEA0183->Vhw.Knots < 999. ) {
                    SendSentenceToAllInstruments( OCPN_DBP_STC_STW, toUsrSpeed_Plugin( m_NMEA0183->Vhw.Knots, g_iDashSpeedUnit ),
                                                  getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ) );
#ifdef _TACTICSPI_H_
                    mStW_Watchdog = gps_watchdog_timeout_ticks;
#endif // _TACTICSPI_H_
                }

                if( !std::isnan(m_NMEA0183->Vhw.DegreesMagnetic) )
                    mHDx_Watchdog = gps_watchdog_timeout_ticks;
                if( !std::isnan(m_NMEA0183->Vhw.DegreesTrue) )
                    mHDT_Watchdog = gps_watchdog_timeout_ticks;

            }
        }

        else if( m_NMEA0183->LastSentenceIDReceived == _T("VTG") ) {
            if( m_NMEA0183->Parse() ) {
                if( mPriCOGSOG >= 2 ) {
                    mPriCOGSOG = 2;
                    //    Special check for unintialized values, as opposed to zero values
                    if( m_NMEA0183->Vtg.SpeedKnots < 999. ) {
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_SOG, toUsrSpeed_Plugin(
                                mSOGFilter.filter(m_NMEA0183->Vtg.SpeedKnots), g_iDashSpeedUnit ),
                            getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ) );
                    } else {
                        //->SetData(_T("---"));
                    }
                    // Vtg.SpeedKilometersPerHour;
                    if( m_NMEA0183->Vtg.TrackDegreesTrue < 999. ) {
                        SendSentenceToAllInstruments( OCPN_DBP_STC_COG,
                                                      mCOGFilter.filter(m_NMEA0183->Vtg.TrackDegreesTrue), _T("\u00B0") );
                    } else {
                        //->SetData(_T("---"));
                    }
                }

                /*
                  m_NMEA0183->Vtg.TrackDegreesMagnetic;
                */
            }
        }
        /* NMEA 0183 Relative (Apparent) Wind Speed and Angle. Wind angle in relation
         * to the vessel's heading, and wind speed measured relative to the moving vessel. */
        else if( m_NMEA0183->LastSentenceIDReceived == _T("VWR") ) {
            if( m_NMEA0183->Parse() ) {
                if( mPriAWA >= 2 ) {
                    mPriAWA = 2;

                    wxString awaunit;
#ifdef _TACTICSPI_H_
                    awaunit = m_NMEA0183->Vwr.DirectionOfWind == Left ? L"\u00B0lr" : L"\u00B0rl";
#else
                    awaunit = m_NMEA0183->Vwr.DirectionOfWind == Left ? _T("\u00B0L") : _T("\u00B0R");
#endif // _TACTICSPI_H_
                    SendSentenceToAllInstruments( OCPN_DBP_STC_AWA,
                                                  m_NMEA0183->Vwr.WindDirectionMagnitude, awaunit );
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_AWS, toUsrSpeed_Plugin(
                            m_NMEA0183->Vwr.WindSpeedKnots, g_iDashWindSpeedUnit ),
                        getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ) );
#ifdef _TACTICSPI_H_
                    this->SetNMEASentence_Arm_AWS_Watchdog();
#endif // _TACTICSPI_H_
                    /*
                      double m_NMEA0183->Vwr.WindSpeedms;
                      double m_NMEA0183->Vwr.WindSpeedKmh;
                    */
                }
            }
        }
        /* NMEA 0183 True wind angle in relation to the vessel's heading, and true wind
         * speed referenced to the water. True wind is the vector sum of the Relative
         * (apparent) wind vector and the vessel's velocity vector relative to the water along
         * the heading line of the vessel. It represents the wind at the vessel if it were
         * stationary relative to the water and heading in the same direction. */
        else if( m_NMEA0183->LastSentenceIDReceived == _T("VWT") ) {
            if( m_NMEA0183->Parse() ) {
                if( mPriTWA >= 2 ) {
                    mPriTWA = 2;
                    wxString vwtunit;
#ifdef _TACTICSPI_H_
                    vwtunit = m_NMEA0183->Vwt.DirectionOfWind == Left ? L"\u00B0lr" : L"\u00B0rl";
#else
                    vwtunit = m_NMEA0183->Vwt.DirectionOfWind == Left ? _T("\u00B0L") : _T("\u00B0R");
#endif // _TACTICSPI_H_
                    SendSentenceToAllInstruments( OCPN_DBP_STC_TWA,
                                                  m_NMEA0183->Vwt.WindDirectionMagnitude, vwtunit );
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_TWS, toUsrSpeed_Plugin(
                            m_NMEA0183->Vwt.WindSpeedKnots, g_iDashWindSpeedUnit ),
                        getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ) );
                    /*
                      double           m_NMEA0183->Vwt.WindSpeedms;
                      double           m_NMEA0183->Vwt.WindSpeedKmh;
                    */
                }
            }
        }

        else if (m_NMEA0183->LastSentenceIDReceived == _T("XDR")) { //Transducer measurement
            /* XDR Transducer types
             * AngularDisplacementTransducer = 'A',
             * TemperatureTransducer = 'C',
             * LinearDisplacementTransducer = 'D',
             * FrequencyTransducer = 'F',
             * HumidityTransducer = 'H',
             * ForceTransducer = 'N',
             * PressureTransducer = 'P',
             * FlowRateTransducer = 'R',
             * TachometerTransducer = 'T',
             * VolumeTransducer = 'V'
             */

            if (m_NMEA0183->Parse()) {
                wxString xdrunit;
                double xdrdata;
                for (int i = 0; i<m_NMEA0183->Xdr.TransducerCnt; i++) {
                    xdrdata = m_NMEA0183->Xdr.TransducerInfo[i].Data;
                    // XDR Airtemp
                    if (m_NMEA0183->Xdr.TransducerInfo[i].Type == _T("C")) {
#ifdef _TACTICSPI_H_
                        double TemperatureValue               = xdrdata;
                        wxString TemperatureUnitOfMeasurement = m_NMEA0183->Xdr.TransducerInfo[i].Unit;
                        checkNMEATemperatureDataAndUnit( TemperatureValue, TemperatureUnitOfMeasurement );
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_ATMP, TemperatureValue, TemperatureUnitOfMeasurement );
#else
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_ATMP, xdrdata , m_NMEA0183->Xdr.TransducerInfo[i].Unit);
#endif // _TACTICSPI_H_
                    }
#ifdef _TACTICSPI_H_
                    // NKE style of XDR Airtemp etc. cf. original Tactics Plugin
                    if (m_NMEA0183->Xdr.TransducerInfo[i].Name == _T("AirTemp") ||
                        m_NMEA0183->Xdr.TransducerInfo[i].Name == _T("ENV_OUTAIR_T") ||
                        m_NMEA0183->Xdr.TransducerInfo[i].Name == _T("ENV_OUTSIDE_T")){
                        double TemperatureValue               = xdrdata;
                        wxString TemperatureUnitOfMeasurement = m_NMEA0183->Xdr.TransducerInfo[i].Unit;
                        checkNMEATemperatureDataAndUnit( TemperatureValue, TemperatureUnitOfMeasurement );
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_ATMP, TemperatureValue, TemperatureUnitOfMeasurement );
                    }
#endif // _TACTICSPI_H_
                    // XDR Pressure
                    if (m_NMEA0183->Xdr.TransducerInfo[i].Type == _T("P")) {
                        if (m_NMEA0183->Xdr.TransducerInfo[i].Unit == _T("B")) {
                            xdrdata *= 1000;
                            SendSentenceToAllInstruments(OCPN_DBP_STC_MDA, xdrdata , _T("mBar") );
                        }
                    }
                    // XDR Pitch (=Nose up/down) or Heel (stb/port)
                    if (m_NMEA0183->Xdr.TransducerInfo[i].Type == _T("A")) {
                        if (m_NMEA0183->Xdr.TransducerInfo[i].Name == _T("PTCH")
                            || m_NMEA0183->Xdr.TransducerInfo[i].Name == _T("PITCH")) {
                            if (m_NMEA0183->Xdr.TransducerInfo[i].Data > 0) {
#ifdef _TACTICSPI_H_
                                xdrunit = L"\u00B0u";
#else
                                xdrunit = _T("\u00B0 Nose up");
#endif // _TACTICSPI_H_                                
                            }
                            else if (m_NMEA0183->Xdr.TransducerInfo[i].Data < 0) {
#ifdef _TACTICSPI_H_
                                xdrunit = L"\u00B0d";
#else
                                xdrunit = _T("\u00B0 Nose down");
                                xdrdata *= -1;
#endif // _TACTICSPI_H_                                
                            }
                            else {
                                xdrunit = _T("\u00B0");
                            }
                            SendSentenceToAllInstruments(OCPN_DBP_STC_PITCH, xdrdata, xdrunit);
                        }
#ifdef _TACTICSPI_H_
                        else if ((m_NMEA0183->Xdr.TransducerInfo[i].Name == _T("ROLL")) ||
                                 (m_NMEA0183->Xdr.TransducerInfo[i].Name == _T("Heel Angle")))
#else
                        else if (m_NMEA0183->Xdr.TransducerInfo[i].Name == _T("ROLL"))
#endif // _TACTICSPI_H_
                        {
                            if (m_NMEA0183->Xdr.TransducerInfo[i].Data > 0) {
#ifdef _TACTICSPI_H_
                                xdrunit = L"\u00B0r";
#else
                                xdrunit = _T("\u00B0 to Starboard");
#endif // _TACTICSPI_H_
                            }
                            else if (m_NMEA0183->Xdr.TransducerInfo[i].Data < 0) {
#ifdef _TACTICSPI_H_
                                xdrunit = L"\u00B0l";
#else
                                xdrunit = _T("\u00B0 to Port");
                                xdrdata *= -1;
#endif // _TACTICSPI_H_
                            }
                            else {
                                xdrunit = _T("\u00B0");
                            }
                            SendSentenceToAllInstruments(OCPN_DBP_STC_HEEL, xdrdata, xdrunit);
                        } 
                        //Nasa style water temp
                        else if (m_NMEA0183->Xdr.TransducerInfo[i].Name == _T("ENV_WATER_T")){
#ifdef _TACTICSPI_H_
                            double TemperatureValue               = xdrdata;
                            wxString TemperatureUnitOfMeasurement = m_NMEA0183->Xdr.TransducerInfo[i].Unit;
                            checkNMEATemperatureDataAndUnit( TemperatureValue, TemperatureUnitOfMeasurement );
                            SendSentenceToAllInstruments(
                                OCPN_DBP_STC_ATMP, TemperatureValue, TemperatureUnitOfMeasurement );
#else
                            SendSentenceToAllInstruments(
                                OCPN_DBP_STC_TMP,
                                m_NMEA0183->Xdr.TransducerInfo[i].Data,m_NMEA0183->Xdr.TransducerInfo[i].Unit);
#endif // _TACTICSPI_H_
                        }
                    }
                } // for XDR transducers
            } // then parse OK
        } // then XDR
        else if (m_NMEA0183->LastSentenceIDReceived == _T("ZDA")) {
            if( m_NMEA0183->Parse() ) {
                if( mPriDateTime >= 2 ) {
                    mPriDateTime = 2;
                    
                    /*
                      wxString m_NMEA0183->Zda.UTCTime;
                      int      m_NMEA0183->Zda.Day;
                      int      m_NMEA0183->Zda.Month;
                      int      m_NMEA0183->Zda.Year;
                      int      m_NMEA0183->Zda.LocalHourDeviation;
                      int      m_NMEA0183->Zda.LocalMinutesDeviation;
                    */
                    wxString dt;
                    dt.Printf( _T("%4d%02d%02d"), m_NMEA0183->Zda.Year, m_NMEA0183->Zda.Month,
                               m_NMEA0183->Zda.Day );
                    dt.Append( m_NMEA0183->Zda.UTCTime );
                    mUTCDateTime.ParseFormat( dt.c_str(), _T("%Y%m%d%H%M%S") );
                }
            }
        }
        //      Process an AIVDO message
        else if( sentence.Mid( 1, 5 ).IsSameAs( _T("AIVDO") ) ) {
            PlugIn_Position_Fix_Ex gpd;
            if( DecodeSingleVDOMessage(sentence, &gpd, &m_VDO_accumulator) ) {
                    
                if( !std::isnan(gpd.Lat) )
                    SendSentenceToAllInstruments( OCPN_DBP_STC_LAT, gpd.Lat, _T("SDMM") );
                    
                if( !std::isnan(gpd.Lon) )
                    SendSentenceToAllInstruments( OCPN_DBP_STC_LON, gpd.Lon, _T("SDMM") );
                    
                SendSentenceToAllInstruments(
                    OCPN_DBP_STC_SOG, toUsrSpeed_Plugin(
                        mSOGFilter.filter(gpd.Sog), g_iDashSpeedUnit),
                    getUsrSpeedUnit_Plugin(g_iDashSpeedUnit));
                SendSentenceToAllInstruments( OCPN_DBP_STC_COG, mCOGFilter.filter(gpd.Cog), _T("\u00B0") );
                if( !std::isnan(gpd.Hdt) ) {
                    SendSentenceToAllInstruments( OCPN_DBP_STC_HDT, gpd.Hdt, _T("\u00B0T") );
                    mHDT_Watchdog = gps_watchdog_timeout_ticks;
                }
            }
        }
    }
#ifdef _TACTICSPI_H_
    else { // SignalK - see https://git.io/Je3W0 for supported NMEA-0183 sentences
        if ( type->IsSameAs( "NMEA0183", false ) ) {

            if ( this->m_pSkData->isSubscribedToAllPaths() )
                this->m_pSkData->UpdateNMEA0183PathList( path, key );
            
            if ( sentenceId->CmpNoCase(_T("DBT")) == 0 ) { // https://git.io/JeYfB
                if ( path->CmpNoCase(_T("environment.depth.belowTransducer")) == 0 ) {
                    if( mPriDepth >= 2 ) {
                        mPriDepth = 2;
                        double depth = value + g_dDashDBTOffset;
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_DPT,
                            toUsrDistance_Plugin( depth / 1852.0, g_iDashDepthUnit ),
                            getUsrDistanceUnit_Plugin( g_iDashDepthUnit ),
                            timestamp );
                    }
                }
            } // DBT
                
            else if ( sentenceId->CmpNoCase(_T("DPT")) == 0 ) { // https://git.io/JeYf4
                bool depthvalue = false;
                if ( path->CmpNoCase(_T("environment.depth.belowTransducer")) == 0 ) {
                    if ( !mSiK_DPT_environmentDepthBelowKeel )
                        depthvalue = true;
                }
                else if ( path->CmpNoCase(_T("environment.depth.belowKeel")) == 0 ) {  // depth + offset
                    depthvalue = true;
                    mSiK_DPT_environmentDepthBelowKeel = true; // lock priority
                }
                if ( depthvalue ) {
                    if( mPriDepth >= 1 ) {
                        mPriDepth = 1;
                        double depth = value + g_dDashDBTOffset;
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_DPT,
                            toUsrDistance_Plugin( depth / 1852.0, g_iDashDepthUnit ),
                            getUsrDistanceUnit_Plugin( g_iDashDepthUnit ),
                            timestamp );
                    }
                }
            } // DPT

            else if ( sentenceId->CmpNoCase(_T("GGA")) == 0 ) { // https://git.io/JeYWl
                if ( path->CmpNoCase(_T("navigation.gnss.methodQuality")) == 0 ) {
                    if ( valStr->CmpNoCase(_T("DGNSS fix")) == 0 ) {
                        mSiK_navigationGnssMethodQuality = 1;
                    }
                }
                else if ( mSiK_navigationGnssMethodQuality > 0 ) {
                    if ( path->CmpNoCase(_T("navigation.position")) == 0 ) {
                        if( (mPriPosition >= 3) && (key != NULL) ) { // See SetPositionFix() - It rules, even if no fix!
                            mPriPosition = 3;
                            if ( key->CmpNoCase(_T("longitude")) == 0 ) // coordinate: https://git.io/JeYry
                                SendSentenceToAllInstruments( OCPN_DBP_STC_LON,
                                                              value,
                                                              _T("SDMM"),
                                                              timestamp );
                            if ( key->CmpNoCase(_T("latitude")) == 0 )
                                SendSentenceToAllInstruments( OCPN_DBP_STC_LAT,
                                                              value,
                                                              _T("SDMM"),
                                                              timestamp );
                        }
                    }
                    else if ( path->CmpNoCase(_T("navigation.gnss.satellites")) == 0 ) {
                        mSatsInView = value;
                    }
                }
            } // GGA

            else if ( sentenceId->CmpNoCase(_T("GLL")) == 0 ) { // https://git.io/JeYQK
                // Note: Signal K does not send delta is no validy flag set, see the link
                if ( path->CmpNoCase(_T("navigation.position")) == 0 ) {
                    if( (mPriPosition >= 2)  && (key != NULL) ) { // See SetPositionFix() - It rules, even if no fix!
                        mPriPosition = 2;
                        if ( key->CmpNoCase(_T("longitude")) == 0 ) // coordinate: https://git.io/JeYry
                            SendSentenceToAllInstruments( OCPN_DBP_STC_LON,
                                                          value,
                                                          _T("SDMM"),
                                                          timestamp );
                        else if ( key->CmpNoCase(_T("latitude")) == 0 )
                            SendSentenceToAllInstruments( OCPN_DBP_STC_LAT,
                                                          value,
                                                          _T("SDMM"),
                                                          timestamp );
                    }
                }
            } // GLL

            // Note: Sentence GSV not implemented in Signal K delta, see https://git.io/JeYdd - see GGA

            else if ( sentenceId->CmpNoCase(_T("HDG")) == 0 ) { // https://git.io/JeYdxn
                if ( path->CmpNoCase(_T("navigation.headingMagnetic")) == 0 ) {
                    if( mPriHeadingM >= 1 ) { 
                        if ( !std::isnan( value )) {
                            mPriHeadingM = 1;
                            mHdm = value * RAD_IN_DEG;
                            SendSentenceToAllInstruments( OCPN_DBP_STC_HDM,
                                                          mHdm,_T("\u00B0"),
                                                          timestamp );
                            mHDx_Watchdog = gps_watchdog_timeout_ticks;
                        }
                    }
                }
                else if ( path->CmpNoCase(_T("navigation.magneticVariation")) == 0 ) {
                    if( mPriVar >= 2 ) { // see comment in NMEA equivalent: not really useful
                        if ( !std::isnan( value )) {
                            if ( value != 0.0 ) {
                                mPriVar = 2;
                                mVar = value * RAD_IN_DEG;
                                SendSentenceToAllInstruments( OCPN_DBP_STC_HMV,
                                                              mVar,
                                                              _T("\u00B0"),
                                                              timestamp );
                                mVar_Watchdog = gps_watchdog_timeout_ticks;
                            }
                        }
                    }
                }
                if ( !std::isnan( mVar )  && !std::isnan( mHdm ) && (mPriHeadingT > 3) ) {
                    mPriHeadingT = 4;
                    double heading = mHdm + mVar;
                    if (heading < 0)
                        heading += 360;
                    else if (heading >= 360.0)
                        heading -= 360;
                    SendSentenceToAllInstruments(OCPN_DBP_STC_HDT,
                                                 heading,
                                                 _T("\u00B0"),
                                                 timestamp );
                    mHDT_Watchdog = gps_watchdog_timeout_ticks;
                }
            } // HDG
        
            else if ( sentenceId->CmpNoCase(_T("HDM")) == 0 ) { // https://git.io/JeYdxn
                if ( path->CmpNoCase(_T("navigation.headingMagnetic")) == 0 ) {
                    if( mPriHeadingM >= 2 ) { 
                        if ( !std::isnan( value )) {
                            mPriHeadingM = 2;
                            mHdm = value * RAD_IN_DEG;
                            SendSentenceToAllInstruments( OCPN_DBP_STC_HDM,
                                                          mHdm,
                                                          _T("\u00B0M"),
                                                          timestamp );
                            mHDx_Watchdog = gps_watchdog_timeout_ticks;
                        }
                    }
                }
            } // HDM
        
            else if ( sentenceId->CmpNoCase(_T("HDT")) == 0 ) { // https://git.io/JeOCC
                if ( path->CmpNoCase(_T("navigation.headingTrue")) == 0 ) {
                    if( mPriHeadingM >= 1 ) { 
                        if ( !std::isnan( value )) {
                            mPriHeadingM = 1;
                            mHdm = value * RAD_IN_DEG;
                            SendSentenceToAllInstruments( OCPN_DBP_STC_HDT,
                                                          mHdm,
                                                          _T("\u00B0T"),
                                                          timestamp );
                            mHDT_Watchdog = gps_watchdog_timeout_ticks;
                        }
                    }
                }
            } // HDT

            // MTA is not implemented in Signal K delta, cf. https://git.io/JeYdd - see XDR

            else if ( sentenceId->CmpNoCase(_T("MDA")) == 0 ) { // https://git.io/JeOWL
                if ( path->CmpNoCase(_T("environment.outside.pressure")) == 0 ) {
                    // Note: value from Signal K is SI units, thus hPa already
                    if ( (value > 800) && (value < 1100) ) {
                        SendSentenceToAllInstruments( OCPN_DBP_STC_MDA,
                                                      value,
                                                      _T("hPa"),
                                                      timestamp );
                    } // then valid pressure in hPa
                    // Note: Dashboard does not deal with other values in MDA as for now so we skip them
                }
            } // MDA

            else if ( sentenceId->CmpNoCase(_T("MTW")) == 0 ) { // https://git.io/JeOwA
                if ( path->CmpNoCase(_T("environment.water.temperature")) == 0 ) {
                    // Note: value from Signal K is SI units, thus we receive Kelvins
                    double TemperatureValue = value - CELCIUS_IN_KELVIN; 
                    wxString TemperatureUnitOfMeasurement = _T("C"); // MTW default
                    checkNMEATemperatureDataAndUnit( TemperatureValue, TemperatureUnitOfMeasurement );
                    SendSentenceToAllInstruments( OCPN_DBP_STC_TMP,
                                                  TemperatureValue,
                                                  TemperatureUnitOfMeasurement,
                                                  timestamp );
                }
            } // MTW

            // MWD is not implemented in Signal K, cf. https://git.io/JeYdd (but calculated in Tactics)

            else if ( sentenceId->CmpNoCase(_T("MWV")) == 0 ) { // https://git.io/JeOov
                if ( path->CmpNoCase(_T("environment.wind.speedApparent")) == 0 ) {
                    // Note: value from Signal K is SI units, thus we receive m/s
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_AWS,
                        toUsrSpeed_Plugin( value * MS_IN_KNOTS,
                                           g_iDashWindSpeedUnit ),
                        getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ),
                        timestamp );
                    this->SetNMEASentence_Arm_AWS_Watchdog();
                }
                else if ( path->CmpNoCase(_T("environment.wind.angleApparent")) == 0 ) {
                    if( mPriAWA >= 1 ) {
                        mPriAWA = 1;
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_AWA,
                            std::abs( value ) * RAD_IN_DEG,
                            ( value < 0 ? L"\u00B0lr" : L"\u00B0rl" ),
                            timestamp );
                    } // AWA priority
                }
                else if ( path->CmpNoCase(_T("environment.wind.speedTrue")) == 0 ) {
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_TWS,
                        toUsrSpeed_Plugin( value * MS_IN_KNOTS,
                                           g_iDashWindSpeedUnit ),
                        getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ),
                        timestamp );
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_TWS2,
                        toUsrSpeed_Plugin( value * MS_IN_KNOTS,
                                           g_iDashWindSpeedUnit ),
                        getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ),
                        timestamp );
                    this->SetNMEASentence_Arm_TWS_Watchdog();
                }
                else if ( path->CmpNoCase(_T("environment.wind.angleTrueWater")) == 0 ) {
                    if( mPriTWA >= 1 ) {
                        mPriTWA = 1;
                        SendSentenceToAllInstruments( OCPN_DBP_STC_TWA,
                                                      std::abs( value ) * RAD_IN_DEG,
                                                      ( value < 0 ? L"\u00B0lr" : L"\u00B0rl" ),
                                                      timestamp );
                    } // TWA priority
                }
            } // MWV
        
            else if ( sentenceId->CmpNoCase(_T("RMB")) == 0 ) { // https://git.io/Je3UV
                /* See the comment in the same sentence's interpretation above (when coming
                   from OpenCPN): the controversy of having it here is the same:
                   the infamous VMG interpretation of next destination waypoint is not
                   the same as in Tactics, i.e. for the sailing boat performance criteria.
                   It is kept here since it can be considered belonging to Dashboard which
                   needs to serve also the needs of a cruising sailing and motor boats but
                   it can be useful in the off-shore races, too.
                */
                // Dashboard ignores navigation.courseRhumbline.nextPoint, as for now
                if ( path->CmpNoCase(_T("navigation.courseRhumbline.nextPoint.bearingTrue")) == 0 ) {
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_BRG,
                        value * RAD_IN_DEG,
                        _T("\u00B0"), // as for now, Origin ID not available from Signal K
                        timestamp );
                    this->SetNMEASentence_Arm_BRG_Watchdog();
                }
                else if ( path->CmpNoCase(_T("navigation.courseRhumbline.nextPoint.velocityMadeGood")) == 0 ) {
                    // This is THE carburator for hours of useless "discussions" in forums; comment it out if you don't like it :)
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_VMG,
                        toUsrSpeed_Plugin( value * MS_IN_KNOTS, g_iDashWindSpeedUnit ),
                        getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ),
                        timestamp );
                    this->SetNMEASentence_Arm_VMG_Watchdog();
                }
                else if ( path->CmpNoCase(_T("navigation.courseRhumbline.nextPoint.distance")) == 0 ) {
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_DTW,
                        value * KM_IN_NM,
                        _T("Nm"),
                        timestamp );
                }
            } // RMB

            else if ( sentenceId->CmpNoCase(_T("RMC")) == 0 ) { // https://git.io/Je3T3
                if ( path->CmpNoCase(_T("navigation.position")) == 0 ) {
                    if( (mPriPosition >= 4) && (key != NULL) ) {  // See SetPositionFix() - It rules, even if no fix!
                        mPriPosition = 4;
                        if ( key->CmpNoCase(_T("longitude")) == 0 ) // coordinate: https://git.io/JeYry
                            SendSentenceToAllInstruments( OCPN_DBP_STC_LON,
                                                          value,
                                                          _T("SDMM"),
                                                          timestamp );
                        else if ( key->CmpNoCase(_T("latitude")) == 0 )
                            SendSentenceToAllInstruments( OCPN_DBP_STC_LAT,
                                                          value,
                                                          _T("SDMM"),
                                                          timestamp );
                    } // selected by (low) priority
                }
                else if ( ( path->CmpNoCase(_T("navigation.courseOverGroundTrue")) == 0 ) ||
                          ( path->CmpNoCase(_T("navigation.speedOverGround")) == 0 ) ||
                          ( path->CmpNoCase(_T("navigation.magneticVariation")) == 0 ) ) {
                    if ( mPriCOGSOG >= 3 ) {
                        if ( path->CmpNoCase(_T("navigation.courseOverGroundTrue")) == 0 ) {
                            mPriCOGSOG = 3;
                            SendSentenceToAllInstruments(
                                OCPN_DBP_STC_COG,
                                mCOGFilter.filter( value * RAD_IN_DEG ),
                                _T("\u00B0"),
                                timestamp );
                        }
                        else if ( path->CmpNoCase(_T("navigation.speedOverGround")) == 0 ) {
                            mPriCOGSOG = 3;
                            SendSentenceToAllInstruments(
                                OCPN_DBP_STC_SOG,
                                toUsrSpeed_Plugin( mSOGFilter.filter( value * MS_IN_KNOTS ),
                                                   g_iDashSpeedUnit ),
                                getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ),
                                timestamp );
                        }
                        else if ( path->CmpNoCase(_T("navigation.magneticVariation")) == 0 ) {
                            mPriCOGSOG = 3;
                            SendSentenceToAllInstruments(
                                OCPN_DBP_STC_MCOG,
                                value * RAD_IN_DEG,
                                _T("\u00B0M"),
                                timestamp );
                        }
                    } // mPriCOGSOG
                } // then COGSOG contents
                else if ( path->CmpNoCase(_T("navigation.datetime")) == 0 ) {
                    if( mPriDateTime >= 3 ) {
                        mPriDateTime = 3;
                        wxString datetime = *valStr;
                        mUTCDateTime.ParseISOCombined( datetime.BeforeLast('.') ); // rfc3359 not understood
                    } // mPriDateTime
                } // then date/time update received with the above data
            } // RMC

            else if ( sentenceId->CmpNoCase(_T("RSA")) == 0 ) { // https://git.io/Je3sA
                if ( path->CmpNoCase(_T("steering.rudderAngle")) == 0 ) {
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_RSA,
                        value * RAD_IN_DEG,
                        _T("\u00B0"),
                        timestamp );
                }
            } // RSA

            else if ( sentenceId->CmpNoCase(_T("VHW")) == 0 ) { // https://git.io/Je3GE
                if ( path->CmpNoCase(_T("navigation.headingTrue")) == 0 ) {
                    if ( mPriHeadingT >= 2 ) {
                        mPriHeadingT = 2;
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_HDT,
                            value * RAD_IN_DEG,
                            _T("\u00B0T"),
                            timestamp );
                        mHDT_Watchdog = gps_watchdog_timeout_ticks;
                    } // priority activation
                }
                else if ( path->CmpNoCase(_T("navigation.headingMagnetic")) == 0 ) {
                    if ( mPriHeadingM >= 3 ) {
                        mPriHeadingM = 3;
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_HDM,
                            value * RAD_IN_DEG,
                            _T("\u00B0M"),
                            timestamp );
                        mHDx_Watchdog = gps_watchdog_timeout_ticks;
                    } // priority activation
                }
                else if ( path->CmpNoCase(_T("navigation.speedThroughWater")) == 0 ) {
                    SendSentenceToAllInstruments(
                        OCPN_DBP_STC_STW,
                        toUsrSpeed_Plugin( value * MS_IN_KNOTS, g_iDashSpeedUnit ),
                        getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ),
                        timestamp );
                    mStW_Watchdog = gps_watchdog_timeout_ticks;
                }
            } // VHW

            else if ( sentenceId->CmpNoCase(_T("VLW")) == 0 ) { // https://git.io/JeOrS
                if ( path->CmpNoCase(_T("navigation.trip.log")) == 0 ) {
                    // Note: value from Signal K is "as received", i.e. nautical miles
                    if ( value >= 0.0 )
                        SendSentenceToAllInstruments( OCPN_DBP_STC_VLW1,
                                                      toUsrDistance_Plugin( value,
                                                                            g_iDashDistanceUnit ),
                                                      getUsrDistanceUnit_Plugin( g_iDashDistanceUnit ),
                                                      timestamp );
                }
                else if ( path->CmpNoCase(_T("navigation.log")) == 0 ) {
                    if ( value >= 0.0 )
                        SendSentenceToAllInstruments( OCPN_DBP_STC_VLW2,
                                                      toUsrDistance_Plugin( value,
                                                                            g_iDashDistanceUnit ),
                                                      getUsrDistanceUnit_Plugin( g_iDashDistanceUnit ),
                                                      timestamp );
                }
            } // VLW

            else if ( sentenceId->CmpNoCase(_T("VTG")) == 0 ) { // https://git.io/Je3Zp
                // for now, Dashboard ignores "navigation.courseOverGroundMagnetic"           
                if ( mPriCOGSOG >= 2 ) {
                    if ( path->CmpNoCase(_T("navigation.speedOverGround")) == 0 ) {
                        mPriCOGSOG = 2;
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_SOG,
                            toUsrSpeed_Plugin(
                                mSOGFilter.filter( value * MS_IN_KNOTS ),
                                g_iDashSpeedUnit ),
                            getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ),
                            timestamp );
                    }
                    else if ( path->CmpNoCase(_T("navigation.courseOverGroundTrue")) == 0 ) {
                        mPriCOGSOG = 2;
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_COG,
                            mCOGFilter.filter( value * RAD_IN_DEG ),
                            _T("\u00B0"),
                            timestamp );
                    }
                } // priority activation
            } // VTG

            else if ( sentenceId->CmpNoCase(_T("VWR")) == 0 ) { // 
                if( mPriAWA >= 2 ) {
                    if ( path->CmpNoCase(_T("environment.wind.speedApparent")) == 0 ) {
                        mPriAWA = 2;
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_AWS,
                            toUsrSpeed_Plugin(
                                value * MS_IN_KNOTS, g_iDashWindSpeedUnit ),
                            getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ),
                            timestamp );
                        this->SetNMEASentence_Arm_AWS_Watchdog();
                    }
                    else if ( path->CmpNoCase(_T("environment.wind.angleApparent")) == 0 ) {
                        mPriAWA = 2;
                        SendSentenceToAllInstruments(
                            OCPN_DBP_STC_AWA,
                            std::abs( value ) * RAD_IN_DEG,
                            ( value < 0 ? L"\u00B0lr" : L"\u00B0rl" ),
                            timestamp );
                    }
                } // prirority activation
            } // VWR

            // VWT not implemented as for now (but Tactics calculates it)

            /*
              XDR is not implemented in the Signal K TCP delta stream yet but it is
              available from the NMEA stream which the OpenCPN receives from Signal K.
              Therefore it is searched and executed in the above non-SignalK section.
            */
        } // then NMEA-0183 delta from Signal K
        else if ( type->IsSameAs( "NMEA2000", false ) ) {

            if ( this->m_pSkData->isSubscribedToAllPaths() )
                this->m_pSkData->UpdateNMEA2000PathList( path, key );

            this->SendDataToAllPathSubscribers(
                ( key == NULL ? *path : (*path + _T(".") + *key) ),
                ( std::isnan( value ) ? 0.0 : value), L"", timestamp );

        } // then NMEA-2000 delta from Signal K - send to subscribers if any
                
    } // else Signal K

#endif // _TACTICSPI_H_
}

void dashboard_pi::SetPositionFix( PlugIn_Position_Fix &pfix )
{
    if( mPriPosition >= 1 ) {
        mPriPosition = 1;
        SendSentenceToAllInstruments( OCPN_DBP_STC_LAT, pfix.Lat, _T("SDMM") );
        SendSentenceToAllInstruments( OCPN_DBP_STC_LON, pfix.Lon, _T("SDMM") );
    }
    if( mPriCOGSOG >= 1 ) {
        double dMagneticCOG;
        mPriCOGSOG = 1;
        SendSentenceToAllInstruments( OCPN_DBP_STC_SOG, toUsrSpeed_Plugin( mSOGFilter.filter(pfix.Sog), g_iDashSpeedUnit ), getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ) );
        SendSentenceToAllInstruments( OCPN_DBP_STC_COG, mCOGFilter.filter(pfix.Cog), _T("\u00B0") );
        dMagneticCOG = mCOGFilter.get() - pfix.Var;
        if ( dMagneticCOG < 0.0 ) dMagneticCOG = 360.0 + dMagneticCOG;
        if ( dMagneticCOG > 360.0 ) dMagneticCOG = dMagneticCOG - 360.0;
        SendSentenceToAllInstruments( OCPN_DBP_STC_MCOG, dMagneticCOG , _T("\u00B0M") );
    }
    if( mPriVar >= 1 ) {
        if( !std::isnan( pfix.Var ) ){
            mPriVar = 1;
            mVar = pfix.Var;
            mVar_Watchdog = gps_watchdog_timeout_ticks;

            SendSentenceToAllInstruments( OCPN_DBP_STC_HMV, pfix.Var, _T("\u00B0") );
        }
    }
    if( mPriDateTime >= 6 ) { //We prefer the GPS datetime
        mPriDateTime = 6;
        mUTCDateTime.Set( pfix.FixTime );
        mUTCDateTime = mUTCDateTime.ToUTC();
    }
    mSatsInView = pfix.nSats;
    //    SendSentenceToAllInstruments( OCPN_DBP_STC_SAT, mSatsInView, _T("") );

}

void dashboard_pi::SetCursorLatLon( double lat, double lon )
{
#ifdef _TACTICSPI_H_
    this->TacticsSetCursorLatLon(lat, lon);
#endif // _TACTICSPI_H_
    SendSentenceToAllInstruments( OCPN_DBP_STC_PLA, lat, _T("SDMM") );
    SendSentenceToAllInstruments( OCPN_DBP_STC_PLO, lon, _T("SDMM") );

}

void dashboard_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
    if(message_id == _T("WMM_VARIATION_BOAT"))
    {

        // construct the JSON root object
        wxJSONValue  root;
        // construct a JSON parser
        wxJSONReader reader;

        // now read the JSON text and store it in the 'root' structure
        // check for errors before retreiving values...
        int numErrors = reader.Parse( message_body, &root );
        if ( numErrors > 0 )  {
            //              const wxArrayString& errors = reader.GetErrors();
            return;
        }

        // get the DECL value from the JSON message
        wxString decl = root[_T("Decl")].AsString();
        double decl_val;
        decl.ToDouble(&decl_val);


        if( mPriVar >= 4 ) {
            mPriVar = 4;
            mVar = decl_val;
            mVar_Watchdog = gps_watchdog_timeout_ticks;
            SendSentenceToAllInstruments( OCPN_DBP_STC_HMV, mVar, _T("\u00B0") );
        }
    }
}

int dashboard_pi::GetToolbarToolCount( void )
{
    return 1;
}

void dashboard_pi::ShowPreferencesDialog( wxWindow* parent )
{
    DashboardPreferencesDialog *dialog = new DashboardPreferencesDialog( parent, wxID_ANY,
                                                                         m_ArrayOfDashboardWindow
#ifdef _TACTICSPI_H_
                                                                         , GetCommonName(),
                                                                         GetNameVersion(),
                                                                         wxDefaultPosition
#endif // _TACTICSPI_H_
     );

    if( dialog->ShowModal() == wxID_OK ) {
        delete g_pFontTitle;
        g_pFontTitle = new wxFont( dialog->m_pFontPickerTitle->GetSelectedFont() );
        delete g_pFontData;
        g_pFontData = new wxFont( dialog->m_pFontPickerData->GetSelectedFont() );
        delete g_pFontLabel;
        g_pFontLabel = new wxFont( dialog->m_pFontPickerLabel->GetSelectedFont() );
        delete g_pFontSmall;
        g_pFontSmall = new wxFont( dialog->m_pFontPickerSmall->GetSelectedFont() );

        // OnClose should handle that for us normally but it doesn't seems to do so
        // We must save changes first
        dialog->SaveDashboardConfig();
        m_ArrayOfDashboardWindow.Clear();
        m_ArrayOfDashboardWindow = dialog->m_Config;

        ApplyConfig();
        SaveConfig();
        SetToolbarItemState( m_toolbar_item_id, GetDashboardWindowShownCount() != 0 );
    }
    dialog->Destroy();
}

void dashboard_pi::SetColorScheme( PI_ColorScheme cs )
{
#ifdef _TACTICSPI_H_
    m_colorScheme = cs;
#endif // _TACTICSPI_H_
    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindow *dashboard_window =
            m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow;
        if( dashboard_window ) {
            dashboard_window->SetColorScheme( cs );
#ifdef _TACTICSPI_H_
            dashboard_window->SendColorSchemeToAllJSInstruments( cs );
#endif // _TACTICSPI_H_
        }
    }
}

int dashboard_pi::GetDashboardWindowShownCount()
{
    int cnt = 0;

    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindow *dashboard_window = m_ArrayOfDashboardWindow.Item( i )->m_pDashboardWindow;
        if( dashboard_window ) {
            wxAuiPaneInfo &pane = m_pauimgr->GetPane( dashboard_window );
            if( pane.IsOk() && pane.IsShown() ) cnt++;
        }
    }
    return cnt;
}

#ifndef _TACTICSPI_H_
void dashboard_pi::OnPaneClose( wxAuiManagerEvent& event )
{
    // if name is unique, we should use it
    DashboardWindow *dashboard_window = (DashboardWindow *) event.pane->window;
    int cnt = 0;
    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item( i );
        DashboardWindow *d_w = cont->m_pDashboardWindow;
        if( d_w ) {
            // we must not count this one because it is being closed
            if( dashboard_window != d_w ) {
                wxAuiPaneInfo &pane = m_pauimgr->GetPane( d_w );
                if( pane.IsOk() && pane.IsShown() ) cnt++;
            } else {
                cont->m_bIsVisible = false;
            }
        }
    }
    SetToolbarItemState( m_toolbar_item_id, cnt != 0 );

    event.Skip();
}
#endif // (not) _TACTICSPI_H_

void dashboard_pi::OnToolbarToolCallback( int id )
{
    int cnt = GetDashboardWindowShownCount();

    bool b_anyviz = false;
    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item( i );
        if( cont->m_bIsVisible ) {
            b_anyviz = true;
            break;
        }
    }

    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item( i );
        DashboardWindow *dashboard_window = cont->m_pDashboardWindow;
        if( dashboard_window ) {
            wxAuiPaneInfo &pane = m_pauimgr->GetPane( dashboard_window );
            if( pane.IsOk() ) {
                bool b_reset_pos = false;

#ifdef __WXMSW__
                //  Support MultiMonitor setups which an allow negative window positions.
                //  If the requested window title bar does not intersect any installed monitor,
                //  then default to simple primary monitor positioning.
                RECT frame_title_rect;
                frame_title_rect.left = pane.floating_pos.x;
                frame_title_rect.top = pane.floating_pos.y;
                frame_title_rect.right = pane.floating_pos.x + pane.floating_size.x;
                frame_title_rect.bottom = pane.floating_pos.y + 30;

                if( NULL == MonitorFromRect( &frame_title_rect, MONITOR_DEFAULTTONULL ) ) b_reset_pos =
                                                                                              true;
#else

                //    Make sure drag bar (title bar) of window intersects wxClient Area of screen, with a little slop...
                wxRect window_title_rect;// conservative estimate
                window_title_rect.x = pane.floating_pos.x;
                window_title_rect.y = pane.floating_pos.y;
                window_title_rect.width = pane.floating_size.x;
                window_title_rect.height = 30;

                wxRect ClientRect = wxGetClientDisplayRect();
                ClientRect.Deflate(60, 60);// Prevent the new window from being too close to the edge
                if(!ClientRect.Intersects(window_title_rect))
                    b_reset_pos = true;

#endif

                if( b_reset_pos ) pane.FloatingPosition( 50, 50 );

                if( cnt == 0 )
                    if( b_anyviz )
                        pane.Show( cont->m_bIsVisible );
                    else {
                        cont->m_bIsVisible = cont->m_bPersVisible;
                        pane.Show( cont->m_bIsVisible );
                    }
                else
                    pane.Show( false );
            }

            //  This patch fixes a bug in wxAUIManager
            //  FS#548
            // Dropping a DashBoard Window right on top on the (supposedly fixed) chart bar window
            // causes a resize of the chart bar, and the Dashboard window assumes some of its properties
            // The Dashboard window is no longer grabbable...
            // Workaround:  detect this case, and force the pane to be on a different Row.
            // so that the display is corrected by toggling the dashboard off and back on.
            if( ( pane.dock_direction == wxAUI_DOCK_BOTTOM ) && pane.IsDocked() ) pane.Row( 2 );
        }
    }
    // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
    // to actual status to ensure right status upon toolbar rebuild
#ifdef _TACTICSPI_H_
    int iToolbarToolCallbackShownWindows = GetDashboardWindowShownCount();
    ( iToolbarToolCallbackShownWindows != 0 ?
        m_bToggledStateVisible = true : m_bToggledStateVisible = false );
    SetToolbarItemState( m_toolbar_item_id, m_bToggledStateVisible );
    this->SetToggledStateVisible( m_bToggledStateVisible );
#else
    SetToolbarItemState( m_toolbar_item_id, GetDashboardWindowShownCount() != 0/*cnt==0*/);
#endif // _TACTICSPI_H_
    m_pauimgr->Update();
}

#ifdef _TACTICSPI_H_
void dashboard_pi::OnContextMenuItemCallback(int id)
{
    // so far only thes  parent class deals with this event
    this->TacticsOnContextMenuItemCallback(id);
    return;
}
#endif // _TACTICSPI_H_

void dashboard_pi::UpdateAuiStatus( void )
{
    //    This method is called by OpenCPN (pluginmanager.cpp) after the PlugIn is initialized
    //    and the frame has done its initial layout, possibly from a saved wxAuiManager "Perspective"
    //    (see also OCPN_AUIManager.cpp, it knows type "Dashboard" but not "Dashboard_Tactics"). 
    //    It is a chance for the PlugIn to syncronize itself internally with the state of any Panes that
    //    were added to the frame in the PlugIn ctor.

    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item( i );
        wxAuiPaneInfo &pane = m_pauimgr->GetPane( cont->m_pDashboardWindow );
        // Initialize visible state as perspective is loaded now
        cont->m_bIsVisible = ( pane.IsOk() && pane.IsShown() );

    }
    m_pauimgr->Update();

    //    We use this callback here to keep the context menu selection in sync with the window state
#ifdef _TACTICSPI_H_
    int iUpdateAuiShownWindows = GetDashboardWindowShownCount();
    ( iUpdateAuiShownWindows != 0 ?
        m_bToggledStateVisible = true : m_bToggledStateVisible = false );
    SetToolbarItemState( m_toolbar_item_id, m_bToggledStateVisible );
    this->SetToggledStateVisible( m_bToggledStateVisible );
#else
    SetToolbarItemState( m_toolbar_item_id, GetDashboardWindowShownCount() != 0 );
#endif // _TACTICSPI_H_
}

bool dashboard_pi::LoadConfig( void )
{
    wxFileConfig *pConf = (wxFileConfig *) m_pconfig;

    if( pConf ) {

        pConf->SetPath( _T("/PlugIns/Dashboard") );

        wxString version;
        pConf->Read( _T("Version"), &version, wxEmptyString );
        wxString config;
        pConf->Read( _T("FontTitle"), &config, wxEmptyString );
        if( !config.IsEmpty() ) g_pFontTitle->SetNativeFontInfo( config );
        pConf->Read( _T("FontData"), &config, wxEmptyString );
        if( !config.IsEmpty() ) g_pFontData->SetNativeFontInfo( config );
        pConf->Read( _T("FontLabel"), &config, wxEmptyString );
        if( !config.IsEmpty() ) g_pFontLabel->SetNativeFontInfo( config );
        pConf->Read( _T("FontSmall"), &config, wxEmptyString );
        if( !config.IsEmpty() ) g_pFontSmall->SetNativeFontInfo( config );

        pConf->Read( _T("SpeedometerMax"), &g_iDashSpeedMax, 12 );
        pConf->Read( _T("COGDamp"), &g_iDashCOGDamp, 0);
        pConf->Read( _T("SpeedUnit"), &g_iDashSpeedUnit, 0 );
        pConf->Read( _T("SOGDamp"), &g_iDashSOGDamp, 0);
        pConf->Read( _T("DepthUnit"), &g_iDashDepthUnit, 3 );
        g_iDashDepthUnit = wxMax(g_iDashDepthUnit, 3);

        pConf->Read( _T("DepthOffset"), &g_dDashDBTOffset, 0 );

        pConf->Read( _T("DistanceUnit"), &g_iDashDistanceUnit, 0 );
        pConf->Read( _T("WindSpeedUnit"), &g_iDashWindSpeedUnit, 0 );

        pConf->Read( _T("UTCOffset"), &g_iUTCOffset, 0 );
#ifdef _TACTICSPI_H_
        pConf->Read( _T("TemperatureUnit"), &g_iDashTemperatureUnit, 0 );
#endif // _TACTICSPI_H_

        int d_cnt;
        pConf->Read( _T("DashboardCount"), &d_cnt, -1 );
        // TODO: Memory leak? We should destroy everything first
        m_ArrayOfDashboardWindow.Clear();
        if( version.IsEmpty() && d_cnt == -1 ) {
            m_config_version = 1;
            // Let's load version 1 or default settings.
            int i_cnt;
            pConf->Read( _T("InstrumentCount"), &i_cnt, -1 );
            wxArrayInt ar;
#ifdef _TACTICSPI_H_
            wxArrayString idar;
#endif // _TACTICSPI_H_
            if( i_cnt != -1 ) {
                for( int i = 0; i < i_cnt; i++ ) {
                    int id;
#ifdef _TACTICSPI_H_
                    wxString ids;
                    pConf->Read( wxString::Format( _T("Instrument%d"), i + 1 ), &id, -1 );
                    pConf->Read( wxString::Format( _T("InstrumentID%d"), i + 1 ), &ids, _T("") );
                    if( id != -1 ) {
                        ar.Add( id );
                        idar.Add ( ids );
                    }
#else
                    pConf->Read( wxString::Format( _T("Instrument%d"), i + 1 ), &id, -1 );
                    if( id != -1 ) ar.Add( id );
#endif // _TACTICSPI_H_
                }
            } else {
                // This is the default instrument list
#ifdef _TACTICSPI_H_
                ar.Add( ID_DBP_I_POS ); idar.Add( _T("") );
                ar.Add( ID_DBP_D_COG ); idar.Add( _T("") );
                ar.Add( ID_DBP_D_GPS ); idar.Add( _T("") );
#else
                ar.Add( ID_DBP_I_POS );
                ar.Add( ID_DBP_D_COG );
                ar.Add( ID_DBP_D_GPS );
#endif // _TACTICSPI_H_
            }

#ifdef _TACTICSPI_H_
            DashboardWindowContainer *cont = new DashboardWindowContainer( NULL, MakeName(),
                                                                           _("Dashboard_Tactics"),
                                                                           _T("V"), ar, idar );
#else
            DashboardWindowContainer *cont = new DashboardWindowContainer( NULL, MakeName(),
                                                                           _("Dashboard"),
                                                                           _T("V"), ar );
#endif // _TACTICSPI_H_
            cont->m_bPersVisible = true;
            m_ArrayOfDashboardWindow.Add(cont);

        } else {
            // Version 2
            m_config_version = 2;
            bool b_onePersisted = false;
            for( int i = 0; i < d_cnt; i++ ) {
                pConf->SetPath( wxString::Format( _T("/PlugIns/Dashboard/Dashboard%d"), i + 1 ) );
                wxString name;
                pConf->Read( _T("Name"), &name, MakeName() );
                wxString caption;
                pConf->Read( _T("Caption"), &caption,
#ifdef _TACTICSPI_H_
                             _("Dashboard_Tactics") );
#else
                             _("Dashboard") );
#endif // _TACTICSPI_H_
                wxString orient;
                pConf->Read( _T("Orientation"), &orient, _T("V") );
                int i_cnt;
                pConf->Read( _T("InstrumentCount"), &i_cnt, -1 );
                bool b_persist;
                pConf->Read( _T("Persistence"), &b_persist, 1 );

                wxArrayInt ar;
#ifdef _TACTICSPI_H_
                wxArrayString idar;
                for( int j = 0; j < i_cnt; j++ ) {
#else
                for( int i = 0; i < i_cnt; i++ ) {
#endif // _TACTICSPI_H_
                    int id;
#ifdef _TACTICSPI_H_
                    wxString ids;
                    pConf->Read( wxString::Format( _T("Instrument%d"), j + 1 ), &id, -1 );
                    pConf->Read( wxString::Format( _T("InstrumentID%d"), j + 1 ), &ids, _T("") );
                    if( id != -1 ) {
                        ar.Add( id );
                        idar.Add ( ids );
                    }
                }
                // TODO: Do not add if GetCount == 0
                DashboardWindowContainer *cont = new DashboardWindowContainer( NULL, name, caption, orient, ar, idar );
#else
                    pConf->Read( wxString::Format( _T("Instrument%d"), i + 1 ), &id, -1 );
                    if( id != -1 ) ar.Add( id );
                }
                // TODO: Do not add if GetCount == 0
                DashboardWindowContainer *cont = new DashboardWindowContainer( NULL, name, caption, orient, ar );
#endif // _TACTICSPI_H_
                cont->m_bPersVisible = b_persist;

                if(b_persist)
                    b_onePersisted = true;

                m_ArrayOfDashboardWindow.Add(cont);

            }

            // Make sure at least one dashboard is scheduled to be visible
            if( m_ArrayOfDashboardWindow.Count() && !b_onePersisted){
                DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item(0);
                if(cont)
                    cont->m_bPersVisible = true;
            }

        }
        return true;
    } else
        return false;
}

bool dashboard_pi::SaveConfig( void )
{
    wxFileConfig *pConf = (wxFileConfig *) m_pconfig;
    if( pConf ) {
        pConf->SetPath( _T("/PlugIns/Dashboard") );
        pConf->Write( _T("Version"), _T("2") );
        pConf->Write( _T("FontTitle"), g_pFontTitle->GetNativeFontInfoDesc() );
        pConf->Write( _T("FontData"), g_pFontData->GetNativeFontInfoDesc() );
        pConf->Write( _T("FontLabel"), g_pFontLabel->GetNativeFontInfoDesc() );
        pConf->Write( _T("FontSmall"), g_pFontSmall->GetNativeFontInfoDesc() );

        pConf->Write( _T("SpeedometerMax"), g_iDashSpeedMax );
        pConf->Write( _T("COGDamp"), g_iDashCOGDamp );
        pConf->Write( _T("SpeedUnit"), g_iDashSpeedUnit );
        pConf->Write( _T("SOGDamp"), g_iDashSOGDamp );
        pConf->Write( _T("DepthUnit"), g_iDashDepthUnit );
        pConf->Write( _T("DepthOffset"), g_dDashDBTOffset );
        pConf->Write( _T("DistanceUnit"), g_iDashDistanceUnit );
        pConf->Write( _T("WindSpeedUnit"), g_iDashWindSpeedUnit );
#ifdef _TACTICSPI_H_
        pConf->Write( _T("TemperatureUnit"), g_iDashTemperatureUnit );
#endif // _TACTICSPI_H_
        pConf->Write( _T("UTCOffset"), g_iUTCOffset );

        pConf->Write( _T("DashboardCount" ), (int) m_ArrayOfDashboardWindow.GetCount() );
        for( unsigned int i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
            DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item( i );
            pConf->SetPath( wxString::Format( _T("/PlugIns/Dashboard/Dashboard%d"), i + 1 ) );
            pConf->Write( _T("Name"), cont->m_sName );
            pConf->Write( _T("Caption"), cont->m_sCaption );
            pConf->Write( _T("Orientation"), cont->m_sOrientation );
            pConf->Write( _T("Persistence"), cont->m_bPersVisible );

            pConf->Write( _T("InstrumentCount"), (int) cont->m_aInstrumentList.GetCount() );
#ifdef _TACTICSPI_H_
            for( unsigned int j = 0; j < cont->m_aInstrumentList.GetCount(); j++ ) {
                pConf->Write( wxString::Format( _T("Instrument%d"), j + 1 ),
                              cont->m_aInstrumentList.Item( j ) );
                pConf->Write( wxString::Format( _T("InstrumentID%d"), j + 1 ),
                              cont->m_aInstrumentIDs.Item( j ) );
            }
#else
            for( unsigned int j = 0; j < cont->m_aInstrumentList.GetCount(); j++ )
                pConf->Write( wxString::Format( _T("Instrument%d"), j + 1 ),
                              cont->m_aInstrumentList.Item( j ) );
#endif // _TACTICSPI_H_
        }
        return true;
    } else
        return false;
}

void dashboard_pi::ApplyConfig(
#ifdef _TACTICSPI_H_
    bool init
#else
    void
#endif // _TACTICSPI_H_
    )
{
    // Reverse order to handle deletes
    for( size_t i = m_ArrayOfDashboardWindow.GetCount(); i > 0; i-- ) {
        DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item( i - 1 );
        int orient = ( cont->m_sOrientation == _T("V") ? wxVERTICAL : wxHORIZONTAL );
        if( cont->m_bIsDeleted ) {
            if( cont->m_pDashboardWindow ) {
                m_pauimgr->DetachPane( cont->m_pDashboardWindow );
                cont->m_pDashboardWindow->Close();
                cont->m_pDashboardWindow->Destroy();
                cont->m_pDashboardWindow = NULL;
            }
            m_ArrayOfDashboardWindow.Remove( cont );
            delete cont;
        }
#ifdef _TACTICSPI_H_
        else {
            DashboardWindowContainer *newcont = new DashboardWindowContainer( cont );
            /*
              Prepare a new window pane with instruments if first time or if a floating
              replacement is needed
            */
            wxAuiPaneInfo p_cont;
            bool wIsDocked = false;
            if ( newcont->m_pDashboardWindow ) {
                p_cont = m_pauimgr->GetPane( newcont->m_pDashboardWindow );
                if ( p_cont.IsOk() ) {
                    if ( p_cont.IsDocked() ) {
                    wIsDocked = true;
                    } // then window is in a pane which is docked
                } // then a valid pane
            } // then this is non-init (run-time) and there is a window pane
            bool addpane = false;
            bool rebuildpane = false; // either or but not both
            if ( init ) {
                addpane = true;
            } // in the init we need always a new pane, pro-forma
            else {
                if ( cont->m_pDashboardWindow ) {
                    if( !cont->m_pDashboardWindow->isInstrumentListEqual( newcont->m_aInstrumentList ) ) {
                        addpane = true;
                    } // then a change in instruments replacement window is needed, needs a pane
                    else {
                        p_cont = m_pauimgr->GetPane( cont->m_pDashboardWindow );
                        if ( !p_cont.IsOk() ) {
                            addpane = true;
                        } // then there is no pane for this window, create one (with a replacement window)
                        else {
                            if ( wIsDocked ) {
                                if ( !newcont->m_bIsDocked ) {
                                    cont->m_bIsDocked = newcont->m_bIsDocked = true;
                                    rebuildpane = true;
                                } // has been just docked, a rerarrangement is needed (ov50 some cases)
                            } // then there is a pane, unmodified instuments, docked
                            else {
                                if ( newcont->m_bIsDocked ) {
                                    cont->m_bIsDocked = newcont->m_bIsDocked = false;
                                } // from docked to undocked, just register, no need for rearrangement (ov50)
                                int orientNow = cont->m_pDashboardWindow->GetSizerOrientation();
                                if ( (orientNow == wxHORIZONTAL) &&
                                     (newcont->m_sOrientation == _T("V")) ) {
                                    addpane = true;
                                } // then orientation change request to vertical
                                if ( (orientNow == wxVERTICAL) &&
                                     (newcont->m_sOrientation == _T("H")) ) {
                                    addpane = true;
                                } // then orientation change request to horizontal
                            } // else there is a pane, unmodified instruments, floating
                        } // else pane is OK
                    } // else there is no change in the instrument list
                } // then there is an instrument dashboard window
                else {
                    addpane = true;
                } // else there is no instrument window
            } // else not init, study run-time dashboard window

            bool NewDashboardCreated = false;
            if ( addpane ) {
                newcont->m_pDashboardWindow = new DashboardWindow(
                    GetOCPNCanvasWindow(), wxID_ANY,
                    m_pauimgr, this, orient, (init ? cont : newcont),
                    GetCommonName(), this->m_pSkData );
                newcont->m_pDashboardWindow->Show( false );
                newcont->m_pDashboardWindow->SetInstrumentList(
                    newcont->m_aInstrumentList, newcont->m_aInstrumentIDs );
                if ( !init )
                    newcont->m_sName = MakeName();
                NewDashboardCreated = true;
            } // then a pane will be added, create a window for it.
            /*
              Position of the frame, initial or existing.
            */
            bool vertical = true;
            if ( orient == wxHORIZONTAL )
                vertical = false;

            wxPoint position;
            if ( wIsDocked )
                position = wxDefaultPosition;
            else {
                position = m_pluginFrame->GetPosition();
                position.x += 100;
                position.y += 100;
            }
            if ( !init && NewDashboardCreated ) {
                if ( newcont->m_pDashboardWindow ) {
                    if ( p_cont.IsOk() ) {
                        if ( !wIsDocked )
                            position = p_cont.floating_pos;
                    } // then let's study if we can put the window in its original position
                } // then there is a window in this pane
            } // then this is a run-time call
            /*
              The logic for creating a new window pane is as follows
              init: always create a new window pane
              otherwise: create only if change in the contents, orientation, etc.
            */
            wxAuiPaneInfo p;
            if ( addpane ) {
                wxSize sz = newcont->m_pDashboardWindow->GetMinSize();
                // Mac has a little trouble with initial Layout() sizing...
#ifdef __WXOSX__
                if(sz.x == 0)
                    sz.IncTo( wxSize( 160, 388) );
#endif
                p = wxAuiPaneInfo().Name( newcont->m_sName ).Caption( newcont->m_sCaption ).CaptionVisible(
                    false ).TopDockable( !vertical ).BottomDockable( !vertical ).LeftDockable(
                        false ).RightDockable( vertical ).MinSize( sz ).BestSize( sz ).FloatingSize(
                            sz ).FloatingPosition( position ).Float().Show( false ).Gripper(false) ;
            } // then it was necessary to add new pane for init or replacement resizing
            else {
                m_pauimgr->GetPane( newcont->m_pDashboardWindow ).Caption( newcont->m_sCaption ).Show( newcont->m_bIsVisible );
            } // else is non-init run on an existing and unmodified pane, keep it
            if ( addpane && !init ) {
                newcont->m_bPersVisible = cont->m_bIsVisible;
                if ( cont->m_pDashboardWindow ) {
                    m_pauimgr->DetachPane( cont->m_pDashboardWindow );
                    cont->m_pDashboardWindow->Close();
                    cont->m_pDashboardWindow->Destroy();
                } // then this is an existing window in an existing window pane, replaced with a new one
                m_ArrayOfDashboardWindow.Remove( cont );
                m_ArrayOfDashboardWindow.Add( newcont );
                m_pauimgr->AddPane( newcont->m_pDashboardWindow, p, position);
                newcont->m_pDashboardWindow->Show( newcont->m_bIsVisible );
                m_pauimgr->GetPane( newcont->m_pDashboardWindow ).Show( newcont->m_bIsVisible );
                m_pauimgr->Update();
            } // then we have created a pane and it is a replacement of an exiting pane, detach/destroy the old
            else {
                if ( init ) {
                    m_pauimgr->AddPane( newcont->m_pDashboardWindow, p, position);
                    newcont->m_pDashboardWindow->Show( newcont->m_bIsVisible );
                    newcont->m_bPersVisible = newcont->m_bIsVisible;
                    m_pauimgr->GetPane( newcont->m_pDashboardWindow ).Show( newcont->m_bIsVisible );
                    cont->m_pDashboardWindow = newcont->m_pDashboardWindow;
                    if ( wIsDocked ) {
                        cont->m_bIsDocked = true;  // Memo ov50: never comes here in Init() - docked pane is nor recognized as such
                    } // was created as docked, however the container constructor defaults to floating
                    //cont->m_pDashboardWindow->SetMinSizes();
                    m_pauimgr->Update();
                } // then a brand new window, register it
                else {
                    m_pauimgr->GetPane( cont->m_pDashboardWindow ).Show( newcont->m_bIsVisible ).Caption( newcont->m_sCaption );
                    if ( rebuildpane ) {
                        cont->m_pDashboardWindow->RebuildPane(
                            newcont->m_aInstrumentList, newcont->m_aInstrumentIDs );
                        if ( wIsDocked ) {
                            cont->m_bIsDocked = true;
                        } // was docked and rebuilt, however the constructor defaults to floating
                    }
                    m_pauimgr->Update();
                    if ( NewDashboardCreated ) {
                        newcont->m_pDashboardWindow->Close();
                        newcont->m_pDashboardWindow->Destroy();
                        newcont->m_pDashboardWindow = NULL;
                    } // then, just in case, garbage collection
                } // no need to do a replacement or to create a new window
            } // else brand new pane or no action
        } // else not a deleted window, to be created or recreated
#else
        else if( !cont->m_pDashboardWindow ) {
            // A new dashboard is created
            cont->m_pDashboardWindow = new DashboardWindow(
                GetOCPNCanvasWindow(), wxID_ANY,
                m_pauimgr, this, orient, cont, this->m_pSkData
                );
            cont->m_pDashboardWindow->SetInstrumentList( cont->m_aInstrumentList, cont->m_aInstrumentIDs );
            bool vertical = orient == wxVERTICAL;
            wxSize sz = cont->m_pDashboardWindow->GetMinSize();
            // Mac has a little trouble with initial Layout() sizing...
#ifdef __WXOSX__
            if(sz.x == 0)
                sz.IncTo( wxSize( 160, 388) );
#endif
            wxAuiPaneInfo p = wxAuiPaneInfo().Name( cont->m_sName ).Caption( cont->m_sCaption ).CaptionVisible( false ).TopDockable(
                !vertical ).BottomDockable( !vertical ).LeftDockable( vertical ).RightDockable( vertical ).MinSize(
                    sz ).BestSize( sz ).FloatingSize( sz ).FloatingPosition( 100, 100 ).Float().Show( cont->m_bIsVisible ).Gripper(false) ;

            m_pauimgr->AddPane( cont->m_pDashboardWindow, p);
        } else {
            wxAuiPaneInfo& pane = m_pauimgr->GetPane( cont->m_pDashboardWindow );
            pane.Caption( cont->m_sCaption ).Show( cont->m_bIsVisible );
            if( !cont->m_pDashboardWindow->isInstrumentListEqual( cont->m_aInstrumentList ) ) {
                cont->m_pDashboardWindow->SetInstrumentList(
                    cont->m_aInstrumentList, cont->m_aInstrumentIDs );
                wxSize sz = cont->m_pDashboardWindow->GetMinSize();
                pane.MinSize( sz ).BestSize( sz ).FloatingSize( sz );
            }
            if( cont->m_pDashboardWindow->GetSizerOrientation() != orient ) {
                cont->m_pDashboardWindow->ChangePaneOrientation( orient, false );
            }
        }
#endif // _TACTICSPI_H_
    }  // for dashboard window arrays

#ifdef _TACTICSPI_H_
    this->TacticsApplyConfig();
#endif // _TACTICSPI_H_

    m_pauimgr->Update();
    mSOGFilter.setFC(g_iDashSOGDamp ? 1.0 / (2.0*g_iDashSOGDamp) : 0.0);
    mCOGFilter.setFC(g_iDashCOGDamp ? 1.0 / (2.0*g_iDashCOGDamp) : 0.0);
    mCOGFilter.setType(IIRFILTER_TYPE_DEG);
}

void dashboard_pi::PopulateContextMenu( wxMenu* menu )
{
    for( size_t i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item( i );
        wxMenuItem* item = menu->AppendCheckItem( i+1, cont->m_sCaption );
        item->Check( cont->m_bIsVisible );
    }
}

void dashboard_pi::ShowDashboard( size_t id, bool visible )
{
    if ( id < m_ArrayOfDashboardWindow.GetCount() ) {
        DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item( id );
        m_pauimgr->GetPane( cont->m_pDashboardWindow ).Show( visible );
        cont->m_bIsVisible = visible;
        cont->m_bPersVisible = visible;
        m_pauimgr->Update();
    }
}

#ifdef _TACTICSPI_H_
bool dashboard_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
    return this->TacticsRenderOverlay( dc, vp );
}
bool dashboard_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
    return this->TacticsRenderGLOverlay( pcontext, vp );
}
#endif // _TACTICSPI_H_

/* DashboardPreferencesDialog
 *
 */

DashboardPreferencesDialog::DashboardPreferencesDialog(
    wxWindow *parent, wxWindowID id,
    wxArrayOfDashboard config
#ifdef _TACTICSPI_H_
    , wxString commonName, wxString versionName, wxPoint pos ) :
    TacticsPreferencesDialog ( parent, id, commonName + " " + versionName + _(" Preferences"), pos )
#else
     ) :wxDialog( parent, id, _("Dashboard preferences"),
              wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
#endif // _TACTICSPI_H_
{
    Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DashboardPreferencesDialog::OnCloseDialog ),
             NULL, this );

    // Copy original config
    m_Config = wxArrayOfDashboard( config );
    //      Build Dashboard Page for Toolbox
    int border_size = 2;

    wxBoxSizer* itemBoxSizerMainPanel = new wxBoxSizer( wxVERTICAL );
    SetSizer( itemBoxSizerMainPanel );

    wxNotebook *itemNotebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP );

    itemBoxSizerMainPanel->Add( itemNotebook, 1, wxALL | wxEXPAND, border_size );

#ifdef _TACTICSPI_H_
    m_itemNotebook = itemNotebook;
    this->TacticsPreferencesInit( m_itemNotebook, border_size );
#endif // _TACTICSPI_H_

    wxPanel *itemPanelNotebook01 = new wxPanel( itemNotebook, wxID_ANY, wxDefaultPosition,
                                                wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer *itemFlexGridSizer01 = new wxFlexGridSizer( 2 );
    itemFlexGridSizer01->AddGrowableCol( 1 );
    itemPanelNotebook01->SetSizer( itemFlexGridSizer01 );
    itemNotebook->AddPage( itemPanelNotebook01,
#ifdef _TACTICSPI_H_
                           commonName
#else
                           _("Dashboard")
#endif // _TACTICSPI_H_
        );

    wxBoxSizer *itemBoxSizer01 = new wxBoxSizer( wxVERTICAL );
    itemFlexGridSizer01->Add( itemBoxSizer01, 1, wxEXPAND | wxTOP | wxLEFT, border_size );

    wxImageList *imglist1 = new wxImageList( 32, 32, true, 1);
    imglist1->Add(
#ifdef _TACTICSPI_H_
        *_img_dashboard_tactics_pi
#else
        *_img_dashboard_pi
#endif // _TACTICSPI_H_
        );

    m_pListCtrlDashboards = new wxListCtrl(
        itemPanelNotebook01, wxID_ANY, wxDefaultPosition,
        wxSize( 50, 200 ), wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL );
    m_pListCtrlDashboards->AssignImageList( imglist1, wxIMAGE_LIST_SMALL );
    m_pListCtrlDashboards->InsertColumn( 0, _T("") );
    m_pListCtrlDashboards->Connect(
        wxEVT_COMMAND_LIST_ITEM_SELECTED,
        wxListEventHandler(DashboardPreferencesDialog::OnDashboardSelected), NULL, this );
    m_pListCtrlDashboards->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED,
                                    wxListEventHandler(DashboardPreferencesDialog::OnDashboardSelected), NULL, this );
    itemBoxSizer01->Add( m_pListCtrlDashboards, 1, wxEXPAND, 0 );

    wxBoxSizer *itemBoxSizer02 = new wxBoxSizer( wxHORIZONTAL );
    itemBoxSizer01->Add( itemBoxSizer02 );

    m_pButtonAddDashboard = new wxBitmapButton( itemPanelNotebook01, wxID_ANY, *_img_plus,
                                                wxDefaultPosition, wxDefaultSize );
    itemBoxSizer02->Add( m_pButtonAddDashboard, 0, wxALIGN_CENTER, 2 );
    m_pButtonAddDashboard->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                                    wxCommandEventHandler(DashboardPreferencesDialog::OnDashboardAdd), NULL, this );
    m_pButtonDeleteDashboard = new wxBitmapButton( itemPanelNotebook01, wxID_ANY, *_img_minus,
                                                   wxDefaultPosition, wxDefaultSize );
    itemBoxSizer02->Add( m_pButtonDeleteDashboard, 0, wxALIGN_CENTER, 2 );
    m_pButtonDeleteDashboard->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                                       wxCommandEventHandler(DashboardPreferencesDialog::OnDashboardDelete), NULL, this );

    m_pPanelDashboard = new wxPanel( itemPanelNotebook01, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize, wxBORDER_SUNKEN );
    itemFlexGridSizer01->Add( m_pPanelDashboard, 1, wxEXPAND | wxTOP | wxRIGHT, border_size );

    wxBoxSizer* itemBoxSizer03 = new wxBoxSizer( wxVERTICAL );
    m_pPanelDashboard->SetSizer( itemBoxSizer03 );

    wxStaticBox* itemStaticBox02 = new wxStaticBox( m_pPanelDashboard, wxID_ANY,
#ifdef _TACTICSPI_H_
                                                    commonName
#else
                                                    _("Dashboard")
#endif // _TACTICSPI_H_
        );
    wxStaticBoxSizer* itemStaticBoxSizer02 = new wxStaticBoxSizer( itemStaticBox02, wxHORIZONTAL );
    itemBoxSizer03->Add( itemStaticBoxSizer02, 0, wxEXPAND | wxALL, border_size );
    wxFlexGridSizer *itemFlexGridSizer = new wxFlexGridSizer( 2 );
    itemFlexGridSizer->AddGrowableCol( 1 );
    itemStaticBoxSizer02->Add( itemFlexGridSizer, 1, wxEXPAND | wxALL, 0 );

    m_pCheckBoxIsVisible = new wxCheckBox( m_pPanelDashboard, wxID_ANY, _("show this dashboard"),
                                           wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer->Add( m_pCheckBoxIsVisible, 0, wxEXPAND | wxALL, border_size );
    wxStaticText *itemDummy01 = new wxStaticText( m_pPanelDashboard, wxID_ANY, _T("") );
    itemFlexGridSizer->Add( itemDummy01, 0, wxEXPAND | wxALL, border_size );

    wxStaticText* itemStaticText01 = new wxStaticText( m_pPanelDashboard, wxID_ANY, _("Caption:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer->Add( itemStaticText01, 0, wxEXPAND | wxALL, border_size );
    m_pTextCtrlCaption = new wxTextCtrl( m_pPanelDashboard, wxID_ANY, _T(""), wxDefaultPosition,
                                         wxDefaultSize );
    itemFlexGridSizer->Add( m_pTextCtrlCaption, 0, wxEXPAND | wxALL, border_size );

    wxStaticText* itemStaticText02 = new wxStaticText( m_pPanelDashboard, wxID_ANY,
                                                       _("Orientation:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer->Add( itemStaticText02, 0, wxEXPAND | wxALL, border_size );
    m_pChoiceOrientation = new wxChoice( m_pPanelDashboard, wxID_ANY, wxDefaultPosition,
                                         wxSize( 120, -1 ) );
    m_pChoiceOrientation->Append( _("Vertical") );
    m_pChoiceOrientation->Append( _("Horizontal") );
    itemFlexGridSizer->Add( m_pChoiceOrientation, 0, wxALIGN_RIGHT | wxALL, border_size );

    wxImageList *imglist = new wxImageList( 20, 20, true, 2 );
    imglist->Add( *_img_instrument );
    imglist->Add( *_img_dial );

    wxStaticBox* itemStaticBox03 = new wxStaticBox( m_pPanelDashboard, wxID_ANY, _("Instruments") );
    wxStaticBoxSizer* itemStaticBoxSizer03 = new wxStaticBoxSizer( itemStaticBox03, wxHORIZONTAL );
    itemBoxSizer03->Add( itemStaticBoxSizer03, 1, wxEXPAND | wxALL, border_size );

    m_pListCtrlInstruments = new wxListCtrl( m_pPanelDashboard, wxID_ANY, wxDefaultPosition,
                                             wxSize( -1, 200 ), wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL );
    itemStaticBoxSizer03->Add( m_pListCtrlInstruments, 1, wxEXPAND | wxALL, border_size );
    m_pListCtrlInstruments->AssignImageList( imglist, wxIMAGE_LIST_SMALL );
    m_pListCtrlInstruments->InsertColumn( 0, _("Instruments") );
    m_pListCtrlInstruments->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED,
                                     wxListEventHandler(DashboardPreferencesDialog::OnInstrumentSelected), NULL, this );
    m_pListCtrlInstruments->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED,
                                     wxListEventHandler(DashboardPreferencesDialog::OnInstrumentSelected), NULL, this );

    wxBoxSizer* itemBoxSizer04 = new wxBoxSizer( wxVERTICAL );
    itemStaticBoxSizer03->Add( itemBoxSizer04, 0, wxALIGN_TOP | wxALL, border_size );
    m_pButtonAdd = new wxButton( m_pPanelDashboard, wxID_ANY, _("Add"), wxDefaultPosition,
                                 wxSize( 20, -1 ) );
    itemBoxSizer04->Add( m_pButtonAdd, 0, wxEXPAND | wxALL, border_size );
    m_pButtonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                           wxCommandEventHandler(DashboardPreferencesDialog::OnInstrumentAdd), NULL, this );

    /* TODO  Instrument Properties
       m_pButtonEdit = new wxButton( m_pPanelDashboard, wxID_ANY, _("Edit"), wxDefaultPosition,
       wxDefaultSize );
       itemBoxSizer04->Add( m_pButtonEdit, 0, wxEXPAND | wxALL, border_size );
       m_pButtonEdit->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
       wxCommandEventHandler(DashboardPreferencesDialog::OnInstrumentEdit), NULL, this );
    */
    m_pButtonDelete = new wxButton( m_pPanelDashboard, wxID_ANY, _("Delete"), wxDefaultPosition,
                                    wxSize( 20, -1 ) );
    itemBoxSizer04->Add( m_pButtonDelete, 0, wxEXPAND | wxALL, border_size );
    m_pButtonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                              wxCommandEventHandler(DashboardPreferencesDialog::OnInstrumentDelete), NULL, this );
    itemBoxSizer04->AddSpacer( 10 );
    m_pButtonUp = new wxButton( m_pPanelDashboard, wxID_ANY, _("Up"), wxDefaultPosition,
                                wxDefaultSize );
    itemBoxSizer04->Add( m_pButtonUp, 0, wxEXPAND | wxALL, border_size );
    m_pButtonUp->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                          wxCommandEventHandler(DashboardPreferencesDialog::OnInstrumentUp), NULL, this );
    m_pButtonDown = new wxButton( m_pPanelDashboard, wxID_ANY, _("Down"), wxDefaultPosition,
                                  wxDefaultSize );
    itemBoxSizer04->Add( m_pButtonDown, 0, wxEXPAND | wxALL, border_size );
    m_pButtonDown->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                            wxCommandEventHandler(DashboardPreferencesDialog::OnInstrumentDown), NULL, this );

    wxScrolledWindow *itemPanelNotebook02 = new wxScrolledWindow(
        itemNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxVSCROLL );
    int scrollRate = 5;
#ifdef __OCPN__ANDROID__
    scrollRate = 1;
#endif
    itemPanelNotebook02->SetScrollRate(0, scrollRate);

    wxBoxSizer* itemBoxSizer05 = new wxBoxSizer( wxVERTICAL );
    itemPanelNotebook02->SetSizer( itemBoxSizer05 );
    itemNotebook->AddPage( itemPanelNotebook02, _("Appearance") );

    wxStaticBox* itemStaticBox01 = new wxStaticBox( itemPanelNotebook02, wxID_ANY, _("Fonts") );
    wxStaticBoxSizer* itemStaticBoxSizer01 = new wxStaticBoxSizer( itemStaticBox01, wxHORIZONTAL );
    itemBoxSizer05->Add( itemStaticBoxSizer01, 0, wxEXPAND | wxALL, border_size );
    wxFlexGridSizer *itemFlexGridSizer03 = new wxFlexGridSizer( 2 );
    itemFlexGridSizer03->AddGrowableCol( 1 );
    itemStaticBoxSizer01->Add( itemFlexGridSizer03, 1, wxEXPAND | wxALL, 0 );
    wxStaticText* itemStaticText04 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Title:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer03->Add( itemStaticText04, 0, wxEXPAND | wxALL, border_size );
    m_pFontPickerTitle = new wxFontPickerCtrl( itemPanelNotebook02, wxID_ANY, *g_pFontTitle,
                                               wxDefaultPosition, wxDefaultSize );
    itemFlexGridSizer03->Add( m_pFontPickerTitle, 0, wxALIGN_RIGHT | wxALL, 0 );
    wxStaticText* itemStaticText05 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Data:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer03->Add( itemStaticText05, 0, wxEXPAND | wxALL, border_size );
    m_pFontPickerData = new wxFontPickerCtrl( itemPanelNotebook02, wxID_ANY, *g_pFontData,
                                              wxDefaultPosition, wxDefaultSize );
    itemFlexGridSizer03->Add( m_pFontPickerData, 0, wxALIGN_RIGHT | wxALL, 0 );
    wxStaticText* itemStaticText06 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Label:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer03->Add( itemStaticText06, 0, wxEXPAND | wxALL, border_size );
    m_pFontPickerLabel = new wxFontPickerCtrl( itemPanelNotebook02, wxID_ANY, *g_pFontLabel,
                                               wxDefaultPosition, wxDefaultSize );
    itemFlexGridSizer03->Add( m_pFontPickerLabel, 0, wxALIGN_RIGHT | wxALL, 0 );
    wxStaticText* itemStaticText07 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Small:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer03->Add( itemStaticText07, 0, wxEXPAND | wxALL, border_size );
    m_pFontPickerSmall = new wxFontPickerCtrl( itemPanelNotebook02, wxID_ANY, *g_pFontSmall,
                                               wxDefaultPosition, wxDefaultSize );
    itemFlexGridSizer03->Add( m_pFontPickerSmall, 0, wxALIGN_RIGHT | wxALL, 0 );
    //      wxColourPickerCtrl

    wxStaticBox* itemStaticBox04 = new wxStaticBox( itemPanelNotebook02, wxID_ANY, _("Units, Ranges, Formats") );
    wxStaticBoxSizer* itemStaticBoxSizer04 = new wxStaticBoxSizer( itemStaticBox04, wxHORIZONTAL );
    itemBoxSizer05->Add( itemStaticBoxSizer04, 0, wxEXPAND | wxALL, border_size );
    wxFlexGridSizer *itemFlexGridSizer04 = new wxFlexGridSizer( 2 );
    itemFlexGridSizer04->AddGrowableCol( 1 );
    itemStaticBoxSizer04->Add( itemFlexGridSizer04, 1, wxEXPAND | wxALL, 0 );
    wxStaticText* itemStaticText08 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Speedometer max value:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText08, 0, wxEXPAND | wxALL, border_size );
    m_pSpinSpeedMax = new wxSpinCtrl( itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 10, 100, g_iDashSpeedMax );
    itemFlexGridSizer04->Add( m_pSpinSpeedMax, 0, wxALIGN_RIGHT | wxALL, 0 );

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Speed Over Ground Damping Factor:"),
                                                       wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer04->Add(itemStaticText10, 0, wxEXPAND | wxALL, border_size);
    m_pSpinSOGDamp = new wxSpinCtrl(itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, g_iDashSOGDamp);
    itemFlexGridSizer04->Add(m_pSpinSOGDamp, 0, wxALIGN_RIGHT | wxALL, 0);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("COG Damping Factor:"),
                                                       wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer04->Add(itemStaticText11, 0, wxEXPAND | wxALL, border_size);
    m_pSpinCOGDamp = new wxSpinCtrl(itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, g_iDashCOGDamp);
    itemFlexGridSizer04->Add(m_pSpinCOGDamp, 0, wxALIGN_RIGHT | wxALL, 0);

    wxStaticText* itemStaticText12 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _( "Local Time Offset From UTC:" ),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText12, 0, wxEXPAND | wxALL, border_size );
    wxString m_UTCOffsetChoices[] = {
        _T( "-12:00" ), _T( "-11:30" ), _T( "-11:00" ), _T( "-10:30" ), _T( "-10:00" ), _T( "-09:30" ),
        _T( "-09:00" ), _T( "-08:30" ), _T( "-08:00" ), _T( "-07:30" ), _T( "-07:00" ), _T( "-06:30" ),
        _T( "-06:00" ), _T( "-05:30" ), _T( "-05:00" ), _T( "-04:30" ), _T( "-04:00" ), _T( "-03:30" ),
        _T( "-03:00" ), _T( "-02:30" ), _T( "-02:00" ), _T( "-01:30" ), _T( "-01:00" ), _T( "-00:30" ),
        _T( " 00:00" ), _T( " 00:30" ), _T( " 01:00" ), _T( " 01:30" ), _T( " 02:00" ), _T( " 02:30" ),
        _T( " 03:00" ), _T( " 03:30" ), _T( " 04:00" ), _T( " 04:30" ), _T( " 05:00" ), _T( " 05:30" ),
        _T( " 06:00" ), _T( " 06:30" ), _T( " 07:00" ), _T( " 07:30" ), _T( " 08:00" ), _T( " 08:30" ),
        _T( " 09:00" ), _T( " 09:30" ), _T( " 10:00" ), _T( " 10:30" ), _T( " 11:00" ), _T( " 11:30" ),
        _T( " 12:00" )
    };
    int m_UTCOffsetNChoices = sizeof( m_UTCOffsetChoices ) / sizeof( wxString );
    m_pChoiceUTCOffset = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_UTCOffsetNChoices, m_UTCOffsetChoices, 0 );
    m_pChoiceUTCOffset->SetSelection( g_iUTCOffset + 24 );
    itemFlexGridSizer04->Add( m_pChoiceUTCOffset, 0, wxALIGN_RIGHT | wxALL, 0 );

    wxStaticText* itemStaticText09 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Boat speed units:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText09, 0, wxEXPAND | wxALL, border_size );
    wxString m_SpeedUnitChoices[] = { _("Honor OpenCPN settings"), _("Kts"), _("mph"), _("km/h"), _("m/s") };
    int m_SpeedUnitNChoices = sizeof( m_SpeedUnitChoices ) / sizeof( wxString );
    m_pChoiceSpeedUnit = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_SpeedUnitNChoices, m_SpeedUnitChoices, 0 );
    m_pChoiceSpeedUnit->SetSelection( g_iDashSpeedUnit + 1 );
    itemFlexGridSizer04->Add( m_pChoiceSpeedUnit, 0, wxALIGN_RIGHT | wxALL, 0 );

    wxStaticText* itemStaticTextDepthU = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Depth units:"),
                                                           wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticTextDepthU, 0, wxEXPAND | wxALL, border_size );
    wxString m_DepthUnitChoices[] = { _("Meters"), _("Feet"), _("Fathoms"), _("Inches"), _("Centimeters") };
    int m_DepthUnitNChoices = sizeof( m_DepthUnitChoices ) / sizeof( wxString );
    m_pChoiceDepthUnit = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DepthUnitNChoices, m_DepthUnitChoices, 0 );
    m_pChoiceDepthUnit->SetSelection( g_iDashDepthUnit - 3);
    itemFlexGridSizer04->Add( m_pChoiceDepthUnit, 0, wxALIGN_RIGHT | wxALL, 0 );
    wxString dMess = wxString::Format(_("Depth Offset (%s):"),m_DepthUnitChoices[g_iDashDepthUnit-3]);
    wxStaticText* itemStaticDepthO = new wxStaticText(itemPanelNotebook02, wxID_ANY, dMess,
                                                      wxDefaultPosition, wxDefaultSize, 0);
    double DepthOffset;
    switch (g_iDashDepthUnit - 3) {
    case 1:
        DepthOffset = g_dDashDBTOffset * 3.2808399;
        break;
    case 2:
        DepthOffset = g_dDashDBTOffset * 0.54680665;
        break;
    case 3:
        DepthOffset = g_dDashDBTOffset * 39.3700787;
        break;
    case 4:
        DepthOffset = g_dDashDBTOffset * 100;
        break;
    default:
        DepthOffset = g_dDashDBTOffset;
    }
    itemFlexGridSizer04->Add(itemStaticDepthO, 0, wxEXPAND | wxALL, border_size);
    m_pSpinDBTOffset = new wxSpinCtrlDouble(itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100, 100, DepthOffset, 0.1);
    itemFlexGridSizer04->Add(m_pSpinDBTOffset, 0, wxALIGN_RIGHT | wxALL, 0);

    wxStaticText* itemStaticText0b = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Distance units:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText0b, 0, wxEXPAND | wxALL, border_size );
    wxString m_DistanceUnitChoices[] = { _("Honor OpenCPN settings"), _("Nautical miles"), _("Statute miles"), _("Kilometers"), _("Meters") };
    int m_DistanceUnitNChoices = sizeof( m_DistanceUnitChoices ) / sizeof( wxString );
    m_pChoiceDistanceUnit = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DistanceUnitNChoices, m_DistanceUnitChoices, 0 );
    m_pChoiceDistanceUnit->SetSelection( g_iDashDistanceUnit + 1 );
    itemFlexGridSizer04->Add( m_pChoiceDistanceUnit, 0, wxALIGN_RIGHT | wxALL, 0 );

    wxStaticText* itemStaticText0a = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Wind speed units:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText0a, 0, wxEXPAND | wxALL, border_size );
    wxString m_WSpeedUnitChoices[] = { _("Kts"), _("mph"), _("km/h"), _("m/s") };
    int m_WSpeedUnitNChoices = sizeof( m_WSpeedUnitChoices ) / sizeof( wxString );
    m_pChoiceWindSpeedUnit = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_WSpeedUnitNChoices, m_WSpeedUnitChoices, 0 );
    m_pChoiceWindSpeedUnit->SetSelection( g_iDashWindSpeedUnit );
    itemFlexGridSizer04->Add( m_pChoiceWindSpeedUnit, 0, wxALIGN_RIGHT | wxALL, 0 );

#ifdef _TACTICSPI_H_
    wxStaticText* itemStaticText0c = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Temperature units:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText0c, 0, wxEXPAND | wxALL, border_size );
    wxString m_TempUnitChoices[] = { _("Celsius"), _("Fahrenheit") };
    int m_TempUnitNChoices = sizeof( m_TempUnitChoices ) / sizeof( wxString );
    m_pChoiceTemperatureUnit = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_TempUnitNChoices, m_TempUnitChoices, 0 );
    m_pChoiceTemperatureUnit->SetSelection( g_iDashTemperatureUnit );
    itemFlexGridSizer04->Add( m_pChoiceTemperatureUnit, 0, wxALIGN_RIGHT | wxALL, 0 );
#endif // _TACTICSPI_H_

    //////////////////////////////////////////////////////////////
#ifdef _TACTICSPI_H_
    this->TacticsPreferencesPanel();
#endif // _TACTICSPI_H_

    wxStdDialogButtonSizer* DialogButtonSizer = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
    itemBoxSizerMainPanel->Add( DialogButtonSizer, 0, wxALIGN_RIGHT | wxALL, 5 );

    curSel = -1;
    for( size_t i = 0; i < m_Config.GetCount(); i++ ) {
        m_pListCtrlDashboards->InsertItem( i, 0 );
        // Using data to store m_Config index for managing deletes
        m_pListCtrlDashboards->SetItemData( i, i );
    }
    m_pListCtrlDashboards->SetColumnWidth( 0, wxLIST_AUTOSIZE );

    UpdateDashboardButtonsState();
    UpdateButtonsState();
    SetMinSize( wxSize( 450, -1 ) );
    Fit();
    if ( pos == wxDefaultPosition )
        Center();
}

void DashboardPreferencesDialog::OnCloseDialog( wxCloseEvent& event )
{
    SaveDashboardConfig();
    event.Skip();
}

void DashboardPreferencesDialog::SaveDashboardConfig()
{
    g_iDashSpeedMax = m_pSpinSpeedMax->GetValue();
    g_iDashCOGDamp = m_pSpinCOGDamp->GetValue();
    g_iDashSOGDamp = m_pSpinSOGDamp->GetValue();
    g_iUTCOffset = m_pChoiceUTCOffset->GetSelection() - 24;
    g_iDashSpeedUnit = m_pChoiceSpeedUnit->GetSelection() - 1;
    double DashDBTOffset = m_pSpinDBTOffset->GetValue();
    switch (g_iDashDepthUnit - 3) {
    case 1:
        g_dDashDBTOffset = DashDBTOffset / 3.2808399;
        break;
    case 2:
        g_dDashDBTOffset = DashDBTOffset / 0.54680665;
        break;
    case 3:
        g_dDashDBTOffset = DashDBTOffset / 39.3700787;
        break;
    case 4:
        g_dDashDBTOffset = DashDBTOffset / 100;
        break;
    default:
        g_dDashDBTOffset = DashDBTOffset;
    }
    g_iDashDepthUnit = m_pChoiceDepthUnit->GetSelection() + 3;
    g_iDashDistanceUnit = m_pChoiceDistanceUnit->GetSelection() - 1;
    g_iDashWindSpeedUnit = m_pChoiceWindSpeedUnit->GetSelection();
#ifdef _TACTICSPI_H_
    g_iDashTemperatureUnit = m_pChoiceTemperatureUnit->GetSelection();
#endif // _TACTICSPI_H_

#ifdef _TACTICSPI_H_
    this->SaveTacticsConfig();
#endif // _TACTICSPI_H_

    if( curSel != -1 ) {
        DashboardWindowContainer *cont = m_Config.Item( curSel );
        cont->m_bIsVisible = m_pCheckBoxIsVisible->IsChecked();
        cont->m_bPersVisible = cont->m_bIsVisible;
        cont->m_sCaption = m_pTextCtrlCaption->GetValue();
        cont->m_sOrientation =
            m_pChoiceOrientation->GetSelection() ==
            0 ? _T("V") : _T("H");
#ifdef _TACTICSPI_H_
        DashboardWindowContainer *oldcont = new DashboardWindowContainer( cont );
#endif // _TACTICSPI_H_
        cont->m_aInstrumentList.Clear();
#ifdef _TACTICSPI_H_
        cont->m_aInstrumentIDs.Clear();
        int id, j, oldmax;
        bool idMatch;
        for( int i = 0; i < m_pListCtrlInstruments->GetItemCount(); i++ ) {
            id = (int) m_pListCtrlInstruments->GetItemData( i );
            j = 0;
            oldmax = oldcont->m_aInstrumentIDs.GetCount();
            idMatch = false;
            while ( !idMatch && (j < oldmax) ) {
                if ( id == oldcont->m_aInstrumentList.Item( j ) )
                    idMatch = true;
                else
                    j++;
            } // while searching IDs from the old instrument container
            wxString ids = _T("");
            if ( idMatch ) {
                ids = oldcont->m_aInstrumentIDs.Item( j );
                oldcont->m_aInstrumentIDs.RemoveAt( j );
                oldcont->m_aInstrumentList.RemoveAt( j );
            }
            cont->m_aInstrumentList.Add( id );
            cont->m_aInstrumentIDs.Add( ids );
        } // for number of selected instruments
#else
        for( int i = 0; i < m_pListCtrlInstruments->GetItemCount(); i++ )
            cont->m_aInstrumentList.Add(
                (int) m_pListCtrlInstruments->GetItemData( i ) );
#endif // _TACTICSPI_H_
    }
}

void DashboardPreferencesDialog::OnDashboardSelected( wxListEvent& event )
{
    // save changes
    SaveDashboardConfig();
    UpdateDashboardButtonsState();
}

void DashboardPreferencesDialog::UpdateDashboardButtonsState()
{
    long item = -1;
    item = m_pListCtrlDashboards->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    bool enable = ( item != -1 );

    //  Disable the Dashboard Delete button if the parent(Dashboard) of this dialog is selected.
    bool delete_enable = enable;
    if( item != -1 ) {
#ifdef _TACTICSPI_H_
        /*
          In this implemenation the dialog parent is the plugin, not any particular dashboard window
          so that we can destroy even the window from which we the dialog was started from.
          However, let's follow the principle to always leave at least one window to be consistent
          with the Dashboard-only code. It is also practical, and less confusing!
        */
        int NumberOfVisible = 0;
        int NumberOfItemsLeft = (int) m_pListCtrlDashboards->GetItemCount();
        if ( NumberOfItemsLeft <= 1)
            delete_enable = false;
        else {
            for ( int i = 0; i < NumberOfItemsLeft; i++ ) {
                if ( m_Config.Item( i )->m_bIsVisible )
                    NumberOfVisible++;
            } // For items available to delete
            if ( NumberOfVisible <= 1 )
                delete_enable = false;
        }
#else
        int sel = m_pListCtrlDashboards->GetItemData( item );
        DashboardWindowContainer *cont = m_Config.Item( sel );
        DashboardWindow *dash_sel = cont->m_pDashboardWindow;
        if(dash_sel == GetParent())
            delete_enable = false;
#endif // _TACTICSPI_H_
    }
    m_pButtonDeleteDashboard->Enable( delete_enable );

    m_pPanelDashboard->Enable( enable );

    if( item != -1 ) {
        curSel = m_pListCtrlDashboards->GetItemData( item );
        DashboardWindowContainer *cont = m_Config.Item( curSel );
        m_pCheckBoxIsVisible->SetValue( cont->m_bIsVisible );
        m_pTextCtrlCaption->SetValue( cont->m_sCaption );
        m_pChoiceOrientation->SetSelection( cont->m_sOrientation == _T("V") ? 0 : 1 );
        m_pListCtrlInstruments->DeleteAllItems();
        for( size_t i = 0; i < cont->m_aInstrumentList.GetCount(); i++ ) {
            wxListItem item;
            if (getListItemForInstrument(
                    item, cont->m_aInstrumentList.Item( i ) ) ) {
                item.SetId( m_pListCtrlInstruments->GetItemCount() );
                m_pListCtrlInstruments->InsertItem( item );
            }
        }

        m_pListCtrlInstruments->SetColumnWidth( 0, wxLIST_AUTOSIZE );
    } else {
        curSel = -1;
        m_pCheckBoxIsVisible->SetValue( false );
        m_pTextCtrlCaption->SetValue( _T("") );
        m_pChoiceOrientation->SetSelection( 0 );
        m_pListCtrlInstruments->DeleteAllItems();
    }
    //      UpdateButtonsState();
}

void DashboardPreferencesDialog::OnDashboardAdd( wxCommandEvent& event )
{
    int idx = m_pListCtrlDashboards->GetItemCount();
    m_pListCtrlDashboards->InsertItem( idx, 0 );
    // Data is index in m_Config
    m_pListCtrlDashboards->SetItemData( idx, m_Config.GetCount() );
    wxArrayInt ar;
#ifdef _TACTICSPI_H_
    wxArrayString idar;
    DashboardWindowContainer *dwc = new DashboardWindowContainer( NULL, MakeName(),
                                                                  _("Dashboard_Tactics"),
                                                                  _T("V"), ar, idar );
#else
    DashboardWindowContainer *dwc = new DashboardWindowContainer( NULL, MakeName(),
                                                                  _("Dashboard"),
                                                                  _T("V"), ar );
#endif // _TACTICSPI_H_
    dwc->m_bIsVisible = true;
    dwc->m_bPersVisible = true;
    m_Config.Add( dwc );
}

void DashboardPreferencesDialog::OnDashboardDelete( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_pListCtrlDashboards->GetNextItem( itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    int idx = m_pListCtrlDashboards->GetItemData( itemID );
    m_pListCtrlDashboards->DeleteItem( itemID );
    m_Config.Item( idx )->m_bIsDeleted = true;
    UpdateDashboardButtonsState();
}

void DashboardPreferencesDialog::OnInstrumentSelected( wxListEvent& event )
{
    UpdateButtonsState();
}

void DashboardPreferencesDialog::UpdateButtonsState()
{
    long item = -1;
    item = m_pListCtrlInstruments->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    bool enable = ( item != -1 );

    m_pButtonDelete->Enable( enable );
    //    m_pButtonEdit->Enable( false ); // TODO: Properties
    m_pButtonUp->Enable( item > 0 );
    m_pButtonDown->Enable( item != -1 && item < m_pListCtrlInstruments->GetItemCount() - 1 );
}

void DashboardPreferencesDialog::OnInstrumentAdd( wxCommandEvent& event )
{
    AddInstrumentDlg pdlg( (wxWindow *) event.GetEventObject(), wxID_ANY );

    if( pdlg.ShowModal() == wxID_OK ) {
        wxListItem item;
        (void) getListItemForInstrument( item, pdlg.GetInstrumentAdded() );
        item.SetId( m_pListCtrlInstruments->GetItemCount() );
        m_pListCtrlInstruments->InsertItem( item );
        m_pListCtrlInstruments->SetColumnWidth( 0, wxLIST_AUTOSIZE );
        UpdateButtonsState();
    }
}

void DashboardPreferencesDialog::OnInstrumentDelete( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_pListCtrlInstruments->GetNextItem( itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    m_pListCtrlInstruments->DeleteItem( itemID );
    UpdateButtonsState();
}

void DashboardPreferencesDialog::OnInstrumentEdit( wxCommandEvent& event )
{
    // TODO: Instument options
}

void DashboardPreferencesDialog::OnInstrumentUp( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_pListCtrlInstruments->GetNextItem( itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    wxListItem item;
    item.SetId( itemID );
    item.SetMask( wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE | wxLIST_MASK_DATA );
    m_pListCtrlInstruments->GetItem( item );
    item.SetId( itemID - 1 );
    m_pListCtrlInstruments->DeleteItem( itemID );
    m_pListCtrlInstruments->InsertItem( item );
    m_pListCtrlInstruments->SetItemState( itemID - 1, wxLIST_STATE_SELECTED,
                                          wxLIST_STATE_SELECTED );
    UpdateButtonsState();
}

void DashboardPreferencesDialog::OnInstrumentDown( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_pListCtrlInstruments->GetNextItem( itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    wxListItem item;
    item.SetId( itemID );
    item.SetMask( wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE | wxLIST_MASK_DATA );
    m_pListCtrlInstruments->GetItem( item );
    item.SetId( itemID + 1 );
    m_pListCtrlInstruments->DeleteItem( itemID );
    m_pListCtrlInstruments->InsertItem( item );
    m_pListCtrlInstruments->SetItemState( itemID + 1, wxLIST_STATE_SELECTED,
                                          wxLIST_STATE_SELECTED );
    UpdateButtonsState();
}

//----------------------------------------------------------------
//
//    Add Instrument Dialog Implementation
//
//----------------------------------------------------------------
#ifdef _TACTICSPI_H_
/* Provide a callback method to tweak the alphabetical ascending order of the instrument list(s)
   so that tactics and performance instruments are listed at the end, after the dashbaord std.
   instruments. */
int wxCALLBACK InstrumentListSortCallback (wxIntPtr item1, wxIntPtr item2, wxIntPtr WXUNUSED(sortData))
{
    if ( IsTacticsInstrument( item1 ) && !IsTacticsInstrument( item2 ) ) {
        return 1;
    } // first instrument is Tactics and second is Dashboard, Dashboard instruments first
    else {
        if ( !IsTacticsInstrument( item1 ) && IsTacticsInstrument( item2 ) ) {
            return -1;
        } // first instrument is Dashboard and second is Tactics, keep that way
    } // else check the other way around
    // Both are the same, either Tactics or Dashboard, let the alphabeting order prevail
    const wxString capt1 = getInstrumentCaption (item1);
    return capt1.CmpNoCase ( getInstrumentCaption (item2) );
}
#endif // _TACTICSPI_H

AddInstrumentDlg::AddInstrumentDlg( wxWindow *pparent, wxWindowID id ) :
    wxDialog( pparent, id, _("Add instrument"), wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_DIALOG_STYLE )
{
    wxBoxSizer* itemBoxSizer01 = new wxBoxSizer( wxVERTICAL );
    SetSizer( itemBoxSizer01 );
    wxStaticText* itemStaticText01 = new wxStaticText( this, wxID_ANY,
                                                       _("Select instrument to add:")
#ifdef _TACTICSPI_H_
                                                       + _(L"\n(\u2191Tactics at the end of the list)")
#endif // _TACTICSPI_H_
                                                       , wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer01->Add( itemStaticText01, 0, wxEXPAND | wxALL, 5 );

    wxImageList *imglist = new wxImageList( 20, 20, true, 2 );
    imglist->Add( *_img_instrument );
    imglist->Add( *_img_dial );

    m_pListCtrlInstruments = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( 250, 180 ),
                                             wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING );
    itemBoxSizer01->Add( m_pListCtrlInstruments, 0, wxEXPAND | wxALL, 5 );
    m_pListCtrlInstruments->AssignImageList( imglist, wxIMAGE_LIST_SMALL );
    m_pListCtrlInstruments->InsertColumn( 0, _("Instruments") );

    wxStdDialogButtonSizer* DialogButtonSizer = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
    itemBoxSizer01->Add( DialogButtonSizer, 0, wxALIGN_RIGHT | wxALL, 5 );

    for( unsigned int i = ID_DBP_I_POS; i < ID_DBP_LAST_ENTRY; i++ ) { //do not reference an instrument, but the last dummy entry in the list
        wxListItem item;
        if( IsObsolete( i ) ) continue;
        if ( getListItemForInstrument( item, i ) ) {
            item.SetId( i );
            m_pListCtrlInstruments->InsertItem( item );
        }
    }
#ifdef _TACTICSPI_H_
    // Provide a dual sorting callback so that Tactics and Dashboard instruments are kept apart.
    m_pListCtrlInstruments->SortItems( InstrumentListSortCallback, 0);
#endif // _TACTICSPI_H_
    m_pListCtrlInstruments->SetColumnWidth( 0, wxLIST_AUTOSIZE );
    m_pListCtrlInstruments->SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    Fit();
}

unsigned int AddInstrumentDlg::GetInstrumentAdded()
{
    long itemID = -1;
    itemID = m_pListCtrlInstruments->GetNextItem( itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    return (int) m_pListCtrlInstruments->GetItemData( itemID );
}

//----------------------------------------------------------------
//
//    Dashboard Window Implementation
//
//----------------------------------------------------------------
wxBEGIN_EVENT_TABLE (DashboardWindow, TacticsWindow)
   EVT_CLOSE (DashboardWindow::OnClose)
wxEND_EVENT_TABLE ()

DashboardWindow::DashboardWindow(
    wxWindow *pparent, wxWindowID id, wxAuiManager *auimgr,
    dashboard_pi *plugin, int orient,
    DashboardWindowContainer *mycont, wxString commonName, SkData *pSkData ) :
    TacticsWindow ( pparent, id, (tactics_pi *) plugin, commonName, pSkData )
    // please see wxWindow contructor parameters, defined by TacticsWindow class implementation
{
    m_pauimgr = auimgr;
    m_plugin = plugin;
    m_Container = mycont;

    //wx2.9      itemBoxSizer = new wxWrapSizer( orient );
    itemBoxSizer = new wxBoxSizer( orient );
#ifdef _TACTICSPI_H_
    SetSizerAndFit( itemBoxSizer );
#else
    SetSizer( itemBoxSizer );
#endif // _TACTICSPI_H_
#ifdef _TACTICSPI_H_
    // Dynamic binding used for event handlers
    Bind( wxEVT_SIZE, &DashboardWindow::OnSize, this );
    Bind( wxEVT_CONTEXT_MENU, &DashboardWindow::OnContextMenu, this );
    Bind( wxEVT_COMMAND_MENU_SELECTED, &DashboardWindow::OnContextMenuSelect, this);
#else
    Connect( wxEVT_SIZE, wxSizeEventHandler( DashboardWindow::OnSize ), NULL, this );
    Connect( wxEVT_CONTEXT_MENU, wxContextMenuEventHandler( DashboardWindow::OnContextMenu ), NULL,
             this );
    Connect( wxEVT_COMMAND_MENU_SELECTED,
             wxCommandEventHandler( DashboardWindow::OnContextMenuSelect ), NULL, this );
#endif // _TACTICSPI_H_
}

DashboardWindow::~DashboardWindow()
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
        DashboardInstrumentContainer *pdic = m_ArrayOfInstrument.Item( i );
        if ( pdic->m_pInstrument != NULL ) {
            delete pdic;
        }
    }
}

#ifdef _TACTICSPI_H_
void DashboardWindow::OnClose( wxCloseEvent &event )
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
        DashboardInstrumentContainer *pdic = m_ArrayOfInstrument.Item( i );
        if ( pdic->m_pInstrument != NULL ) {
            pdic->m_pInstrument->Close();
        }
    }
    event.Skip(); // Destroy() must be called
}

void DashboardWindow::RebuildPane( wxArrayInt list, wxArrayString listIDs )
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
        DashboardInstrumentContainer *pdic = m_ArrayOfInstrument.Item( i );
        if ( pdic->m_pInstrument != NULL ) {
            pdic->m_pInstrument->Close();
            delete pdic;
        }
    }
    SetInstrumentList( list, listIDs );
}

#endif // _TACTICSPI_H_

void DashboardWindow::SetMinSizes( )
{
    for( unsigned int i=0; i<m_ArrayOfInstrument.size(); i++ ) {
        DashboardInstrument* inst = m_ArrayOfInstrument.Item(i)->m_pInstrument;
        wxSize instMinSize = inst->GetSize(
            itemBoxSizer->GetOrientation(), GetClientSize() );
        inst->SetMinSize( instMinSize );
    }
    Layout();
    Refresh();
}

void DashboardWindow::OnSize( wxSizeEvent &event )
{
    event.Skip();
    SetMinSizes();
}

void DashboardWindow::OnContextMenu( wxContextMenuEvent &event )
{
    wxMenu* contextMenu = new wxMenu();

    wxAuiPaneInfo &pane = m_pauimgr->GetPane( this );
    if ( pane.IsOk() ) {
        if ( pane.IsDocked() ) {
            contextMenu->Append( ID_DASH_UNDOCK, _( "Undock" ) );
        } // then docked
        else {
            wxMenuItem* btnVertical = contextMenu->AppendRadioItem( ID_DASH_VERTICAL, _("Vertical") );
            btnVertical->Check( itemBoxSizer->GetOrientation() == wxVERTICAL );
            wxMenuItem* btnHorizontal = contextMenu->AppendRadioItem( ID_DASH_HORIZONTAL, _("Horizontal") );
            btnHorizontal->Check( itemBoxSizer->GetOrientation() == wxHORIZONTAL );
            contextMenu->AppendSeparator();
        } // else non-docked
    } // then a pane

    m_plugin->PopulateContextMenu( contextMenu );

#ifdef _TACTICSPI_H_
    contextMenu->AppendSeparator();
    this->InsertTacticsIntoContextMenu ( contextMenu );
#endif // _TACTICSPI_H_

    contextMenu->AppendSeparator();
    contextMenu->Append( ID_DASH_PREFS, _("Preferences") );
    PopupMenu( contextMenu );
    delete contextMenu;
}

void DashboardWindow::OnContextMenuSelect( wxCommandEvent& event )
{
    if( event.GetId() < ID_DASH_PREFS ) { // Toggle dashboard visibility
        m_plugin->ShowDashboard( event.GetId()-1, event.IsChecked() );
        SetToolbarItemState( m_plugin->GetToolbarItemId(), m_plugin->GetDashboardWindowShownCount() != 0 );
    }

    switch( event.GetId() ){
    case ID_DASH_PREFS: {
      m_plugin->ShowPreferencesDialog(
#ifdef _TACTICSPI_H_
                m_plugin->pGetPluginFrame() // running dialog from plugin, can delete all windows
#else
                this
#endif //  _TACTICSPI_H_
        );
        return; // Does it's own save.
    }
    case ID_DASH_VERTICAL: {
#ifdef _TACTICSPI_H_
        m_Container->m_sOrientation = _T("V");
        m_plugin->SetApplySaveWinRequest();
        return;
#else
        ChangePaneOrientation( wxVERTICAL, true );
        m_Container->m_sOrientation = _T("V");
        break;
#endif // _TACTICSPI_H_
    }
    case ID_DASH_HORIZONTAL: {
#ifdef _TACTICSPI_H_
        m_Container->m_sOrientation = _T("H");
        m_plugin->SetApplySaveWinRequest();
        return;
#else
        ChangePaneOrientation( wxHORIZONTAL, true );
        m_Container->m_sOrientation = _T("H");
        break;
#endif // _TACTICSPI_H_
    }
    case ID_DASH_UNDOCK: {
        ChangePaneOrientation( GetSizerOrientation(), true );
#ifdef _TACTICSPI_H_
        break;      // Actually, the pane name has changed so better save
#else
        return;     // Nothing changed so nothing need be saved
#endif //  _TACTICSPI_H_
    }
#ifdef _TACTICSPI_H_
    default:
        this->TacticsInContextMenuAction( event.GetId() );
#endif // _TACTICSPI_H_
    }
    m_plugin->SaveConfig();
}

void DashboardWindow::SetColorScheme( PI_ColorScheme cs )
{
    DimeWindow( this );

    //  Improve appearance, especially in DUSK or NIGHT palette
    wxColour col;
    GetGlobalColor( _T("DASHL"), &col );
    SetBackgroundColour( col );

    Refresh( false );
}

void DashboardWindow::ChangePaneOrientation( int orient, bool updateAUImgr )
{
#ifdef _TACTICSPI_H_
    wxRect rect = m_plugin->pGetPluginFrame()->GetRect();
    wxPoint position;
    position.x = rect.x + 100;
    position.y = rect.y + 100;
    if ( orient == this->GetSizerOrientation() ) {
        wxAuiPaneInfo p = m_pauimgr->GetPane( m_Container->m_pDashboardWindow );
        if ( p.IsOk() && p.IsDocked() ) {
            rect = m_plugin->pGetPluginFrame()->GetRect();
            if ( p.dock_direction ==  wxAUI_DOCK_RIGHT )
                position.x = rect.x + rect.width - 325;
            else
                if ( p.dock_direction ==  wxAUI_DOCK_BOTTOM)
                    position.y = rect.y + rect.height - 400;
            m_pauimgr->GetPane( m_Container->m_pDashboardWindow ).FloatingPosition( position ).Float(); // undock if docked
            m_pauimgr->DetachPane( this );
            wxSize sz = GetMinSize();
            m_Container->m_sName = MakeName();
            bool isvertical = ( (orient == wxVERTICAL) ? true : false );
            m_pauimgr->AddPane(
                this, wxAuiPaneInfo().Name( m_Container->m_sName ).Caption(
                    m_Container->m_sCaption ).CaptionVisible( true ).TopDockable( !isvertical ).BottomDockable(
                        !isvertical ).LeftDockable( false ).RightDockable( isvertical ).MinSize( sz ).BestSize(
                    sz ).FloatingSize( sz ).FloatingPosition( position ).Float().Show( m_Container->m_bIsVisible ) );
            if ( updateAUImgr ){
                m_pauimgr->Update();
            } // then update (saving in event handler)
            return;
        } // then a docked container, undock request
    } // then this is not orientation request, check if this is undock request
    // orientation change request service follows
#endif // _TACTICSPI_H_
    m_pauimgr->DetachPane( this );
    SetSizerOrientation( orient );
    bool vertical = orient == wxVERTICAL;
    //wxSize sz = GetSize( orient, wxDefaultSize );
    wxSize sz = GetMinSize();
    // We must change Name to reset AUI perpective
    m_Container->m_sName = MakeName();
    m_pauimgr->AddPane(
        this, wxAuiPaneInfo().Name( m_Container->m_sName ).Caption(
            m_Container->m_sCaption ).CaptionVisible( true ).TopDockable( !vertical ).BottomDockable(
                !vertical ).LeftDockable(
#ifdef _TACTICSPI_H_
                    false
#else
                    vertical
#endif // _TACTICSPI_H_
                    ).RightDockable( vertical ).MinSize( sz ).BestSize(
                    sz ).FloatingSize( sz ).FloatingPosition(
#ifdef _TACTICSPI_H_
                        position
#else
                        100, 100
#endif // _TACTICSPI_H_
                        ).Float().Show( m_Container->m_bIsVisible ) );
    if ( updateAUImgr ) m_pauimgr->Update();
}

void DashboardWindow::SetSizerOrientation( int orient )
{
    itemBoxSizer->SetOrientation( orient );
    /* We must reset all MinSize to ensure we start with new default */
    wxWindowListNode* node = GetChildren().GetFirst();
    while(node) {
        node->GetData()->SetMinSize( wxDefaultSize );
        node = node->GetNext();
    }
// #endif // _TACTICSPI_H_
    SetMinSize( wxDefaultSize );
    Fit();
    SetMinSize( itemBoxSizer->GetMinSize() );
}

int DashboardWindow::GetSizerOrientation()
{
    return itemBoxSizer->GetOrientation();
}

bool isArrayIntEqual( const wxArrayInt& l1, const wxArrayOfInstrument &l2 )
{
    if( l1.GetCount() != l2.GetCount() ) return false;

    for( size_t i = 0; i < l1.GetCount(); i++ )
        if( l1.Item( i ) != l2.Item( i )->m_ID ) return false;

    return true;
}

bool DashboardWindow::isInstrumentListEqual( const wxArrayInt& list )
{
    return isArrayIntEqual( list, m_ArrayOfInstrument );
}

#ifdef _TACTICSPI_H_
void DashboardWindow::SetInstrumentList( wxArrayInt list, wxArrayString listIDs )
#else
void DashboardWindow::SetInstrumentList( wxArrayInt list )
#endif // _TACTICSPI_H_
{
    /* options
       ID_DBP_D_SOG: config max value, show STW optional
       ID_DBP_D_COG:  +SOG +HDG? +BRG?
       ID_DBP_D_AWS: config max value. Two arrows for AWS+TWS?
       ID_DBP_D_VMG: config max value
       ID_DBP_I_DPT: config unit (meter, feet, fathoms)
       ID_DBP_D_DPT: show temp optional
       // compass: use COG or HDG
       // velocity range
       // rudder range

       */

    m_ArrayOfInstrument.Clear();

    itemBoxSizer->Clear( true );

#ifdef _TACTICSPI_H_
    itemBoxSizer->SetSizeHints( this );
    Layout();
    Refresh();
#endif // _TACTICSPI_H_

    DashboardInstrument *instrument;
    
    for( size_t i = 0; i < list.GetCount(); i++ ) {
        int id = list.Item( i );
#ifdef _TACTICSPI_H_
        wxString ids = listIDs.Item( i );
#endif // _TACTICSPI_H_
        instrument = NULL;
        switch( id ){
        case ID_DBP_I_POS:
            instrument = new DashboardInstrument_Position( this, wxID_ANY,
                                                           getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_SOG:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_SOG, _T("%5.2f") );
            break;
        case ID_DBP_D_SOG:
            instrument = new DashboardInstrument_Speedometer( this, wxID_ANY,
                                                              getInstrumentCaption( id ), OCPN_DBP_STC_SOG, 0, g_iDashSpeedMax );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionLabel( g_iDashSpeedMax / 20 + 1,
                                                                         DIAL_LABEL_HORIZONTAL );
            //(DashboardInstrument_Dial *)instrument->SetOptionMarker(0.1, DIAL_MARKER_SIMPLE, 5);
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMarker( 0.5,
                                                                          DIAL_MARKER_SIMPLE, 2 );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionExtraValue(
                OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );
            break;
        case ID_DBP_I_COG:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_COG, _T("%.0f") );
            break;
        case ID_DBP_M_COG:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_MCOG, _T("%.0f") );
            break;
        case ID_DBP_D_COG:
            instrument = new DashboardInstrument_Compass( this, wxID_ANY,
                                                          getInstrumentCaption( id ), OCPN_DBP_STC_COG );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMarker( 5,
                                                                          DIAL_MARKER_SIMPLE, 2 );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionLabel( 30,
                                                                         DIAL_LABEL_ROTATED );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionExtraValue(
                OCPN_DBP_STC_SOG, _T("SOG\n%.2f"), DIAL_POSITION_BOTTOMLEFT );
            break;
        case ID_DBP_D_HDT:
            instrument = new DashboardInstrument_Compass( this, wxID_ANY,
                                                          getInstrumentCaption( id ), OCPN_DBP_STC_HDT );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMarker( 5,
                                                                          DIAL_MARKER_SIMPLE, 2 );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionLabel( 30,
                                                                         DIAL_LABEL_ROTATED );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionExtraValue(
                OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );
            break;
        case ID_DBP_I_STW:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_STW, _T("%.2f") );
            break;
        case ID_DBP_I_HDT: //true heading
            // TODO: Option True or Magnetic
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_HDT, _T("%.0f") );
            break;
        case ID_DBP_I_HDM:  //magnetic heading
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_HDM, _T("%.0f") );
            break;
        case ID_DBP_D_AW:
        case ID_DBP_D_AWA:
            instrument = new DashboardInstrument_Wind( this, wxID_ANY,
                                                       getInstrumentCaption( id ), OCPN_DBP_STC_AWA );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMainValue( _T("%.0f"),
                                                                             DIAL_POSITION_BOTTOMLEFT );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionExtraValue(
                OCPN_DBP_STC_AWS, _T("%.1f"), DIAL_POSITION_INSIDE );
            break;
        case ID_DBP_I_AWS:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_AWS, _T("%.2f") );
            break;
        case ID_DBP_D_AWS:
            instrument = new DashboardInstrument_Speedometer( this, wxID_ANY,
                                                              getInstrumentCaption( id ), OCPN_DBP_STC_AWS, 0, 45 );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionLabel( 5,
                                                                         DIAL_LABEL_HORIZONTAL );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMarker( 1,
                                                                          DIAL_MARKER_SIMPLE, 5 );
#ifdef _TACTICSPI_H_
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMainValue( _T("A:%.2f"),
#else
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMainValue( _T("A %.2f"),
#endif // _TACTICSPI_H_
                                                                             DIAL_POSITION_BOTTOMLEFT );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionExtraValue(
#ifdef _TACTICSPI_H_
                OCPN_DBP_STC_TWS, _T("T:%.1f"), DIAL_POSITION_BOTTOMRIGHT );
#else
                OCPN_DBP_STC_TWS, _T("T %.1f"), DIAL_POSITION_BOTTOMRIGHT );
#endif // _TACTICSPI_H_
            break;
        case ID_DBP_D_TW: //True Wind angle +-180deg on boat axis
            instrument = new DashboardInstrument_TrueWindAngle( this, wxID_ANY,
                                                                getInstrumentCaption( id ), OCPN_DBP_STC_TWA );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMainValue( _T("%.0f"),
                                                                             DIAL_POSITION_BOTTOMLEFT );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionExtraValue(
                OCPN_DBP_STC_TWS, _T("%.1f"), DIAL_POSITION_INSIDE );
            break;
        case ID_DBP_D_AWA_TWA: //App/True Wind angle +-180deg on boat axis
            instrument = new DashboardInstrument_AppTrueWindAngle(this, wxID_ANY,
                                                                  getInstrumentCaption(id), OCPN_DBP_STC_AWA | OCPN_DBP_STC_TWA);
            ((DashboardInstrument_Dial *)instrument)->SetOptionMainValue(_T("%.0f"),
                                                                         DIAL_POSITION_NONE);
            ((DashboardInstrument_Dial *)instrument)->SetOptionExtraValue(
                OCPN_DBP_STC_TWS | OCPN_DBP_STC_AWS, _T("%.1f"), DIAL_POSITION_NONE);
            break;
        case ID_DBP_D_TWD: //True Wind direction and speed
            instrument = new DashboardInstrument_WindCompass( this, wxID_ANY,
                                                              getInstrumentCaption( id ), OCPN_DBP_STC_TWD );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMainValue( _T("%.0f"),
                                                                             DIAL_POSITION_BOTTOMLEFT );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionExtraValue(
                OCPN_DBP_STC_TWS2, _T("%.1f"), DIAL_POSITION_INSIDE );
            break;
        case ID_DBP_I_DPT:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_DPT, _T("%5.1f") );
            break;
        case ID_DBP_D_DPT:
            instrument = new DashboardInstrument_Depth( this, wxID_ANY,
                                                        getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_TMP: //water temperature
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_TMP, _T("%2.1f") );
            break;
        case ID_DBP_I_MDA: //barometric pressure
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
#ifdef _TACTICSPI_H_
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_MDA, _T("%5.1f") );
#else
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_MDA, _T("%5.3f") );
#endif // _TACTICSPI_H_
            break;
        case ID_DBP_D_MDA: //barometric pressure
            instrument = new DashboardInstrument_Speedometer( this, wxID_ANY,
                                                              getInstrumentCaption( id ), OCPN_DBP_STC_MDA, 940, 1040 );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionLabel( 10,
                                                                         DIAL_LABEL_HORIZONTAL );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMarker( 5,
                                                                          DIAL_MARKER_SIMPLE, 1 );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMainValue( _T("%5.3f"),
                                                                             DIAL_POSITION_INSIDE );
            break;
        case ID_DBP_I_ATMP: //air temperature
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_ATMP, _T("%2.1f") );
            break;
        case ID_DBP_I_VLW1: // Trip Log
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_VLW1, _T("%2.1f") );
            break;

        case ID_DBP_I_VLW2: // Sum Log
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_VLW2, _T("%2.1f") );
            break;

        case ID_DBP_I_TWA: //true wind angle
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_TWA, _T("%5.0f") );
            break;
        case ID_DBP_I_TWD: //true wind direction
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_TWD, _T("%5.0f") );
            break;
        case ID_DBP_I_TWS: // true wind speed
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_TWS, _T("%2.2f") );
            break;
        case ID_DBP_I_AWA: //apparent wind angle
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_AWA, _T("%5.0f") );
            break;
        case ID_DBP_I_VMG:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_VMG, _T("%5.2f") );
            break;
        case ID_DBP_D_VMG:
            instrument = new DashboardInstrument_Speedometer( this, wxID_ANY,
                                                              getInstrumentCaption( id ), OCPN_DBP_STC_VMG, 0, g_iDashSpeedMax );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionLabel( 1,
                                                                         DIAL_LABEL_HORIZONTAL );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionMarker( 0.5,
                                                                          DIAL_MARKER_SIMPLE, 2 );
            ( (DashboardInstrument_Dial *) instrument )->SetOptionExtraValue(
                OCPN_DBP_STC_SOG, _T("SOG\n%.2f"), DIAL_POSITION_BOTTOMLEFT );
            break;
        case ID_DBP_I_RSA:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_RSA, _T("%5.0f") );
            break;
        case ID_DBP_D_RSA:
            instrument = new DashboardInstrument_RudderAngle( this, wxID_ANY,
                                                              getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_SAT:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_SAT, _T("%5.0f") );
            break;
        case ID_DBP_D_GPS:
            instrument = new DashboardInstrument_GPS( this, wxID_ANY,
                                                      getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_PTR:
            instrument = new DashboardInstrument_Position( this, wxID_ANY,
                                                           getInstrumentCaption( id ), OCPN_DBP_STC_PLA, OCPN_DBP_STC_PLO );
            break;
        case ID_DBP_I_GPSUTC:
            instrument = new DashboardInstrument_Clock( this, wxID_ANY,
                                                        getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_SUN:
            instrument = new DashboardInstrument_Sun( this, wxID_ANY,
                                                      getInstrumentCaption( id ) );
            break;
        case ID_DBP_D_MON:
            instrument = new DashboardInstrument_Moon( this, wxID_ANY,
                                                       getInstrumentCaption( id ) );
            break;
        case ID_DBP_D_WDH:
            instrument = new DashboardInstrument_WindDirHistory(this, wxID_ANY,
                                                                getInstrumentCaption( id ) );
            break;
        case ID_DBP_D_BPH:
            instrument = new DashboardInstrument_BaroHistory(this, wxID_ANY,
                                                             getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_FOS:
            instrument = new DashboardInstrument_FromOwnship( this, wxID_ANY,
                                                              getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_PITCH:
            instrument = new DashboardInstrument_Single(this, wxID_ANY,
                                                        getInstrumentCaption(id), OCPN_DBP_STC_PITCH, _T("%2.1f"));
            break;
        case ID_DBP_I_HEEL:
            instrument = new DashboardInstrument_Single(this, wxID_ANY,
                                                        getInstrumentCaption(id), OCPN_DBP_STC_HEEL, _T("%2.1f"));
            break;
            // any clock display with "LCL" in the format string is converted from UTC to local TZ
        case ID_DBP_I_SUNLCL:
            instrument = new DashboardInstrument_Sun( this, wxID_ANY,
                                                      getInstrumentCaption( id ), _T( "%02i:%02i:%02i LCL" ) );
            break;
        case ID_DBP_I_GPSLCL:
            instrument = new DashboardInstrument_Clock( this, wxID_ANY,
                                                        getInstrumentCaption( id ), OCPN_DBP_STC_CLK, _T( "%02i:%02i:%02i LCL" ) );
            break;
        case ID_DBP_I_CPULCL:
            instrument = new DashboardInstrument_CPUClock( this, wxID_ANY,
                                                           getInstrumentCaption( id ), _T( "%02i:%02i:%02i LCL" ) );
			break;
#ifdef _TACTICSPI_H_
		case ID_DBP_I_CURRDIR:
			instrument = new DashboardInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_CURRDIR, _T("%2.0f"));
			break;
		case ID_DBP_I_CURRSPD:
			instrument = new DashboardInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_CURRSPD, _T("%2.2f"));
			break;
        case ID_DBP_I_LEEWAY:
            instrument = new DashboardInstrument_Single(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_LEEWAY,
                _T("%2.1f"));
            break;
        case ID_DBP_D_BRG:  // Bearing Compass
            instrument = new TacticsInstrument_BearingCompass(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_COG |
                OCPN_DBP_STC_BRG | OCPN_DBP_STC_CURRDIR |
                OCPN_DBP_STC_CURRSPD | OCPN_DBP_STC_TWA |
                OCPN_DBP_STC_LEEWAY | OCPN_DBP_STC_HDT |
                OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON |
                OCPN_DBP_STC_STW | OCPN_DBP_STC_AWA |
                OCPN_DBP_STC_TWS | OCPN_DBP_STC_TWD);
            ((DashboardInstrument_Dial *) instrument)->SetOptionMarker(
                5, DIAL_MARKER_SIMPLE, 2);
            ((DashboardInstrument_Dial *) instrument)->SetOptionLabel(
                30, DIAL_LABEL_ROTATED);
            ((DashboardInstrument_Dial *)
             instrument)->SetOptionExtraValue(
                 OCPN_DBP_STC_DTW, _T("%.2f"), DIAL_POSITION_TOPLEFT);
            break;
        case ID_DBP_D_POLCOMP: // Polar Compass
            instrument = new TacticsInstrument_PolarCompass(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_COG |
                OCPN_DBP_STC_BRG | OCPN_DBP_STC_CURRDIR |
                OCPN_DBP_STC_CURRSPD | OCPN_DBP_STC_TWA |
                OCPN_DBP_STC_LEEWAY | OCPN_DBP_STC_HDT |
                OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON |
                OCPN_DBP_STC_STW | OCPN_DBP_STC_AWA |
                OCPN_DBP_STC_TWS | OCPN_DBP_STC_TWD);
            ((DashboardInstrument_Dial *) instrument)->SetOptionMarker(
                5, DIAL_MARKER_SIMPLE, 2);
            ((DashboardInstrument_Dial *) instrument)->SetOptionLabel(
                30, DIAL_LABEL_ROTATED);
            ((DashboardInstrument_Dial *)
             instrument)->SetOptionExtraValue(
                 OCPN_DBP_STC_DTW, _T("%.2f"), DIAL_POSITION_TOPLEFT);
            break;
        case ID_DBP_I_TWAMARK:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id),
                OCPN_DBP_STC_BRG | OCPN_DBP_STC_TWD |
                OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON, _T("%5.0f"));
            ((TacticsInstrument_PerformanceSingle *)
             instrument)->SetDisplayType(TWAMARK);
            break;
		case ID_DBP_I_POLSPD:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
            ((TacticsInstrument_PerformanceSingle *)
             instrument)->SetDisplayType(POLARSPEED);
            break;
        case ID_DBP_I_POLVMG:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
            ((TacticsInstrument_PerformanceSingle *)
             instrument)->SetDisplayType(POLARVMG);
            break;
        case ID_DBP_I_POLTVMG:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
            ((TacticsInstrument_PerformanceSingle *)
             instrument)->SetDisplayType(POLARTARGETVMG);
            break;
        case ID_DBP_I_POLTVMGANGLE:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
            ((TacticsInstrument_PerformanceSingle *)
             instrument)->SetDisplayType(POLARTARGETVMGANGLE);
            break;
        case ID_DBP_I_POLCMG:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_COG | OCPN_DBP_STC_SOG | OCPN_DBP_STC_BRG |
                OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON, _T("%.2f"));
            ((TacticsInstrument_PerformanceSingle *)
             instrument)->SetDisplayType(POLARCMG);
            break;
        case ID_DBP_I_POLTCMG:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS | OCPN_DBP_STC_HDT |
                OCPN_DBP_STC_BRG | OCPN_DBP_STC_TWD | OCPN_DBP_STC_LAT |
                OCPN_DBP_STC_LON, _T("%.2f"));
            ((TacticsInstrument_PerformanceSingle *)
             instrument)->SetDisplayType(POLARTARGETCMG);
            break;
        case ID_DBP_I_POLTCMGANGLE:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS | OCPN_DBP_STC_HDT |
                OCPN_DBP_STC_BRG | OCPN_DBP_STC_TWD | OCPN_DBP_STC_LAT |
                OCPN_DBP_STC_LON, _T("%.2f"));
            ((TacticsInstrument_PerformanceSingle *)
             instrument)->SetDisplayType(POLARTARGETCMGANGLE);
            break;
        case ID_DBP_V_IFLX:
            instrument = new TacticsInstrument_StreamoutSingle(
                this, wxID_ANY,
                getInstrumentCaption(id),
                OCPN_DBP_STC_LAT          |
                OCPN_DBP_STC_LON          |
                OCPN_DBP_STC_SOG          |
                OCPN_DBP_STC_COG          |
                OCPN_DBP_STC_STW          |
                OCPN_DBP_STC_HDM          |
                OCPN_DBP_STC_HDT          |
                OCPN_DBP_STC_HMV          |
                OCPN_DBP_STC_BRG          |
                OCPN_DBP_STC_AWA          |
                OCPN_DBP_STC_AWS          |
                OCPN_DBP_STC_TWA          |
                OCPN_DBP_STC_TWS          |
                OCPN_DBP_STC_DPT          |
                OCPN_DBP_STC_TMP          |
                OCPN_DBP_STC_VMG          |
                OCPN_DBP_STC_RSA          |
                OCPN_DBP_STC_SAT          |
                OCPN_DBP_STC_PLA          |
                OCPN_DBP_STC_PLO          |
                OCPN_DBP_STC_ATMP         |
                OCPN_DBP_STC_TWD          |
                OCPN_DBP_STC_TWS2         |
                OCPN_DBP_STC_VLW1         |
                OCPN_DBP_STC_VLW2         |
                OCPN_DBP_STC_MDA          |
                OCPN_DBP_STC_MCOG         |
                OCPN_DBP_STC_PITCH        |
                OCPN_DBP_STC_HEEL         |
                OCPN_DBP_STC_LEEWAY       |
                OCPN_DBP_STC_CURRDIR      |
                OCPN_DBP_STC_CURRSPD      |
                OCPN_DBP_STC_DTW          |
                OCPN_DBP_STC_TWAMARK      |
                OCPN_DBP_STC_POLPERF      |
                OCPN_DBP_STC_POLSPD       |
                OCPN_DBP_STC_POLVMG       |
                OCPN_DBP_STC_POLTVMG      |
                OCPN_DBP_STC_POLTVMGANGLE |
                OCPN_DBP_STC_POLCMG       |
                OCPN_DBP_STC_POLTCMG      |
                OCPN_DBP_STC_POLTCMGANGLE |
                OCPN_DBP_STC_SKSUBSCRIBE,
                _T("%s"),
                m_plugin->m_mtxNofStreamOut,
                m_plugin->m_nofStreamOut,
                m_plugin->m_echoStreamerShow,
                m_plugin->GetStandardPath(),
                this->m_pSkData );
            break;
        case ID_DBP_V_INSK:
            instrument = new TacticsInstrument_StreamInSkSingle(
                this, wxID_ANY,
                getInstrumentCaption(id),
                0ULL,
                _T("%s"),
                m_plugin->m_mtxNofStreamInSk,
                m_plugin->m_nofStreamInSk,
                m_plugin->m_echoStreamerInSkShow,
                m_plugin->GetStandardPath(),
                this->m_pSkData );
            break;
        case ID_DBP_D_POLPERF:
            instrument = new TacticsInstrument_PolarPerformance(
                this, wxID_ANY, getInstrumentCaption(id));
            break;
        case ID_DBP_D_AVGWIND:
            instrument = new TacticsInstrument_AvgWindDir(
                this, wxID_ANY, getInstrumentCaption(id));
            break;
        case ID_DBP_D_ENGDJG:
            if ( ids.IsEmpty() )
                ids = GetUUID();
            instrument = new DashboardInstrument_EngineDJG( // Dial instrument
                this, wxID_ANY, ids, m_plugin->m_colorScheme );
            break;
        case ID_DBP_D_TSETUI:
            if ( ids.IsEmpty() )
                ids = GetUUID();
            instrument = new DashboardInstrument_TimesTUI( // Time-series DB graph
                this, wxID_ANY, ids, m_plugin->m_colorScheme );
            break;
#endif // _TACTICSPI_H_
        }
        if( instrument ) {
            instrument->instrumentTypeId = id;
#ifdef _TACTICSPI_H_
            Unbind( wxEVT_SIZE, &DashboardWindow::OnSize, this );
#endif // _TACTICSPI_H_
#ifdef _TACTICSPI_H_
            m_ArrayOfInstrument.Add(
                new DashboardInstrumentContainer(
                    id, instrument, instrument->GetCapacity(), ids ) );
#else
            m_ArrayOfInstrument.Add(
                new DashboardInstrumentContainer(
                    id, instrument, instrument->GetCapacity() ) );
#endif // _TACTICSPI_H_
            itemBoxSizer->Add( instrument, 0, wxEXPAND, 0 );
#ifdef _TACTICSPI_H_
            Bind( wxEVT_SIZE, &DashboardWindow::OnSize, this );
            itemBoxSizer->SetSizeHints( this );
            Layout();
#endif // _TACTICSPI_H_
            if( itemBoxSizer->GetOrientation() == wxHORIZONTAL ) {
                itemBoxSizer->AddSpacer( 5 );
#ifdef _TACTICSPI_H_
                itemBoxSizer->SetSizeHints( this );
                Layout();
#endif // _TACTICSPI_H_
            }
        }
#ifdef _TACTICSPI_H_
        m_Container->m_aInstrumentIDs.Item( i ) = ids; // UUID for persistance
#endif // _TACTICSPI_H_
    } // for items in the list
#ifdef _TACTICSPI_H_
    itemBoxSizer->SetSizeHints( this );
    Layout();
    Refresh();
#else
    Fit();
    SetMinSize( itemBoxSizer->GetMinSize() );
    Layout();
#endif // _TACTICSPI_H_

}

void DashboardWindow::SendSentenceToAllInstruments(
#ifdef _TACTICSPI_H_
    unsigned long long st,
#else
    int st,
#endif // _TACTICSPI_H_
    double value, wxString unit
#ifdef _TACTICSPI_H_
        , long long timestamp
#endif // _TACTICSPI_H_
    )
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {

        if (
#ifdef _TACTICSPI_H_
            (!((m_ArrayOfInstrument.Item( i )->m_cap_flag & st) == 0ULL))
#else
             m_ArrayOfInstrument.Item( i )->m_cap_flag & st
#endif // _TACTICSPI_H_
                )
            m_ArrayOfInstrument.Item( i )->m_pInstrument->SetData(
                st, value, unit
#ifdef _TACTICSPI_H_
                , timestamp
#endif // _TACTICSPI_H_
                );
    }
}

void DashboardWindow::SendSatInfoToAllInstruments( int cnt, int seq, SAT_INFO sats[4] )
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
        if(
#ifdef _TACTICSPI_H_
             (!((m_ArrayOfInstrument.Item( i )->m_cap_flag &
                 OCPN_DBP_STC_GPS) == 0ULL))
#else
             ( m_ArrayOfInstrument.Item( i )->m_cap_flag & OCPN_DBP_STC_GPS )
#endif // _TACTICSPI_H_
            && m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf(
                CLASSINFO(DashboardInstrument_GPS)))
            ((DashboardInstrument_GPS*) m_ArrayOfInstrument.Item(i)->m_pInstrument)->SetSatInfo(
                cnt, seq, sats);
    }
}

void DashboardWindow::SendUtcTimeToAllInstruments( wxDateTime value )
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
         if(
#ifdef _TACTICSPI_H_
             (!((m_ArrayOfInstrument.Item( i )->m_cap_flag &
                 OCPN_DBP_STC_CLK) == 0ULL))
             && m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf( CLASSINFO( DashboardInstrument_Clock ) ) )
#else
             // ( m_ArrayOfInstrument.Item( i )->m_cap_flag & OCPN_DBP_STC_GPS ) // default _D_GPS has no SetUtcTime()!
             ( m_ArrayOfInstrument.Item( i )->m_cap_flag & OCPN_DBP_STC_CLK )
             && m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf( CLASSINFO( DashboardInstrument_Clock ) ) )
#endif // _TACTICSPI_H_
            //                  || m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf( CLASSINFO( DashboardInstrument_Sun ) )
            //                  || m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf( CLASSINFO( DashboardInstrument_Moon ) ) ) )
            ((DashboardInstrument_Clock*) m_ArrayOfInstrument.Item(i)->m_pInstrument)->SetUtcTime( value );
    }
}

#ifdef _TACTICSPI_H_
void DashboardWindow::SendColorSchemeToAllJSInstruments( PI_ColorScheme cs )
{
    for ( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
        if ( m_ArrayOfInstrument.Item( i )->m_pInstrument != NULL )
            m_ArrayOfInstrument.Item(i)->m_pInstrument->setColorScheme( cs );
    }
}
#endif // _TACTICSPI_H_
