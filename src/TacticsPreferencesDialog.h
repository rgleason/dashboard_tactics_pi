/******************************************************************************
 * $Id: TacticsPreferencesDialog.h, v1.0 2016/06/27 tom_BigSpeedy Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Tactics Plugin
 * Author:   Thomas Rauch
 *   (Inspired by original work from Jean-Eudes Onfray)
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

#ifndef _TACTICSPREFERENCESDIALOG_H_
#define _TACTICSPREFERENCESDIALOG_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/fileconf.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/listctrl.h>

#include "icons.h"
#include "DashboardWindowContainer.h"

class TacticsPreferencesDialog : public wxDialog
{
public:
    TacticsPreferencesDialog(
        wxWindow *pparent, wxWindowID id, const wxString derivtitle, wxPoint pos = wxDefaultPosition );
    ~TacticsPreferencesDialog() {}

    virtual void TacticsPreferencesInit(
        wxNotebook *itemNotebook, int border_size) final;
    virtual void TacticsPreferencesPanel(void) final;
    virtual void SaveTacticsConfig(void) final;

    void SelectPolarFile(wxCommandEvent& event);
    void OnAWSAWACorrectionUpdated(wxCommandEvent& event);
    void OnManualHeelUpdate(wxCommandEvent& event);

    wxNotebook                   *m_itemNotebook;
    int                           m_border_size;

    wxSpinCtrlDouble             *m_alphaDeltCoG; //TR
    wxSpinCtrlDouble             *m_alphaLaylineDampFactor;//TR
    wxSpinCtrl                   *m_minLayLineWidth;//TR
    wxSpinCtrl                   *m_maxLayLineWidth;//TR
    wxSpinCtrlDouble             *m_LeewayFactor;//TR
    wxSpinCtrl                   *m_AlphaCurrDir; //TR
    wxSpinCtrlDouble             *m_fixedLeeway;//TR
    wxButton                     *m_buttonLoadPolar;//TR
    wxButton                     *m_buttonPrefsApply;//TR
    wxTextCtrl                   *m_pTextCtrlPolar; //TR
    wxSpinCtrlDouble             *m_pLaylineLength; //TR
    wxSpinCtrlDouble             *m_heel5_45;
    wxSpinCtrlDouble             *m_heel5_90;
    wxSpinCtrlDouble             *m_heel5_135;
    wxSpinCtrlDouble             *m_heel10_45;
    wxSpinCtrlDouble             *m_heel10_90;
    wxSpinCtrlDouble             *m_heel10_135;
    wxSpinCtrlDouble             *m_heel15_45;
    wxSpinCtrlDouble             *m_heel15_90;
    wxSpinCtrlDouble             *m_heel15_135;
    wxSpinCtrlDouble             *m_heel20_45;
    wxSpinCtrlDouble             *m_heel20_90;
    wxSpinCtrlDouble             *m_heel20_135;
    wxSpinCtrlDouble             *m_heel25_45;
    wxSpinCtrlDouble             *m_heel25_90;
    wxSpinCtrlDouble             *m_heel25_135;
    wxTextCtrl                   *m_UseHeelSensor;
    wxCheckBox                   *m_CurrentOnChart;
    wxRadioButton                *m_ButtonLeewayFactor;
    wxRadioButton                *m_ButtonFixedLeeway;
    wxRadioButton                *m_ButtonHeelInput;
    wxRadioButton                *m_ButtonUseHeelSensor;
    wxCheckBox                   *m_CorrectSTWwithLeeway;
    wxCheckBox                   *m_CorrectAWwithHeel;
    wxCheckBox                   *m_ForceTrueWindCalculation;
    wxCheckBox                   *m_UseSOGforTWCalc;
    wxCheckBox                   *m_ShowWindbarbOnChart;
    wxCheckBox                   *m_ShowPolarOnChart;
    wxRadioButton                *m_ButtonExpNKE;
    wxCheckBox                   *m_ExpPerfData01;
    wxCheckBox                   *m_ExpPerfData02;
    wxCheckBox                   *m_ExpPerfData03;
    wxCheckBox                   *m_ExpPerfData04;
    wxCheckBox                   *m_ExpPerfData05;
    wxCheckBox                   *m_ExpFileData01;
    wxCheckBox                   *m_ExpFileData02;
    wxTextCtrl                   *m_pDataExportSeparator;
    wxCheckBox                   *m_PersistentChartPolarAnimation;
private:
};

#endif
