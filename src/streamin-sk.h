/***************************************************************************
* $Id: streamout.h, v1.0 2019/08/08 DarthVader $
*
* Project:  OpenCPN
* Purpose:  dashboard_tactics_pi plug-in streaming out
* Author:   Petri Makijarvi
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

#ifndef __STREANINSK_H__
#define __STREANINSK_H__
using namespace std;
#include <mutex>
#include <vector>
#include <queue>


#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/thread.h>
#include <wx/socket.h>

#include "instrument.h"
class DashboardWindow;

enum StreamInSkSingleStateMachine {
    SSKM_STATE_UNKNOWN, SSKM_STATE_DISPLAYRELAY, SSKM_STATE_INIT, SSKM_STATE_CONFIGURED,
    SSKM_STATE_READY, SSKM_STATE_FAIL };

enum SocketInSkThreadStateMachine {
    SKTM_STATE_UNKNOWN, SKTM_STATE_INIT, SKTM_STATE_ERROR, SKTM_STATE_CONNECTING,
    SKTM_STATE_READY };

#define SSKM_TICK_COUNT 1000 // tick for streamout class periodical jobs = 1s
#define SSKM_START_GRACE_COUNT 15 // tick is not accurate at startup

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    TacticsInstrument_StreamInSkSingle
//|
//| DESCRIPTION:
//|    This class creates a single data stream in instrument
//|
//+------------------------------------------------------------------------------
class TacticsInstrument_StreamInSkSingle : public DashboardInstrument, public wxThreadHelper
{
public:
	TacticsInstrument_StreamInSkSingle(
        DashboardWindow *pparent, wxWindowID id, wxString title, unsigned long long cap, wxString format,
        std::mutex &mtxNofStreamInSk, int &nofStreamInSk, wxString &echoStreamerInSkShow, wxString confdir);
	~TacticsInstrument_StreamInSkSingle();

	wxSize GetSize(int orient, wxSize hint);
    void sLL(long long cnt, wxString& retString);
    void SetData(unsigned long long st, double data, wxString unit, long long timestamp=0LL){};
    void OnStreamInSkUpdTimer(wxTimerEvent& event);
    void SetNMEASentence(wxString& delta);

protected:

    TacticsInstrument_StreamInSkSingle *m_frame;

    int               m_state;
    wxThread         *m_thread;
    wxTimer          *m_timer;
    std::mutex       *m_mtxNofStreamInSk;
    int              *m_nofStreamInSk;
    wxString         *m_echoStreamerInSkShow;
    wxString          m_data;
    wxString          m_format;
    int               m_DataHeight;
    wxString          m_confdir;
    wxString          m_configFileName;
    bool              m_configured;

    int               m_stateComm;
    int               m_updatesSent;
    int               m_startGraceCnt;
    bool              m_cmdThreadStop;
    wxString          m_threadMsg;

    // From configuration file
    wxString          m_source;
    wxString          m_api;
    int               m_connectionRetry;
    wxString          m_timestamps;
    bool              m_stamp;
    int               m_verbosity;

    bool LoadConfig(void);
    void SaveConfig(void);
    void Draw(wxGCDC* dc);
    void OnClose(wxCloseEvent& evt);
    wxThread::ExitCode Entry(void);
    void OnThreadUpdate(wxThreadEvent& evt);
    
private :

    wxFileConfig     *m_pconfig;
    DashboardWindow  *m_pparent;

    wxDECLARE_EVENT_TABLE();

};

#endif // not defined __STREANINSK_H__

