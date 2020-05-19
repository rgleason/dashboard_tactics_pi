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
#define _DASHBOARDINSTRUMENTCONTAINER_H_

#include "instrument.h"

// !!! WARNING !!!
// do not change the order, add new instruments at the end, before ID_DBP_LAST_ENTRY!
// otherwise, for users with an existing opencpn.ini file, their instruments are changing !
enum eInstruments {
    ID_DBP_I_POS, ID_DBP_I_SOG, ID_DBP_D_SOG, ID_DBP_I_COG, ID_DBP_D_COG, ID_DBP_I_STW,
    ID_DBP_I_HDT, ID_DBP_D_AW, ID_DBP_D_AWA, ID_DBP_I_AWS, ID_DBP_D_AWS, ID_DBP_D_TW,
    ID_DBP_I_DPT, ID_DBP_D_DPT, ID_DBP_I_TMP, ID_DBP_I_VMG, ID_DBP_D_VMG, ID_DBP_I_RSA,
    ID_DBP_D_RSA, ID_DBP_I_SAT, ID_DBP_D_GPS, ID_DBP_I_PTR, ID_DBP_I_GPSUTC, ID_DBP_I_SUN,
    ID_DBP_D_MON, ID_DBP_I_ATMP, ID_DBP_I_AWA, ID_DBP_I_TWA, ID_DBP_I_TWD, ID_DBP_I_TWS,
    ID_DBP_D_TWD, ID_DBP_I_HDM, ID_DBP_D_HDT, ID_DBP_D_WDH, ID_DBP_I_VLW1, ID_DBP_I_VLW2,
    ID_DBP_D_MDA,ID_DBP_I_MDA,ID_DBP_D_BPH, ID_DBP_I_FOS, ID_DBP_M_COG, ID_DBP_I_PITCH,
    ID_DBP_I_HEEL, ID_DBP_D_AWA_TWA, ID_DBP_I_GPSLCL, ID_DBP_I_CPULCL, ID_DBP_I_SUNLCL,
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
    ID_DBP_LAST_ENTRY /* This has a reference in one of the routines; defining a "LAST_ENTRY" and
                         setting the reference to it, is one codeline less to change (and find)
                         when adding new instruments :-) */
};

// global helper functions for enumerated instrument list information
bool IsTacticsInstrument( int id );
bool IsObsolete( int id );
wxString getInstrumentCaption( unsigned int id );
bool getListItemForInstrument( wxListItem &item, unsigned int id );

class DashboardInstrumentContainer
{
public:
    DashboardInstrumentContainer(
        int id, DashboardInstrument *instrument,
        unsigned long long capa, wxString ids = _T("") )
        {
            m_ID = id;
            m_pInstrument = instrument;
            m_cap_flag = capa;
            m_IDs = ids;
        };
    ~DashboardInstrumentContainer(){ delete m_pInstrument; };
    DashboardInstrument    *m_pInstrument;
    int                     m_ID;
    unsigned long long      m_cap_flag;
    wxString                m_IDs;
};

WX_DEFINE_ARRAY(DashboardInstrumentContainer *, wxArrayOfInstrument);

#endif // _DASHBOARDINSTRUMENTCONTAINER_H_
