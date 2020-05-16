/**************************************************************************
* $Id: DoubleExpSmooth.cpp, v1.0 2016/06/07 tom_BigSpeedy Exp $
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

#include "DoubleExpSmooth.h"

/***********************************************************************************
Class for double exonential Smoothing, DES
------------------------------------------------------------------------------------
Formula taken from 
Double Exponential Smoothing: An Alternative to Kalman Filter-Based Predictive Tracking
Joseph J. LaViola Jr.
Brown University Technology Center
for Advanced Scientific Computing and Visualization
PO Box 1910, Providence, RI, 02912, USA
jjl@cs.brown.edu
--------------------------------------------------------------------------------------
T = 1;
Spt = alpha*pt + (1 - alpha)*Sptmin1;
Sp2t = alpha*Spt + (1 - alpha)*Sp2tmin1;
ptplusT = (2 + alpha*T / (1 - alpha))*Spt - (1 + alpha*T / (1 - alpha)) * Sp2t;

************************************************************************************/
DoubleExpSmooth::DoubleExpSmooth(double newalpha)
{
  alpha = newalpha;
  T = 1;
  //SmoothedValue = NAN;
  SpT = NAN;
  oldSpT = NAN;
  Sp2T = NAN;
  oldSp2T = NAN;
  predPosT = NAN;
}
/***********************************************************************************

************************************************************************************/
DoubleExpSmooth::~DoubleExpSmooth(void)
{
}
/***********************************************************************************

************************************************************************************/
double DoubleExpSmooth::GetSmoothVal(double input)
{
  
  if (std::isnan(SpT)) SpT = input;
  if (std::isnan(Sp2T)) Sp2T = input;

  oldSpT = SpT;
  oldSp2T = Sp2T;

  SpT = alpha*input + (1 - alpha)*oldSpT;
  Sp2T = alpha*SpT + (1 - alpha)*oldSp2T;
  predPosT = (2 + alpha*T / (1 - alpha))*SpT - (1 + alpha*T / (1 - alpha)) * Sp2T;
  return predPosT;
}
/***********************************************************************************

************************************************************************************/
void DoubleExpSmooth::SetAlpha(double newalpha)
{
  alpha = newalpha;
}
/***********************************************************************************

************************************************************************************/
double DoubleExpSmooth::GetAlpha()
{
  return alpha;
}
/***********************************************************************************

************************************************************************************/
void DoubleExpSmooth::SetInitVal(double init)
{
  SpT = init;
  Sp2T = init;
}

