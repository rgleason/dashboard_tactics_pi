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

#ifndef __STREAMOUT_H__
#define __STREAMOUT_H__

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/socket.h>
#include <wx/thread.h>

#include "instrument.h"

enum StreamoutSingleStateMachine {
    SSSM_STATE_UNKNOWN, SSSM_STATE_DISPLAYRELAY, SSSM_STATE_INIT, SSSM_STATE_CONFIGURED,
    SSSM_STATE_READY, SSSM_STATE_FAIL };

enum dbgThreadRun {
    DBGRES_THR_RUN_UNKNOWN, DBGRES_THR_RUN_ERROR, DBGRES_THR_RUN_OK };


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
        std::mutex &mtxNofStreamOut, int &nofStreamOut, wxString &echoStreamerShow, wxString confdir);
	~TacticsInstrument_StreamoutSingle();

	wxSize GetSize(int orient, wxSize hint);
	void SetData(unsigned long long st, double data, wxString unit);

protected:

    class sentenceSchema
    {
    public:
        sentenceSchema(void) {
            st = 0ULL;
            bStore = false;
            iInterval = 0;
            lastTimeStamp = 0L;
            sMeasurement = wxEmptyString;
            sProp1 = wxEmptyString;
            sProp2 = wxEmptyString;
            sProp3 = wxEmptyString;
            sField1 = wxEmptyString;
            sField2 = wxEmptyString;
            sField3 = wxEmptyString;
        };
        sentenceSchema( const sentenceSchema& source) {
            st = source.st;
            bStore = source.bStore;
            iInterval = source.iInterval;
            lastTimeStamp = source.lastTimeStamp;
            sMeasurement = source.sMeasurement;
            sProp1 = source.sProp1;
            sProp2 = source.sProp2;
            sProp3 = source.sProp3;
            sField1 = source.sField1;
            sField2 = source.sField2;
            sField3 = source.sField3;
        };
        const sentenceSchema& operator = (const sentenceSchema &source) {
            if ( this != &source) {
                st = source.st;
                bStore = source.bStore;
                iInterval = source.iInterval;
                lastTimeStamp = source.lastTimeStamp;
                sMeasurement = source.sMeasurement;
                sProp1 = source.sProp1;
                sProp2 = source.sProp2;
                sProp3 = source.sProp3;
                sField1 = source.sField1;
                sField2 = source.sField2;
                sField3 = source.sField3;
            }
            return *this;
        };
        unsigned long long st;
        bool bStore;
        int iInterval;
        long long lastTimeStamp;
        wxString sMeasurement;
        wxString sProp1;
        wxString sProp2;
        wxString sProp3;
        wxString sField1;
        wxString sField2;
        wxString sField3;
    }; // This class presents the elements of the configuration file
    
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

    sentenceSchema    schema;
    std::vector<sentenceSchema> vSchema;

    wxSocketClient   *socket;
    std::mutex        m_mtxSocket;
    int               m_dgbThreadRun;
    float             m_outData1;
    wxString          m_outUnit1;

    // From configuration file
    wxString          m_apiServer;
    wxString          m_apiURL;
    wxString          m_apiAut;
    int               m_connectionRetry;
    wxString          m_timestamps;
    int               m_verbosity;

    bool GetSchema(unsigned long long st, sentenceSchema& schema);
    bool LoadConfig(void);
    void SaveConfig(void);
    void Draw(wxGCDC* dc);
    void OnClose(wxCloseEvent& evt);
    wxThread::ExitCode TacticsInstrument_StreamoutSingle::Entry(void);
    void OnThreadUpdate(wxThreadEvent& evt);
    
private :

    wxDECLARE_EVENT_TABLE();

};

#endif // not defined __STREAMOUT_H__

