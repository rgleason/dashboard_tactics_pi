/***************************************************************************
* $Id: StreamoutSchema.h, v1.0 2019/11/30 DarthVader $
*
* Project:  OpenCPN
* Purpose:  dashboard_tactics_pi plug-in streaming out
* Author:   Petri Makijarvi
*       (Inspired by original work from Jean-Eudes Onfray)
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

#ifndef __STREAMOUTSCHEMA_H__
#define __STREAMOUTSCHEMA_H__

/*
  StreamoutSchema and LineProtocol are classes presetening the fast storage
  objects needed to map the configuration, normally in JSON configuration
  file into the database line protocol.
*/

class StreamoutSchema
{
public:
    StreamoutSchema(void) {
        stc = wxEmptyString;
        st = 0ULL;
        bStore = false;
        iInterval = 0;
        lastTimeStamp = 0LL;
        sMeasurement = wxEmptyString;
        sProp1 = wxEmptyString;
        sProp2 = wxEmptyString;
        sProp3 = wxEmptyString;
        sField1 = wxEmptyString;
        sField2 = wxEmptyString;
        sField3 = wxEmptyString;
        sSkpathe = wxEmptyString;
    };
    StreamoutSchema( const StreamoutSchema& source) {
#define StreamoutSchemaCopy(__SS_SOURCE__)  stc = __SS_SOURCE__.stc; st = __SS_SOURCE__.st; bStore = __SS_SOURCE__.bStore; \
        iInterval = __SS_SOURCE__.iInterval; lastTimeStamp = __SS_SOURCE__.lastTimeStamp; \
        sMeasurement = __SS_SOURCE__.sMeasurement; sProp1 = __SS_SOURCE__.sProp1; sProp2 = __SS_SOURCE__.sProp2; \
        sProp3 = __SS_SOURCE__.sProp3; sField1 = __SS_SOURCE__.sField1; sField2 = __SS_SOURCE__.sField2; \
        sField3 = __SS_SOURCE__.sField3; sSkpathe = __SS_SOURCE__.sSkpathe
        StreamoutSchemaCopy(source);
    };
    const StreamoutSchema& operator = (const StreamoutSchema &source) {
        if ( this != &source) {
            StreamoutSchemaCopy(source);
        }
        return *this;
    };
    wxString stc;
    unsigned long long st;
    bool bStore;
    int iInterval;
    long long lastTimeStamp;
    wxString sMeasurement;
    wxString sProp1;
    wxString sProp2;
    wxString sProp3;
    wxString sField1;
    wxString sField2;
    wxString sField3;
    wxString sSkpathe;
};

#endif // not defined __STREAMOUTSCHEMA_H__

