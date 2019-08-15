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
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
using namespace std;
#include <mutex>

// #include <wx/dir.h>
// #include <wx/filefn.h>
// #include <wx/textfile.h>
// #include <wx/tokenzr.h>
#include <wx/wfstream.h> 
// #include <wx/txtstrm.h> 
// #include <wx/math.h>
// #include <wx/stdpaths.h>
// #include <wx/progdlg.h>
// #include <wx/gdicmn.h>
#include <wx/fileconf.h>
// #include "nmea0183/nmea0183.h"

#include "streamout.h"
// #include <map>
// #include <cmath>
#include "ocpn_plugin.h"
#include "wx/jsonreader.h"
#include "plugin_ids.h"

extern wxString g_path_to_PolarFile;
extern int g_iDashWindSpeedUnit;
extern int g_iDashSpeedUnit;
extern int g_iSpeedFormat;

wxBEGIN_EVENT_TABLE (TacticsInstrument_StreamoutSingle, DashboardInstrument)
   EVT_THREAD (myID_THREAD_IFLXAPI, TacticsInstrument_StreamoutSingle::OnThreadUpdate)
   EVT_CLOSE(TacticsInstrument_StreamoutSingle::OnClose)
wxEND_EVENT_TABLE ()

// ----------------------------------------------------------------
//
//    TacticsInstrument_StreamoutSingle
//
//----------------------------------------------------------------
TacticsInstrument_StreamoutSingle::TacticsInstrument_StreamoutSingle(
    wxWindow *pparent, wxWindowID id, wxString title, unsigned long long cap_flag, wxString format,
    std::mutex &mtxNofStreamOut, int &nofStreamOut, wxString &echoStreamerShow, wxString confdir)
	:DashboardInstrument(pparent, id, title, cap_flag)
{
    std::unique_lock<std::mutex> lck_mtxNofStreamOut( mtxNofStreamOut);
    nofStreamOut++;
    m_mtxNofStreamOut = &mtxNofStreamOut;
    m_nofStreamOut = &nofStreamOut;
    m_echoStreamerShow = &echoStreamerShow;
    m_state = SSSM_STATE_UNKNOWN;
    if ( nofStreamOut > 1) {
        m_state = SSSM_STATE_DISPLAYRELAY;
        m_data = echoStreamerShow;
        return;
    }
    m_state = SSSM_STATE_INIT;
    m_data = _T("---");
    echoStreamerShow = m_data;
    m_format = format;
    m_DataHeight = 0;
    m_confdir = confdir;
    m_pconfig = GetOCPNConfigObject();
    m_configFileName = wxEmptyString;
    m_verbosity = 0;

    wxString emptyStr = wxEmptyString;
    emptyStr = emptyStr.wc_str();
    m_apiServer = emptyStr;
    m_apiURL = emptyStr;
    m_apiAut = emptyStr;
    m_connectionRetry = 0;
    m_timestamps = emptyStr;

    m_configured = LoadConfig();

    if ( !m_configured )
        return;
    m_state = SSSM_STATE_CONFIGURED;

    socket = new wxSocketClient();
    std::unique_lock<std::mutex> init_m_mtxSocket( m_mtxSocket, std::defer_lock );
    if ( CreateThread( wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR ) {
        wxLogMessage ("dashboard_tactics_pi: DB Streamer failed : could not create communication thread.");
        m_state = SSSM_STATE_FAIL;
        return;
    } // will not talk
    m_dgbThreadRun = DBGRES_THR_RUN_UNKNOWN;
    m_outData1 = 0.0;
    m_outUnit1 = wxEmptyString;
    
    m_state = SSSM_STATE_READY;
    
}
/***********************************************************************************

************************************************************************************/
TacticsInstrument_StreamoutSingle::~TacticsInstrument_StreamoutSingle()
{
    std::unique_lock<std::mutex> lck_mtxNofStreamOut( *m_mtxNofStreamOut);
    SaveConfig();
    *m_nofStreamOut--;
}
/***********************************************************************************

************************************************************************************/
wxSize TacticsInstrument_StreamoutSingle::GetSize(int orient, wxSize hint)
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
void TacticsInstrument_StreamoutSingle::Draw(wxGCDC* dc)
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
bool TacticsInstrument_StreamoutSingle::GetSchema(unsigned long long st, sentenceSchema &schema)
{
    bool retval = false;
    for ( unsigned int i = 0; i < vSchema.size(); i++ ) {
        if ( vSchema[i].st == st ) {
            schema = vSchema[i];
            retval = true;
            break;
        } 
    }
    return retval;
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamoutSingle::SetData(unsigned long long st, double data, wxString unit)
{
    if (std::isnan(data))
        return;

    if ( m_state == SSSM_STATE_DISPLAYRELAY ) {
        m_data = *m_echoStreamerShow;
        return;
    }
    
    if ( (m_state == SSSM_STATE_READY) && m_configured ) {
        if ( !GetSchema( st, schema ) )
            return;
        m_outData1 = data;
        m_outUnit1 = unit;
        if ( GetThread()->Run() != wxTHREAD_NO_ERROR ) {
            if ( m_dgbThreadRun != DBGRES_THR_RUN_ERROR ) {
                m_dgbThreadRun = DBGRES_THR_RUN_ERROR;
                if ( m_verbosity != 0 )
                    wxLogMessage ("dashboard_tactics_pi: Error : Influx DB Streamer: Cannot start socket thread");
            }
        }
        else {
            m_dgbThreadRun = DBGRES_THR_RUN_OK;
            if ( m_verbosity > 1 )
                wxLogMessage ("dashboard_tactics_pi: OK: Influx DB Streamer: Socket thread is started");
        }
    } // Ok to transmit

    /*
    
    if (st == OCPN_DBP_STC_STW){
    
        //convert to knots first
        mSTW = fromUsrSpeed_Plugin(data, g_iDashSpeedUnit);
        stwunit = unit;
    }
    else if (st == OCPN_DBP_STC_TWA){
        mTWA = data;
    }
    else if (st == OCPN_DBP_STC_COG){
        mCOG = data;
    }
    else if (st == OCPN_DBP_STC_SOG){
        //convert to knots first
        //mSOG = data;
        mSOG = fromUsrSpeed_Plugin(data, g_iDashSpeedUnit);
    }
    else if (st == OCPN_DBP_STC_LAT) {
        m_lat = data;
    }
    else if (st == OCPN_DBP_STC_LON) {
        m_lon = data;
    }
    else if (st == OCPN_DBP_STC_BRG){
        mBRG = data;
    }
    else if (st == OCPN_DBP_STC_TWS){
        //mTWS = data;
        //convert to knots
        mTWS = fromUsrSpeed_Plugin(data, g_iDashWindSpeedUnit);
    }
    else if (st == OCPN_DBP_STC_HDT){
        mHDT = data;
    }
    else if (st == OCPN_DBP_STC_TWD){
        mTWD = data;
    }
    */
 
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamoutSingle::OnClose( wxCloseEvent &evt )
{
    return;    
}
/***********************************************************************************
/***********************************************************************************

************************************************************************************/
wxThread::ExitCode TacticsInstrument_StreamoutSingle::Entry( )
{
    while ( !GetThread()->TestDestroy() ) {

        // wxString data = "home=Cosby&favorite+flavor=flies";
        // wxString host = "www.somehost.com";
        // wxString path = "/path/file.cgi";//can be php or any other file
	

        //Set up header
        wxString header = wxEmptyString;
        header = header.wc_str();
        wxString data = header;

        data += schema.sMeasurement;
        data += L",propr1=";
        data += schema.sProp1;
        data += L" ";
        data += schema.sField1;
        data += L"=";
        data += wxString::Format("%d", static_cast<int>(m_outData1) );

        header += "POST ";
        header += m_apiURL;
        header += L" HTTP/1.1\n";

        header += L"Host: ";
        header += m_apiServer;
        header += L"\n";
	
        header += "User-Agent: OpenCPN/5.0\n";
        header += "Accept: */*\n";

        header += m_apiAut;
        header += "\n";

        header += "Content-Length: ";
        header += wxString::Format("%d", data.Len());
        header += "\n";

        header += "Content-Type: application/x-www-form-urlencoded\n";
        header += "\n";

        if ( m_verbosity > 1) {
            wxLogMessage("dashboard_tactics_pi: VERBOSE config : InfluxDB API header : %s", header);
            wxLogMessage("dashboard_tactics_pi: VERBOSE config : InfluxDB API data   : %s", data);
        }

        wxIPV4address * address = new wxIPV4address();
        wxUniChar separator = 0x3a;
        address->Hostname(m_apiServer.BeforeFirst(separator));
        address->Service(m_apiServer.AfterFirst(separator));

        if ( !socket->Connect(*address) ) {
            if ( m_verbosity > 0)
                wxLogMessage("dashboard_tactics_pi: ERROR : InfluxDB not connected : %s", m_apiServer);
        }
        else {
            if ( m_verbosity > 1)
                wxLogMessage("dashboard_tactics_pi: VERBOSE config : InfluxDB API socket connection : %s", m_apiServer);

            socket->Write(header.c_str(),header.Len());
            socket->Write(data.c_str(),data.Len());
            //Get Response
            wxString buf;
            buf.Alloc(1000);
            void *vptr;
            vptr = &buf.char_str();
            socket->Read( vptr, 1000);
            buf.Shrink();
            //Trim response to what was read from stream
            buf = buf.SubString( 0, socket->LastCount() - 1 );
            if ( m_verbosity > 1)
                wxLogMessage("dashboard_tactics_pi: VERBOSE config : InfluxDB API write returns %d chars: %s",
                             socket->LastCount(), buf);

        }

    } // while destroy
    
    return (wxThread::ExitCode)0;
    
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamoutSingle::OnThreadUpdate( wxThreadEvent &evt )
{
    return;   
}
/***********************************************************************************

************************************************************************************/
bool TacticsInstrument_StreamoutSingle::LoadConfig()
{
    if ( *m_nofStreamOut > 1 )
        return true;
    
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return false;
    pConf->SetPath(_T("/PlugIns/Dashboard/Tactics/Streamout/"));
    pConf->Read(_T("ConfigFile"), &m_configFileName, "streamout.json");
    wxString s = wxFileName::GetPathSeparator();
    wxString confPath = m_confdir + m_configFileName;
    if ( !wxFileExists( confPath ) ) {
        wxString tmplPath  = *GetpSharedDataLocation();
        tmplPath += _T("plugins") + s + _T("dashboard_tactics_pi") + s + _T("data") + s + _T("streamout_template.json");
        if ( !wxFileExists( tmplPath ) ) {
            wxLogMessage ("dashboard_tactics_pi: ERROR - missing template %s", tmplPath);
            return false;
        }
        bool ret = wxCopyFile ( tmplPath, confPath ); 
        if ( !ret ) {
            wxLogMessage ("dashboard_tactics_pi: ERROR - cannot copy template %s to %s", tmplPath, confPath);
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
            wxMessageBox(_("InfluxDB Steamer configuration file parsing error, see log file."));
            for (int i = 0; ( ((size_t) i < errors.GetCount()) && ( i < 10 ) ); i++) {
                wxLogMessage ("dashboard_tactics_pi: ERROR - parsing errors in the configuration file: %s", errors.Item(i) );
            }
            return false;
        }

        if ( !root.HasMember("influxdb") ) throw 100;
        if ( !root["influxdb"].HasMember("server") ) throw 101;
        m_apiServer += root["influxdb"]["server"].AsString();
        if ( !root["influxdb"].HasMember("api") ) throw 102;
        m_apiURL += root["influxdb"]["api"].AsString();
        m_apiURL += L"?";
        if ( !root["influxdb"].HasMember("org") ) throw 103;
        m_apiURL += root["influxdb"]["org"].AsString();
        m_apiURL += L"&";
        if ( !root["influxdb"].HasMember("bucket") ) throw 104;
        m_apiURL += root["influxdb"]["bucket"].AsString();
        m_apiURL += L"&";
        if ( !root["influxdb"].HasMember("precision") ) throw 105;
        m_apiURL += root["influxdb"]["precision"].AsString();
        if ( !root["influxdb"].HasMember("tokenprfx") ) throw 106;
        m_apiAut += root["influxdb"]["tokenprfx"].AsString();
        if ( !root["influxdb"].HasMember("token") ) throw 107;
        m_apiAut += root["influxdb"]["token"].AsString();

        if ( !root.HasMember("streamer") ) throw 200;
        if ( !root["streamer"].HasMember("connectionretry") ) throw 201;
        m_connectionRetry = root["streamer"]["connectionretry"].AsInt();
        if ( !root["streamer"].HasMember("timestamps") ) throw 202;
        m_timestamps += root["streamer"]["timestamps"].AsString();
        if ( !root["streamer"].HasMember("verbosity") ) throw 203;
        m_verbosity = root["streamer"]["verbosity"].AsInt();

        if ( m_verbosity > 1 ) {
            wxLogMessage( "dashboard_tactics_pi: InfluxDB API URL = \"%s\"", m_apiURL );
            wxLogMessage( "dashboard_tactics_pi: InfluxDB API Hdr = \"%s\"", m_apiAut );
            wxLogMessage( "dashboard_tactics_pi: InfluxDB connection retries every %d seconds", m_connectionRetry );
            wxLogMessage( "dashboard_tactics_pi: InfluxDB timestamps = %s", m_timestamps );
        }

        if ( !root.HasMember("dbschema") ) throw 300;
        wxJSONValue dbSchemas = root["dbschema"];
        if ( !dbSchemas.IsArray() ) {
            wxLogMessage ("dashboard_tactics_pi: JSON file %s parsing exception: 'dbschema' is not an array.");
            throw 300;
        }
        int asize = dbSchemas.Size();
        if ( asize == 0 ) {
            wxLogMessage ("dashboard_tactics_pi: JSON file %s parsing exception: 'dbschema' is an array but it is empty.");
            throw 300;
        }
        for ( int i = 0; i < asize; i++ ) {

            if ( !dbSchemas[i].HasMember("mask") ) throw ( 10000 + (i * 100) + 2 );
            int mask = dbSchemas[i]["mask"].AsInt();
            schema.st = 1ULL << mask;
            
            if ( !dbSchemas[i].HasMember("store") ) throw ( 10000 + (i * 100) + 3 );
            schema.bStore = dbSchemas[i]["store"].AsBool();

            schema.lastTimeStamp = 0LL;

            if ( !dbSchemas[i].HasMember("interval") ) throw ( 10000 + (i * 100) + 4 );
            schema.iInterval = dbSchemas[i]["interval"].AsInt();

            if ( !dbSchemas[i].HasMember("measurement") ) throw ( 10000 + (i * 100) + 5 );
            schema.sMeasurement = dbSchemas[i]["measurement"].AsString();

            if ( !dbSchemas[i].HasMember("prop1") ) throw ( 10000 + (i * 100) + 6 );
            schema.sProp1 = dbSchemas[i]["prop1"].AsString();

            if ( !dbSchemas[i].HasMember("prop2") ) throw ( 10000 + (i * 100) + 7 );
            schema.sProp2 = dbSchemas[i]["prop2"].AsString();

            if ( !dbSchemas[i].HasMember("prop3") ) throw ( 10000 + (i * 100) + 8 );
            schema.sProp3 = dbSchemas[i]["prop3"].AsString();

            if ( !dbSchemas[i].HasMember("field1") ) throw ( 10000 + (i * 100) + 9 );
            schema.sField1 = dbSchemas[i]["field1"].AsString();

            if ( !dbSchemas[i].HasMember("field2") ) throw ( 10000 + (i * 100) + 10 );
            schema.sField2 = dbSchemas[i]["field2"].AsString();

            if ( !dbSchemas[i].HasMember("field3") ) throw ( 10000 + (i * 100) + 11 );
            schema.sField3 = dbSchemas[i]["field3"].AsString();

            vSchema.push_back ( schema );
            
        } // while array has sentence schemas defined
    }
    catch (int x) {
        wxString expErr = wxEmptyString;
        if ( (x >= 100) && (x < 200) ) {
            wxLogMessage ("dashboard_tactics_pi: JSON file %s parsing exception: missing expected item %d in 'influxdb'",
                          confPath, (x - 100) );
        }
        else if ( (x >= 200) && (x < 300) ) {
            wxLogMessage ("dashboard_tactics_pi: JSON file %s parsing exception: missing expected item %d in 'streamer'",
                          confPath, (x - 200) );
        }
        else if ( (x >= 300) && (x < 400) ) {
            wxLogMessage ("dashboard_tactics_pi: JSON file %s parsing exception: missing expected item %d in 'dbschema'",
                          confPath, (x - 300) );
        }
        else {
            wxLogMessage ("dashboard_tactics_pi: JSON file %s parsing exception: missing item in 'dbschema' array %d (10000=ignore, 100's=sentence number, 1's=index in sentence)",
                          confPath, x );
        }
        wxMessageBox(_("InfluxDB Steamer configuration file parsing error, see log file."));

        return false;
        
    } // A JSON file can have errors which has sometimes errors which make this old JSON code to break

    return true;
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_StreamoutSingle::SaveConfig()
{
    if ( *m_nofStreamOut > 1 )
        return;
    
    wxFileConfig *pConf = m_pconfig;
    
    if (!pConf)
        return;

    pConf->SetPath(_T("/PlugIns/Dashboard/Tactics/Streamout/"));
    pConf->Write(_T("ConfigFile"),m_configFileName);

    return;
}
