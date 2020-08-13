/***************************************************************************
* $Id: polarcompass.h, v1.0 2016/06/07 tomBigSpeedy Exp $
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

#ifndef __PolarCompass_H__
#define __PolarCompass_H__

#include <wx/wxprec.h>
#include <wx/fileconf.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#ifndef __DERIVEDTIMEOUT_OVERRIDE__
#define __DERIVEDTIMEOUT_OVERRIDE__
#endif // __DERIVEDTIMEOUT_OVERRIDE__
#include "dial.h"

#include "ExpSmooth.h"
#include "DoubleExpSmooth.h"

#define COGRANGE 60
//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    TacticsInstrument_PolarCompass
//|
//| DESCRIPTION:
//|    This class creates a compass style control with Bearing Pointer
//|
//+------------------------------------------------------------------------------

class TacticsInstrument_PolarCompass : public DashboardInstrument_Dial
{
public:
    TacticsInstrument_PolarCompass(wxWindow *parent, wxWindowID id, wxString title, unsigned long long cap_flag);

    ~TacticsInstrument_PolarCompass(void){ SaveConfig(); }

    void SetData(unsigned long long st, double data, wxString unit, long long timestamp=0LL) override;
    void derivedTimeoutEvent(void) override;
    bool SaveConfig(void);
    double m_Bearing;
    double m_ExtraValueDTW;
    double m_CurrDir;
    double m_CurrSpeed;
    double m_currAngleStart;
    double m_TWA;
    double m_TWD;
    double m_AWA;
    double m_TWS;
    double m_Hdt;
    double m_Leeway;
    double m_PolSpd;
    double m_PolSpd_Percent;
    double m_diffCogHdt;
    double m_lat;
    double m_lon;
    double m_StW;
    double m_predictedSog;
    wxString m_BearingUnit;
    wxString m_ExtraValueDTWUnit;
    wxString m_ToWpt;
    wxString m_CurrDirUnit;
    wxString m_CurrSpeedUnit;
    wxString m_StWUnit;
    wxString m_curTack;
    wxString m_targetTack;
    wxString m_LeewayUnit;
    double m_ExpSmoothDegRange;
    double alpha_diffCogHdt;
    double m_LaylineDegRange;
    double m_COGRange[COGRANGE];
    double m_Cog;
    double m_ExpSmoothDiffCogHdt;
    double m_oldExpSmoothDiffCogHdt;
    ExpSmooth  *mExpSmDegRange;

private:
    bool LoadConfig(void);
    bool m_timeout;
    wxFileConfig     *m_pconfig;

protected:
    void DrawBackground(wxGCDC* dc);
    void DrawForeground(wxGCDC* dc);
    void DrawBearing(wxGCDC* dc);
    void DrawWindAngles(wxGCDC* dc);
    void DrawBoat(wxGCDC* dc, int cx, int cy, int radius);
    void DrawPolar(wxGCDC* dc);
    void DrawTargetxMGAngle(wxGCDC* dc);
    void DrawTargetAngle(wxGCDC* dc, double TargetAngle, wxString color1, int size);
    void DrawLaylines(wxGCDC* dc);
    virtual void DrawData(wxGCDC* dc, double value, wxString unit, wxString format, DialPositionOption position);
    virtual void Draw(wxGCDC* dc);
    void CalculateLaylineDegreeRange(void);
};

#endif // __PolarCompass_H__

