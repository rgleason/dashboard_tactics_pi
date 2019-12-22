/******************************************************************************
* $Id: skdata.cpp, v1.0 2019/11/30 VaderDarth Exp $
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

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "skdata.h"
#include <string>

//************************************************************************************************************************
// Signal K originated data handling / containers / C++/HTML/CSS/JS
//************************************************************************************************************************

SkData::SkData()
{
    m_pathlist = new SkDataPathList();
    m_nmea0183pathlist = new SkDataPathList();
    m_nmea2000pathlist = new SkDataPathList();
    return;
}

SkData::~SkData()
{
    delete m_pathlist;
    delete m_nmea0183pathlist;
    delete m_nmea2000pathlist;
    return;
}

void SkData::UpdatePathList( SkDataPathList *pathlist, wxString *path, wxString *key )
{
    std::string stdPathFull = std::string( path->mb_str() );
    if ( key != NULL ) {
        std::string stdKey  = std::string( key->mb_str() );
        stdPathFull = stdPathFull + "." + stdKey;
    }
    SkDataPathList::iterator it = std::find(pathlist->begin(), pathlist->end(), stdPathFull);
    if ( it != pathlist->end() )
        return;
    pathlist->push_back( stdPathFull );
    return;
}

void SkData::UpdateNMEA2000PathList( wxString *path, wxString *key )
{
    if ( path == NULL )
        return;
    UpdatePathList( m_nmea2000pathlist, path, key );
    UpdatePathList( m_pathlist, path, key );
}

void SkData::UpdateNMEA0183PathList( wxString *path, wxString *key )
{
    if ( path == NULL )
        return;
    UpdatePathList( m_nmea0183pathlist, path, key );
    UpdatePathList( m_pathlist, path, key );
}

