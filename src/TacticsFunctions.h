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

/* CTest in ../tests/test-01_tactics_functions.cpp */

/*********************************************************************
 * FUNCTION:   CCW (CounterClockWise) (from cutil.h of OpenCPN)
 *
 * PURPOSE
 * Determines, given three points, if when travelling from the first to
 * the second to the third, we travel in a counterclockwise direction.
 *
 * RETURN VALUE
 * (int) 1 if the movement is in a counterclockwise direction, -1 if
 * not.Taken from cutil
*********************************************************************/
inline int tacticsCCW(wxRealPoint p0, wxRealPoint p1, wxRealPoint p2) {
	double dx1, dx2;
	double dy1, dy2;

	dx1 = p1.x - p0.x; dx2 = p2.x - p0.x;
	dy1 = p1.y - p0.y; dy2 = p2.y - p0.y;

	/* This is basically a slope comparison: we don't do divisions because

	* of divide by zero possibilities with pure horizontal and pure
	* vertical lines.
	*/
	return ((dx1 * dy2 > dy1 * dx2) ? 1 : -1);

}
/*********************************************************************
 * FUNCTION:   IsLineIntersect (from cutil.h Intersect_FL w/ doubles)
 *
 * PURPOSE
 * Given two line segments, determine if they intersect.
 *
 * RETURN VALUE
 * TRUE if they intersect, FALSE if not.returns true if we have a line intersection.
**********************************************************************/
inline bool IsLineIntersect(wxRealPoint p1, wxRealPoint p2, wxRealPoint p3, wxRealPoint p4)
{
	return (((tacticsCCW(p1, p2, p3) * tacticsCCW(p1, p2, p4)) <= 0)
 		&& ((tacticsCCW(p3, p4, p1) * tacticsCCW(p3, p4, p2) <= 0)));

}

wxRealPoint GetLineIntersection(wxRealPoint line1point1, wxRealPoint line1point2, wxRealPoint line2point1, wxRealPoint line2point2);
double getMarkTWA(double twd, double ctm);

/**********************************************************************
 * FUNCTION getTWD
 * PURPOSE
 * Calculates True Wind Direction from boat's heading and
 * from True Wind Angle (TWA).
 * If TWA is from starboard side, give stbd = true ;
 * If TWA is from starboard side, give stbd = false,
 * The functions deals with the 359 - 0 degree problem
 * e.g. : hdt = 10, twa = 20 from port side; the TWD is returned as
 * 350 degrees.
***********************************************************************/
inline double getTWD(double twa, double hdt, bool stbd)
{
    double rettwd = stbd ? hdt + twa : hdt - twa;
    if (rettwd >= 360) rettwd -= 360;
    if (rettwd < 0) rettwd += 360;
    return rettwd;
}

double getDegRange(double max, double min);
double getSignedDegRange(double max, double min);
double rad2deg(double angle);
double deg2rad(double angle);

#endif // __TACTICSFUNCTIONS_H__
