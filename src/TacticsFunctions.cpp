/**************************************************************************
* $Id: Performance.cpp, v1.0 2016/06/07 tom_BigSpeedy Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Thomas Rauch
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
#include <map>
#include <cmath>
using namespace std;

#include <wx/gdicmn.h>

#include "TacticsFunctions.h"

// Tactics functions

/*********************************************************************
 * FUNCTION GetLineIntersection
 * PURPOSE
 * Calculate Line intersection between 2 lines, each described by 2 points
 * return lat/lon of intersection point
 * basic calculation:
 * int p1[] = { -4,  5, 1 };
 * int p2[] = { -2, -5, 1 };
 * int p3[] = { -6,  2, 1 };
 * int p4[] = {  5,  4, 1 };
 * int l1[3], l2[3], s[3];
 * double sch[2];
 * l1[0] = p1[1] * p2[2] - p1[2] * p2[1];
 * l1[1] = p1[2] * p2[0] - p1[0] * p2[2];
 * l1[2] = p1[0] * p2[1] - p1[1] * p2[0];
 * l2[0] = p3[1] * p4[2] - p3[2] * p4[1];
 * l2[1] = p3[2] * p4[0] - p3[0] * p4[2];
 * l2[2] = p3[0] * p4[1] - p3[1] * p4[0];
 * s[0] = l1[1] * l2[2] - l1[2] * l2[1];
 * s[1] = l1[2] * l2[0] - l1[0] * l2[2];
 * s[2] = l1[0] * l2[1] - l1[1] * l2[0];
 * sch[0] = (double)s[0] / (double)s[2];
 * sch[1] = (double)s[1] / (double)s[2];
**********************************************************************/
wxRealPoint GetLineIntersection(wxRealPoint line1point1, wxRealPoint line1point2, wxRealPoint line2point1, wxRealPoint line2point2)
{
	wxRealPoint intersect;
	intersect.x = -999.;
	intersect.y = -999.;
	if (IsLineIntersect(line1point1, line1point2, line2point1, line2point2)){
		double line1[3], line2[3], s[3];
		line1[0] = line1point1.y * 1. - 1. * line1point2.y;
		line1[1] = 1. * line1point2.x - line1point1.x * 1.;
		line1[2] = line1point1.x * line1point2.y - line1point1.y * line1point2.x;
		line2[0] = line2point1.y * 1. - 1. * line2point2.y;
		line2[1] = 1. * line2point2.x - line2point1.x * 1.;
		line2[2] = line2point1.x * line2point2.y - line2point1.y * line2point2.x;
		s[0] = line1[1] * line2[2] - line1[2] * line2[1];
		s[1] = line1[2] * line2[0] - line1[0] * line2[2];
		s[2] = line1[0] * line2[1] - line1[1] * line2[0];
		intersect.x = s[0] / s[2];
		intersect.y = s[1] / s[2];
	}
	return intersect;
}

/**********************************************************************
 * FUNCTION getMarkTWA
 * PURPOSE
 * Returns the (smaller) TWA of a given TWD and Course.
 * getMarkTWAUsed for Target-CMG calculation.
 * It covers the 359 - 0 degree problem
 * e.g. : TWD = 350, ctm = 10; the TWA is returned as 20 degrees
 * (and not 340 if we'd do a simple TWD - ctm)
***********************************************************************/
double getMarkTWA(double twd, double ctm)
{
	double val, twa;
	if (twd > 180)
	{
		val = twd - 180;
		if (ctm < val)
			twa = 360 - twd + ctm;
		else
			twa = twd > ctm ? twd - ctm : ctm - twd;
	}
	else
	{
		val = twd + 180;
		if (ctm > val)
			twa = 360 - ctm + twd;
		else
			twa = twd > ctm ? twd - ctm : ctm - twd;
	}
	return twa;
}

/**********************************************************************
 * FUNCTION getDegRange
 * PURPOSE
 * Returns the (smaller) degree range of 2 angular values
 * on the compass rose (without sign)
 * It covers the 359 - 0 degree problem
 * e.g. : max = 350, min = 10; the rage is returned as 20 degrees
 * (and not 340 if we'd do a simple max - min)
**********************************************************************/
double getDegRange(double max, double min)
{
	double val, range;
	if (max > 180)
	{
		val = max - 180;
		if (min < val)
			range = 360 - max + min;
		else
			range = max > min ? max - min : min - max;
	}
	else
	{
		val = max + 180;
		if (min > val)
			range = 360 - min + max;
		else
			range = max > min ? max - min : min - max;
	}
	return range;
}
/**********************************************************************
 * FUNCTION getSignedDegRange
 * PURPOSE
 * Returns the (smaller) signed degree range of 2 angular values
 * on the compass rose (clockwise is +)
 * It covers the 359 - 0 degree problem
 * e.g. : fromAngle = 350, toAngle = 10; the range is returned as +20 degrees
 * (and not 340 if we'd do a simple fromAngle - toAngle)
***********************************************************************/
double getSignedDegRange(double fromAngle, double toAngle)
{
	double val, range;
	if (fromAngle > 180)
	{
		val = fromAngle - 180;
		if (toAngle < val)
			range = 360 - fromAngle + toAngle;
		else
			range = toAngle - fromAngle;
	}
	else
	{
		val = fromAngle + 180;
		if (toAngle > val)
			range = -(360 - toAngle + fromAngle);
		else
			range = toAngle - fromAngle;
	}
	return range;
}

/**********************************************************************
 * FUNCTION rad2deg
 * PURPOSE
 * Worker function radians to degrees conversion
***********************************************************************/
double rad2deg(double angle)
{
      return angle*180.0/M_PI;
}

/**********************************************************************
 * FUNCTION deg2rad
 * PURPOSE
 * Worker function degrees to radians conversion
***********************************************************************/
double deg2rad(double angle)
{
      return angle/180.0*M_PI;
}

