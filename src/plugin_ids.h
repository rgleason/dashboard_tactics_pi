/***************************************************************************
 * $Id: plugin_ids.h, v1.0 2019/11/30 VaderDarth Exp $
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

#ifndef _PLUGINIDS_H_
#define _PLUGINIDS_H_


enum pluginids {
    myID_wxRESERVED = wxID_HIGHEST,
    myID_EDIT_FIRST,
    myID_FIRST_UNIQUE_ID = myID_EDIT_FIRST + 12500, // arbitrary number
    // threads or timer events as threaded execution
    myID_DBP_I_TIMER_TICK,
    myID_THREAD_WINDHISTORY,
    myID_THREAD_POLARPERFORMANCE,
    myID_THREAD_BAROHISTORY,
    myID_THREAD_IFLXAPI,
    myID_TICK_IFLXAPI,
    myID_THREAD_SK_IN,
    myID_TICK_SK_IN,
    myID_THREAD_AVGWIND,
    myID_TICK_AVGWIND,
    myID_TICK_ENGINEI,
    myID_TICK_ENGINEDJG,
    myID_TICK_TIMESTUI,
    myID_TICK_INSTRUJS,
    // end of tacticsEvents
    myID_EDIT_LAST
};

#endif // _PLUGINIDS_H_
