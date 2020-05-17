/***************************************************************************
* $Id: DoubleExpSmooth.h, v1.0 2016/06/07 tomBigSpeedy Exp $
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

#ifndef __DOUBLEEXPSMOOTH_H__
#define __DOUBLEEXPSMOOTH_H__

/***********************************************************************************
Class for double exonential smoothing, DES
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

Note :
There is only ONE damping factor alpha in this special implementation of DES. 
The second factor beta dropped out. Search the net on the title above, there is
a theoretical description on this
Usage : 
see "Class for exponential smoothing" above

************************************************************************************/
class DoubleExpSmooth
{
public:
  DoubleExpSmooth(double newalpha);
  ~DoubleExpSmooth(void);

  double GetSmoothVal(double input);
  void SetAlpha(double newalpha);
  void SetInitVal(double init);
  double GetAlpha(void);
protected:

private:
  double alpha;
  int T;
  double SpT, oldSpT, Sp2T, oldSp2T, predPosT;

};

#endif // __DOUBLEEXPSMOOTH_H__

