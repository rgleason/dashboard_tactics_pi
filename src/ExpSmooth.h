/***************************************************************************
* $Id: ExpSmooth.h, v1.0 2016/06/07 tomBigSpeedy Exp $
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

#ifndef __EXPSMOOTH_H__
#define __EXPSMOOTH_H__


/*************************************************************************************
Class for exponential smoothing

Usage :
ExpSmooth       *mp_expsmooth;
double alpha,  unsmoothed_data, smoothed_val;
...
mp_expsmooth = new ExpSmooth(alpha);
start_of_any_loop{
  smoothed_val = mp_expsmooth->GetSmoothVal(unsmoothed_data);
  optional:
  mp_expsmooth->SetAlpha(new_alpha);
}
*************************************************************************************/
class ExpSmooth
{
public:
	ExpSmooth(double newalpha);
	~ExpSmooth(void);

	double GetSmoothVal(double input);
	void SetAlpha(double newalpha);
	void SetInitVal(double init);
	double GetAlpha(void);
protected:

private:
	double alpha;
	double oldSmoothedValue;
	double SmoothedValue;

};

#endif // __EXPSMOOTH_H__

