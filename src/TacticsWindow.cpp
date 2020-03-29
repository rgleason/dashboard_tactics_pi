/***************************************************************************
* $Id: TacticsWindow.cpp, v1.0 2019/11/30 VaderDarth Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Thomas Rauch
*       (Inspired by original work from Jean-Eudes Onfray)
***************************************************************************
*   Copyright (C) 2010 by David S. Register                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it underta the terms of the GNU General Public License as published by  *
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

using namespace std;

#include <mutex>

#include "TacticsWindow.h"
#include "tactics_pi.h"
#include "SkData.h"

extern wxString GetUUID(void);

TacticsWindow::TacticsWindow (
    wxWindow *pparent, wxWindowID id,
    tactics_pi *tactics, const wxString derivtitle,
    SkData *pSkData) :
    wxWindow(
        pparent, id, wxDefaultPosition, wxDefaultSize,
        wxBORDER_NONE || wxFULL_REPAINT_ON_RESIZE, derivtitle )
{
    m_plugin = tactics;
    m_callbacks = new callback_map();
    std::unique_lock<std::mutex> init_m_mtxCallBackContainer( m_mtxCallBackContainer, std::defer_lock );
    m_pSkData = pSkData;
    return;
}

TacticsWindow::~TacticsWindow()
{
    delete m_callbacks;
    return;
}

void TacticsWindow::InsertTacticsIntoContextMenu ( wxMenu *contextMenu )
{
    wxMenuItem* btnShowLaylines = contextMenu->AppendCheckItem(
        ID_DASH_LAYLINE, _(L"\u2191Show Laylines"));
    btnShowLaylines->Check(m_plugin->GetLaylineVisibility());

    wxMenuItem* btnShowCurrent = contextMenu->AppendCheckItem(
        ID_DASH_CURRENT, _(L"\u2191Show Current"));
    btnShowCurrent->Check(m_plugin->GetCurrentVisibility());

	wxMenuItem* btnShowWindbarb = contextMenu->AppendCheckItem(
        ID_DASH_WINDBARB, _(L"\u2191Show Windbarb"));
	btnShowWindbarb->Check(m_plugin->GetWindbarbVisibility());

	wxMenuItem* btnShowPolar = contextMenu->AppendCheckItem(
        ID_DASH_POLAR, _(L"\u2191Show Polar"));
	btnShowPolar->Check(m_plugin->GetPolarVisibility());

    return;
}

void TacticsWindow::TacticsInContextMenuAction ( const int eventId )
{
    bool toggled = false;
	switch (eventId){
	case ID_DASH_LAYLINE: {
		m_plugin->ToggleLaylineRender();
        toggled = true;
        break;
	}
	case ID_DASH_CURRENT: {
		m_plugin->ToggleCurrentRender();
        toggled = true;
        break;
	}
	case ID_DASH_POLAR: {
		m_plugin->TogglePolarRender();
        toggled = true;
        break;
	}
	case ID_DASH_WINDBARB: {
		m_plugin->ToggleWindbarbRender();
        toggled = true;
        break;
	}
	}
    /* Port note: Parent cannot save for child,
       child cannot save for parent. */
    if ( toggled )
       m_plugin->SaveConfig();

	return;

}
void TacticsWindow::SendPerfSentenceToAllInstruments(
    unsigned long long st, double value, wxString unit, long long timestamp )
{
    m_plugin->SendSentenceToAllInstruments( st, value, unit, timestamp );
}
void TacticsWindow::SetUpdateSignalK(
        wxString *type, wxString *sentenceId, wxString *talker, wxString *src, int pgn,
        wxString *path, double value, wxString *valStr, long long timestamp, wxString *key )
{
    m_plugin->SetUpdateSignalK( type, sentenceId, talker, src, pgn, path, value, valStr, timestamp, key );
}

wxString TacticsWindow::subscribeTo ( wxString path, callbackFunction callback)
{
    wxString retUUID = GetUUID();
    std::string keyID = std::string( retUUID.mb_str() );
    callbackFunctionTuple newEntry = std::make_tuple(path, callback);
    std::unique_lock<std::mutex> lckmCallBack( m_mtxCallBackContainer );
    m_callbacks->insert ( make_pair(keyID, newEntry) );
    m_pSkData->UpdateSubscriptionList( &path, NULL) ;
    return retUUID;
}

void TacticsWindow::unsubscribeFrom ( wxString callbackUUID )
{
    std::string keyID = std::string( callbackUUID.mb_str() );
    std::unique_lock<std::mutex> lckmCallBack( m_mtxCallBackContainer );
    callback_map::iterator it = m_callbacks->find( keyID );
    if ( it != m_callbacks->end() ) {
            m_callbacks->erase( it );
    } // key found, delete
}
void TacticsWindow::SendDataToAllPathSubscribers(
    wxString path, double value, wxString unit, long long timestamp )
{
    std::unique_lock<std::mutex> lckmCallBack( m_mtxCallBackContainer );
    callback_map::iterator it = m_callbacks->begin();
    callbackFunctionPair  thisEntry;
    callbackFunctionTuple thisSubscriber;
    std::string keyID;
    wxString subscribedPath;
    callbackFunction callThis;
    while ( it != m_callbacks->end() ) {
        thisEntry = *it;
        keyID = std::get<0>(thisEntry);
        if ( !keyID.empty() ) {
            thisSubscriber = std::get<1>(thisEntry);
            subscribedPath = std::get<0>(thisSubscriber);
            if ( subscribedPath == path ) {
                    callThis = std::get<1>(thisSubscriber);
                    callThis ( value, unit, timestamp );
            }
        }
        ++it;
    }
}
wxString TacticsWindow::getAllNMEA0183JsOrderedList()
{
    return m_pSkData->getAllNMEA0183JsOrderedList();
}
wxString TacticsWindow::getAllNMEA2000JsOrderedList()
{
    return m_pSkData->getAllNMEA2000JsOrderedList();
}
wxString TacticsWindow::getAllDbSchemasJsOrderedList()
{
    return m_pSkData->getAllDbSchemasJsOrderedList();
}
wxString TacticsWindow::getDbSchemaJs( wxString *path )
{
    return m_pSkData->getDbSchemaJs( path );
}
void TacticsWindow::collectAllSignalKDeltaPaths()
{
    m_pSkData->subscribeToAllPaths();
}
void TacticsWindow::collectAllDbSchemaPaths()
{
    m_pSkData->recordAllDbSchemas();
}
