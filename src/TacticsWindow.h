/******************************************************************************
 * $Id: TacticsWindow.h, v1.0 v1.0 2019/11/30 VaderDarth Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Jean-Eudes Onfray
 *
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

#ifndef _TACTICSWINDOW_H_
#define _TACTICSWINDOW_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <unordered_map>
#include <functional>
#include <mutex>

class tactics_pi;
class SkData;

// helpers for the call-back methods in instruments subsribing to signal paths
typedef std::function<void  (double, wxString, long long)> callbackFunction;
typedef std::tuple<wxString, callbackFunction> callbackFunctionTuple;
typedef std::pair<wxString, callbackFunctionTuple> callbackFunctionPair;
typedef std::unordered_multimap<std::string, callbackFunctionTuple> callback_map;

class TacticsWindow : public wxWindow
{
public:
    TacticsWindow(
        wxWindow* pparent, wxWindowID id,
        tactics_pi* tactics, const wxString derivtitle,
        SkData* pSkData);
    ~TacticsWindow();

    virtual void InsertTacticsIntoContextMenu (
        wxMenu *contextMenu ) final;
    virtual void TacticsInContextMenuAction (
        const int eventId ) final;

    void SendPerfSentenceToAllInstruments(
        unsigned long long st, double value, wxString unit, long long timestamp );
    void SetUpdateSignalK(
        wxString* type, wxString* sentenceId, wxString* talker, wxString* src, int pgn,
        wxString* path, double value, wxString* valStr, long long timestamp, wxString* key=NULL);
    wxString subscribeTo ( wxString path, callbackFunction callback);
    void unsubscribeFrom ( wxString callbackUUID );
    void SendDataToAllPathSubscribers(
        wxString path, double value, wxString unit, long long timestamp );
    wxString getAllNMEA0183JsOrderedList(void);
    wxString getAllNMEA2000JsOrderedList(void);
    wxString getAllDbSchemasJsOrderedList(void);
    wxString getDbSchemaJs( wxString* path );
    void collectAllSignalKDeltaPaths(void); 
    void collectAllDbSchemaPaths(void); 
protected:
    std::mutex          m_mtxCallBackContainer;
    callback_map       *m_callbacks;
    SkData             *m_pSkData;

private:
    tactics_pi*         m_plugin;

};

#endif // _TACTICSWINDOW_H_
