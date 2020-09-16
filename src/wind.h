/******************************************************************************
 * $Id: wind.h, v1.0 2010/08/05 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Jean-Eudes Onfray
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */

#ifndef __Wind_H__
#define __Wind_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#ifndef __DERIVEDTIMEOUT_OVERRIDE__
#define __DERIVEDTIMEOUT_OVERRIDE__
#endif // __DERIVEDTIMEOUT_OVERRIDE__
#include "dial.h"

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    DashboardInstrument_Wind
//|
//| DESCRIPTION:
//|    This class creates a wind style control
//|
//+------------------------------------------------------------------------------
class DashboardInstrument_Wind: public DashboardInstrument_Dial
{
public:
    DashboardInstrument_Wind( wxWindow *parent, wxWindowID id, wxString title,
                                             unsigned long long cap_flag
        );
    void derivedTimeoutEvent(void) override {};
    ~DashboardInstrument_Wind(void){}

private:

protected:
    void DrawBackground(wxGCDC* dc) override;
};

class DashboardInstrument_WindCompass: public DashboardInstrument_Dial
{
public:
    DashboardInstrument_WindCompass( wxWindow *parent, wxWindowID id, wxString title,
                                     unsigned long long cap_flag
        );
    void derivedTimeoutEvent(void) override {};
    ~DashboardInstrument_WindCompass(void){}

private:

protected:
    void DrawBackground(wxGCDC* dc);
};

class DashboardInstrument_TrueWindAngle: public DashboardInstrument_Dial
{
public:
    DashboardInstrument_TrueWindAngle( wxWindow *parent, wxWindowID id, wxString title,
                                       unsigned long long cap_flag
        );
    void derivedTimeoutEvent(void) override {};
    ~DashboardInstrument_TrueWindAngle(void){}

private:

protected:

    void DrawBackground(wxGCDC* dc) override;
};
/*****************************************************************************
Apparent & True wind angle combined in one dial instrument
Author: Thomas Rauch
******************************************************************************/
class DashboardInstrument_AppTrueWindAngle : public DashboardInstrument_Dial
{
public:
	DashboardInstrument_AppTrueWindAngle(wxWindow *parent, wxWindowID id, wxString title,
                                         unsigned long long cap_flag
        );
    void derivedTimeoutEvent(void) override;
	~DashboardInstrument_AppTrueWindAngle(void){}
    void SetData(
        unsigned long long st, double data, wxString unit,
        long long timestamp=0LL) override;

private:

protected:
	double m_MainValueApp;
    double m_MainValueTrue;
	double m_ExtraValueApp;
    double m_ExtraValueTrue;
    double m_TWD;
    wxString m_TWDUnit;
	wxString m_ExtraValueAppUnit;
    wxString m_ExtraValueTrueUnit;
    wxString m_MainValueAppUnit;
    wxString m_MainValueTrueUnit;
	DialPositionOption m_MainValueOption1;
    DialPositionOption m_MainValueOption2;
    DialPositionOption m_ExtraValueOption1;
    DialPositionOption m_ExtraValueOption2;
	void DrawBackground(wxGCDC* dc) override;
	virtual void Draw(wxGCDC* dc) override;
	virtual void DrawForeground(wxGCDC* dc) override;
	virtual void DrawData(
        wxGCDC* dc, double value, wxString unit, wxString format,
        DialPositionOption position) override;


};

#endif // __Wind_H__

