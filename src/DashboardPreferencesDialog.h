/******************************************************************************
 * $Id: DashboardPreferencesDialog.h, v1.0 2010/08/05 SethDart Exp $
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

#ifndef _DASHBOARDPREFERENCESDIALOG_H_
#define _DASHBOARDPREFERENCESDIALOG_H_

#include "TacticsPreferencesDialog.h"
  
class DashboardPreferencesDialog : public
    TacticsPreferencesDialog
{
public:
    DashboardPreferencesDialog( wxWindow *pparent, wxWindowID id, wxArrayOfDashboard config
                                , wxString commonName, wxString nameVersion, wxPoint pos = wxDefaultPosition
        );
    ~DashboardPreferencesDialog() {}

    void OnCloseDialog(wxCloseEvent& event);
    void OnDashboardSelected(wxListEvent& event);
    void OnDashboardAdd(wxCommandEvent& event);
    void OnDashboardDelete(wxCommandEvent& event);
    void OnInstrumentSelected(wxListEvent& event);
    void OnInstrumentAdd(wxCommandEvent& event);
    void OnInstrumentEdit(wxCommandEvent& event);
    void OnInstrumentDelete(wxCommandEvent& event);
    void OnInstrumentUp(wxCommandEvent& event);
    void OnInstrumentDown(wxCommandEvent& event);
    void SaveDashboardConfig();

    wxArrayOfDashboard            m_Config;
    wxNotebook                   *m_itemNotebook;
    wxFontPickerCtrl             *m_pFontPickerTitle;
    wxFontPickerCtrl             *m_pFontPickerData;
    wxFontPickerCtrl             *m_pFontPickerLabel;
    wxFontPickerCtrl             *m_pFontPickerSmall;
    wxSpinCtrl                   *m_pSpinSpeedMax;
    wxSpinCtrl                   *m_pSpinCOGDamp;
    wxSpinCtrl                   *m_pSpinSOGDamp;
    wxChoice                     *m_pChoiceUTCOffset;
    wxChoice                     *m_pChoiceSpeedUnit;
    wxChoice                     *m_pChoiceDepthUnit;
    wxSpinCtrlDouble             *m_pSpinDBTOffset;
    wxChoice                     *m_pChoiceDistanceUnit;
    wxChoice                     *m_pChoiceWindSpeedUnit;
    wxChoice                     *m_pChoiceTemperatureUnit;

private:
    void UpdateDashboardButtonsState(void);
    void UpdateButtonsState(void);
    int                           curSel;
    wxListCtrl                   *m_pListCtrlDashboards;
    wxBitmapButton               *m_pButtonAddDashboard;
    wxBitmapButton               *m_pButtonDeleteDashboard;
    wxPanel                      *m_pPanelDashboard;
    wxTextCtrl                   *m_pTextCtrlCaption;
    wxCheckBox                   *m_pCheckBoxIsVisible;
    wxChoice                     *m_pChoiceOrientation;
    wxListCtrl                   *m_pListCtrlInstruments;
    wxButton                     *m_pButtonAdd;
    wxButton                     *m_pButtonEdit;
    wxButton                     *m_pButtonDelete;
    wxButton                     *m_pButtonUp;
    wxButton                     *m_pButtonDown;
};

#endif
