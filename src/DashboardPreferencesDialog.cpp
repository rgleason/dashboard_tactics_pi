/***************************************************************************
 * $Id: DashboardPreferencesDialog.cpp, v1.0 2010/08/05 SethDart Exp $
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

#include "dashboard_pi.h"
#include "icons.h"

// This module has been detached from dashboard_pi.cpp defining below:
extern wxFont *g_pFontTitle;
extern wxFont *g_pFontData;
extern wxFont *g_pFontLabel;
extern wxFont *g_pFontSmall;
extern int g_iDashSpeedMax;
extern int g_iDashCOGDamp;
extern int g_iDashSpeedUnit;
extern int g_iDashSOGDamp;
extern int g_iDashDepthUnit;
extern int g_iDashDistanceUnit;
extern int g_iDashWindSpeedUnit;
extern int g_iDashTemperatureUnit;
extern int g_iUTCOffset;
extern double g_dDashDBTOffset;

extern bool getListItemForInstrument( wxListItem &item, unsigned int id );
extern wxString MakeName( void );   

DashboardPreferencesDialog::DashboardPreferencesDialog(
    wxWindow *parent, wxWindowID id,
    wxArrayOfDashboard config
    , wxString commonName, wxString versionName, wxPoint pos ) :
    TacticsPreferencesDialog ( parent, id, commonName + " " + versionName + _(" Preferences"), pos )
{
    Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DashboardPreferencesDialog::OnCloseDialog ),
             NULL, this );

    // Copy original config
    m_Config = wxArrayOfDashboard( config );
    //      Build Dashboard Page for Toolbox
    int border_size = 2;

    wxBoxSizer* itemBoxSizerMainPanel = new wxBoxSizer( wxVERTICAL );
    SetSizer( itemBoxSizerMainPanel );

    wxNotebook *itemNotebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP );

    itemBoxSizerMainPanel->Add( itemNotebook, 1, wxALL | wxEXPAND, border_size );

    m_itemNotebook = itemNotebook;
    this->TacticsPreferencesInit( m_itemNotebook, border_size );

    wxPanel *itemPanelNotebook01 = new wxPanel( itemNotebook, wxID_ANY, wxDefaultPosition,
                                                wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer *itemFlexGridSizer01 = new wxFlexGridSizer( 2 );
    itemFlexGridSizer01->AddGrowableCol( 1 );
    itemPanelNotebook01->SetSizer( itemFlexGridSizer01 );
    itemNotebook->AddPage( itemPanelNotebook01,
                           commonName
        );

    wxBoxSizer *itemBoxSizer01 = new wxBoxSizer( wxVERTICAL );
    itemFlexGridSizer01->Add( itemBoxSizer01, 1, wxEXPAND | wxTOP | wxLEFT, border_size );

    wxImageList *imglist1 = new wxImageList( 32, 32, true, 1);
    imglist1->Add(
        *_img_dashboard_tactics_pi
        );

    m_pListCtrlDashboards = new wxListCtrl(
        itemPanelNotebook01, wxID_ANY, wxDefaultPosition,
        wxSize( 50, 200 ), wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL );
    m_pListCtrlDashboards->AssignImageList( imglist1, wxIMAGE_LIST_SMALL );
    m_pListCtrlDashboards->InsertColumn( 0, _T("") );
    m_pListCtrlDashboards->Connect(
        wxEVT_COMMAND_LIST_ITEM_SELECTED,
        wxListEventHandler(DashboardPreferencesDialog::OnDashboardSelected), NULL, this );
    m_pListCtrlDashboards->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED,
                                    wxListEventHandler(DashboardPreferencesDialog::OnDashboardSelected), NULL, this );
    itemBoxSizer01->Add( m_pListCtrlDashboards, 1, wxEXPAND, 0 );

    wxBoxSizer *itemBoxSizer02 = new wxBoxSizer( wxHORIZONTAL );
    itemBoxSizer01->Add( itemBoxSizer02 );

    m_pButtonAddDashboard = new wxBitmapButton( itemPanelNotebook01, wxID_ANY, *_img_plus,
                                                wxDefaultPosition, wxDefaultSize );
    itemBoxSizer02->Add( m_pButtonAddDashboard, 0, wxALIGN_CENTER, 2 );
    m_pButtonAddDashboard->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                                    wxCommandEventHandler(DashboardPreferencesDialog::OnDashboardAdd), NULL, this );
    m_pButtonDeleteDashboard = new wxBitmapButton( itemPanelNotebook01, wxID_ANY, *_img_minus,
                                                   wxDefaultPosition, wxDefaultSize );
    itemBoxSizer02->Add( m_pButtonDeleteDashboard, 0, wxALIGN_CENTER, 2 );
    m_pButtonDeleteDashboard->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                                       wxCommandEventHandler(DashboardPreferencesDialog::OnDashboardDelete), NULL, this );

    m_pPanelDashboard = new wxPanel( itemPanelNotebook01, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize, wxBORDER_SUNKEN );
    itemFlexGridSizer01->Add( m_pPanelDashboard, 1, wxEXPAND | wxTOP | wxRIGHT, border_size );

    wxBoxSizer* itemBoxSizer03 = new wxBoxSizer( wxVERTICAL );
    m_pPanelDashboard->SetSizer( itemBoxSizer03 );

    wxStaticBox* itemStaticBox02 = new wxStaticBox( m_pPanelDashboard, wxID_ANY,
                                                    commonName
        );
    wxStaticBoxSizer* itemStaticBoxSizer02 = new wxStaticBoxSizer( itemStaticBox02, wxHORIZONTAL );
    itemBoxSizer03->Add( itemStaticBoxSizer02, 0, wxEXPAND | wxALL, border_size );
    wxFlexGridSizer *itemFlexGridSizer = new wxFlexGridSizer( 2 );
    itemFlexGridSizer->AddGrowableCol( 1 );
    itemStaticBoxSizer02->Add( itemFlexGridSizer, 1, wxEXPAND | wxALL, 0 );

    m_pCheckBoxIsVisible = new wxCheckBox( m_pPanelDashboard, wxID_ANY, _("show this dashboard"),
                                           wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer->Add( m_pCheckBoxIsVisible, 0, wxEXPAND | wxALL, border_size );
    wxStaticText *itemDummy01 = new wxStaticText( m_pPanelDashboard, wxID_ANY, _T("") );
    itemFlexGridSizer->Add( itemDummy01, 0, wxEXPAND | wxALL, border_size );

    wxStaticText* itemStaticText01 = new wxStaticText( m_pPanelDashboard, wxID_ANY, _("Caption:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer->Add( itemStaticText01, 0, wxEXPAND | wxALL, border_size );
    m_pTextCtrlCaption = new wxTextCtrl( m_pPanelDashboard, wxID_ANY, _T(""), wxDefaultPosition,
                                         wxDefaultSize );
    itemFlexGridSizer->Add( m_pTextCtrlCaption, 0, wxEXPAND | wxALL, border_size );

    wxStaticText* itemStaticText02 = new wxStaticText( m_pPanelDashboard, wxID_ANY,
                                                       _("Orientation:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer->Add( itemStaticText02, 0, wxEXPAND | wxALL, border_size );
    m_pChoiceOrientation = new wxChoice( m_pPanelDashboard, wxID_ANY, wxDefaultPosition,
                                         wxSize( 120, -1 ) );
    m_pChoiceOrientation->Append( _("Vertical") );
    m_pChoiceOrientation->Append( _("Horizontal") );
    itemFlexGridSizer->Add( m_pChoiceOrientation, 0, wxALIGN_RIGHT | wxALL, border_size );

    wxImageList *imglist = new wxImageList( 20, 20, true, 2 );
    imglist->Add( *_img_instrument );
    imglist->Add( *_img_dial );

    wxStaticBox* itemStaticBox03 = new wxStaticBox( m_pPanelDashboard, wxID_ANY, _("Instruments") );
    wxStaticBoxSizer* itemStaticBoxSizer03 = new wxStaticBoxSizer( itemStaticBox03, wxHORIZONTAL );
    itemBoxSizer03->Add( itemStaticBoxSizer03, 1, wxEXPAND | wxALL, border_size );

    m_pListCtrlInstruments = new wxListCtrl( m_pPanelDashboard, wxID_ANY, wxDefaultPosition,
                                             wxSize( -1, 200 ), wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL );
    itemStaticBoxSizer03->Add( m_pListCtrlInstruments, 1, wxEXPAND | wxALL, border_size );
    m_pListCtrlInstruments->AssignImageList( imglist, wxIMAGE_LIST_SMALL );
    m_pListCtrlInstruments->InsertColumn( 0, _("Instruments") );
    m_pListCtrlInstruments->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED,
                                     wxListEventHandler(DashboardPreferencesDialog::OnInstrumentSelected), NULL, this );
    m_pListCtrlInstruments->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED,
                                     wxListEventHandler(DashboardPreferencesDialog::OnInstrumentSelected), NULL, this );

    wxBoxSizer* itemBoxSizer04 = new wxBoxSizer( wxVERTICAL );
    itemStaticBoxSizer03->Add( itemBoxSizer04, 0, wxALIGN_TOP | wxALL, border_size );
    m_pButtonAdd = new wxButton( m_pPanelDashboard, wxID_ANY, _("Add"), wxDefaultPosition,
                                 wxSize( 20, -1 ) );
    itemBoxSizer04->Add( m_pButtonAdd, 0, wxEXPAND | wxALL, border_size );
    m_pButtonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                           wxCommandEventHandler(DashboardPreferencesDialog::OnInstrumentAdd), NULL, this );

    /* TODO  Instrument Properties
       m_pButtonEdit = new wxButton( m_pPanelDashboard, wxID_ANY, _("Edit"), wxDefaultPosition,
       wxDefaultSize );
       itemBoxSizer04->Add( m_pButtonEdit, 0, wxEXPAND | wxALL, border_size );
       m_pButtonEdit->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
       wxCommandEventHandler(DashboardPreferencesDialog::OnInstrumentEdit), NULL, this );
    */
    m_pButtonDelete = new wxButton( m_pPanelDashboard, wxID_ANY, _("Delete"), wxDefaultPosition,
                                    wxSize( 20, -1 ) );
    itemBoxSizer04->Add( m_pButtonDelete, 0, wxEXPAND | wxALL, border_size );
    m_pButtonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                              wxCommandEventHandler(DashboardPreferencesDialog::OnInstrumentDelete), NULL, this );
    itemBoxSizer04->AddSpacer( 10 );
    m_pButtonUp = new wxButton( m_pPanelDashboard, wxID_ANY, _("Up"), wxDefaultPosition,
                                wxDefaultSize );
    itemBoxSizer04->Add( m_pButtonUp, 0, wxEXPAND | wxALL, border_size );
    m_pButtonUp->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                          wxCommandEventHandler(DashboardPreferencesDialog::OnInstrumentUp), NULL, this );
    m_pButtonDown = new wxButton( m_pPanelDashboard, wxID_ANY, _("Down"), wxDefaultPosition,
                                  wxDefaultSize );
    itemBoxSizer04->Add( m_pButtonDown, 0, wxEXPAND | wxALL, border_size );
    m_pButtonDown->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
                            wxCommandEventHandler(DashboardPreferencesDialog::OnInstrumentDown), NULL, this );

    wxScrolledWindow *itemPanelNotebook02 = new wxScrolledWindow(
        itemNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxVSCROLL );
    int scrollRate = 5;
#ifdef __OCPN__ANDROID__
    scrollRate = 1;
#endif
    itemPanelNotebook02->SetScrollRate(0, scrollRate);

    wxBoxSizer* itemBoxSizer05 = new wxBoxSizer( wxVERTICAL );
    itemPanelNotebook02->SetSizer( itemBoxSizer05 );
    itemNotebook->AddPage( itemPanelNotebook02, _("Appearance") );

    wxStaticBox* itemStaticBox01 = new wxStaticBox( itemPanelNotebook02, wxID_ANY, _("Fonts") );
    wxStaticBoxSizer* itemStaticBoxSizer01 = new wxStaticBoxSizer( itemStaticBox01, wxHORIZONTAL );
    itemBoxSizer05->Add( itemStaticBoxSizer01, 0, wxEXPAND | wxALL, border_size );
    wxFlexGridSizer *itemFlexGridSizer03 = new wxFlexGridSizer( 2 );
    itemFlexGridSizer03->AddGrowableCol( 1 );
    itemStaticBoxSizer01->Add( itemFlexGridSizer03, 1, wxEXPAND | wxALL, 0 );
    wxStaticText* itemStaticText04 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Title:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer03->Add( itemStaticText04, 0, wxEXPAND | wxALL, border_size );
    m_pFontPickerTitle = new wxFontPickerCtrl( itemPanelNotebook02, wxID_ANY, *g_pFontTitle,
                                               wxDefaultPosition, wxDefaultSize );
    itemFlexGridSizer03->Add( m_pFontPickerTitle, 0, wxALIGN_RIGHT | wxALL, 0 );
    wxStaticText* itemStaticText05 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Data:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer03->Add( itemStaticText05, 0, wxEXPAND | wxALL, border_size );
    m_pFontPickerData = new wxFontPickerCtrl( itemPanelNotebook02, wxID_ANY, *g_pFontData,
                                              wxDefaultPosition, wxDefaultSize );
    itemFlexGridSizer03->Add( m_pFontPickerData, 0, wxALIGN_RIGHT | wxALL, 0 );
    wxStaticText* itemStaticText06 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Label:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer03->Add( itemStaticText06, 0, wxEXPAND | wxALL, border_size );
    m_pFontPickerLabel = new wxFontPickerCtrl( itemPanelNotebook02, wxID_ANY, *g_pFontLabel,
                                               wxDefaultPosition, wxDefaultSize );
    itemFlexGridSizer03->Add( m_pFontPickerLabel, 0, wxALIGN_RIGHT | wxALL, 0 );
    wxStaticText* itemStaticText07 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Small:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer03->Add( itemStaticText07, 0, wxEXPAND | wxALL, border_size );
    m_pFontPickerSmall = new wxFontPickerCtrl( itemPanelNotebook02, wxID_ANY, *g_pFontSmall,
                                               wxDefaultPosition, wxDefaultSize );
    itemFlexGridSizer03->Add( m_pFontPickerSmall, 0, wxALIGN_RIGHT | wxALL, 0 );
    //      wxColourPickerCtrl

    wxStaticBox* itemStaticBox04 = new wxStaticBox( itemPanelNotebook02, wxID_ANY, _("Units, Ranges, Formats") );
    wxStaticBoxSizer* itemStaticBoxSizer04 = new wxStaticBoxSizer( itemStaticBox04, wxHORIZONTAL );
    itemBoxSizer05->Add( itemStaticBoxSizer04, 0, wxEXPAND | wxALL, border_size );
    wxFlexGridSizer *itemFlexGridSizer04 = new wxFlexGridSizer( 2 );
    itemFlexGridSizer04->AddGrowableCol( 1 );
    itemStaticBoxSizer04->Add( itemFlexGridSizer04, 1, wxEXPAND | wxALL, 0 );
    wxStaticText* itemStaticText08 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Speedometer max value:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText08, 0, wxEXPAND | wxALL, border_size );
    m_pSpinSpeedMax = new wxSpinCtrl( itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 10, 100, g_iDashSpeedMax );
    itemFlexGridSizer04->Add( m_pSpinSpeedMax, 0, wxALIGN_RIGHT | wxALL, 0 );

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Speed Over Ground Damping Factor:"),
                                                       wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer04->Add(itemStaticText10, 0, wxEXPAND | wxALL, border_size);
    m_pSpinSOGDamp = new wxSpinCtrl(itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, g_iDashSOGDamp);
    itemFlexGridSizer04->Add(m_pSpinSOGDamp, 0, wxALIGN_RIGHT | wxALL, 0);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("COG Damping Factor:"),
                                                       wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer04->Add(itemStaticText11, 0, wxEXPAND | wxALL, border_size);
    m_pSpinCOGDamp = new wxSpinCtrl(itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, g_iDashCOGDamp);
    itemFlexGridSizer04->Add(m_pSpinCOGDamp, 0, wxALIGN_RIGHT | wxALL, 0);

    wxStaticText* itemStaticText12 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _( "Local Time Offset From UTC:" ),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText12, 0, wxEXPAND | wxALL, border_size );
    wxString m_UTCOffsetChoices[] = {
        _T( "-12:00" ), _T( "-11:30" ), _T( "-11:00" ), _T( "-10:30" ), _T( "-10:00" ), _T( "-09:30" ),
        _T( "-09:00" ), _T( "-08:30" ), _T( "-08:00" ), _T( "-07:30" ), _T( "-07:00" ), _T( "-06:30" ),
        _T( "-06:00" ), _T( "-05:30" ), _T( "-05:00" ), _T( "-04:30" ), _T( "-04:00" ), _T( "-03:30" ),
        _T( "-03:00" ), _T( "-02:30" ), _T( "-02:00" ), _T( "-01:30" ), _T( "-01:00" ), _T( "-00:30" ),
        _T( " 00:00" ), _T( " 00:30" ), _T( " 01:00" ), _T( " 01:30" ), _T( " 02:00" ), _T( " 02:30" ),
        _T( " 03:00" ), _T( " 03:30" ), _T( " 04:00" ), _T( " 04:30" ), _T( " 05:00" ), _T( " 05:30" ),
        _T( " 06:00" ), _T( " 06:30" ), _T( " 07:00" ), _T( " 07:30" ), _T( " 08:00" ), _T( " 08:30" ),
        _T( " 09:00" ), _T( " 09:30" ), _T( " 10:00" ), _T( " 10:30" ), _T( " 11:00" ), _T( " 11:30" ),
        _T( " 12:00" )
    };
    int m_UTCOffsetNChoices = sizeof( m_UTCOffsetChoices ) / sizeof( wxString );
    m_pChoiceUTCOffset = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_UTCOffsetNChoices, m_UTCOffsetChoices, 0 );
    m_pChoiceUTCOffset->SetSelection( g_iUTCOffset + 24 );
    itemFlexGridSizer04->Add( m_pChoiceUTCOffset, 0, wxALIGN_RIGHT | wxALL, 0 );

    wxStaticText* itemStaticText09 = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Boat speed units:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText09, 0, wxEXPAND | wxALL, border_size );
    wxString m_SpeedUnitChoices[] = { _("Honor OpenCPN settings"), _("Kts"), _("mph"), _("km/h"), _("m/s") };
    int m_SpeedUnitNChoices = sizeof( m_SpeedUnitChoices ) / sizeof( wxString );
    m_pChoiceSpeedUnit = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_SpeedUnitNChoices, m_SpeedUnitChoices, 0 );
    m_pChoiceSpeedUnit->SetSelection( g_iDashSpeedUnit + 1 );
    itemFlexGridSizer04->Add( m_pChoiceSpeedUnit, 0, wxALIGN_RIGHT | wxALL, 0 );

    wxStaticText* itemStaticTextDepthU = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Depth units:"),
                                                           wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticTextDepthU, 0, wxEXPAND | wxALL, border_size );
    wxString m_DepthUnitChoices[] = { _("Meters"), _("Feet"), _("Fathoms"), _("Inches"), _("Centimeters") };
    int m_DepthUnitNChoices = sizeof( m_DepthUnitChoices ) / sizeof( wxString );
    m_pChoiceDepthUnit = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DepthUnitNChoices, m_DepthUnitChoices, 0 );
    m_pChoiceDepthUnit->SetSelection( g_iDashDepthUnit - 3);
    itemFlexGridSizer04->Add( m_pChoiceDepthUnit, 0, wxALIGN_RIGHT | wxALL, 0 );
    wxString dMess = wxString::Format(_("Depth Offset (%s):"),m_DepthUnitChoices[g_iDashDepthUnit-3]);
    wxStaticText* itemStaticDepthO = new wxStaticText(itemPanelNotebook02, wxID_ANY, dMess,
                                                      wxDefaultPosition, wxDefaultSize, 0);
    double DepthOffset;
    switch (g_iDashDepthUnit - 3) {
    case 1:
        DepthOffset = g_dDashDBTOffset * 3.2808399;
        break;
    case 2:
        DepthOffset = g_dDashDBTOffset * 0.54680665;
        break;
    case 3:
        DepthOffset = g_dDashDBTOffset * 39.3700787;
        break;
    case 4:
        DepthOffset = g_dDashDBTOffset * 100;
        break;
    default:
        DepthOffset = g_dDashDBTOffset;
    }
    itemFlexGridSizer04->Add(itemStaticDepthO, 0, wxEXPAND | wxALL, border_size);
    m_pSpinDBTOffset = new wxSpinCtrlDouble(itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100, 100, DepthOffset, 0.1);
    itemFlexGridSizer04->Add(m_pSpinDBTOffset, 0, wxALIGN_RIGHT | wxALL, 0);

    wxStaticText* itemStaticText0b = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Distance units:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText0b, 0, wxEXPAND | wxALL, border_size );
    wxString m_DistanceUnitChoices[] = { _("Honor OpenCPN settings"), _("Nautical miles"), _("Statute miles"), _("Kilometers"), _("Meters") };
    int m_DistanceUnitNChoices = sizeof( m_DistanceUnitChoices ) / sizeof( wxString );
    m_pChoiceDistanceUnit = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DistanceUnitNChoices, m_DistanceUnitChoices, 0 );
    m_pChoiceDistanceUnit->SetSelection( g_iDashDistanceUnit + 1 );
    itemFlexGridSizer04->Add( m_pChoiceDistanceUnit, 0, wxALIGN_RIGHT | wxALL, 0 );

    wxStaticText* itemStaticText0a = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Wind speed units:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText0a, 0, wxEXPAND | wxALL, border_size );
    wxString m_WSpeedUnitChoices[] = { _("Kts"), _("mph"), _("km/h"), _("m/s") };
    int m_WSpeedUnitNChoices = sizeof( m_WSpeedUnitChoices ) / sizeof( wxString );
    m_pChoiceWindSpeedUnit = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_WSpeedUnitNChoices, m_WSpeedUnitChoices, 0 );
    m_pChoiceWindSpeedUnit->SetSelection( g_iDashWindSpeedUnit );
    itemFlexGridSizer04->Add( m_pChoiceWindSpeedUnit, 0, wxALIGN_RIGHT | wxALL, 0 );

    wxStaticText* itemStaticText0c = new wxStaticText( itemPanelNotebook02, wxID_ANY, _("Temperature units:"),
                                                       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer04->Add( itemStaticText0c, 0, wxEXPAND | wxALL, border_size );
    wxString m_TempUnitChoices[] = { _("Celsius"), _("Fahrenheit") };
    int m_TempUnitNChoices = sizeof( m_TempUnitChoices ) / sizeof( wxString );
    m_pChoiceTemperatureUnit = new wxChoice( itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_TempUnitNChoices, m_TempUnitChoices, 0 );
    m_pChoiceTemperatureUnit->SetSelection( g_iDashTemperatureUnit );
    itemFlexGridSizer04->Add( m_pChoiceTemperatureUnit, 0, wxALIGN_RIGHT | wxALL, 0 );

    //////////////////////////////////////////////////////////////
    this->TacticsPreferencesPanel();

    wxStdDialogButtonSizer* DialogButtonSizer = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
    itemBoxSizerMainPanel->Add( DialogButtonSizer, 0, wxALIGN_RIGHT | wxALL, 5 );

    curSel = -1;
    for( size_t i = 0; i < m_Config.GetCount(); i++ ) {
        m_pListCtrlDashboards->InsertItem( i, 0 );
        // Using data to store m_Config index for managing deletes
        m_pListCtrlDashboards->SetItemData( i, i );
    }
    m_pListCtrlDashboards->SetColumnWidth( 0, wxLIST_AUTOSIZE );

    UpdateDashboardButtonsState();
    UpdateButtonsState();
    SetMinSize( wxSize( 450, -1 ) );
    Fit();
    if ( pos == wxDefaultPosition )
        Center();
}

void DashboardPreferencesDialog::OnCloseDialog( wxCloseEvent& event )
{
    SaveDashboardConfig();
    event.Skip();
}

void DashboardPreferencesDialog::SaveDashboardConfig()
{
    g_iDashSpeedMax = m_pSpinSpeedMax->GetValue();
    g_iDashCOGDamp = m_pSpinCOGDamp->GetValue();
    g_iDashSOGDamp = m_pSpinSOGDamp->GetValue();
    g_iUTCOffset = m_pChoiceUTCOffset->GetSelection() - 24;
    g_iDashSpeedUnit = m_pChoiceSpeedUnit->GetSelection() - 1;
    double DashDBTOffset = m_pSpinDBTOffset->GetValue();
    switch (g_iDashDepthUnit - 3) {
    case 1:
        g_dDashDBTOffset = DashDBTOffset / 3.2808399;
        break;
    case 2:
        g_dDashDBTOffset = DashDBTOffset / 0.54680665;
        break;
    case 3:
        g_dDashDBTOffset = DashDBTOffset / 39.3700787;
        break;
    case 4:
        g_dDashDBTOffset = DashDBTOffset / 100;
        break;
    default:
        g_dDashDBTOffset = DashDBTOffset;
    }
    g_iDashDepthUnit = m_pChoiceDepthUnit->GetSelection() + 3;
    g_iDashDistanceUnit = m_pChoiceDistanceUnit->GetSelection() - 1;
    g_iDashWindSpeedUnit = m_pChoiceWindSpeedUnit->GetSelection();
    g_iDashTemperatureUnit = m_pChoiceTemperatureUnit->GetSelection();

    this->SaveTacticsConfig();

    if( curSel != -1 ) {
        DashboardWindowContainer *cont = m_Config.Item( curSel );
        cont->m_bIsVisible = m_pCheckBoxIsVisible->IsChecked();
        cont->m_bPersVisible = cont->m_bIsVisible;
        cont->m_sCaption = m_pTextCtrlCaption->GetValue();
        cont->m_sOrientation =
            m_pChoiceOrientation->GetSelection() ==
            0 ? _T("V") : _T("H");
        DashboardWindowContainer *oldcont = new DashboardWindowContainer( cont );
        cont->m_aInstrumentList.Clear();
        cont->m_aInstrumentIDs.Clear();
        int id, j, oldmax;
        bool idMatch;
        for( int i = 0; i < m_pListCtrlInstruments->GetItemCount(); i++ ) {
            id = (int) m_pListCtrlInstruments->GetItemData( i );
            j = 0;
            oldmax = oldcont->m_aInstrumentIDs.GetCount();
            idMatch = false;
            while ( !idMatch && (j < oldmax) ) {
                if ( id == oldcont->m_aInstrumentList.Item( j ) )
                    idMatch = true;
                else
                    j++;
            } // while searching IDs from the old instrument container
            wxString ids = _T("");
            if ( idMatch ) {
                ids = oldcont->m_aInstrumentIDs.Item( j );
                oldcont->m_aInstrumentIDs.RemoveAt( j );
                oldcont->m_aInstrumentList.RemoveAt( j );
            }
            cont->m_aInstrumentList.Add( id );
            cont->m_aInstrumentIDs.Add( ids );
        } // for number of selected instruments
    }
}

void DashboardPreferencesDialog::OnDashboardSelected( wxListEvent& event )
{
    // save changes
    SaveDashboardConfig();
    UpdateDashboardButtonsState();
}

void DashboardPreferencesDialog::UpdateDashboardButtonsState()
{
    long item = -1;
    item = m_pListCtrlDashboards->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    bool enable = ( item != -1 );

    //  Disable the Dashboard Delete button if the parent(Dashboard) of this dialog is selected.
    bool delete_enable = enable;
    if( item != -1 ) {
        /*
          In this implemenation the dialog parent is the plugin, not any particular dashboard window
          so that we can destroy even the window from which we the dialog was started from.
          However, let's follow the principle to always leave at least one window to be consistent
          with the Dashboard-only code. It is also practical, and less confusing!
        */
        int NumberOfVisible = 0;
        int NumberOfItemsLeft = (int) m_pListCtrlDashboards->GetItemCount();
        if ( NumberOfItemsLeft <= 1)
            delete_enable = false;
        else {
            for ( int i = 0; i < NumberOfItemsLeft; i++ ) {
                if ( m_Config.Item( i )->m_bIsVisible )
                    NumberOfVisible++;
            } // For items available to delete
            if ( NumberOfVisible <= 1 )
                delete_enable = false;
        }
    }
    m_pButtonDeleteDashboard->Enable( delete_enable );

    m_pPanelDashboard->Enable( enable );

    if( item != -1 ) {
        curSel = m_pListCtrlDashboards->GetItemData( item );
        DashboardWindowContainer *cont = m_Config.Item( curSel );
        m_pCheckBoxIsVisible->SetValue( cont->m_bIsVisible );
        m_pTextCtrlCaption->SetValue( cont->m_sCaption );
        m_pChoiceOrientation->SetSelection( cont->m_sOrientation == _T("V") ? 0 : 1 );
        m_pListCtrlInstruments->DeleteAllItems();
        for( size_t i = 0; i < cont->m_aInstrumentList.GetCount(); i++ ) {
            wxListItem item;
            if (getListItemForInstrument(
                    item, cont->m_aInstrumentList.Item( i ) ) ) {
                item.SetId( m_pListCtrlInstruments->GetItemCount() );
                m_pListCtrlInstruments->InsertItem( item );
            }
        }

        m_pListCtrlInstruments->SetColumnWidth( 0, wxLIST_AUTOSIZE );
    } else {
        curSel = -1;
        m_pCheckBoxIsVisible->SetValue( false );
        m_pTextCtrlCaption->SetValue( _T("") );
        m_pChoiceOrientation->SetSelection( 0 );
        m_pListCtrlInstruments->DeleteAllItems();
    }
    //      UpdateButtonsState();
}

void DashboardPreferencesDialog::OnDashboardAdd( wxCommandEvent& event )
{
    int idx = m_pListCtrlDashboards->GetItemCount();
    m_pListCtrlDashboards->InsertItem( idx, 0 );
    // Data is index in m_Config
    m_pListCtrlDashboards->SetItemData( idx, m_Config.GetCount() );
    wxArrayInt ar;
    wxArrayString idar;
    DashboardWindowContainer *dwc = new DashboardWindowContainer( NULL, MakeName(),
                                                                  _("Dashboard_Tactics"),
                                                                  _T("V"), ar, idar );
    dwc->m_bIsVisible = true;
    dwc->m_bPersVisible = true;
    m_Config.Add( dwc );
}

void DashboardPreferencesDialog::OnDashboardDelete( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_pListCtrlDashboards->GetNextItem( itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    int idx = m_pListCtrlDashboards->GetItemData( itemID );
    m_pListCtrlDashboards->DeleteItem( itemID );
    m_Config.Item( idx )->m_bIsDeleted = true;
    UpdateDashboardButtonsState();
}

void DashboardPreferencesDialog::OnInstrumentSelected( wxListEvent& event )
{
    UpdateButtonsState();
}

void DashboardPreferencesDialog::UpdateButtonsState()
{
    long item = -1;
    item = m_pListCtrlInstruments->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    bool enable = ( item != -1 );

    m_pButtonDelete->Enable( enable );
    //    m_pButtonEdit->Enable( false ); // TODO: Properties
    m_pButtonUp->Enable( item > 0 );
    m_pButtonDown->Enable( item != -1 && item < m_pListCtrlInstruments->GetItemCount() - 1 );
}

void DashboardPreferencesDialog::OnInstrumentAdd( wxCommandEvent& event )
{
    AddInstrumentDlg pdlg( (wxWindow *) event.GetEventObject(), wxID_ANY );

    if( pdlg.ShowModal() == wxID_OK ) {
        wxListItem item;
        (void) getListItemForInstrument( item, pdlg.GetInstrumentAdded() );
        item.SetId( m_pListCtrlInstruments->GetItemCount() );
        m_pListCtrlInstruments->InsertItem( item );
        m_pListCtrlInstruments->SetColumnWidth( 0, wxLIST_AUTOSIZE );
        UpdateButtonsState();
    }
}

void DashboardPreferencesDialog::OnInstrumentDelete( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_pListCtrlInstruments->GetNextItem( itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    m_pListCtrlInstruments->DeleteItem( itemID );
    UpdateButtonsState();
}

void DashboardPreferencesDialog::OnInstrumentEdit( wxCommandEvent& event )
{
    // TODO: Instument options
}

void DashboardPreferencesDialog::OnInstrumentUp( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_pListCtrlInstruments->GetNextItem( itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    wxListItem item;
    item.SetId( itemID );
    item.SetMask( wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE | wxLIST_MASK_DATA );
    m_pListCtrlInstruments->GetItem( item );
    item.SetId( itemID - 1 );
    m_pListCtrlInstruments->DeleteItem( itemID );
    m_pListCtrlInstruments->InsertItem( item );
    m_pListCtrlInstruments->SetItemState( itemID - 1, wxLIST_STATE_SELECTED,
                                          wxLIST_STATE_SELECTED );
    UpdateButtonsState();
}

void DashboardPreferencesDialog::OnInstrumentDown( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_pListCtrlInstruments->GetNextItem( itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

    wxListItem item;
    item.SetId( itemID );
    item.SetMask( wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE | wxLIST_MASK_DATA );
    m_pListCtrlInstruments->GetItem( item );
    item.SetId( itemID + 1 );
    m_pListCtrlInstruments->DeleteItem( itemID );
    m_pListCtrlInstruments->InsertItem( item );
    m_pListCtrlInstruments->SetItemState( itemID + 1, wxLIST_STATE_SELECTED,
                                          wxLIST_STATE_SELECTED );
    UpdateButtonsState();
}
