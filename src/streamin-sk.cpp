/**************************************************************************
* $Id: streamout.cpp, v1.0 2019/08/08 DarthVader Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Petri Makijarvi
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
#include <limits>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/wfstream.h> 
#include <wx/fileconf.h>
#include <wx/socket.h>

#include "streamout.h"
#include "ocpn_plugin.h"
#include "wx/jsonreader.h"
#include "plugin_ids.h"

extern int g_iDashWindSpeedUnit;
extern int g_iDashSpeedUnit;
extern int g_iSpeedFormat;

wxBEGIN_EVENT_TABLE (TacticsInstrument_StreamInSkSingle, DashboardInstrument)
   EVT_THREAD (myID_THREAD_SK_IN, TacticsInstrument_StreamInSkSingle::OnThreadUpdate)
   EVT_TIMER (myID_TICK_SK_IN, TacticsInstrument_StreamInSkSingle::OnStreamOutUpdTimer)
   EVT_CLOSE (TacticsInstrument_StreamInSkSingle::OnClose)
wxEND_EVENT_TABLE ()

// ----------------------------------------------------------------
//
//    TacticsInstrument_StreamInSkSingle
//
//----------------------------------------------------------------
TacticsInstrument_StreamInSkSingle::TacticsInstrument_StreamInSkSingle(
    wxWindow *pparent, wxWindowID id, wxString title, unsigned long long cap_flag, wxString format,
    std::mutex &mtxNofStreamInSk, int &nofStreamInSk, wxString &echoStreamerShow, wxString confdir)
	:DashboardInstrument(pparent, id, title, cap_flag)
{
    m_frame = this;
    std::unique_lock<std::mutex> lck_mtxNofStreamInSk( mtxNofStreamInSk);
    nofStreamInSk++;
    m_mtxNofStreamInSk = &mtxNofStreamInSk;
    m_nofStreamInSk = &nofStreamInSk;
    m_echoStreamerShow = &echoStreamerShow;
    m_state = SSKM_STATE_UNKNOWN;

    wxString emptyStr = wxEmptyString;
    emptyStr = emptyStr.wc_str();

    m_data  = emptyStr;
    if ( nofStreamInSk > 1) {
        wxString checkHalt = echoStreamerShow.wc_str();
        wxString isHalt = L"\u2013 HALT \u2013";
        if ( !checkHalt.IsSameAs( isHalt ) ) {
            m_state = SSKM_STATE_DISPLAYRELAY;
            m_data = echoStreamerShow;
            return;
        } // check against case that there is halted slave displays and this is a restart
    }
    m_state = SSKM_STATE_INIT;
    m_data += L"\u2013 \u2013 \u2013";
    echoStreamerShow = m_data;
    m_format = format;
    m_DataHeight = 0;
    m_confdir = confdir;
    m_pconfig = GetOCPNConfigObject();
    m_configFileName = wxEmptyString;

    m_source = emptyStr;
    m_sourceAsFilePath = emptyStr;
    m_linesPerWrite = 0;
    m_api = emptyStr;
    m_org = emptyStr;
    m_bucket = emptyStr;
    m_precision = emptyStr;
    m_token = emptyStr;
    m_connectionRetry = 0;
    m_timestamps = emptyStr;
    m_stamp = true;
    m_verbosity = 0;

    m_configured = LoadConfig();

    if ( !m_configured )
        return;
    m_state = SSKM_STATE_CONFIGURED;

    m_pushedInFifo    = 0LL;
    m_poppedFromFifo  = 0LL;
    m_writtenToOutput = 0LL;
    m_stateFifoOverFlow = SKTM_FIFO_OFW_UNKNOWN;
    std::unique_lock<std::mutex> init_m_mtxQLine( m_mtxQLine, std::defer_lock );
    m_stateComm = SKTM_STATE_UNKNOWN;
    m_cmdThreadStop = false;
    m_threadMsg = emptyStr;
    if ( CreateThread() != wxTHREAD_NO_ERROR ) {
        if ( m_verbosity > 0)
            wxLogMessage ("dashboard_tactics_pi: Delta Streamer FAILED : Signal K Delta Streamer : could not create communication thread.");
        m_state = SSKM_STATE_FAIL;
        return;
    } // will not talk
    m_thread = GetThread();
    m_thread->SetPriority( ((wxPRIORITY_MAX * 9) / 10) );
    if ( m_thread->Run() != wxTHREAD_NO_ERROR ) {
        if ( m_verbosity > 0)
            wxLogMessage ("dashboard_tactics_pi: Delta Streamer FAILED : Signal K Delta Streamer: cannot run communication thread.");
        m_state = SSKM_STATE_FAIL;
        return;
    }
    m_timer = new wxTimer( this, myID_TICK_SK_IN );
    m_timer->Start( SSKM_TICK_COUNT, wxTIMER_CONTINUOUS );
    m_state = SSKM_STATE_READY;
}
/***********************************************************************************

************************************************************************************/
TacticsInstrument_StreamInSkSingle::~TacticsInstrument_StreamInSkSingle()
{
    std::unique_lock<std::mutex> lck_mtxNofStreamInSk( *m_mtxNofStreamInSk);
    (*m_nofStreamInSk)--;
    if ( m_state == SSKM_STATE_DISPLAYRELAY )
        return;
    this->m_timer->Stop();
    delete this->m_timer;
    m_data = L"\u2013 HALT \u2013";
    *m_echoStreamerShow = m_data;
    SaveConfig();
    if ( GetThread() ) {
        if ( m_thread->IsRunning() ) {
            m_cmdThreadStop = true;
            m_thread->Wait();
        }
    }
}
/***********************************************************************************

************************************************************************************/
wxSize TacticsInstrument_StreamInSkSingle::GetSize(int orient, wxSize hint)
{
	wxClientDC dc(this);
	int w;
	dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
	dc.GetTextExtent(_T("000"), &w, &m_DataHeight, 0, 0, g_pFontData);

	if (orient == wxHORIZONTAL) {
		return wxSize(DefaultWidth, wxMax(hint.y, m_TitleHeight + m_DataHeight));
	}
	else {
		return wxSize(wxMax(hint.x, DefaultWidth), m_TitleHeight + m_DataHeight);
	}
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamInSkSingle::Draw(wxGCDC* dc)
{
	wxColour cl;
#ifdef __WXMSW__
	wxBitmap tbm(dc->GetSize().x, m_DataHeight, -1);
	wxMemoryDC tdc(tbm);
	wxColour c2;
	GetGlobalColor(_T("DASHB"), &c2);
	tdc.SetBackground(c2);
	tdc.Clear();

	tdc.SetFont(*g_pFontData);
	GetGlobalColor(_T("DASHF"), &cl);
	tdc.SetTextForeground(cl);

	tdc.DrawText(m_data, 10, 0);

	tdc.SelectObject(wxNullBitmap);

	dc->DrawBitmap(tbm, 0, m_TitleHeight, false);
#else
	dc->SetFont(*g_pFontData);
	GetGlobalColor(_T("DASHF"), &cl);
	dc->SetTextForeground(cl);

	dc->DrawText(m_data, 10, m_TitleHeight);

#endif

}
/***********************************************************************************

************************************************************************************/
bool TacticsInstrument_StreamInSkSingle::GetSchema(
    unsigned long long st, long long msNow, sentenceSchema &schema)
{
    for ( unsigned int i = 0; i < vSchema.size(); i++ ) {
        if ( vSchema[i].st == st ) {
            schema = vSchema[i];
            if ( !schema.bStore )
                return false;
            if ( schema.iInterval == 0 ) {
                schema.lastTimeStamp = msNow;
                vSchema[i].lastTimeStamp = msNow;
                return true;
            }
            long long timeLapse = (msNow - schema.lastTimeStamp) / 1000;
            if ( static_cast<long long>(vSchema[i].iInterval) <= timeLapse ) {
                schema.lastTimeStamp = msNow;
                vSchema[i].lastTimeStamp = msNow;
                return true;
            }
        } 
    }
    return false;
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamInSkSingle::sLL(long long cnt, wxString &retString)
{
    wxLongLong wxLL = cnt;
    wxString sBuffer = wxLL.ToString();
    retString = sBuffer.wc_str();
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamInSkSingle::OnClose( wxCloseEvent &evt )
{
    if ( m_state == SSKM_STATE_DISPLAYRELAY )
        return;
    if ( m_verbosity > 1)
        wxLogMessage ("dashboard_tactics_pi: OnClose() : received CloseEvent, waiting on comm. thread.");
    if ( GetThread() ) {
        if ( m_thread->IsRunning() ) {
            m_cmdThreadStop = true;
            m_thread->Wait();
        }
    }
    Destroy();
}
/***********************************************************************************
/***********************************************************************************

************************************************************************************/
wxThread::ExitCode TacticsInstrument_StreamInSkSingle::Entry( )
{
#define giveUpConnectionRetry100ms(__MS_100__) int waitMilliSeconds = 0; \
    while ( (!m_thread->TestDestroy()) && (waitMilliSeconds < (m_connectionRetry * __MS_100__ * 100)) ) { \
    wxMilliSleep( 100 ); \
    waitMilliSeconds += 100; \
}
#define __NOT_STOP_THREAD__ (!m_thread->TestDestroy() && !m_cmdThreadStop)
#define __STOP_THREAD__ (m_thread->TestDestroy() || m_cmdThreadStop)
#define __CNTROUT__ sLL (  m_writtenToOutput, sWrittenToOutput ); \
                    m_data = sWrittenToOutput; *m_echoStreamerShow = m_data
#define __INCR_CNTROUT__ m_writtenToOutput = m_writtenToOutput + 1LL; \
                         sLL (  m_writtenToOutput, sWrittenToOutput ); \
                         m_data = sWrittenToOutput; *m_echoStreamerShow = m_data


    m_stateComm = SKTM_STATE_INIT;
    m_stateFifoOverFlow = SKTM_FIFO_OFW_NOT_BLOCKING;

    wxSocketClient *socket  = NULL;
    wxIPV4address  *address = NULL;
    wxFile         *file    = NULL;
    bool            fileOp  = true;
    wxString        sWrittenToOutput = wxEmptyString;
    size_t          wFdOut  = 0;
    wxThreadEvent event( wxEVT_THREAD, myID_THREAD_SK_IN );

    if ( m_sourceAsFilePath.IsEmpty() ) { 
        wxSocketBase::Initialize();
        socket = new wxSocketClient();
        socket->SetTimeout( 5 );
        address = new wxIPV4address();
        wxUniChar separator = 0x3a;
        address->Hostname(m_source.BeforeFirst(separator));
        address->Service(m_source.AfterFirst(separator));
        if ( m_verbosity > 1) {
            m_threadMsg = wxString::Format(
                "dashboard_tactics_pi: Delta Streamer : SKTM_STATE_INIT : (%s:%s)",
                m_source.BeforeFirst(separator), m_source.AfterFirst(separator));
            wxQueueEvent( m_frame, event.Clone() );
        }
        fileOp = false;
    } // then HTTP API
    else {
        file = new wxFile( m_sourceAsFilePath, wxFile::write );
        if ( !file->IsOpened() ) {
            if ( m_verbosity > 0) {
                m_threadMsg = wxString::Format(
                    "dashboard_tactics_pi: ERROR : Delta Streamer : opening file for writing : %s",
                    m_sourceAsFilePath);
                wxQueueEvent( m_frame, event.Clone() );
            } // then failed to open the file for writing
            wxMilliSleep( 1000 );
            return (wxThread::ExitCode)0;
        } // then the file opening did not quite succeeded
    } // else file based Line Protocol 

    wxString sCnxPrg[3];
    sCnxPrg[0] = L"\u2192 \u2013 \u2013";
    sCnxPrg[1] = L"\u2013 \u2192 \u2013";
    sCnxPrg[2] = L"\u2013 \u2013 \u2192";
    int iCnxPrg = 2;

    wxString header = wxEmptyString;
    header += "POST ";
    header += "/api/";
    header += m_api;
    header += "/write?org=";
    header += m_org;
    header += "&bucket=";
    header += m_bucket;
    header += "&precision=";
    header += m_precision;
    header += " HTTP/1.1\r\n";

    header += "Host: ";
    header += m_source;
    header += "\r\n";	

    header += "User-Agent: OpenCPN/5.0\r\n";
    header += "Accept: */*\r\n";

    header += "Authorization: Token ";
    header += m_token;
    header += "\r\n";
    header += "Content-Type: application/x-www-form-urlencoded";
    header += "\r\n";
    header += "Connection: keep-alive";
    header += "\r\n";
    header += "Content-Length: "; // from this starts the dynamic part

    std::unique_lock<std::mutex> mtxQLine( m_mtxQLine, std::defer_lock );
    m_writtenToOutput = 0LL;

    if ( fileOp )
        m_stateComm = SKTM_STATE_READY;
    else
        m_stateComm = SKTM_STATE_CONNECTING;

    while (__NOT_STOP_THREAD__) {

        if ( (m_stateComm == SKTM_STATE_CONNECTING) || (m_stateComm == SKTM_STATE_ERROR) ) {
            wxString connectionErr = wxEmptyString;
            giveUpConnectionRetry100ms(5);
            if ( __NOT_STOP_THREAD__ ) {
                ( (iCnxPrg >= 2) ? iCnxPrg = 0 : iCnxPrg++ );
                m_data = sCnxPrg[iCnxPrg];
                *m_echoStreamerShow = m_data;
                if ( !socket->Connect( *address, false ) ) {
                    if ( !socket->WaitOnConnect() ) {
                        connectionErr += _T(" (timeout)");
                    }
                    else {
                        if ( !socket->IsConnected() ) {
                            connectionErr += _T(" (refused by peer)");
                        }
                    }
                }
                if ( __NOT_STOP_THREAD__) {
                    if ( connectionErr.IsEmpty() ) {
                        m_stateComm = SKTM_STATE_READY;
                        if ( m_verbosity > 1) {
                            m_threadMsg = _T("dashboard_tactics_pi: Delta Streamer : SKTM_STATE_READY");
                            wxQueueEvent( m_frame, event.Clone() );
                        }
                    }
                    else {
                        m_stateComm = SKTM_STATE_ERROR;
                        if ( m_verbosity > 1) {
                            m_threadMsg = _T("dashboard_tactics_pi: Delta Streamer : SKTM_STATE_ERROR");
                            m_threadMsg += connectionErr;
                            wxQueueEvent( m_frame, event.Clone() );
                        }
                        giveUpConnectionRetry100ms(5);
                        m_stateComm = SKTM_STATE_CONNECTING; 
                    } // else error state wait until attempting again
                } // then thread does not need to terminate
            } // then thread does not need to terminate
        } // then need to attempt to connect()
        
        if ( m_stateComm == SKTM_STATE_READY ) {
            
            wxString sData = wxEmptyString;
            int linesPrepared = 0;        
            __CNTROUT__;

            while ( (linesPrepared < m_linesPerWrite) && __NOT_STOP_THREAD__ ) {
                
                mtxQLine.lock();
                if (qLine.empty()) {
                    mtxQLine.unlock();
                    wxMilliSleep( 100 );
                    if (__STOP_THREAD__) {
                            break;
                    }
                } // no data to be sent
                else {
                    lineProtocol lineOut = qLine.front();
                    qLine.pop();
                    m_poppedFromFifo = m_poppedFromFifo + 1LL;
                    mtxQLine.unlock();

                    if ( linesPrepared > 0 ) {
                        if ( !fileOp )
                        sData += "\n";
                    }
                    sData += lineOut.measurement;
                    if ( !lineOut.tag_key1.IsEmpty() ) {
                        sData += ",";
                        sData += lineOut.tag_key1;
                        sData += "=";
                        sData += lineOut.tag_value1;
                        if ( !lineOut.tag_key2.IsEmpty() ) {
                            sData += ",";
                            sData += lineOut.tag_key2;
                            sData += "=";
                            sData += lineOut.tag_value2;
                            if ( !lineOut.tag_key3.IsEmpty() ) {
                                sData += ",";
                                sData += lineOut.tag_key3;
                                sData += "=";
                                sData += lineOut.tag_value3;
                            }
                        }
                    }
                    sData += " ";
                    sData += lineOut.field_key1;
                    sData += "=";
                    sData += lineOut.field_value1; // all data is expected to be float
                    if ( !lineOut.field_key2.IsEmpty() ) {
                        sData += ",";
                        sData += lineOut.field_key2;
                        sData += "=";             // if later on we save units as strings : += "\"";
                        sData += lineOut.field_value2;
                        // if later on we save units as string:      sData += "\"";
                        if ( !lineOut.field_key3.IsEmpty() ) {
                            sData += ",";
                            sData += lineOut.field_key3;
                            sData += "=";         // see above the comment about strings
                            sData += lineOut.field_value3;
                            // see above the comment about stings:   sData += "\"";
                        }
                    }
                    if ( !lineOut.timestamp.IsEmpty() ) {
                        sData += " ";
                        sData += lineOut.timestamp;
                    }
                    if ( fileOp )
                        sData += "\n";
                    linesPrepared++;
                    
                    if (__STOP_THREAD__)
                        break;
                } // else there is valid data in the queue
            } // while number of lines to prepare

            if (__STOP_THREAD__)
                        break;
                
            if ( fileOp ) {
                
                file->Write( sData );
                wFdOut += sData.Len();
                if ( wFdOut >= 4096 ) {
                    file->Flush();
                    wFdOut = 0;
                } // then let's flush the buffer every 2kB (if the fsync() is available in the OS)
                
                __INCR_CNTROUT__;
                
                wxMilliSleep(100);
                    
            } // then write to file
            else {
                    
                wxString sHdrOut = header;
                sHdrOut += wxString::Format("%d", sData.Len() );
                sHdrOut += "\r\n";
                sHdrOut += "\r\n";
                sHdrOut += sData;
                wxScopedCharBuffer scb = sHdrOut.mb_str();
                size_t len = scb.length();

                socket->Write( scb.data(), len );

                if ( m_verbosity > 4) {
                    m_threadMsg = wxString::Format("dashboard_tactics_pi: written to socket:\n%s", sHdrOut);
                    wxQueueEvent( m_frame, event.Clone() );
                } // for big time debugging only, use tail -f opencpn.log | grep dashboard_tactics_pi

                if ( !socket->Error() ) {

                    __INCR_CNTROUT__;

                    int waitMilliSeconds = 0;
                    bool readAvailable = false;
                    while ( __NOT_STOP_THREAD__ && !readAvailable &&
                            (waitMilliSeconds < (m_connectionRetry * 500)) ) {
                        char c;
                        ( socket->Peek(&c,1).LastCount()==0 ? readAvailable = false : readAvailable = true );
                        if ( !readAvailable) {
                            wxMilliSleep( 20 );
                            waitMilliSeconds += 20;
                        }
                    }
                    if (__STOP_THREAD__)
                        break;
                    if ( readAvailable ) {
                        wxCharBuffer buf(100);
                        socket->Read( buf.data(), 100 );
                        wxString sBuf = buf;
                        bool sBufError = true;
                        if ( sBuf.Contains("HTTP/1.1 204") )
                            sBufError = false;
                        unsigned int lostReadCount = std::numeric_limits<unsigned int>::max();
                        int lostLoop100s = 0;
                        while ( lostReadCount > 0 ) {
                            wxCharBuffer lostbuf(100);
                            socket->Read( lostbuf.data(), 100 );
                            lostReadCount = socket->LastReadCount();
                            lostLoop100s += lostReadCount;
                        }
                        if ( sBufError ) {
                            if ( m_verbosity > 2) {
                                m_threadMsg = wxString::Format(
                                    "dashboard_tactics_pi: Delta Streamer : Data Rejected (%s) : %s",
                                    sBuf, sData);
                                wxQueueEvent( m_frame, event.Clone() );
                            }
                        } // then error from the DB write API
                    } // then read buffer of the socket contains some data
                    else {
                        m_stateComm = SKTM_STATE_ERROR;
                        socket->Close();
                        if ( m_verbosity > 1) {
                            m_threadMsg = _T("dashboard_tactics_pi: Delta Streamer : Error - no data received from server.");
                            wxQueueEvent( m_frame, event.Clone() );
                        }
                        giveUpConnectionRetry100ms(5);
                    } // else no data in the socket buffer
                } // then no error in the socket write
                else {
                    m_stateComm = SKTM_STATE_ERROR;
                    socket->Close();
                    if ( m_verbosity > 1) {
                        m_threadMsg = _T("dashboard_tactics_pi: Delta Streamer : socket Write() error.");
                        wxQueueEvent( m_frame, event.Clone() );
                    }
                    giveUpConnectionRetry100ms(5);
                } // else error in the socket Write()
            } // else network (socket) operation
        } // else data available to be sent
    } // while not to be stopped / destroyed
        
    if ( fileOp ) {
        if ( file->IsOpened() ) {
            (void) file->Flush();
            wxMilliSleep( 20 );
            (void) file->Close();
        }
        delete file;
    } // then close file operations
    else {
        socket->Destroy();
        wxSocketBase::Shutdown();
        delete address;
    } // else socket operation termination
    
    return (wxThread::ExitCode)0;
    
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamInSkSingle::OnThreadUpdate( wxThreadEvent &evt )
{
    wxLogMessage ("%s", m_threadMsg);
    m_threadMsg = wxEmptyString;
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamInSkSingle::OnStreamInSkUpdTimer( wxTimerEvent &evt )
{
    if ( m_thread->TestDestroy() || m_cmdThreadStop )
        return;
    if ( m_thread->IsAlive() ) {
        if ( m_thread->IsPaused() ) {
            if ( m_verbosity > 3) {
                wxLogMessage("dashboard_tactics_pi: DEBUG : OnStreamInSkUpdTime : m_thread IsPaused()");
                } // for big time debugging only, use tail -f opencpn.log | grep dashboard_tactics_pi
            m_thread->Resume();
        }
    }
    if ( m_verbosity > 3 ) {
        std::unique_lock<std::mutex> lckmQline( m_mtxQLine );
        long long pushDelta = m_pushedInFifo - m_poppedFromFifo;
        wxString sPushedInFifo; sLL( m_pushedInFifo, sPushedInFifo );
        wxString sPoppedFromFifo; sLL( m_poppedFromFifo, sPoppedFromFifo );
        lckmQline.unlock();
        wxString sBlockingLimit; sLL( SKTM_MAX_UNWRITTEN_FIFO_ELEMENTS_BLOCKING, sBlockingLimit );
        wxString sUnblockingLimit; sLL( SKTM_MAX_UNWRITTEN_FIFO_ELEMENTS_UNBLOCKING, sUnblockingLimit );
        wxString sPushDelta; sLL( pushDelta, sPushDelta );
        wxLogMessage(
            "dashboard_tactics_pi: DEBUG : OnStreamInSkUpdTime : FIFO : (In %s Out %s Delta %s Block %s Unblock %s)",
            sPushedInFifo, sPoppedFromFifo, sPushDelta, sBlockingLimit, sUnblockingLimit );
    } // for BIG time debugging only, use tail -f opencpn.log | grep dashboard_tactics_pi
}
/***********************************************************************************

************************************************************************************/
bool TacticsInstrument_StreamInSkSingle::LoadConfig()
{
    if ( *m_nofStreamInSk > 1 )
        return true;
    
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return false;
    pConf->SetPath(_T("/PlugIns/Dashboard/Tactics/SteaminSk/"));
    pConf->Read(_T("ConfigFile"), &m_configFileName, "streamin-sk.json");
    wxString s = wxFileName::GetPathSeparator();
    wxString confPath = m_confdir + m_configFileName;
    if ( !wxFileExists( confPath ) ) {
        wxString tmplPath  = *GetpSharedDataLocation();
        tmplPath += _T("plugins") + s + _T("dashboard_tactics_pi") + s + _T("data") + s + _T("streamout_sk_template.json");
        if ( !wxFileExists( tmplPath ) ) {
            wxLogMessage ("dashboard_tactics_pi: ERROR - missing template %s", tmplPath);
            m_data = L"\u2013 No template. \u2013";
            *m_echoStreamerShow = m_data;
            return false;
        }
        bool ret = wxCopyFile ( tmplPath, confPath ); 
        if ( !ret ) {
            wxLogMessage ("dashboard_tactics_pi: ERROR - cannot copy template %s to %s", tmplPath, confPath);
            m_data = L"\u2013 ConfigFile? \u2013";
            *m_echoStreamerShow = m_data;
            return false;
        }
    }

    try {
        wxFileInputStream jsonStream( confPath );
        wxJSONValue  root;
        wxJSONReader reader;
        int numErrors = reader.Parse( jsonStream, &root );

        if ( numErrors > 0 )  {
            const wxArrayString& errors = reader.GetErrors();
            wxMessageBox(
                _("Signal K Steamer configuration file parsing error, see log file."));
            for (int i = 0; ( ((size_t) i < errors.GetCount()) && ( i < 10 ) ); i++) {
                wxLogMessage(
                    "dashboard_tactics_pi: ERROR - parsing errors in the configuration file: %s", errors.Item(i) );
            }
            return false;
        }

        if ( !root.HasMember("streamin-sk") ) throw 100;
        if ( !root["streamin-sk"].HasMember("source") ) throw 101;
        m_source += root["streamin-sk"]["source"].AsString();
        wxUniChar separator = 0x3a;
        if ( m_source.Find( separator ) == wxNOT_FOUND ) {
            wxLogMessage(
                "dashboard_tactics_pi: ERROR - Signal K source config file missing ':' in 'source', now : %s", m_source);
            return false;
        }
        if ( !root["streamin-sk"].HasMember("api") ) throw 102;
        m_api += root["streamin-sk"]["api"].AsString();
        if ( !root["streamin-sk"].HasMember("connectionretry") ) throw 103;
        m_connectionRetry = root["streamer"]["connectionretry"].AsInt();
        if ( m_connectionRetry <=0)
            m_connectionRetry = 1; // cannot be <=0 ; used to throttle the thread
        if ( !root["streamer"].HasMember("timestamps") ) throw 104;
        m_timestamps += root["streamer"]["timestamps"].AsString();
        if ( m_timestamps.IsSameAs( _T("server"), false ) )
            m_stamp = false;
        if ( !root["streamer"].HasMember("verbosity") ) throw 105;
        m_verbosity = root["streamer"]["verbosity"].AsInt();

        if ( m_verbosity > 1 ) {
            wxLogMessage( "dashboard_tactics_pi: Signal K source       = \"%s\"",  m_source );
            wxLogMessage( "dashboard_tactics_pi: Signal K API version  = \"%s\"",  m_api );
            wxLogMessage( "dashboard_tactics_pi: Signal K conn.retry   = %d s.",   m_connectionRetry );
            wxLogMessage( "dashboard_tactics_pi: Signal K timestamps   = \"%s\"",  m_timestamps );
            wxLogMessage( "dashboard_tactics_pi: Streamer verbosity    = %d",      m_verbosity );
        }

    }
    catch (int x) {
        wxString expErr = wxEmptyString;
        if ( (x >= 100) && (x < 200) ) {
            wxLogMessage ("dashboard_tactics_pi: JSON file %s parsing exception: missing expected item %d in 'streamin-sk'",
                          confPath, (x - 100) );
        }
        wxMessageBox(_("Signal K Steamer configuration file parsing error, see log file."));

        return false;
        
    } // A JSON file can have errors which make this old JSON code to break

    return true;
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamInSkSingle::SaveConfig()
{
    if ( *m_nofStreamInSk > 1 )
        return;
    
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return;

    pConf->SetPath(_T("/PlugIns/Dashboard/Tactics/SteaminSk/"));
    pConf->Write(_T("ConfigFile"),m_configFileName);

    return;
}

/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamInSkSingle::SetNMEASentence(wxString& delta)
{
    return;
}
