/***************************************************************************
* $Id: TacticsFunctions.h, v1.0 2016/06/07 tomBigSpeedy Exp $
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

#ifndef __TACTICSFUNCTIONS_H__
#define __TACTICSFUNCTIONS_H__

#include <wx/gdicmn.h>

inline int myCCW(wxRealPoint p0, wxRealPoint p1, wxRealPoint p2);
inline bool IsLineIntersect(wxRealPoint p1, wxRealPoint p2, wxRealPoint p3, wxRealPoint p4);
wxRealPoint GetLineIntersection(wxRealPoint line1point1, wxRealPoint line1point2, wxRealPoint line2point1, wxRealPoint line2point2);
double CalcPolarTimeToMark(double distance, double twa, double tws);
double getMarkTWA(double twd, double ctm);
double getDegRange(double max, double min);
double getSignedDegRange(double max, double min);
double rad2deg(double angle);
double deg2rad(double angle);

#endif // __TACTICSFUNCTIONS_H__
