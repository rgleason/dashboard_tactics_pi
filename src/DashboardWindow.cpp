/***************************************************************************
 * $Id: DashboardWindow.cpp, v1.0 2010/08/05 SethDart Exp $
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

#include "DashboardWindow.h"


// Dashboard global definitions and functions
extern int g_iDashSpeedMax;
extern wxString GetUUID( void );
extern wxString MakeName( void );

// Helper functions to obtain information from enumerated instrument list
extern wxString getInstrumentCaption( unsigned int id );


//----------------------------------------------------------------
//
//    Dashboard Window Implementation
//
//----------------------------------------------------------------
wxBEGIN_EVENT_TABLE (DashboardWindow, TacticsWindow)
   EVT_CLOSE (DashboardWindow::OnClose)
wxEND_EVENT_TABLE ()

DashboardWindow::DashboardWindow(
    wxWindow *pparent, wxWindowID id, wxAuiManager *auimgr,
    dashboard_pi *plugin, int orient,
    DashboardWindowContainer *mycont, wxString commonName, SkData *pSkData ) :
    TacticsWindow ( pparent, id, (tactics_pi *) plugin, commonName, pSkData )
    // please see wxWindow contructor parameters, defined by TacticsWindow class implementation
{
    m_pauimgr = auimgr;
    m_plugin = plugin;
    m_Container = mycont;

    //wx2.9      itemBoxSizer = new wxWrapSizer( orient );
    itemBoxSizer = new wxBoxSizer( orient );
    SetSizerAndFit( itemBoxSizer );
    // Dynamic binding used for event handlers
    Bind( wxEVT_SIZE, &DashboardWindow::OnSize, this );
    Bind( wxEVT_CONTEXT_MENU, &DashboardWindow::OnContextMenu, this );
    Bind( wxEVT_COMMAND_MENU_SELECTED, &DashboardWindow::OnContextMenuSelect, this);
}

DashboardWindow::~DashboardWindow()
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
        DashboardInstrumentContainer *pdic = m_ArrayOfInstrument.Item( i );
        if ( pdic->m_pInstrument != NULL ) {
            delete pdic;
        }
    }
}

void DashboardWindow::OnClose( wxCloseEvent &event )
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
        DashboardInstrumentContainer *pdic = m_ArrayOfInstrument.Item( i );
        if ( pdic->m_pInstrument != NULL ) {
            pdic->m_pInstrument->Close();
        }
    }
    event.Skip(); // Destroy() must be called
}

void DashboardWindow::RebuildPane( wxArrayInt list, wxArrayString listIDs )
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
        DashboardInstrumentContainer *pdic = m_ArrayOfInstrument.Item( i );
        if ( pdic->m_pInstrument != NULL ) {
            pdic->m_pInstrument->Close();
            delete pdic;
        }
    }
    SetInstrumentList( list, listIDs );
}


void DashboardWindow::SetMinSizes( )
{
    for( unsigned int i=0; i<m_ArrayOfInstrument.size(); i++ ) {
        DashboardInstrument* inst = m_ArrayOfInstrument.Item(i)->m_pInstrument;
        wxSize instMinSize = inst->GetSize(
            itemBoxSizer->GetOrientation(), GetClientSize() );
        inst->SetMinSize( instMinSize );
    }
    Layout();
    Refresh();
}

void DashboardWindow::OnSize( wxSizeEvent &event )
{
    event.Skip();
    SetMinSizes();
}

void DashboardWindow::OnContextMenu( wxContextMenuEvent &event )
{
    wxMenu* contextMenu = new wxMenu();

    wxAuiPaneInfo &pane = m_pauimgr->GetPane( this );
    if ( pane.IsOk() ) {
        if ( pane.IsDocked() ) {
            contextMenu->Append( ID_DASH_UNDOCK, _( "Undock" ) );
        } // then docked
        else {
            wxMenuItem* btnVertical = contextMenu->AppendRadioItem( ID_DASH_VERTICAL, _("Vertical") );
            btnVertical->Check( itemBoxSizer->GetOrientation() == wxVERTICAL );
            wxMenuItem* btnHorizontal = contextMenu->AppendRadioItem( ID_DASH_HORIZONTAL, _("Horizontal") );
            btnHorizontal->Check( itemBoxSizer->GetOrientation() == wxHORIZONTAL );
            contextMenu->AppendSeparator();
        } // else non-docked
    } // then a pane

    m_plugin->PopulateContextMenu( contextMenu );

    contextMenu->AppendSeparator();
    this->InsertTacticsIntoContextMenu ( contextMenu );

    contextMenu->AppendSeparator();
    contextMenu->Append( ID_DASH_PREFS, _("Preferences") );
    PopupMenu( contextMenu );
    delete contextMenu;
}

void DashboardWindow::OnContextMenuSelect( wxCommandEvent& event )
{
    if( event.GetId() < ID_DASH_PREFS ) { // Toggle dashboard visibility
        m_plugin->ShowDashboard( event.GetId()-1, event.IsChecked() );
        SetToolbarItemState( m_plugin->GetToolbarItemId(), m_plugin->GetDashboardWindowShownCount() != 0 );
    }

    switch( event.GetId() ){
    case ID_DASH_PREFS: {
      m_plugin->ShowPreferencesDialog(
                m_plugin->pGetPluginFrame() // running dialog from plugin, can delete all windows
        );
        return; // Does it's own save.
    }
    case ID_DASH_VERTICAL: {
        m_Container->m_sOrientation = _T("V");
        m_plugin->SetApplySaveWinRequest();
        return;
    }
    case ID_DASH_HORIZONTAL: {
        m_Container->m_sOrientation = _T("H");
        m_plugin->SetApplySaveWinRequest();
        return;
    }
    case ID_DASH_UNDOCK: {
        ChangePaneOrientation( GetSizerOrientation(), true );
        break;      // Actually, the pane name has changed so better save
    }
    default:
        this->TacticsInContextMenuAction( event.GetId() );
    }
    m_plugin->SaveConfig();
}

void DashboardWindow::SetColorScheme( PI_ColorScheme cs )
{
    DimeWindow( this );

    //  Improve appearance, especially in DUSK or NIGHT palette
    wxColour col;
    GetGlobalColor( _T("DASHL"), &col );
    SetBackgroundColour( col );

    Refresh( false );
}

void DashboardWindow::ChangePaneOrientation( int orient, bool updateAUImgr )
{
    wxRect rect = m_plugin->pGetPluginFrame()->GetRect();
    wxPoint position;
    position.x = rect.x + 100;
    position.y = rect.y + 100;
    if ( orient == this->GetSizerOrientation() ) {
        wxAuiPaneInfo p = m_pauimgr->GetPane( m_Container->m_pDashboardWindow );
        if ( p.IsOk() && p.IsDocked() ) {
            rect = m_plugin->pGetPluginFrame()->GetRect();
            if ( p.dock_direction ==  wxAUI_DOCK_RIGHT )
                position.x = rect.x + rect.width - 325;
            else
                if ( p.dock_direction ==  wxAUI_DOCK_BOTTOM)
                    position.y = rect.y + rect.height - 400;
            m_pauimgr->GetPane( m_Container->m_pDashboardWindow ).FloatingPosition( position ).Float(); // undock if docked
            m_pauimgr->DetachPane( this );
            wxSize sz = GetMinSize();
            m_Container->m_sName = MakeName();
            bool isvertical = ( (orient == wxVERTICAL) ? true : false );
            m_pauimgr->AddPane(
                this, wxAuiPaneInfo().Name( m_Container->m_sName ).Caption(
                    m_Container->m_sCaption ).CaptionVisible( true ).TopDockable( !isvertical ).BottomDockable(
                        !isvertical ).LeftDockable( false ).RightDockable( isvertical ).MinSize( sz ).BestSize(
                    sz ).FloatingSize( sz ).FloatingPosition( position ).Float().Show( m_Container->m_bIsVisible ) );
            if ( updateAUImgr ){
                m_pauimgr->Update();
            } // then update (saving in event handler)
            return;
        } // then a docked container, undock request
    } // then this is not orientation request, check if this is undock request
    // orientation change request service follows
    m_pauimgr->DetachPane( this );
    SetSizerOrientation( orient );
    bool vertical = orient == wxVERTICAL;
    //wxSize sz = GetSize( orient, wxDefaultSize );
    wxSize sz = GetMinSize();
    // We must change Name to reset AUI perpective
    m_Container->m_sName = MakeName();
    m_pauimgr->AddPane(
        this, wxAuiPaneInfo().Name( m_Container->m_sName ).Caption(
            m_Container->m_sCaption ).CaptionVisible( true ).TopDockable( !vertical ).BottomDockable(
                !vertical ).LeftDockable(
                    false
                    ).RightDockable( vertical ).MinSize( sz ).BestSize(
                    sz ).FloatingSize( sz ).FloatingPosition(
                        position
                        ).Float().Show( m_Container->m_bIsVisible ) );
    if ( updateAUImgr ) m_pauimgr->Update();
}

void DashboardWindow::SetSizerOrientation( int orient )
{
    itemBoxSizer->SetOrientation( orient );
    /* We must reset all MinSize to ensure we start with new default */
    wxWindowListNode* node = GetChildren().GetFirst();
    while(node) {
        node->GetData()->SetMinSize( wxDefaultSize );
        node = node->GetNext();
    }
    SetMinSize( wxDefaultSize );
    Fit();
    SetMinSize( itemBoxSizer->GetMinSize() );
}

int DashboardWindow::GetSizerOrientation()
{
    return itemBoxSizer->GetOrientation();
}

bool isArrayIntEqual( const wxArrayInt& l1, const wxArrayOfInstrument &l2 )
{
    if( l1.GetCount() != l2.GetCount() ) return false;

    for( size_t i = 0; i < l1.GetCount(); i++ )
        if( l1.Item( i ) != l2.Item( i )->m_ID ) return false;

    return true;
}

bool DashboardWindow::isInstrumentListEqual( const wxArrayInt& list )
{
    return isArrayIntEqual( list, m_ArrayOfInstrument );
}

void DashboardWindow::SetInstrumentList( wxArrayInt list, wxArrayString listIDs )
{
    /* options
       ID_DBP_D_SOG: config max value, show STW optional
       ID_DBP_D_COG:  +SOG +HDG? +BRG?
       ID_DBP_D_AWS: config max value. Two arrows for AWS+TWS?
       ID_DBP_D_VMG: config max value
       ID_DBP_I_DPT: config unit (meter, feet, fathoms)
       ID_DBP_D_DPT: show temp optional
       // compass: use COG or HDG
       // velocity range
       // rudder range

       */

    m_ArrayOfInstrument.Clear();

    itemBoxSizer->Clear( true );

    itemBoxSizer->SetSizeHints( this );
    Layout();
    Refresh();

    for( size_t i = 0; i < list.GetCount(); i++ ) {

        DashboardInstrument *instrument;
        instrument = NULL;
        int id = list.Item( i );
        wxString ids = listIDs.Item( i );

        switch( id ){
        case ID_DBP_I_POS:
            instrument = new DashboardInstrument_Position( this, wxID_ANY,
                                                           getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_SOG:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_SOG, _T("%5.2f") );
            break;
        case ID_DBP_D_SOG:
            instrument = new DashboardInstrument_Speedometer( this, wxID_ANY,
                                                              getInstrumentCaption( id ), OCPN_DBP_STC_SOG, 0, g_iDashSpeedMax );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionLabel( g_iDashSpeedMax / 20 + 1,
                                                                         DIAL_LABEL_HORIZONTAL );
            //(DashboardInstrument_Dial *)instrument->SetOptionMarker(0.1, DIAL_MARKER_SIMPLE, 5);
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMarker( 0.5,
                                                                          DIAL_MARKER_SIMPLE, 2 );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionExtraValue(
                OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );
            break;
        case ID_DBP_I_COG:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_COG, _T("%.0f") );
            break;
        case ID_DBP_M_COG:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_MCOG, _T("%.0f") );
            break;
        case ID_DBP_D_COG:
            instrument = new DashboardInstrument_Compass( this, wxID_ANY,
                                                          getInstrumentCaption( id ), OCPN_DBP_STC_COG );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMarker( 5,
                                                                          DIAL_MARKER_SIMPLE, 2 );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionLabel( 30,
                                                                         DIAL_LABEL_ROTATED );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionExtraValue(
                OCPN_DBP_STC_SOG, _T("SOG\n%.2f"), DIAL_POSITION_BOTTOMLEFT );
            break;
        case ID_DBP_D_HDT:
            instrument = new DashboardInstrument_Compass( this, wxID_ANY,
                                                          getInstrumentCaption( id ), OCPN_DBP_STC_HDT );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMarker( 5,
                                                                          DIAL_MARKER_SIMPLE, 2 );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionLabel( 30,
                                                                         DIAL_LABEL_ROTATED );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionExtraValue(
                OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );
            break;
        case ID_DBP_I_STW:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_STW, _T("%.2f") );
            break;
        case ID_DBP_I_HDT: //true heading
            // TODO: Option True or Magnetic
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_HDT, _T("%.0f") );
            break;
        case ID_DBP_I_HDM:  //magnetic heading
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_HDM, _T("%.0f") );
            break;
        case ID_DBP_D_AW:
        case ID_DBP_D_AWA:
            instrument = new DashboardInstrument_Wind( this, wxID_ANY,
                                                       getInstrumentCaption( id ), OCPN_DBP_STC_AWA );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMainValue( _T("%.0f"),
                                                                             DIAL_POSITION_BOTTOMLEFT );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionExtraValue(
                OCPN_DBP_STC_AWS, _T("%.1f"), DIAL_POSITION_INSIDE );
            break;
        case ID_DBP_I_AWS:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_AWS, _T("%.2f") );
            break;
        case ID_DBP_D_AWS:
            instrument = new DashboardInstrument_Speedometer( this, wxID_ANY,
                                                              getInstrumentCaption( id ), OCPN_DBP_STC_AWS, 0, 45 );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionLabel( 5,
                                                                         DIAL_LABEL_HORIZONTAL );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMarker( 1,
                                                                          DIAL_MARKER_SIMPLE, 5 );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMainValue( _T("A:%.2f"),
                                                                             DIAL_POSITION_BOTTOMLEFT );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionExtraValue(
                OCPN_DBP_STC_TWS, _T("T:%.1f"), DIAL_POSITION_BOTTOMRIGHT );
            break;
        case ID_DBP_D_TW: //True Wind angle +-180deg on boat axis
            instrument = new DashboardInstrument_TrueWindAngle( this, wxID_ANY,
                                                                getInstrumentCaption( id ), OCPN_DBP_STC_TWA );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMainValue( _T("%.0f"),
                                                                             DIAL_POSITION_BOTTOMLEFT );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionExtraValue(
                OCPN_DBP_STC_TWS, _T("%.1f"), DIAL_POSITION_INSIDE );
            break;
        case ID_DBP_D_AWA_TWA: //App/True Wind angle +-180deg on boat axis
            instrument = new DashboardInstrument_AppTrueWindAngle(this, wxID_ANY,
                                                                  getInstrumentCaption(id), OCPN_DBP_STC_AWA | OCPN_DBP_STC_TWA);
            static_cast <DashboardInstrument_Dial *>(instrument)->SetOptionMainValue(_T("%.0f"),
                                                                         DIAL_POSITION_NONE);
            static_cast <DashboardInstrument_Dial *>(instrument)->SetOptionExtraValue(
                OCPN_DBP_STC_TWS | OCPN_DBP_STC_AWS, _T("%.1f"), DIAL_POSITION_NONE);
            break;
        case ID_DBP_D_TWD: //True Wind direction and speed
            instrument = new DashboardInstrument_WindCompass( this, wxID_ANY,
                                                              getInstrumentCaption( id ), OCPN_DBP_STC_TWD );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMainValue( _T("%.0f"),
                                                                             DIAL_POSITION_BOTTOMLEFT );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionExtraValue(
                OCPN_DBP_STC_TWS2, _T("%.1f"), DIAL_POSITION_INSIDE );
            break;
        case ID_DBP_I_DPT:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_DPT, _T("%5.1f") );
            break;
        case ID_DBP_D_DPT:
            instrument = new DashboardInstrument_Depth( this, wxID_ANY,
                                                        getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_TMP: //water temperature
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_TMP, _T("%2.1f") );
            break;
        case ID_DBP_I_MDA: //barometric pressure
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_MDA, _T("%5.1f") );
            break;
        case ID_DBP_D_MDA: //barometric pressure
            instrument = new DashboardInstrument_Speedometer( this, wxID_ANY,
                                                              getInstrumentCaption( id ), OCPN_DBP_STC_MDA, 940, 1040 );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionLabel( 10,
                                                                         DIAL_LABEL_HORIZONTAL );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMarker( 5,
                                                                          DIAL_MARKER_SIMPLE, 1 );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMainValue( _T("%5.3f"),
                                                                             DIAL_POSITION_INSIDE );
            break;
        case ID_DBP_I_ATMP: //air temperature
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_ATMP, _T("%2.1f") );
            break;
        case ID_DBP_I_VLW1: // Trip Log
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_VLW1, _T("%2.1f") );
            break;

        case ID_DBP_I_VLW2: // Sum Log
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_VLW2, _T("%2.1f") );
            break;

        case ID_DBP_I_TWA: //true wind angle
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_TWA, _T("%5.0f") );
            break;
        case ID_DBP_I_TWD: //true wind direction
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_TWD, _T("%5.0f") );
            break;
        case ID_DBP_I_TWS: // true wind speed
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_TWS, _T("%2.2f") );
            break;
        case ID_DBP_I_AWA: //apparent wind angle
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_AWA, _T("%5.0f") );
            break;
        case ID_DBP_I_VMG:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_VMG, _T("%5.2f") );
            break;
        case ID_DBP_D_VMG:
            instrument = new DashboardInstrument_Speedometer( this, wxID_ANY,
                                                              getInstrumentCaption( id ), OCPN_DBP_STC_VMG, 0, g_iDashSpeedMax );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionLabel( 1,
                                                                         DIAL_LABEL_HORIZONTAL );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionMarker( 0.5,
                                                                          DIAL_MARKER_SIMPLE, 2 );
            static_cast <DashboardInstrument_Dial *> (instrument)->SetOptionExtraValue(
                OCPN_DBP_STC_SOG, _T("SOG\n%.2f"), DIAL_POSITION_BOTTOMLEFT );
            break;
        case ID_DBP_I_RSA:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_RSA, _T("%5.0f") );
            break;
        case ID_DBP_D_RSA:
            instrument = new DashboardInstrument_RudderAngle( this, wxID_ANY,
                                                              getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_SAT:
            instrument = new DashboardInstrument_Single( this, wxID_ANY,
                                                         getInstrumentCaption( id ), OCPN_DBP_STC_SAT, _T("%5.0f") );
            break;
        case ID_DBP_D_GPS:
            instrument = new DashboardInstrument_GPS( this, wxID_ANY,
                                                      getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_PTR:
            instrument = new DashboardInstrument_Position( this, wxID_ANY,
                                                           getInstrumentCaption( id ), OCPN_DBP_STC_PLA, OCPN_DBP_STC_PLO );
            break;
        case ID_DBP_I_GPSUTC:
            instrument = new DashboardInstrument_Clock( this, wxID_ANY,
                                                        getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_SUN:
            instrument = new DashboardInstrument_Sun( this, wxID_ANY,
                                                      getInstrumentCaption( id ) );
            break;
        case ID_DBP_D_MON:
            instrument = new DashboardInstrument_Moon( this, wxID_ANY,
                                                       getInstrumentCaption( id ) );
            break;
        case ID_DBP_D_WDH:
            instrument = new DashboardInstrument_WindDirHistory(this, wxID_ANY,
                                                                getInstrumentCaption( id ) );
            break;
        case ID_DBP_D_BPH:
            instrument = new DashboardInstrument_BaroHistory(this, wxID_ANY,
                                                             getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_FOS:
            instrument = new DashboardInstrument_FromOwnship( this, wxID_ANY,
                                                              getInstrumentCaption( id ) );
            break;
        case ID_DBP_I_PITCH:
            instrument = new DashboardInstrument_Single(this, wxID_ANY,
                                                        getInstrumentCaption(id), OCPN_DBP_STC_PITCH, _T("%2.1f"));
            break;
        case ID_DBP_I_HEEL:
            instrument = new DashboardInstrument_Single(this, wxID_ANY,
                                                        getInstrumentCaption(id), OCPN_DBP_STC_HEEL, _T("%2.1f"));
            break;
            // any clock display with "LCL" in the format string is converted from UTC to local TZ
        case ID_DBP_I_SUNLCL:
            instrument = new DashboardInstrument_Sun( this, wxID_ANY,
                                                      getInstrumentCaption( id ), _T( "%02i:%02i:%02i LCL" ) );
            break;
        case ID_DBP_I_GPSLCL:
            instrument = new DashboardInstrument_Clock( this, wxID_ANY,
                                                        getInstrumentCaption( id ), OCPN_DBP_STC_CLK, _T( "%02i:%02i:%02i LCL" ) );
            break;
        case ID_DBP_I_CPULCL:
            instrument = new DashboardInstrument_CPUClock( this, wxID_ANY,
                                                           getInstrumentCaption( id ), _T( "%02i:%02i:%02i LCL" ) );
			break;
		case ID_DBP_I_CURRDIR:
			instrument = new DashboardInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_CURRDIR, _T("%2.0f"));
			break;
		case ID_DBP_I_CURRSPD:
			instrument = new DashboardInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_CURRSPD, _T("%2.2f"));
			break;
        case ID_DBP_I_LEEWAY:
            instrument = new DashboardInstrument_Single(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_LEEWAY,
                _T("%2.1f"));
            break;
        case ID_DBP_D_BRG:  // Bearing Compass
            instrument = new TacticsInstrument_BearingCompass(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_COG |
                OCPN_DBP_STC_BRG | OCPN_DBP_STC_CURRDIR |
                OCPN_DBP_STC_CURRSPD | OCPN_DBP_STC_TWA |
                OCPN_DBP_STC_LEEWAY | OCPN_DBP_STC_HDT |
                OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON |
                OCPN_DBP_STC_STW | OCPN_DBP_STC_AWA |
                OCPN_DBP_STC_TWS | OCPN_DBP_STC_TWD);
            static_cast <DashboardInstrument_Dial *>(instrument)->SetOptionMarker(
                5, DIAL_MARKER_SIMPLE, 2);
            static_cast <DashboardInstrument_Dial *>(instrument)->SetOptionLabel(
                30, DIAL_LABEL_ROTATED);
            static_cast <DashboardInstrument_Dial *>(instrument)->SetOptionExtraValue(
                 OCPN_DBP_STC_DTW, _T("%.2f"), DIAL_POSITION_TOPLEFT);
            break;
        case ID_DBP_D_POLCOMP: // Polar Compass
            instrument = new TacticsInstrument_PolarCompass(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_COG |
                OCPN_DBP_STC_BRG | OCPN_DBP_STC_CURRDIR |
                OCPN_DBP_STC_CURRSPD | OCPN_DBP_STC_TWA |
                OCPN_DBP_STC_LEEWAY | OCPN_DBP_STC_HDT |
                OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON |
                OCPN_DBP_STC_STW | OCPN_DBP_STC_AWA |
                OCPN_DBP_STC_TWS | OCPN_DBP_STC_TWD);
            static_cast <DashboardInstrument_Dial *>(instrument)->SetOptionMarker(
                5, DIAL_MARKER_SIMPLE, 2);
            static_cast <DashboardInstrument_Dial *>(instrument)->SetOptionLabel(
                30, DIAL_LABEL_ROTATED);
            static_cast <DashboardInstrument_Dial *>(instrument)->SetOptionExtraValue(
                 OCPN_DBP_STC_DTW, _T("%.2f"), DIAL_POSITION_TOPLEFT);
            break;
        case ID_DBP_I_TWAMARK:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id),
                OCPN_DBP_STC_BRG | OCPN_DBP_STC_TWD |
                OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON, _T("%5.0f"));
            static_cast <TacticsInstrument_PerformanceSingle *>(instrument)->SetDisplayType(TWAMARK);
            break;
		case ID_DBP_I_POLSPD:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
            static_cast <TacticsInstrument_PerformanceSingle *>(instrument)->SetDisplayType(POLARSPEED);
            break;
        case ID_DBP_I_POLVMG:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
            static_cast <TacticsInstrument_PerformanceSingle *>(instrument)->SetDisplayType(POLARVMG);
            break;
        case ID_DBP_I_POLTVMG:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
            static_cast <TacticsInstrument_PerformanceSingle *>(instrument)->SetDisplayType(POLARTARGETVMG);
            break;
        case ID_DBP_I_POLTVMGANGLE:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
            static_cast <TacticsInstrument_PerformanceSingle *>(instrument)->SetDisplayType(POLARTARGETVMGANGLE);
            break;
        case ID_DBP_I_POLCMG:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_COG | OCPN_DBP_STC_SOG | OCPN_DBP_STC_BRG |
                OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON, _T("%.2f"));
            static_cast <TacticsInstrument_PerformanceSingle *>(instrument)->SetDisplayType(POLARCMG);
            break;
        case ID_DBP_I_POLTCMG:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS | OCPN_DBP_STC_HDT |
                OCPN_DBP_STC_BRG | OCPN_DBP_STC_TWD | OCPN_DBP_STC_LAT |
                OCPN_DBP_STC_LON, _T("%.2f"));
            static_cast <TacticsInstrument_PerformanceSingle *>(instrument)->SetDisplayType(POLARTARGETCMG);
            break;
        case ID_DBP_I_POLTCMGANGLE:
            instrument = new TacticsInstrument_PerformanceSingle(
                this, wxID_ANY,
                getInstrumentCaption(id), OCPN_DBP_STC_STW |
                OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS | OCPN_DBP_STC_HDT |
                OCPN_DBP_STC_BRG | OCPN_DBP_STC_TWD | OCPN_DBP_STC_LAT |
                OCPN_DBP_STC_LON, _T("%.2f"));
            static_cast <TacticsInstrument_PerformanceSingle *>(instrument)->SetDisplayType(POLARTARGETCMGANGLE);
            break;
        case ID_DBP_V_IFLX:
            instrument = new TacticsInstrument_StreamoutSingle(
                this, wxID_ANY,
                getInstrumentCaption(id),
                OCPN_DBP_STC_LAT          |
                OCPN_DBP_STC_LON          |
                OCPN_DBP_STC_SOG          |
                OCPN_DBP_STC_COG          |
                OCPN_DBP_STC_STW          |
                OCPN_DBP_STC_HDM          |
                OCPN_DBP_STC_HDT          |
                OCPN_DBP_STC_HMV          |
                OCPN_DBP_STC_BRG          |
                OCPN_DBP_STC_AWA          |
                OCPN_DBP_STC_AWS          |
                OCPN_DBP_STC_TWA          |
                OCPN_DBP_STC_TWS          |
                OCPN_DBP_STC_DPT          |
                OCPN_DBP_STC_TMP          |
                OCPN_DBP_STC_VMG          |
                OCPN_DBP_STC_RSA          |
                OCPN_DBP_STC_SAT          |
                OCPN_DBP_STC_PLA          |
                OCPN_DBP_STC_PLO          |
                OCPN_DBP_STC_ATMP         |
                OCPN_DBP_STC_TWD          |
                OCPN_DBP_STC_TWS2         |
                OCPN_DBP_STC_VLW1         |
                OCPN_DBP_STC_VLW2         |
                OCPN_DBP_STC_MDA          |
                OCPN_DBP_STC_MCOG         |
                OCPN_DBP_STC_PITCH        |
                OCPN_DBP_STC_HEEL         |
                OCPN_DBP_STC_LEEWAY       |
                OCPN_DBP_STC_CURRDIR      |
                OCPN_DBP_STC_CURRSPD      |
                OCPN_DBP_STC_DTW          |
                OCPN_DBP_STC_TWAMARK      |
                OCPN_DBP_STC_POLPERF      |
                OCPN_DBP_STC_POLSPD       |
                OCPN_DBP_STC_POLVMG       |
                OCPN_DBP_STC_POLTVMG      |
                OCPN_DBP_STC_POLTVMGANGLE |
                OCPN_DBP_STC_POLCMG       |
                OCPN_DBP_STC_POLTCMG      |
                OCPN_DBP_STC_POLTCMGANGLE |
                OCPN_DBP_STC_SKSUBSCRIBE,
                _T("%s"),
                m_plugin->m_mtxNofStreamOut,
                m_plugin->m_nofStreamOut,
                m_plugin->m_echoStreamerShow,
                m_plugin->GetStandardPath(),
                this->m_pSkData );
            break;
        case ID_DBP_V_INSK:
            instrument = new TacticsInstrument_StreamInSkSingle(
                this, wxID_ANY,
                getInstrumentCaption(id),
                0ULL,
                _T("%s"),
                m_plugin->m_mtxNofStreamInSk,
                m_plugin->m_nofStreamInSk,
                m_plugin->m_echoStreamerInSkShow,
                m_plugin->GetStandardPath(),
                this->m_pSkData );
            break;
        case ID_DBP_D_POLPERF:
            instrument = new TacticsInstrument_PolarPerformance(
                this, wxID_ANY, getInstrumentCaption(id));
            break;
        case ID_DBP_D_AVGWIND:
            instrument = new TacticsInstrument_AvgWindDir(
                this, wxID_ANY, getInstrumentCaption(id));
            break;
        case ID_DBP_D_ENGDJG:
            if ( ids.IsEmpty() )
                ids = GetUUID();
            instrument = new DashboardInstrument_EngineDJG( // Dial instrument
                this, wxID_ANY, ids, m_plugin->m_colorScheme );
            break;
        case ID_DBP_D_TSETUI:
            if ( ids.IsEmpty() )
                ids = GetUUID();
            instrument = new DashboardInstrument_TimesTUI( // Time-series DB graph
                this, wxID_ANY, ids, m_plugin->m_colorScheme );
            break;
        }
        if( instrument ) {
            instrument->instrumentTypeId = id;
            Unbind( wxEVT_SIZE, &DashboardWindow::OnSize, this );
            m_ArrayOfInstrument.Add(
                new DashboardInstrumentContainer(
                    id, instrument, instrument->GetCapacity(), ids ) );
            itemBoxSizer->Add( instrument, 0, wxEXPAND, 0 );
            Bind( wxEVT_SIZE, &DashboardWindow::OnSize, this );
            itemBoxSizer->SetSizeHints( this );
            Layout();
            if( itemBoxSizer->GetOrientation() == wxHORIZONTAL ) {
                itemBoxSizer->AddSpacer( 5 );
                itemBoxSizer->SetSizeHints( this );
                Layout();
            }
        }
        m_Container->m_aInstrumentIDs.Item( i ) = ids; // UUID for persistance
    } // for items in the list
    itemBoxSizer->SetSizeHints( this );
    Layout();
    Refresh();

}

void DashboardWindow::SendSentenceToAllInstruments(
    unsigned long long st,
    double value, wxString unit
        , long long timestamp
    )
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {

        if (
            (!((m_ArrayOfInstrument.Item( i )->m_cap_flag & st) == 0ULL))
                )
            m_ArrayOfInstrument.Item( i )->m_pInstrument->SetData(
                st, value, unit
                , timestamp
                );
    }
}

void DashboardWindow::SendSatInfoToAllInstruments( int cnt, int seq, SAT_INFO sats[4] )
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
        if(
             (!((m_ArrayOfInstrument.Item( i )->m_cap_flag &
                 OCPN_DBP_STC_GPS) == 0ULL))
            && m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf(
                CLASSINFO(DashboardInstrument_GPS)))
            static_cast <DashboardInstrument_GPS*>(m_ArrayOfInstrument.Item(i)->m_pInstrument)->SetSatInfo(
                cnt, seq, sats);
    }
}

void DashboardWindow::SendUtcTimeToAllInstruments( wxDateTime value )
{
    for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
         if(
             (!((m_ArrayOfInstrument.Item( i )->m_cap_flag &
                 OCPN_DBP_STC_CLK) == 0ULL))
             && m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf( CLASSINFO( DashboardInstrument_Clock ) ) )
            //                  || m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf( CLASSINFO( DashboardInstrument_Sun ) )
            //                  || m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf( CLASSINFO( DashboardInstrument_Moon ) ) ) )
             static_cast <DashboardInstrument_Clock*>(m_ArrayOfInstrument.Item(i)->m_pInstrument)->SetUtcTime( value );
    }
}

void DashboardWindow::SendColorSchemeToAllJSInstruments( PI_ColorScheme cs )
{
    for ( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
        if ( m_ArrayOfInstrument.Item( i )->m_pInstrument != NULL )
            m_ArrayOfInstrument.Item(i)->m_pInstrument->setColorScheme( cs );
    }
}
