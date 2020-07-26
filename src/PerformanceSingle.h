/***************************************************************************
* $Id: PerformanceSingle.h, v1.0 2016/06/07 tomBigSpeedy Exp $
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

#ifndef __PERFORMANCESINGLE_H__
#define __PERFORMANCESINGLE_H__

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/dynarray.h>
#include <wx/grid.h>
#include <wx/filename.h>
#include <map>

#include "instrument.h"
#include "plugin_ids.h"

#include "DashboardWindow.h"

#include "DoubleExpSmooth.h"
#include "avg_wind.h"
#include "TacticsFunctions.h"

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    TacticsInstrument_Performance
//|
//| DESCRIPTION:
//|    This class creates a simple Performance Instrument
//|
//+------------------------------------------------------------------------------
class TacticsInstrument_PerformanceSingle : public DashboardInstrument
{
public:
    TacticsInstrument_PerformanceSingle(DashboardWindow *pparent, wxWindowID id, wxString title, unsigned long long cap, wxString format);
    ~TacticsInstrument_PerformanceSingle(){}

    wxSize GetSize(int orient, wxSize hint);
    void SetData(unsigned long long st, double data, wxString unit, long long timestamp=0LL) override;
    void timeoutEvent(void) override;
    void SetDisplayType(int displaytype);
    double            mTWS;
    double            mTWA;
    double            mSTW;
    double            mCMG;
    double            mSOG;
    double            mCOG;
    double            mBRG;
    double            mHDT;
    double            mTWD;
    double            m_lat;
    double            m_lon;
    wxString          stwunit;
    int               m_displaytype;
    bool              m_twamarkUseShortAvg;

protected:
    wxString          m_data;
    wxString          m_format;
    int               m_DataHeight;
    
    void Draw(wxGCDC* dc);
    bool LoadConfig(void);

private :
    wxFileConfig      *m_pconfig;
    DashboardWindow   *m_pparent;
};

#endif // __PERFORMANCESINGLE_H__
