/***************************************************************************
* $Id: TacticsEnums.h, v1.0 2016/06/07 tomBigSpeedy Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Thomas Rauch
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

#ifndef __TACTICSENUMS_H__
#define __TACTICSENUMS_H__


#define WINDDIR 360
#define WINDSPEED 60

enum dbgPolarStat {
    DBGRES_POLAR_UNKNOWN, DBGRES_POLAR_INVALID, DBGRES_POLAR_VALID
};

enum polarPos {
	POLARSPEED, POLARVMG, POLARTARGETVMG, POLARTARGETVMGANGLE, POLARCMG, POLARTARGETCMG, POLARTARGETCMGANGLE, TWAMARK
};

enum dbgTrueWindStartAWS_STC {
    DBGRES_AWS_STC_UNKNOWN, DBGRES_AWS_STC_WAIT, DBGRES_AWS_STC_AVAILABLE_INVALID, DBGRES_AWS_STC_AVAILABLE };
enum dbgTrueWindStartForce {
    DBGRES_FORCE_UNKNOWN, DBGRES_FORCE_SELECTED_TW_AVAILABLE, DBGRES_FORCE_SELECTED_NO_TW_AVAILABLE,
    DBGRES_FORCE_SELECTED_NO_TWD_AVAILABLE, DBGRES_FORCE_NOT_SELECTED_TW_AVAILABLE,
    DBGRES_FORCE_NOT_SELECTED_NO_TW_AVAILABLE };
enum dbgTrueWindStartMval {
    DBGRES_MVAL_UNKNOWN, DBGRES_MVAL_INVALID, DBGRES_MVAL_AVAILABLE, DBGRES_MVAL_IS_ZERO, DBGRES_MVAL_IS_NEG };
enum dbgTrueWindExecStat {
    DBGRES_EXEC_UNKNOWN, DBGRES_EXEC_FALSE, DBGRES_EXEC_TWDONLY_TRUE, DBGRES_EXEC_TRUE };

#endif // __TACTICSENUMS_H__
