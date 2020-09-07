/***************************************************************************
* $Id: TacticsPreferencesDialog.cpp, v1.0 2016/06/07 tomBigSpeedy Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Thomas Rauch
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

#include "TacticsPreferencesDialog.h"

#include "dashboard_pi_ext.h"
#include "tactics_pi_ext.h"

//----------------------------------------------------------------
//    Tactics Preference Dialogs Implementation
//    porting note: as virtual parent for child dialog which will
//                  create and deal with buttons outside the tabs.
//    This panel, normally is not designed to exist alone but
//    being part of the DashboardPreferencesDialog, which invokes
//    in its constructor this panel's TacticsPrefencesPanel()
//    method when it is appropriate to create the panel.
//    For this reason the panel widgets are not initilaized in the
//    constructor.
//----------------------------------------------------------------

// cppcheck-suppress uninitMemberVar
// cppcheck-suppress uninitMemberVarPrivate
TacticsPreferencesDialog::TacticsPreferencesDialog(
    wxWindow *parent, wxWindowID id, const wxString derivtitle, wxPoint pos ) :
	wxDialog(
        parent, id, derivtitle, pos, wxDefaultSize ,
        wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxRESIZE_BORDER)
{
    m_itemNotebook = NULL;
    return;
}

void TacticsPreferencesDialog::TacticsPreferencesInit( wxNotebook *itemNotebook, int border_size )
{
    m_itemNotebook = itemNotebook;
    m_border_size = border_size;
}

void TacticsPreferencesDialog::TacticsPreferencesPanel()
{
    if (m_itemNotebook == NULL)
        return;

    wxScrolledWindow *itemPanelNotebook03 = new wxScrolledWindow(
        m_itemNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxVSCROLL);

    int scrollRate = 5;
#ifdef __OCPN__ANDROID__
    scrollRate = 1;
#endif
    itemPanelNotebook03->SetScrollRate(0, scrollRate);
    //itemNotebook->Layout();

    wxBoxSizer* itemBoxSizer06 = new wxBoxSizer(wxVERTICAL);
    itemPanelNotebook03->SetSizer(itemBoxSizer06);
    m_itemNotebook->AddPage(itemPanelNotebook03, _(L"\u2191Tactics Performance Parameters"));
    //****************************************************************************************************
    wxStaticBox* itemStaticBox05 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Laylines"));
    wxStaticBoxSizer* itemStaticBoxSizer05 = new wxStaticBoxSizer(itemStaticBox05, wxHORIZONTAL);
    itemBoxSizer06->Add(itemStaticBoxSizer05, 0, wxEXPAND | wxALL, m_border_size);

    wxFlexGridSizer *itemFlexGridSizer05 = new wxFlexGridSizer(2);
    itemFlexGridSizer05->AddGrowableCol(1);
    itemStaticBoxSizer05->Add(itemFlexGridSizer05, 1, wxEXPAND | wxALL, 0);
    wxString s;
    //---Layline damping factor -----------------
    wxStaticText* itemStaticText18 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Layline damping factor [0.025-1]:  "),
                                                      wxDefaultPosition, wxDefaultSize, 0);
    itemStaticText18->SetToolTip(_("The layline damping factor determines how fast the  laylines react on your course changes, i.e. your COG changes.\n Low values mean high damping."));
    itemFlexGridSizer05->Add(itemStaticText18, 0, wxEXPAND | wxALL, m_border_size);
    m_alphaLaylineDampFactor = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0.025, 1, g_dalphaLaylinedDampFactor, 0.001);
    itemFlexGridSizer05->Add(m_alphaLaylineDampFactor, 0, wxALIGN_LEFT, 0);
    m_alphaLaylineDampFactor->SetValue(g_dalphaLaylinedDampFactor);
    m_alphaLaylineDampFactor->SetToolTip(_("The layline damping factor determines how fast the  laylines react on your course changes, i.e. your COG changes.\n Low values mean high damping."));

    //--------------------
    wxStaticText* itemStaticText20 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Layline width damping factor [0.025-1]:  "),
                                                      wxDefaultPosition, wxDefaultSize, 0);
    itemStaticText20->SetToolTip(_("The width of the boat laylines is based on the yawing of the boat (vertical axis), i.e. your COG changes.\nThe idea is to display the COG range where you're sailing to."));
    itemFlexGridSizer05->Add(itemStaticText20, 0, wxEXPAND | wxALL, m_border_size);
    m_alphaDeltCoG = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0.025, 1, g_dalphaDeltCoG, 0.001);
    itemFlexGridSizer05->Add(m_alphaDeltCoG, 0, wxALIGN_LEFT, 0);
    m_alphaDeltCoG->SetValue(g_dalphaDeltCoG);
    m_alphaDeltCoG->SetToolTip(_("The width of the boat laylines is based on the yawing of the boat (vertical axis), i.e. your COG changes.\nThe idea is to display the range where you're sailing to."));

    //--------------------
    wxStaticText* itemStaticText19 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Layline length on Chart [nm]:  "),
                                                      wxDefaultPosition, wxDefaultSize, 0);
    itemStaticText19->SetToolTip(_("Length of the boat laylines in [nm]"));

    itemFlexGridSizer05->Add(itemStaticText19, 0, wxEXPAND | wxALL, m_border_size);
    m_pLaylineLength = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0.0, 20.0, g_dLaylineLengthonChart, 0.1);
    itemFlexGridSizer05->Add(m_pLaylineLength, 0, wxALIGN_LEFT | wxALL, 0);
    m_pLaylineLength->SetValue(g_dLaylineLengthonChart);
    m_pLaylineLength->SetToolTip(_("Length of the boat laylines in [nm]"));
    //--------------------
    wxStaticText* itemStaticText21 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Min. Layline Width [\u00B0]:"),
                                                      wxDefaultPosition, wxDefaultSize, 0);
    itemStaticText21->SetToolTip(_("Min. width of boat laylines in degrees."));
    itemFlexGridSizer05->Add(itemStaticText21, 0, wxEXPAND | wxALL, m_border_size);
    m_minLayLineWidth = new wxSpinCtrl(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 20, g_iMinLaylineWidth);
    m_minLayLineWidth->SetToolTip(_("Min. width of boat laylines in degrees."));
    itemFlexGridSizer05->Add(m_minLayLineWidth, 0, wxALIGN_LEFT | wxALL, 0);

    //--------------------
    wxStaticText* itemStaticText22 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Max. Layline Width [\u00B0]:"),
                                                      wxDefaultPosition, wxDefaultSize, 0);
    itemStaticText22->SetToolTip(_("Max. width of boat laylines in degrees."));
    itemFlexGridSizer05->Add(itemStaticText22, 0, wxEXPAND | wxALL, m_border_size);
    m_maxLayLineWidth = new wxSpinCtrl(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 30, g_iMaxLaylineWidth);
    m_maxLayLineWidth->SetToolTip(_("Max. width of boat laylines in degrees."));
    itemFlexGridSizer05->Add(m_maxLayLineWidth, 0, wxALIGN_LEFT | wxALL, 0);
    //****************************************************************************************************
    wxStaticBox* itemStaticBox06 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Leeway"));
    wxStaticBoxSizer* itemStaticBoxSizer06 = new wxStaticBoxSizer(itemStaticBox06, wxHORIZONTAL);
    itemBoxSizer06->Add(itemStaticBoxSizer06, 0, wxEXPAND | wxALL, m_border_size);
    wxFlexGridSizer *itemFlexGridSizer06 = new wxFlexGridSizer(2);
    itemFlexGridSizer06->AddGrowableCol(1);
    itemStaticBoxSizer06->Add(itemFlexGridSizer06, 1, wxEXPAND | wxALL, 0);


    //--------------------
    wxStaticText* itemStaticText23a = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Boat's Leeway factor [0-20]:  "), wxDefaultPosition, wxDefaultSize, 0);
    itemStaticText23a->SetToolTip(_("Leeway='Drift' of boat due to heel/wind influence\nLow values mean high performance of hull\nLeeway = (LeewayFactor * Heel) / STW\u00B2;")); //�
    itemFlexGridSizer06->Add(itemStaticText23a, 0, wxEXPAND | wxALL, m_border_size);
    m_LeewayFactor = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 20, g_dLeewayFactor, 0.01);
    m_LeewayFactor->SetToolTip(_("Leeway='Drift' of boat due to heel/wind influence\nLow values mean high performance of hull\nLeeway = (LeewayFactor * Heel) / STW\u00B2;"));

    itemFlexGridSizer06->Add(m_LeewayFactor, 0, wxALIGN_LEFT | wxALL, 0);
    m_LeewayFactor->SetValue(g_dLeewayFactor);
    //--------------------
    m_ButtonUseHeelSensor = new wxRadioButton(itemPanelNotebook03, wxID_ANY, _("Use Heel Sensor"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    itemFlexGridSizer06->Add(m_ButtonUseHeelSensor, 0, wxALL, 5);
    m_ButtonUseHeelSensor->SetValue(g_bUseHeelSensor);
    m_ButtonUseHeelSensor->SetToolTip(_("Use the internal heel sensor if available\nImportant for the correct calculation of the surface current."));
    wxStaticText* itemStaticText23b = new wxStaticText(itemPanelNotebook03, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer06->Add(itemStaticText23b, 0, wxEXPAND | wxALL, m_border_size);
    //--------------------
    m_ButtonFixedLeeway = new wxRadioButton(itemPanelNotebook03, wxID_ANY, _("fixed/max Leeway [\u00B0]:"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer06->Add(m_ButtonFixedLeeway, 0, wxALL, 5);
    m_ButtonFixedLeeway->SetValue(g_bUseFixedLeeway);
    m_ButtonFixedLeeway->SetToolTip(_("Dual purpose !\nIf Radiobutton is NOT set, then it's used to limit Leeway to a max value.\n If Radiobutton is set, then it fixes Leeway to this constant value."));

    m_ButtonFixedLeeway->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(TacticsPreferencesDialog::OnManualHeelUpdate), NULL, this);

    m_fixedLeeway = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 30, g_dfixedLeeway, 0.01);
    itemFlexGridSizer06->Add(m_fixedLeeway, 0, wxALIGN_LEFT, 0);
    m_fixedLeeway->SetValue(g_dfixedLeeway);
    //--------------------
    m_ButtonHeelInput = new wxRadioButton(itemPanelNotebook03, wxID_ANY, _("manual Heel input:"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer06->Add(m_ButtonHeelInput, 0, wxALL, 5);
    m_ButtonHeelInput->SetValue(g_bManHeelInput);
    m_ButtonHeelInput->SetToolTip(_("If no heel sensor is available, you can create a manual 'heel polar' here.\nJust read/enter the data from a mechanical heel sensor (e.g. on compass).\nUse True Wind Speed & Angle only !\nTake care: motoring w/o sails&heel will show wrong current data !!!"));

    m_ButtonHeelInput->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(TacticsPreferencesDialog::OnManualHeelUpdate), NULL, this);

    wxStaticText* itemStaticText23c = new wxStaticText(itemPanelNotebook03, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer06->Add(itemStaticText23c, 0, wxEXPAND | wxALL, m_border_size);
    //****************************************************************************************************
    wxStaticBox* itemStaticBox07 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Heel"));
    wxStaticBoxSizer* itemStaticBoxSizer07 = new wxStaticBoxSizer(itemStaticBox07, wxHORIZONTAL);
    itemBoxSizer06->Add(itemStaticBoxSizer07, 0, wxEXPAND | wxALL, m_border_size);
    wxFlexGridSizer *itemFlexGridSizer07 = new wxFlexGridSizer(4);
    itemStaticBoxSizer07->Add(itemFlexGridSizer07, 1, wxEXPAND | wxALL, 0);

    //--------------------
    wxStaticText* itemStaticText23T0 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("TWS/TWA "), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer07->Add(itemStaticText23T0, 0, wxEXPAND | wxALL, m_border_size);
    wxStaticText* itemStaticText23T1 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _(" 45\u00B0"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer07->Add(itemStaticText23T1, 0, wxALIGN_CENTER | wxALL, m_border_size);
    wxStaticText* itemStaticText23T2 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _(" 90\u00B0"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer07->Add(itemStaticText23T2, 0, wxALIGN_CENTER | wxALL, m_border_size);
    wxStaticText* itemStaticText23T3 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("135\u00B0"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer07->Add(itemStaticText23T3, 0, wxALIGN_CENTER | wxALL, m_border_size);
    //--------------------
    wxStaticText* itemStaticText23ws5 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("5 kn"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer07->Add(itemStaticText23ws5, 0, wxEXPAND | wxALL, m_border_size);
    m_heel5_45 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[1][1], 0.1);
    itemFlexGridSizer07->Add(m_heel5_45, 0, wxALIGN_LEFT, 0);
    m_heel5_45->SetValue(g_dheel[1][1]);
    //--------------------
    m_heel5_90 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[1][2], 0.1);
    itemFlexGridSizer07->Add(m_heel5_90, 0, wxALIGN_LEFT, 0);
    m_heel5_90->SetValue(g_dheel[1][2]);
    //--------------------
    m_heel5_135 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[1][3], 0.1);
    itemFlexGridSizer07->Add(m_heel5_135, 0, wxALIGN_LEFT, 0);
    m_heel5_135->SetValue(g_dheel[1][3]);
    //--------------------
    wxStaticText* itemStaticText23ws10 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("10 kn"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer07->Add(itemStaticText23ws10, 0, wxEXPAND | wxALL, m_border_size);
    m_heel10_45 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[2][1], 0.1);
    itemFlexGridSizer07->Add(m_heel10_45, 0, wxALIGN_LEFT, 0);
    m_heel10_45->SetValue(g_dheel[2][1]);
    //--------------------
    m_heel10_90 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[2][2], 0.1);
    itemFlexGridSizer07->Add(m_heel10_90, 0, wxALIGN_LEFT, 0);
    m_heel10_90->SetValue(g_dheel[2][2]);
    //--------------------
    m_heel10_135 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[2][3], 0.1);
    itemFlexGridSizer07->Add(m_heel10_135, 0, wxALIGN_LEFT, 0);
    m_heel10_135->SetValue(g_dheel[2][3]);
    //--------------------
    wxStaticText* itemStaticText23ws15 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("15 kn"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer07->Add(itemStaticText23ws15, 0, wxEXPAND | wxALL, m_border_size);

    m_heel15_45 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[3][1], 0.1);
    itemFlexGridSizer07->Add(m_heel15_45, 0, wxALIGN_LEFT, 0);
    m_heel15_45->SetValue(g_dheel[3][1]);
    //--------------------
    m_heel15_90 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[3][2], 0.1);
    itemFlexGridSizer07->Add(m_heel15_90, 0, wxALIGN_LEFT, 0);
    m_heel15_90->SetValue(g_dheel[3][2]);
    //--------------------
    m_heel15_135 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[3][3], 0.1);
    itemFlexGridSizer07->Add(m_heel15_135, 0, wxALIGN_LEFT, 0);
    m_heel15_135->SetValue(g_dheel[3][3]);
    //--------------------
    wxStaticText* itemStaticText23ws20 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("20 kn"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer07->Add(itemStaticText23ws20, 0, wxEXPAND | wxALL, m_border_size);
    m_heel20_45 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[4][1], 0.1);
    itemFlexGridSizer07->Add(m_heel20_45, 0, wxALIGN_LEFT, 0);
    m_heel20_45->SetValue(g_dheel[4][1]);
    //--------------------
    m_heel20_90 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[4][2], 0.1);
    itemFlexGridSizer07->Add(m_heel20_90, 0, wxALIGN_LEFT, 0);
    m_heel20_90->SetValue(g_dheel[4][2]);
    //--------------------
    m_heel20_135 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[4][3], 0.1);
    itemFlexGridSizer07->Add(m_heel20_135, 0, wxALIGN_LEFT, 0);
    m_heel20_135->SetValue(g_dheel[4][3]);
    //--------------------
    wxStaticText* itemStaticText23ws25 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("25 kn"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer07->Add(itemStaticText23ws25, 0, wxEXPAND | wxALL, m_border_size);
    m_heel25_45 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[5][1], 0.1);
    itemFlexGridSizer07->Add(m_heel25_45, 0, wxALIGN_LEFT, 0);
    m_heel25_45->SetValue(g_dheel[5][1]);
    //--------------------
    m_heel25_90 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[5][2], 0.1);
    itemFlexGridSizer07->Add(m_heel25_90, 0, wxALIGN_LEFT, 0);
    m_heel25_90->SetValue(g_dheel[5][2]);
    //--------------------
    m_heel25_135 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[5][3], 0.1);
    itemFlexGridSizer07->Add(m_heel25_135, 0, wxALIGN_LEFT, 0);
    m_heel25_135->SetValue(g_dheel[5][3]);

    //****************************************************************************************************
    wxStaticBox* itemStaticBox08 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Current"));
    wxStaticBoxSizer* itemStaticBoxSizer08 = new wxStaticBoxSizer(itemStaticBox08, wxHORIZONTAL);
    itemBoxSizer06->Add(itemStaticBoxSizer08, 0, wxEXPAND | wxALL, m_border_size);
    wxFlexGridSizer *itemFlexGridSizer08 = new wxFlexGridSizer(2);
    itemFlexGridSizer08->AddGrowableCol(1);
    itemStaticBoxSizer08->Add(itemFlexGridSizer08, 1, wxEXPAND | wxALL, 0);

    //--------------------
    //
    wxStaticText* itemStaticText24 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Current damping factor [1-400]:  "), wxDefaultPosition, wxDefaultSize, 0);
    itemStaticText24->SetToolTip(_("Stabilizes the surface current 'arrow' in the chart overlay, bearing compass and also the numerical instruments\nLow values mean high damping"));
    itemFlexGridSizer08->Add(itemStaticText24, 0, wxEXPAND | wxALL, m_border_size);
    m_AlphaCurrDir = new wxSpinCtrl(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS | wxSP_WRAP, 1, 400, g_dalpha_currdir * 1000);
    itemFlexGridSizer08->Add(m_AlphaCurrDir, 0, wxALIGN_LEFT, 0);
    m_AlphaCurrDir->SetValue(g_dalpha_currdir * 1000);
    m_AlphaCurrDir->SetToolTip(_("Stabilizes the surface current 'arrow' in the chart overlay, bearing compass and also the numerical instruments\nLow values mean high damping"));
    //--------------------
    m_CurrentOnChart = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Display Current on Chart (OpenGL)"));
    itemFlexGridSizer08->Add(m_CurrentOnChart, 0, wxEXPAND, 5);
    m_CurrentOnChart->SetValue(g_bDisplayCurrentOnChart);
    m_CurrentOnChart->SetToolTip(_("The default on program startup"));
    //****************************************************************************************************
    wxStaticBox* itemStaticBox10 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("True Wind"));
    wxStaticBoxSizer* itemStaticBoxSizer10 = new wxStaticBoxSizer(itemStaticBox10, wxHORIZONTAL);
    itemBoxSizer06->Add(itemStaticBoxSizer10, 0, wxEXPAND | wxALL, m_border_size);
    wxFlexGridSizer *itemFlexGridSizer10 = new wxFlexGridSizer(2);
    itemFlexGridSizer10->AddGrowableCol(1);
    itemStaticBoxSizer10->Add(itemFlexGridSizer10, 1, wxEXPAND | wxALL, 0);

    //--------------------
    m_CorrectSTWwithLeeway = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Correct STW with Leeway"));
    itemFlexGridSizer10->Add(m_CorrectSTWwithLeeway, 0, wxEXPAND, 5);
    m_CorrectSTWwithLeeway->SetValue(g_bCorrectSTWwithLeeway);
    m_CorrectSTWwithLeeway->SetToolTip(_("Apply a correction to your log speed throughout the plugin based on the calculated Leeway and Current.\nOnly makes sense with a real heel sensor.\nMake sure your instruments do not already apply this correction !"));
    //--------------------
    m_CorrectAWwithHeel = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Correct AWS/AWA with Heel"));
    itemFlexGridSizer10->Add(m_CorrectAWwithHeel, 0, wxEXPAND, 5);
    m_CorrectAWwithHeel->SetValue(g_bCorrectAWwithHeel);
    m_CorrectAWwithHeel->SetToolTip(_("Use with care, this is normally done by the instruments themselves as soon as you have an integrated, original equipment heel sensor"));

    m_CorrectAWwithHeel->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(TacticsPreferencesDialog::OnAWSAWACorrectionUpdated), NULL, this);
    //--------------------
    m_ForceTrueWindCalculation = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Force True Wind Calculation"));
    itemFlexGridSizer10->Add(m_ForceTrueWindCalculation, 0, wxEXPAND, 5);
    m_ForceTrueWindCalculation->SetValue(g_bForceTrueWindCalculation);
    m_ForceTrueWindCalculation->SetToolTip(_("Internally calculates True Wind data (TWS,TWA,TWD) and uses it within the whole plugin even if there is True Wind data available via NMEA or Signal K."));

    //--------------------
    m_UseSOGforTWCalc = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Use SOG instead of STW for True Wind Calc."));
    itemFlexGridSizer10->Add(m_UseSOGforTWCalc, 0, wxEXPAND, 5);
    m_UseSOGforTWCalc->SetValue(g_bUseSOGforTWCalc);
    m_UseSOGforTWCalc->SetToolTip(_("Recommended. As True Wind blows over the earth surface, we should calc. it with Speed Over Ground.\nEliminates the influence of currents."));

    m_ShowWindbarbOnChart = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Show Wind Barb on Chart (OpenGL)"));
    itemFlexGridSizer10->Add(m_ShowWindbarbOnChart, 0, wxEXPAND, 5);
    m_ShowWindbarbOnChart->SetValue(g_bShowWindbarbOnChart);
    m_ShowWindbarbOnChart->SetToolTip(_("The default on program startup"));

    //****************************************************************************************************
    wxStaticBox* itemStaticBox09 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _(L"Polar - NOTE: \u2191Tactics instruments need your boat's polars!"));
    wxStaticBoxSizer* itemStaticBoxSizer09 = new wxStaticBoxSizer(itemStaticBox09, wxHORIZONTAL);
    itemBoxSizer06->Add(itemStaticBoxSizer09, 0, wxEXPAND | wxALL, m_border_size);
    wxFlexGridSizer *itemFlexGridSizer09 = new wxFlexGridSizer(2);
    itemFlexGridSizer09->AddGrowableCol(1);
    itemStaticBoxSizer09->Add(itemFlexGridSizer09, 1, wxEXPAND | wxALL, 0);

    wxStaticText* itemStaticText30 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Polar file:"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer09->Add(itemStaticText30, 0, wxEXPAND | wxALL, m_border_size);

    m_pTextCtrlPolar = new wxTextCtrl(itemPanelNotebook03, wxID_ANY, g_path_to_PolarFile, wxDefaultPosition, wxDefaultSize);
    itemFlexGridSizer09->Add(m_pTextCtrlPolar, 0, wxALIGN_LEFT | wxEXPAND | wxALL, m_border_size);

    m_buttonLoadPolar = new wxButton(itemPanelNotebook03, wxID_ANY, _("Load"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer09->Add(m_buttonLoadPolar, 0, wxALIGN_LEFT | wxALL, 5);
    m_buttonLoadPolar->Connect(
        wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TacticsPreferencesDialog::SelectPolarFile), NULL, this);

    m_ShowPolarOnChart = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Show Polar on Chart (OpenGL)"));
    itemFlexGridSizer09->Add(m_ShowPolarOnChart, 0, wxEXPAND, 5);
    m_ShowPolarOnChart->SetValue(g_bShowPolarOnChart);
    m_ShowPolarOnChart->SetToolTip(_("The default on program startup"));
    //****************************************************************************************************
    wxStaticBox* itemStaticBoxExpData = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Export NMEA Performance Data"));
    wxStaticBoxSizer* itemStaticBoxSizerExpData = new wxStaticBoxSizer(itemStaticBoxExpData, wxHORIZONTAL);
    itemBoxSizer06->Add(itemStaticBoxSizerExpData, 0, wxEXPAND | wxALL, m_border_size);
    wxFlexGridSizer *itemFlexGridSizerExpData = new wxFlexGridSizer(2);
    itemFlexGridSizerExpData->AddGrowableCol(1);
    itemStaticBoxSizerExpData->Add(itemFlexGridSizerExpData, 1, wxEXPAND | wxALL, 0);
    //-------------------- Radiobutton(s) for different instrument systems -----------
    m_ButtonExpNKE = new wxRadioButton(itemPanelNotebook03, wxID_ANY, _("NKE format ($PNKEP)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    itemFlexGridSizerExpData->Add(m_ButtonExpNKE, 0, wxALL, 5);
    m_ButtonExpNKE->SetValue(true); // fixed value for now
    m_ButtonExpNKE->SetToolTip(_("Currently only set up for NKE instruments. Exports a predefined set of up to 5 NMEA records which are 'known' by NKE instruments and can be displayed there.\nRead the manual how to set up the interface connection !"));

    wxStaticText* itemStaticTextDummy = new wxStaticText(itemPanelNotebook03, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizerExpData->Add(itemStaticTextDummy, 0, wxEXPAND | wxALL, m_border_size);
    //--------------------
    //--------------------
    m_ExpPerfData01 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Target Polar Speed"));
    itemFlexGridSizerExpData->Add(m_ExpPerfData01, 0, wxEXPAND, 5);
    m_ExpPerfData01->SetValue(g_bExpPerfData01);
    //--------------------
    m_ExpPerfData02 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("CoG on other Tack"));
    itemFlexGridSizerExpData->Add(m_ExpPerfData02, 0, wxEXPAND, 5);
    m_ExpPerfData02->SetValue(g_bExpPerfData02);
    //--------------------
    m_ExpPerfData03 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Target-") + g_sVMGSynonym + _(" angle + Perf. %"));
    itemFlexGridSizerExpData->Add(m_ExpPerfData03, 0, wxEXPAND, 5);
    m_ExpPerfData03->SetValue(g_bExpPerfData03);
    //--------------------
    m_ExpPerfData04 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Diff. angle to Target-") + g_sVMGSynonym + _T("/-") + g_sCMGSynonym + _(" + corresp.gain"));
    itemFlexGridSizerExpData->Add(m_ExpPerfData04, 0, wxEXPAND, 5);
    m_ExpPerfData04->SetValue(g_bExpPerfData04);
    //--------------------
    m_ExpPerfData05 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Current Direction + Speed"));
    itemFlexGridSizerExpData->Add(m_ExpPerfData05, 0, wxEXPAND, 5);
    m_ExpPerfData05->SetValue(g_bExpPerfData05);
    //--------------------
    //****************************************************************************************************
    //****************************************************************************************************
    wxStaticBox* itemStaticBoxFileExp = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Export Data to File"));
    wxStaticBoxSizer* itemStaticBoxSizerFileExp = new wxStaticBoxSizer(itemStaticBoxFileExp, wxHORIZONTAL);
    itemBoxSizer06->Add(itemStaticBoxSizerFileExp, 0, wxEXPAND | wxALL, m_border_size);
    wxFlexGridSizer *itemFlexGridSizerFileExp = new wxFlexGridSizer(2);
    itemFlexGridSizerFileExp->AddGrowableCol(1);
    itemStaticBoxSizerFileExp->Add(itemFlexGridSizerFileExp, 1, wxEXPAND | wxALL, 0);
    //    wxStaticText* itemStaticTextDummy2 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0);
    //    itemFlexGridSizerFileExp->Add(itemStaticTextDummy2, 0, wxEXPAND | wxALL, m_border_size);
    //--------------------
    //--------------------
    m_ExpFileData01 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Prepend Clockticks"));
    itemFlexGridSizerFileExp->Add(m_ExpFileData01, 0, wxEXPAND, 5);
    m_ExpFileData01->SetValue(g_bDataExportClockticks);
    m_ExpFileData01->SetToolTip(_("Adds Clockticks to the data exports of BaroHistory, PolarPerformance and WindHistory"));
    //--------------------
    //--------------------
    m_ExpFileData02 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Prepend UTC Timestamp"));
    itemFlexGridSizerFileExp->Add(m_ExpFileData02, 0, wxEXPAND, 5);
    m_ExpFileData02->SetValue(g_bDataExportUTC);
    m_ExpFileData02->SetToolTip(_("Adds ISO8601 UTC-Date&Time to the data exports of BaroHistory, PolarPerformance and WindHistory"));

    wxStaticText* itemStaticText31 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Data Separator :"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizerFileExp->Add(itemStaticText31, 0, wxEXPAND | wxALL, m_border_size);
    m_pDataExportSeparator = new wxTextCtrl(itemPanelNotebook03, wxID_ANY, g_sDataExportSeparator, wxDefaultPosition, wxSize(30, -1), wxTE_LEFT);
    itemFlexGridSizerFileExp->Add(m_pDataExportSeparator, 0, wxALL, m_border_size);
    m_pDataExportSeparator->SetToolTip(_("Sets the separator for the data exports of BaroHistory, PolarPerformance and WindHistory;"));

    //****************************************************************************************************
    m_PersistentChartPolarAnimation = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Persistent Chart Perf. Animations"));
    itemFlexGridSizerExpData->Add(m_PersistentChartPolarAnimation, 0, wxEXPAND, 5);
    m_PersistentChartPolarAnimation->SetValue(g_bPersistentChartPolarAnimation);

    return;
}

void TacticsPreferencesDialog::OnManualHeelUpdate(wxCommandEvent& event)
{
	if (m_ButtonFixedLeeway->GetValue() || m_ButtonHeelInput->GetValue()){
		if (m_CorrectAWwithHeel->IsChecked()){
			wxMessageBox(_("This will also disable the AWA/AWS correction."));
			m_CorrectAWwithHeel->SetValue(false);
		}
	}
}

void TacticsPreferencesDialog::OnAWSAWACorrectionUpdated(wxCommandEvent& event)
{ // check if heel is available
	//...
	if (!m_ButtonUseHeelSensor->GetValue()){
		wxMessageBox(_("This option makes only sense with a real heel sensor."));
		m_CorrectAWwithHeel->SetValue(false);
	}
	else{
		//display warning
		if (m_CorrectAWwithHeel->IsChecked()){
			wxMessageBox(_("Make sure your instruments do not internally correct AWS / AWA with heel.\nThis may result in wrong wind data."));
		}
	}
}

void TacticsPreferencesDialog::SelectPolarFile(wxCommandEvent& event)
{
	wxFileDialog fdlg(
        GetOCPNCanvasWindow(), _("Select a Polar-File"), _T(""));
	if (fdlg.ShowModal() == wxID_CANCEL)
        return;
	g_path_to_PolarFile = fdlg.GetPath();
	BoatPolar->loadPolar(g_path_to_PolarFile);
	if (m_pTextCtrlPolar)
        m_pTextCtrlPolar->SetValue(g_path_to_PolarFile);
}

void TacticsPreferencesDialog::SaveTacticsConfig()
{
    g_dLeewayFactor = m_LeewayFactor->GetValue();
    g_dfixedLeeway = m_fixedLeeway->GetValue();

    g_dalpha_currdir = (double)m_AlphaCurrDir->GetValue() / 1000.0;
    //    g_dalpha_currdir = m_AlphaCurrDir->GetValue();
    g_dalphaDeltCoG = m_alphaDeltCoG->GetValue();
    g_dalphaLaylinedDampFactor = m_alphaLaylineDampFactor->GetValue();
    g_dLaylineLengthonChart = m_pLaylineLength->GetValue();
    g_iMinLaylineWidth = m_minLayLineWidth->GetValue();
    g_iMaxLaylineWidth = m_maxLayLineWidth->GetValue();
    g_bDisplayCurrentOnChart = m_CurrentOnChart->GetValue();
    g_dheel[1][1] = m_heel5_45->GetValue();
    g_dheel[1][2] = m_heel5_90->GetValue();
    g_dheel[1][3] = m_heel5_135->GetValue();
    g_dheel[2][1] = m_heel10_45->GetValue();
    g_dheel[2][2] = m_heel10_90->GetValue();
    g_dheel[2][3] = m_heel10_135->GetValue();
    g_dheel[3][1] = m_heel15_45->GetValue();
    g_dheel[3][2] = m_heel15_90->GetValue();
    g_dheel[3][3] = m_heel15_135->GetValue();
    g_dheel[4][1] = m_heel20_45->GetValue();
    g_dheel[4][2] = m_heel20_90->GetValue();
    g_dheel[4][3] = m_heel20_135->GetValue();
    g_dheel[5][1] = m_heel25_45->GetValue();
    g_dheel[5][2] = m_heel25_90->GetValue();
    g_dheel[5][3] = m_heel25_135->GetValue();

    g_bUseHeelSensor = m_ButtonUseHeelSensor->GetValue();
    g_bUseFixedLeeway = m_ButtonFixedLeeway->GetValue();
    g_bManHeelInput = m_ButtonHeelInput->GetValue();
    g_path_to_PolarFile = m_pTextCtrlPolar->GetValue();
    g_bCorrectSTWwithLeeway = m_CorrectSTWwithLeeway->GetValue();
    g_bCorrectAWwithHeel = m_CorrectAWwithHeel->GetValue();
    g_bForceTrueWindCalculation = m_ForceTrueWindCalculation->GetValue();
    g_bUseSOGforTWCalc = m_UseSOGforTWCalc->GetValue();
    g_bShowWindbarbOnChart = m_ShowWindbarbOnChart->GetValue();
    g_bShowPolarOnChart = m_ShowPolarOnChart->GetValue();
    g_bExpPerfData01 = m_ExpPerfData01->GetValue();
    g_bExpPerfData02 = m_ExpPerfData02->GetValue();
    g_bExpPerfData03 = m_ExpPerfData03->GetValue();
    g_bExpPerfData04 = m_ExpPerfData04->GetValue();
    g_bExpPerfData05 = m_ExpPerfData05->GetValue();
    g_bPersistentChartPolarAnimation =
        m_PersistentChartPolarAnimation->GetValue();
    g_bDataExportClockticks= m_ExpFileData01->GetValue();
    g_bDataExportUTC =  m_ExpFileData02->GetValue();
    g_sDataExportSeparator = m_pDataExportSeparator->GetValue();
}
