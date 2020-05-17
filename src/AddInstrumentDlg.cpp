/***************************************************************************
 * $Id: AddInstrumentDlg.cpp, v1.0 2010/08/05 SethDart Exp $
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

#include <wx/imaglist.h>

#include "AddInstrumentDlg.h"

class dashboard_pi;

#include "DashboardInstrumentContainer.h"

#include "icons.h"

//----------------------------------------------------------------
//
//    Add Instrument Dialog Implementation
//
//----------------------------------------------------------------
/* Provide a callback method to tweak the alphabetical ascending order of the instrument list(s)
   so that tactics and performance instruments are listed at the end, after the dashbaord std.
   instruments. */
int wxCALLBACK InstrumentListSortCallback (wxIntPtr item1, wxIntPtr item2, wxIntPtr WXUNUSED(sortData))
{
    if ( IsTacticsInstrument( item1 ) && !IsTacticsInstrument( item2 ) ) {
        return 1;
    } // first instrument is Tactics and second is Dashboard, Dashboard instruments first
    else {
        if ( !IsTacticsInstrument( item1 ) && IsTacticsInstrument( item2 ) ) {
            return -1;
        } // first instrument is Dashboard and second is Tactics, keep that way
    } // else check the other way around
    // Both are the same, either Tactics or Dashboard, let the alphabeting order prevail
    const wxString capt1 = getInstrumentCaption (item1);
    return capt1.CmpNoCase ( getInstrumentCaption (item2) );
}

AddInstrumentDlg::AddInstrumentDlg( wxWindow *pparent, wxWindowID id ) :
    wxDialog( pparent, id, _("Add instrument"), wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_DIALOG_STYLE )
{
    wxBoxSizer* itemBoxSizer01 = new wxBoxSizer( wxVERTICAL );
    SetSizer( itemBoxSizer01 );
    wxStaticText* itemStaticText01 = new wxStaticText( this, wxID_ANY,
                                                       _("Select instrument to add:")
                                                       + _(L"\n(\u2191Tactics at the end of the list)")
                                                       , wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer01->Add( itemStaticText01, 0, wxEXPAND | wxALL, 5 );

    wxImageList *imglist = new wxImageList( 20, 20, true, 2 );
    imglist->Add( *_img_instrument );
    imglist->Add( *_img_dial );

    m_pListCtrlInstruments = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( 250, 180 ),
                                             wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING );
    itemBoxSizer01->Add( m_pListCtrlInstruments, 0, wxEXPAND | wxALL, 5 );
    m_pListCtrlInstruments->AssignImageList( imglist, wxIMAGE_LIST_SMALL );
    m_pListCtrlInstruments->InsertColumn( 0, _("Instruments") );

    wxStdDialogButtonSizer* DialogButtonSizer = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
    itemBoxSizer01->Add( DialogButtonSizer, 0, wxALIGN_RIGHT | wxALL, 5 );

    for( unsigned int i = ID_DBP_I_POS; i < ID_DBP_LAST_ENTRY; i++ ) { //do not reference an instrument, but the last dummy entry in the list
        wxListItem item;
        if( IsObsolete( i ) ) continue;
        if ( getListItemForInstrument( item, i ) ) {
            item.SetId( i );
            m_pListCtrlInstruments->InsertItem( item );
        }
    }
    // Provide a dual sorting callback so that Tactics and Dashboard instruments are kept apart.
    m_pListCtrlInstruments->SortItems( InstrumentListSortCallback, 0);
    m_pListCtrlInstruments->SetColumnWidth( 0, wxLIST_AUTOSIZE );
    m_pListCtrlInstruments->SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    Fit();
}

unsigned int AddInstrumentDlg::GetInstrumentAdded()
{
    long itemID = -1;
    itemID = m_pListCtrlInstruments->GetNextItem( itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    return (int) m_pListCtrlInstruments->GetItemData( itemID );
}
