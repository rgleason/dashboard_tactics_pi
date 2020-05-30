/***************************************************************************
* $Id: Polar.h, v1.0 2016/06/07 tomBigSpeedy Exp $
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

#ifndef __POLAR_H__
#define __POLAR_H__

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/dynarray.h>
#include <wx/grid.h>
#include <wx/filename.h>
#include <map>

#include "instrument.h"
#include "plugin_ids.h"

#include "PerformanceSingle.h"

#include "TacticsEnums.h"


//*************************************************************************************
class Polar
{

public:
	Polar(TacticsInstrument_PerformanceSingle* parent);
    Polar(tactics_pi *parent);
    ~Polar(void);
	wxFileConfig     *m_pconfig;
	struct pol
	{
	    double   winddir[WINDDIR+1];
		bool     isfix[WINDDIR+1];   //one of the values from the original polar
	} windsp[WINDSPEED+1];
	
    struct optAngle
    {
      TargetxMG tvmg_up; //upwind
      TargetxMG tvmg_dn; //downwind
    }tws[WINDSPEED + 1];

	int				mode;

	void setValue(wxString s, int row, int col);
	void reset();
	void loadPolar(wxString FilePath);        //fill the polar values from file in the lookup table
    void saveLookupTable(wxString FilePath);  //save the outcome
	void completePolar();    //complete the empty spots in the lookup table with simple average calculation
	void CalculateLineAverages(int n, int min, int max);
	void CalculateRowAverages(int i, int min, int max);
    double CalcPolarTimeToMark(double distance, double twa, double tws);
	double GetPolarSpeed(double twa, double tws);
    TargetxMG GetTargetVMGUpwind(double TWS);
    TargetxMG GetTargetVMGDownwind(double TWS);
    double GetAvgPolarSpeed(double twa, double tws);
	double Calc_VMG(double TWA, double StW);
    double Calc_CMG(double heading, double speed, double Brg);
	TargetxMG Calc_TargetVMG(double TWA, double TWS);
    TargetxMG Calc_TargetCMG(double TWS, double TWD, double BRG);
    void Calc_TargetCMG2(double TWS, double TWD, double BRG, TargetxMG *TCMG1, TargetxMG*TCMG2);
    bool isValid(void) { return m_bDataIsValid; };

private:

    wxString          logbookDataPath;
    double            dist;
    bool              m_bDataIsValid;

};

#endif // __POLAR_H__

