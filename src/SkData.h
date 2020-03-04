/******************************************************************************
* $Id: skdata.h, v1.0 2019/11/30 VaderDarth Exp $
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

#ifndef __SKDATA_H__
#define __SKDATA_H__

#include <list>
#include <unordered_map>
#include <functional>

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/jsonval.h"

#include "StreamoutSchema.h"

// Incoming NMEA-2000 PGNs Dashboard inspects
#define PGN_ENG_PARAM_RAP 127488
#define PGN_ENG_PARAM_DYN 127489 
// Signal K conversions, see https://git.io/JeYry
#define DEG_IN_RAD 0.0174532925
#define RAD_IN_DEG 57.2957795
#define CELCIUS_IN_KELVIN 273.15
#define MS_IN_KNOTS 1.943844
#define KM_IN_NM 0.539956803
#define PA_IN_BAR 100000

// Signal K originated data handling / containers / C++/HTML/CSS/JS
typedef std::list<std::string> SkDataPathList;
typedef std::pair<std::string, std::string> dbQueryMapPair;
typedef std::unordered_multimap<std::string, std::string> db_query_map;

class SkData
{
public:
    SkData(void);
    SkData(const SkData& sourceSkData);
    ~SkData();
    void UpdateNMEA2000PathList( wxString* path, wxString* key );
    void UpdateNMEA0183PathList( wxString* path, wxString* key );
    void UpdateSubscriptionList( wxString* path, wxString* key );
    void UpdateStreamoutSchemaList( wxString* url, wxString* org,
                                    wxString* token, wxString* bucket,
                                    StreamoutSchema* schema );
    wxString getAllNMEA2000JsOrderedList(void);
    wxString getAllNMEA0183JsOrderedList(void);
    wxString getAllSubscriptionsJSON(wxJSONValue& pRetJSON);
    wxString getAllDbSchemasJsOrderedList(void);
    wxString getDbSchemaJs( wxString* path );
    void subscribeToAllPaths(void) {m_subscribedToAllPaths = true;};
    void subscribeToSubscriptionList(void) {m_subscribedToAllPaths = false;};
    bool isSubscribedToAllPaths(void) {return m_subscribedToAllPaths;};
    void recordAllDbSchemas(void) {m_recordAllDbSchemas = true;};
    void stopRecordingAllDbSchemas(void) {m_recordAllDbSchemas = false;};
    bool isRecordingAllDbSchemas(void) {return m_recordAllDbSchemas;};
    
protected:
    SkDataPathList       *m_pathlist;
    SkDataPathList       *m_nmea0183pathlist;
    SkDataPathList       *m_nmea2000pathlist;
    SkDataPathList       *m_subscriptionlist;
    db_query_map         *m_dbQueryMap;

private:
    void pushDefaultSubscriptions(void);
    void UpdatePathList ( SkDataPathList* pathlist, wxString* path, wxString* key );
    wxString getAllJsOrderedList(
        SkDataPathList* pathlist,
        wxJSONValue& pRetJSON);
    bool                  m_subscribedToAllPaths;
    bool                  m_recordAllDbSchemas;
}; // class SkData




#endif // __SKDATA_H__
