/***************************************************************************
* $Id: streamout.h, v1.0 2019/08/08 DarthVader $
*
* Project:  OpenCPN
* Purpose:  dashboard_tactics_pi plug-in streaming out
* Author:   Petri Mäkijärvi
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

#ifndef __STREAMOUT_H__
#define __STREAMOUT_H__

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

// #include <wx/dynarray.h>
// #include <wx/grid.h>
// #include <wx/filename.h>
// #include <map>

#include "instrument.h"
// #include "plugin_ids.h"

enum StreamoutSingleStateMachine {
    SSSM_STATE_UNKNOWN, SSSM_STATE_DISPLAYRELAY, SSSM_STATE_INIT };

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    TacticsInstrument_StreamoutSingle
//|
//| DESCRIPTION:
//|    This class creates a single data streamout instrument
//|
//+------------------------------------------------------------------------------
class TacticsInstrument_StreamoutSingle : public DashboardInstrument
{
public:
	TacticsInstrument_StreamoutSingle(
        wxWindow *pparent, wxWindowID id, wxString title, unsigned long long cap, wxString format,
        std::mutex &mtxNofStreamOut, int &nofStreamOut, wxString &echoStreamerShow, wxString confdir);
	~TacticsInstrument_StreamoutSingle();

	wxSize GetSize(int orient, wxSize hint);
	void SetData(unsigned long long st, double data, wxString unit);

protected:
    int               m_state;
    std::mutex       *m_mtxNofStreamOut;
    int              *m_nofStreamOut;
    wxString         *m_echoStreamerShow;
    wxString          m_data;
    wxString          m_format;
    int               m_DataHeight;
    wxString          m_confdir;
    wxString          m_configFileName;
    wxFileConfig     *m_pconfig;
    bool              m_configured;

    // From configuration file
    wxString          m_serverurl;

    bool LoadConfig(void);
    void SaveConfig(void);
    void Draw(wxGCDC* dc);
    
private :

};

#endif // not defined __STREAMOUT_H__

