/***************************************************************************
* $Id: StreamoutSingle.h, v1.0 2019/08/08 DarthVader $
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

#ifndef __STREAMOUT_H__
#define __STREAMOUT_H__
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
#include "StreamoutSchema.h"
#include "LineProtocol.h"
#include "SkData.h"

enum StreamoutSingleStateMachine {
    SSSM_STATE_UNKNOWN, SSSM_STATE_DISPLAYRELAY, SSSM_STATE_INIT, SSSM_STATE_CONFIGURED,
    SSSM_STATE_READY, SSSM_STATE_FAIL };

enum SocketThreadStateMachine {
    STSM_STATE_UNKNOWN, STSM_STATE_INIT, STSM_STATE_ERROR, STSM_STATE_CONNECTING,
    STSM_STATE_READY };

#define STSM_MAX_UNWRITTEN_FIFO_ELEMENTS_BLOCKING   20000LL // roughly 1GB of memory
#define STSM_MAX_UNWRITTEN_FIFO_ELEMENTS_UNBLOCKING 19800LL
#define SSSM_TICK_COUNT 1000 // tick for streamout class periodical jobs = 1s

enum stateFifoOverFlow {
    STSM_FIFO_OFW_UNKNOWN, STSM_FIFO_OFW_NOT_BLOCKING, STSM_FIFO_OFW_BLOCKING };

#define STSM_ALLPATHS_COUNT 65 // How long time in ticks we can be asked to subscribe to all paths (long time to allow slow paths, like barometer)


//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    TacticsInstrument_StreamoutSingle
//|
//| DESCRIPTION:
//|    This class creates a single data streamout instrument
//|
//+------------------------------------------------------------------------------
class TacticsInstrument_StreamoutSingle : public DashboardInstrument, public wxThreadHelper
{
public:
	TacticsInstrument_StreamoutSingle(
        wxWindow *pparent, wxWindowID id, wxString title, unsigned long long cap, wxString format,
        std::mutex &mtxNofStreamOut, int &nofStreamOut, wxString &echoStreamerShow, wxString confdir,
        SkData* skdata);
	~TacticsInstrument_StreamoutSingle();

	wxSize GetSize(int orient, wxSize hint);
	void SetData(unsigned long long st, double data, wxString unit, long long timestamp=0LL);
    void OnStreamOutUpdTimer(wxTimerEvent& event);
    virtual void timeoutEvent(void){};
    
protected:

    TacticsInstrument_StreamoutSingle *m_frame;
    
    int               m_state;
    wxThread         *m_thread;
    wxTimer          *m_timer;
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

    std::vector<StreamoutSchema> vSchema;
    long long         m_pushedInFifo;
    long long         m_poppedFromFifo;
    long long         m_writtenToOutput;
    int               m_stateFifoOverFlow;
    std::queue<LineProtocol> qLine;
    std::mutex        m_mtxQLine;
    int               m_stateComm;
    bool              m_cmdThreadStop;
    wxSocketClient    m_socket;
    wxString          m_threadMsg;
    int               m_cntSchemaRegisterAll;

    // From configuration file
    wxString          m_target;
    wxString          m_targetAsFilePath;
    wxString          m_api;
    wxString          m_org;
    wxString          m_bucket;
    wxString          m_precision;
    wxString          m_token;
    int               m_connectionRetry;
    int               m_linesPerWrite;
    wxString          m_timestamps;
    bool              m_stamp;
    int               m_verbosity;

    bool GetSchema(unsigned long long st, wxString UnitOrSkPath, long long msNow, StreamoutSchema& schema);
    void sLL(long long cnt, wxString& retString);
    bool LoadConfig(void);
    void SaveConfig(void);
    void Draw(wxGCDC* dc);
    void OnClose(wxCloseEvent& evt);
    wxThread::ExitCode Entry(void);
    void OnThreadUpdate(wxThreadEvent& evt);
    
private :

    SkData           *m_pSkData;

    wxDECLARE_EVENT_TABLE();

};

#endif // not defined __STREAMOUT_H__

