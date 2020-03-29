/***************************************************************************
* $Id: LineProtocol.h, v1.0 2019/11/30 DarthVader $
*
* Project:  OpenCPN
* Purpose:  dashboard_tactics_pi plug-in streaming out
* Author:   Petri Makijarvi
*       (Inspired by original work from Jean-Eudes Onfray)
***************************************************************************
*   Copyrigoht (C) 2010 by David S. Register                               *
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

#ifndef __LINEPROTOCOL_H__
#define __LINEPROTOCOL_H__

/*
  StreamoutSchema and LineProtocol are classes presetening the fast storage
  objects needed to map the configuration, normally in JSON configuration
  file into the database line protocol.
*/


class LineProtocol
{
public:
    LineProtocol(void) {
        measurement = wxEmptyString;
        tag_key1 = wxEmptyString;
        tag_value1 = wxEmptyString;
        tag_key2 = wxEmptyString;
        tag_value2 = wxEmptyString;
        tag_key3 = wxEmptyString;
        tag_value3 = wxEmptyString;
        field_key1 = wxEmptyString;
        field_value1 = wxEmptyString;
        field_key2 = wxEmptyString;
        field_value2 = wxEmptyString;
        field_key3 = wxEmptyString;
        field_value3 = wxEmptyString;
        timestamp = wxEmptyString;
    };
    LineProtocol( const LineProtocol& source) {
#define LineProtocolCopy(__LP_SOURCE__) measurement = __LP_SOURCE__.measurement; tag_key1 = __LP_SOURCE__.tag_key1; \
        tag_value1 = __LP_SOURCE__.tag_value1; tag_key2 = __LP_SOURCE__.tag_key2; tag_value2 = __LP_SOURCE__.tag_value2; \
        tag_key3 = __LP_SOURCE__.tag_key3; tag_value3 = __LP_SOURCE__.tag_value3; field_key1 = __LP_SOURCE__.field_key1; \
        field_value1 = __LP_SOURCE__.field_value1; field_key2 = __LP_SOURCE__.field_key2; \
        field_value2 = __LP_SOURCE__.field_value2; field_key3 = __LP_SOURCE__.field_key3; \
        field_value3 = __LP_SOURCE__.field_value3;timestamp = __LP_SOURCE__.timestamp
        LineProtocolCopy(source);
    };
    const LineProtocol& operator = (const LineProtocol &source) {
        if ( this != &source) {
            LineProtocolCopy(source);
        }
        return *this;
    };
    wxString measurement;
    wxString tag_key1;
    wxString tag_value1;
    wxString tag_key2;
    wxString tag_value2;
    wxString tag_key3;
    wxString tag_value3;
    wxString field_key1;
    wxString field_value1;
    wxString field_key2;
    wxString field_value2;
    wxString field_key3;
    wxString field_value3;
    wxString timestamp;
}; // This class presents the line propotocol elements in the data FIFO queue 

#endif // not defined __LINEPROTOCOL_H__

