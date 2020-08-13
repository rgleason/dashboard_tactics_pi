/**************************************************************************
* $Id: ExpSmooth.cpp, v1.0 2016/06/07 tom_BigSpeedy Exp $
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

#include "ExpSmooth.h"


/***********************************************************************************
simple class for exonential smoothing
************************************************************************************/
ExpSmooth::ExpSmooth(double a)
{
	alpha = a;
	SmoothedValue = NAN;
	oldSmoothedValue = NAN;
}
/***********************************************************************************

************************************************************************************/
ExpSmooth::~ExpSmooth(void)
{
}
/***********************************************************************************

************************************************************************************/
double ExpSmooth::GetSmoothVal(double input)
{
	if (std::isnan(SmoothedValue)) SmoothedValue = input;
	oldSmoothedValue = SmoothedValue;
	SmoothedValue = alpha*input + (1 - alpha)*oldSmoothedValue;
	return SmoothedValue;
}
/***********************************************************************************

************************************************************************************/
void ExpSmooth::SetAlpha(double newalpha)
{
	alpha = newalpha;
}
/***********************************************************************************

************************************************************************************/
double ExpSmooth::GetAlpha()
{
	return alpha;
}
/***********************************************************************************

************************************************************************************/
void ExpSmooth::SetInitVal(double init)
{
	SmoothedValue = init;
	oldSmoothedValue = init;
}

