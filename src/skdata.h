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
using namespace std;
#include <list>

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

// Signal K originated data handling / containers / C++/HTML/CSS/JS
typedef std::list<std::string> SkDataPathList;
class SkData
{
public:
    SkData(void);
    ~SkData();
    void UpdateNMEA2000PathList( wxString* path, wxString* key );
    void UpdateNMEA0183PathList( wxString* path, wxString* key );
    wxString getAllNMEA2000JsOrderedList(void);
    wxString getAllNMEA0183JsOrderedList(void);
protected:
    SkDataPathList       *m_pathlist;
    SkDataPathList       *m_nmea0183pathlist;
    SkDataPathList       *m_nmea2000pathlist;
private:
    void UpdatePathList ( SkDataPathList* pathlist, wxString* path, wxString* key );
    wxString getAllJsOrderedList( SkDataPathList* pathlist );
}; // class SkData




#endif // __SKDATA_H__
