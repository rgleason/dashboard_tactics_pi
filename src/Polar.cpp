/**************************************************************************
* $Id: Polar.cpp, v1.0 2016/06/07 tom_BigSpeedy Exp $
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

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/math.h>
#include <wx/stdpaths.h>
#include <wx/progdlg.h>
#include <wx/gdicmn.h>
#include <wx/fileconf.h>

#include "dashboard_pi.h"

#include "nmea0183/nmea0183.h"

#include "TacticsFunctions.h"
#include "PerformanceSingle.h"
#include "Polar.h"

#include "dashboard_pi_ext.h"
#include "tactics_pi_ext.h"

/***********************************************************************************

************************************************************************************/
Polar::Polar(TacticsInstrument_PerformanceSingle* parent)
{
    m_pconfig = GetOCPNConfigObject();
    windsp[0].winddir[0] = 0;
    windsp[0].isfix[0] = false; // see below reset()
    tws[0].tvmg_up.TargetAngle = tws[0].tvmg_up.TargetSpeed = 0;
    tws[0].tvmg_dn.TargetAngle = tws[0].tvmg_dn.TargetSpeed = 0;
    reset();
    mode = 0;

    wxString s = wxFileName::GetPathSeparator();
    wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
#ifdef __WXOSX__
	wxString stdPath = std_path.GetUserConfigDir();   // should be ~/Library/Preferences
	stdPath += s + _T("opencpn");
#else
#  ifdef __WXMSW__
	wxString stdPath = std_path.GetConfigDir();
#  else // default to __WXGTK__
	wxString stdPath = std_path.GetUserDataDir();
#  endif // __WXMSW__
#endif // __WXOSX__
    wxString basePath = stdPath + s + _T("plugins") + s + _T("dashboard_tactics_pi") + s + _T("data") + s;
    logbookDataPath = basePath;

    dist = 0.0;
    m_bDataIsValid = false;
}
Polar::Polar(tactics_pi* parent)
{
    m_pconfig = GetOCPNConfigObject();
    windsp[0].winddir[0] = 0;
    windsp[0].isfix[0] = false; // see below reset()
    tws[0].tvmg_up.TargetAngle = tws[0].tvmg_up.TargetSpeed = 0;
    tws[0].tvmg_dn.TargetAngle = tws[0].tvmg_dn.TargetSpeed = 0;
    reset();
    mode = 0;

    wxString s = wxFileName::GetPathSeparator();
    wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
#ifdef __WXOSX__
	wxString stdPath = std_path.GetUserConfigDir();   // should be ~/Library/Preferences
	stdPath += s + _T("opencpn");
#else
#  ifdef __WXMSW__
	wxString stdPath = std_path.GetConfigDir();
#  else // default to __WXGTK__
	wxString stdPath = std_path.GetUserDataDir();
#  endif // __WXMSW__
#endif // __WXOSX__
    wxString basePath = stdPath + s + _T("plugins") + s + _T("dashboard_tactics_pi") + s + _T("data") + s;
    logbookDataPath = basePath;

    dist = 0.0;
    m_bDataIsValid = false;
}
/***********************************************************************************
Destructor
************************************************************************************/
Polar::~Polar(void)
{
}
/***********************************************************************************
Load a new polar file and read it into the polar array
Parameter
(input) FilePath = path to polar file;
_T("")     : open dialog and update  g_path_to_PolarFile
_T("NULL") : just initialize array (to work w/o polar)
************************************************************************************/
void Polar::loadPolar(wxString FilePath)
{
    wxString filePath = _T("NULL");
    wxString fname = _T("");

    if (FilePath == _T("")) { //input parameter empty, ask the path from the user
        wxFileDialog fdlg(
            GetOCPNCanvasWindow(),
            _("^Tactics Performance Parameters: Select a polar file"), _T(""));
        if (fdlg.ShowModal() == wxID_CANCEL)
            return;
        filePath = fdlg.GetPath();
        fname = filePath;
    }
    else {
        filePath = FilePath;
        fname = filePath;
	}

    this->reset();

    if (filePath == _T("NULL"))
        return;
    
    wxFileInputStream stream(filePath);
    wxTextInputStream in(stream);
    wxString wdirstr, wsp;

    bool first = true;
    int mode = -1, row = -1, sep = -1;
    wxArrayString WS, WSS;

    while (!stream.Eof())
    {
        int col = 0, i = 0;
        wxString s;

        wxString str = in.ReadLine();               // read line by line
        if (stream.Eof()) break;
        if (first)
        {
            WS = wxStringTokenize(str, _T(";,\t "));
            WS[0] = WS[0].Upper();
            if (WS[0].Find(_T("TWA\\TWS")) != -1 ||
                WS[0].Find(_T("TWA/TWS")) != -1 ||
                WS[0].Find(_T("TWA")) != -1)
            {
                mode = 1;
                sep = 1;
            }
            else if (WS[0].IsNumber())
            {
                mode = 2;
                sep = 1;
                col = wxAtoi(WS[0]);

                for (i = 1; i < (int)WS.GetCount(); i += 2)
                {
                    row = wxAtoi(WS[i]);
                    s = WS[i + 1];

                    if (col > WINDSPEED - 1) break;
                    if (s == _T("0") || s == _T("0.00") ||
                        s == _T("0.0") || s == _T("0.000")){
                        continue;
                    }
                    if (col < WINDSPEED + 1) {
                        setValue(s, row, col);
                    }
                }
            }
            else if (!WS[0].IsNumber()){
                continue;
            }

            if (sep == -1){
                wxMessageBox(_("Format in this file not recognised"));
                return;
            }

            first = false;
            if (mode != 0)
                continue;
        }
        if (mode == 1) // Formats OCPN/QTVlm/MAXSea/CVS 
        {
            WSS = wxStringTokenize(str, _T(";,\t "));
            if (WSS[0] == _T("0") && mode == 1)
            {
                row++;
                continue;
            }
            row = wxAtoi(WSS[0]);
            for (i = 1; i < (int)WSS.GetCount(); i++)
            {
                s = WSS[i];
                if (col > WINDSPEED - 1) break;
                if (s == _T("0") || s == _T("0.00") ||
                    s == _T("0.0") || s == _T("0.000")){
                    continue;
                }
                col = wxAtoi(WS[i]);
                setValue(s, row, col);
            }
        }

        if (mode == 2) // Format Expedition
        {
            WS = wxStringTokenize(str, _T(";,\t "));
            //x = wxAtoi(WS[0]);
            //col = (x + 1) / 2 - 1;
            col = wxAtoi(WS[0]);

            for (i = 1; i < (int)WS.GetCount(); i += 2)
            {
                //x = wxAtoi(WS[i]);
                //row = (x + 2) / 5 - 1;
                row = wxAtoi(WS[i]);
                s = WS[i + 1];
                if (col > WINDSPEED - 1) break;
                if (s == _T("0") || s == _T("0.00") ||
                    s == _T("0.0") || s == _T("0.000"))
                {
                    continue;
                }
                //if (col < 21)
                if (col < WINDSPEED + 1){
                    setValue(s, row, col);
                }
            }
        }
    }
    completePolar();
    g_path_to_PolarFile = filePath;
    //wxMessageBox(_T("Polar ") + fname + _T(" loaded"));
    m_bDataIsValid = true;
    return;
}
/***********************************************************************************
Save the lookup table created from the input file
Parameter
(input) FilePath = path to output file;
************************************************************************************/
void Polar::saveLookupTable( wxString FilePath )
{
    if ( FilePath == _T("NULL") )
        return;

    wxFileOutputStream outstream( FilePath );
    wxTextOutputStream out(outstream);

    if ( m_bDataIsValid ) {
        wxString str = _T("d/s");
        for (int i = 0; i <= WINDSPEED; i++){
            str = wxString::Format(_T("%s\t%02d"), str, i);
        }
        str = str + _T("\n");
        out.WriteString(str);               // write line by line
        for (int n = 0; n < WINDDIR; n++){
            str = wxString::Format(_T("%d"), n);
            for (int i = 0; i <= WINDSPEED; i++){
                str = wxString::Format(_T("%s\t%.2f"), str, windsp[i].winddir[n]);
            }
            str = str + _T("\n");
            out.WriteString(str);               // write line by line
        }
    }
    else {
        out.WriteString( _T("invalid data\n") );
    }
    outstream.Close();
}

/***********************************************************************************

************************************************************************************/
void Polar::setValue(wxString s, int dir, int spd)
{
  s.Replace(_T(","), _T("."));
  double speed = wxAtof(s);
  
  //if (speed > 0.0 && speed <= WINDSPEED && dir >= 0 && dir <WINDDIR)
  if ( (spd > 0) && (spd <= WINDSPEED) && (dir >= 0) && (dir <WINDDIR) )
    {
      windsp[spd].winddir[dir] = speed;
      windsp[spd].isfix[dir] = true;
      //for cmg : fill the second half of the polar
      windsp[spd].winddir[360-dir] = speed;
      windsp[spd].isfix[360- dir] = true;
    }
}
/***********************************************************************************
Clear the polar lookup table with default values
************************************************************************************/
void Polar::reset()
{
    for (int n = 0; n < WINDDIR; n++) {
        windsp[0].winddir[n] = 0;
        windsp[0].isfix[n] = false;
    }
    for (int i = 1; i <= WINDSPEED; i++) {
        windsp[i].winddir[0] = 0;
        windsp[i].isfix[0] = false;
        for (int n = 1; n < WINDDIR; n++) {
            windsp[i].winddir[n] = NAN;
            windsp[i].isfix[n] = false;
        }
    }
    for (int i = 0; i <= WINDSPEED; i++) {
        tws[i].tvmg_up.TargetAngle = tws[i].tvmg_up.TargetSpeed = 0;
        tws[i].tvmg_dn.TargetAngle = tws[i].tvmg_dn.TargetSpeed = 0;
    }
    m_bDataIsValid = false;
}

/***********************************************************************************
Complete the boat polar, i.e. create the lookup table
Calculates the intermediate values per degree & knot of the given polar
No extrapolation.
************************************************************************************/
void Polar::completePolar()
{
	for (int n = 0; n < WINDDIR; n++)
	{
		int min_index = WINDSPEED;
		int max_index = 0;
		int i = 0;
		bool ret = false;
		//get min/max index (i) with data.
		//first we fill the gaps in the lines, between existing values
		while (i <= WINDSPEED) {
			if (!std::isnan(windsp[i].winddir[n]))
			{
				if (i < min_index) min_index = i;
				if (i > max_index) max_index = i;
				ret = true;
			}
			if (ret )
                CalculateLineAverages(n, min_index, max_index);
			i++;
		}
	}
	for (int i = 0; i <= WINDSPEED; i++)
	{
		int min_index = WINDDIR;
		int max_index = 0;
		int n = 0;
		bool ret = false;
		//get min/max index (i) with data.
		//now we fill the gaps in the rows, between existing values
		while (n < WINDDIR) {
			if (!std::isnan(windsp[i].winddir[n]))
			{
				if (n < min_index) min_index = n;
				if (n > max_index) max_index = n;
				ret = true;
			}
			if ( ret )
                CalculateRowAverages(i, min_index, max_index);
			n++;
		}
	}
    //fill the TargetVMG lookup table now
    for (int i = 0; i <= WINDSPEED; i++){
      tws[i].tvmg_up = Calc_TargetVMG(45.0, (double)i);
      tws[i].tvmg_dn = Calc_TargetVMG(120.0, (double)i);
    }
    //for (int j = 0; j <= WINDSPEED; j++){
    //wxLogMessage("TWS=%d, UP: TargetAngle=%.2f, TargetSpeed=%.2f, DOWN: TargetAngle=%.2f, TargetSpeed=%.2f", j, tws[j].tvmg_up.TargetAngle, tws[j].tvmg_up.TargetSpeed, tws[j].tvmg_dn.TargetAngle, tws[j].tvmg_dn.TargetSpeed);
    //}
}
TargetxMG Polar::GetTargetVMGUpwind(double TWS)
{
  return( tws[wxRound(TWS)].tvmg_up);
}
TargetxMG Polar::GetTargetVMGDownwind(double TWS)
{
  return(tws[wxRound(TWS)].tvmg_dn);
}

/***********************************************************************************

************************************************************************************/
void Polar::CalculateLineAverages(int n, int min, int max)
{
	int j;
	int cur_min;
	j = min;
	cur_min = min;
	while (j <= max) {
		j++;
		int count = 0;
		while (j <= max && std::isnan(windsp[j].winddir[n])) // find next cell which is NOT empty
		{
			j++;
		}
		count = j - cur_min;
        for (int k = cur_min + 1, m = 1; k < cur_min + count; k++, m++){
          windsp[k].winddir[n] = windsp[cur_min].winddir[n] + (windsp[j].winddir[n] - windsp[cur_min].winddir[n]) / count * m;
//TR temp, fill 2nd half
         // windsp[k].winddir[360-n] = windsp[k].winddir[n];
        }
		cur_min = j;
	}

}
/***********************************************************************************

************************************************************************************/
void Polar::CalculateRowAverages(int i, int min, int max)
{
	int j;
	int cur_min;
	j = min;
	cur_min = min;
	while (j <= max) {
		j++;
		int count = 0;
		while (j <= max && std::isnan(windsp[i].winddir[j])) // find next cell which is NOT empty
		{
			j++;
		}
		count = j - cur_min;
		for (int k = cur_min + 1, m = 1; k < cur_min + count; k++, m++)
			windsp[i].winddir[k] = windsp[i].winddir[cur_min] + (windsp[i].winddir[j] - windsp[i].winddir[cur_min]) / count * m;

		cur_min = j;
	}

}
/***********************************************************************************
Return the polar speed with averaging of wind speed.
We're still roúnding the TWA, as this is a calculated value anyway and I doubt
it will have an accuracy < 1°.
With this simplified approach of averaging only TWS we can reduce some load ...
************************************************************************************/
double Polar::GetPolarSpeed(double twa, double tws)
{
  //original w/o averaging:
  //return (windsp[wxRound(tws)].winddir[wxRound(twa)]);
  double  fws, avspd1, avspd2;
  int twsmin, i_twa;

//wxLogMessage("-- GetPolarSpeed() - twa=%f tws=%f", twa, tws);
  if (std::isnan(twa) || std::isnan(tws))
      return NAN;
  // to do : limits to be checked (0°, 180°, etc.)
  i_twa = wxRound(twa); //the next lower full true wind angle value of the polar array
  twsmin = (int)tws; //the next lower full true wind speed value of the polar array
  fws = tws - twsmin; // factor tws (how much are we above twsmin)
  //do the vertical averaging btw. the 2 surrounding polar twa angles
  avspd1 = windsp[twsmin].winddir[i_twa] ;
  avspd2 = windsp[twsmin + 1].winddir[i_twa];
  // now do the horizontal averaging btw. the 2 surrounding polar tws values ...
  //if (std::isnan(avspd1) || std::isnan(avspd1))
    return ((std::isnan(avspd1) || std::isnan(avspd2))?NAN: avspd1 + (avspd2 - avspd1)*fws);
}
/***********************************************************************************
Get the polar speed with full averaging of the input data of both TWA and TWS.
The polar is stored as a lookup table (2dim array) in steps of 1 kt / 1°.
Instead of rounding up/down to the next full value as done in original GetPolarSpeed() we're
averaging both TWA & TWS.
Currently not used ...
************************************************************************************/
double Polar::GetAvgPolarSpeed(double twa, double tws)
{
  double fangle, fws,  avspd1, avspd2, av_Spd;
  int twsmin, twamin;

  // to do : limits to be checked (0°, 180°, etc.)
  twamin = (int)twa; //the next lower full true wind angle value of the polar array
  twsmin = (int)tws; //the next lower full true wind speed value of the polar array
  fangle = twa - twamin; //factor twa (how much are we above twamin)
  fws = tws - twsmin; // factor tws (how much are we above twsmin)
  //do the vertical averaging btw. the 2 surrounding polar twa angles
  avspd1 = windsp[twsmin].winddir[twamin] + (windsp[twsmin].winddir[twamin + 1] - windsp[twsmin].winddir[twamin])*fangle;
  avspd2 = windsp[twsmin + 1].winddir[twamin] + (windsp[twsmin + 1].winddir[twamin + 1] - windsp[twsmin + 1].winddir[twamin])*fangle;
  // now do the horizontal averaging btw. the 2 surrounding polar tws values.
  av_Spd = avspd1 + (avspd2 - avspd1)*fws;
  //wxLogMessage("TWA=%.1f,TWS=%.1f, =%f, av_Spd=%f", twa, tws, av_Spd);
  return av_Spd;
}

/***********************************************************************************
Basic VMG(Velocity made good) measured against the wind direction
************************************************************************************/
double Polar::Calc_VMG(double TWA, double StW)
{
	return fabs(StW * cos(TWA * M_PI / 180.) );
}
/***********************************************************************************
Calculate opt. VMG (angle & speed) for up- and downwind courses (w/o a bearing to a mark)
************************************************************************************/
TargetxMG Polar::Calc_TargetVMG(double TWA, double TWS)
{
	TargetxMG TVMG;
	TVMG.TargetAngle = -999;
	TVMG.TargetSpeed = -999;
	double calcvmg;
	int i_tws = wxRound(TWS);
	int k;
	if (TWA <90) { //upwind
		for (k = 1; k < 90; k++){
			if (!std::isnan(windsp[i_tws].winddir[k])){
				calcvmg = windsp[i_tws].winddir[k] * cos((double)(k*M_PI / 180.));
                if (calcvmg < 0) calcvmg = -calcvmg;
				if (calcvmg > TVMG.TargetSpeed ){
					TVMG.TargetSpeed = calcvmg;
					TVMG.TargetAngle = (double)k;
				}
			}
		}
	}
	if (TWA >= 90) {  //downwind
		for ( k = 180; k > 90; k--){
			if (!std::isnan(windsp[i_tws].winddir[k] ) ){
				calcvmg = windsp[i_tws].winddir[k] * cos((double)k*M_PI / 180.);
                if (calcvmg < 0) calcvmg = -calcvmg;
				//wxLogMessage("cosval=%f, calcvmg=%f", cosval, calcvmg);
				if (calcvmg > TVMG.TargetSpeed ) {
					TVMG.TargetSpeed = calcvmg;
					TVMG.TargetAngle = (double) k;
				}

			}
		}

	}
	if (TVMG.TargetAngle == -999)TVMG.TargetAngle = NAN;
	if (TVMG.TargetSpeed == -999)TVMG.TargetSpeed = NAN;
	return TVMG;
}
/***********************************************************************************
 Calculate CMG (Course made good) to bearing = the speed towards the bearing
************************************************************************************/
double Polar::Calc_CMG(double heading, double speed, double Brg)
{
	//double Cmg = speed * cos((heading - Brg)* M_PI / 180.);
  double Cmg = speed * cos((getDegRange(heading,Brg))* M_PI / 180.);
	//return fabs(Cmg);
    return Cmg;
}
/***********************************************************************************
Calculate opt. CMG (angle & speed) for up- and downwind courses with bearing to a mark

       TWD
 CMG    |   .  * * *
   o    .  * /       *
    \.  |*  /          *
     \  |  /            *  
      \ | /              *
       \|/               * 
                        *
************************************************************************************/

/*
  Calculate the target CMG (=VMC)
  The theoretical approach is to calculate the tangens from the bearing line to the polar curve.
  As this is not (easily) possible (or I don't know how to do), I use another approach :
  The procedure is to determine the diff-angle btw. TWD and BRG. Then we "rotate" the polar
  by this diff-angle. For the given windspeed, we can now query all boatspeeds from the polar
  in a range of -90°..diff-angle..+90° around the new vertical point (diff-angle), and find the max speed 
  with "boatspeed * cos (angle)"; the returned angle is the TWA-angle for opt. CMG
  with reference to TWD
*/

TargetxMG Polar::Calc_TargetCMG(double TWS, double TWD,  double BRG)
{
    TargetxMG TCMG,tcmg2;
    TCMG.TargetAngle = NAN;
    TCMG.TargetSpeed = -999;
    double cmg;
    int i_tws = wxRound(TWS);
    double range = getSignedDegRange(TWD, BRG);
    int vPolarAngle = wxRound(range);  //polar is rotated by this angle, this is "vertical" now
    int k = 0;
    int start = 0;
    int iIargetAngle = -999;
    start = vPolarAngle - 90; 
    if (start < 0) start += 360;  // oder 180 ?
    for (k = 0; k <= 180; k++){
        int curAngle = k + start;
        if (curAngle > 359) curAngle -= 360;
        int polang = curAngle;
        double diffAngle = curAngle - range;
        if (diffAngle > 359) diffAngle -= 360;
        if (diffAngle < -359) diffAngle += 360;
        if (!std::isnan(windsp[i_tws].winddir[polang])){
            cmg = windsp[i_tws].winddir[polang] * cos(diffAngle*M_PI / 180.);
            if (cmg > TCMG.TargetSpeed) {
                TCMG.TargetSpeed = cmg;
                iIargetAngle = curAngle;
            }
        }
    }
    if (TCMG.TargetSpeed == -999)
        TCMG.TargetSpeed = NAN;
    if (iIargetAngle != -999)
        TCMG.TargetAngle = (double)iIargetAngle;
    if (TCMG.TargetAngle > 180)
        TCMG.TargetAngle = 360. - TCMG.TargetAngle;
    return TCMG;
}
/**********************************************************************************
in certain cases there exists a second, lower cmg on the other tack
This routine returns both cmg's if available, otherwise NAN

in the small chart below :
cmg-max : is the higher of both cmg's, *TCMGMax in the routine below
cmg-2   : the second possible cmg
in general :
cmg = boat_speed*cos(hdg - brg)

HDG     : target - heading, based on polar
BRG     : Bearing to waypoint
TWD     : True Wind Direction

boat_speed = boat_speed at target-hdg = speed from polar

As the polar is rotated now (polar-0° is in TWD direction)--> hdg = polarangle + diffangle
with diffangle = angle btw.TWD and BRG

                ^
       TWD      | BRG   / HDG
         \      |______x______cmg-max
cmg-min   \     |  *  /   *
   __x_____\____|*   /      *
  *     *   \  *|   /        *
 *          *\* |  /         *
 *            \ | /         *
  *            \|/         *
   *            \         *
     *           \      *
        *         *   *
           *   *

**********************************************************************************************/
void Polar::Calc_TargetCMG2(double TWS, double TWD, double BRG, TargetxMG *TCMGMax, TargetxMG* TCMGMin)
{
  TargetxMG* TCMG1 = new TargetxMG;
  TargetxMG* TCMG2 = new TargetxMG;
  TCMG1->TargetAngle = -999;
  TCMG1->TargetSpeed = -999;
  TCMG2->TargetAngle = -999;
  TCMG2->TargetSpeed = -999;
  double cmg;

  int i_tws = wxRound(TWS);  //still rounding here, not averaging ...to be done
  // wxLogMessage("-- Calc_TargetCMG2() - range?");
  double range = getSignedDegRange(TWD, BRG);
  // wxLogMessage("range =%f", range);
  double diffAngle;
//  int vPolarAngle = wxRound(range);  //polar is rotated by this angle, this is "vertical" now
  int k = 0;
  int curAngle;
  int start = 0;
  //start = vPolarAngle - 180;
  start = 0;
//  if (start < 0) start += 360;  // oder 180 ?
  for (k = 0; k <= 180; k++){
    curAngle = k + start;
    if (curAngle > 359) curAngle -= 360;
    diffAngle = curAngle - range;
    if (diffAngle > 359) diffAngle -= 360;
    if (diffAngle < -359) diffAngle += 360;
    if (!std::isnan(windsp[i_tws].winddir[curAngle])){
      cmg = windsp[i_tws].winddir[curAngle] * cos(diffAngle*M_PI / 180.);
      if (cmg > TCMG1->TargetSpeed){
        TCMG1->TargetSpeed = cmg;
        TCMG1->TargetAngle = (double)curAngle;
       }
    }
  }
  if (TCMG1->TargetSpeed <= 0){
    TCMG1->TargetSpeed = -999;
    TCMG1->TargetAngle = NAN;
  }

//  start = vPolarAngle ;
  start = 180;
//  if (start < 0) start += 360;  // oder 180 ?
  for (k = 0; k <= 180; k++){
    curAngle = k + start;
    if (curAngle > 359) curAngle -= 360;
    diffAngle = curAngle - range;
    if (diffAngle > 359) diffAngle -= 360;
    if (diffAngle < -359) diffAngle += 360;
    if (!std::isnan(windsp[i_tws].winddir[curAngle])){
      cmg = windsp[i_tws].winddir[curAngle] * cos(diffAngle*M_PI / 180.);
      if (cmg > TCMG2->TargetSpeed) {
        TCMG2->TargetSpeed = cmg;
        TCMG2->TargetAngle = (double)curAngle;
      }
    }
  }
  if (TCMG2->TargetSpeed <= 0){
    TCMG2->TargetSpeed = -999;
    TCMG2->TargetAngle = NAN;
  }
  if (TCMG1->TargetSpeed > TCMG2->TargetSpeed){
    TCMGMax->TargetSpeed = TCMG1->TargetSpeed;
    TCMGMax->TargetAngle = TCMG1->TargetAngle;
    TCMGMin->TargetAngle = TCMG2->TargetAngle;
    TCMGMin->TargetSpeed = TCMG2->TargetSpeed;
  }
  else {
    TCMGMax->TargetSpeed = TCMG2->TargetSpeed;
    TCMGMax->TargetAngle = TCMG2->TargetAngle;
    TCMGMin->TargetAngle = TCMG1->TargetAngle;
    TCMGMin->TargetSpeed = TCMG1->TargetSpeed;
  }
  if (TCMGMax->TargetSpeed == -999) TCMGMax->TargetSpeed = NAN;
  if (TCMGMin->TargetSpeed == -999) TCMGMin->TargetSpeed = NAN;

}
