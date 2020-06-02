/***************************************************************************
 * $Id: DashboardConfig.cpp, v1.0 2010/08/05 SethDart Exp $
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

using namespace std;

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "dashboard_pi.h"

#include "dashboard_pi_ext.h"

#include "DashboardFunctions.h"

#include "DashboardInstrumentContainer.h"

bool dashboard_pi::LoadConfig( void )
{
    wxFileConfig *pConf = static_cast <wxFileConfig *>(m_pconfig);

    if( pConf ) {
        pConf->SetPath( _T("/PlugIns/DashT") );
        LoadDashboardBasePart( pConf );
        LoadDashboardInstruments( pConf );
        pConf->SetPath( _T("/PlugIns/DashT/Dials") );
        LoadDialSettings( pConf );
        pConf->SetPath( _T("/PlugIns/DashT") );
        return true;
    }
    else
        return false;
}

void dashboard_pi::LoadDashboardBasePart( wxFileConfig *pConf ) {

    wxString version;
    pConf->Read( _T("Version"), &version, wxEmptyString );
    wxString config;
    pConf->Read( _T("FontTitle"), &config, wxEmptyString );
    if( !config.IsEmpty() ) g_pFontTitle->SetNativeFontInfo( config );
    pConf->Read( _T("FontData"), &config, wxEmptyString );
    if( !config.IsEmpty() ) g_pFontData->SetNativeFontInfo( config );
    pConf->Read( _T("FontLabel"), &config, wxEmptyString );
    if( !config.IsEmpty() ) g_pFontLabel->SetNativeFontInfo( config );
    pConf->Read( _T("FontSmall"), &config, wxEmptyString );
    if( !config.IsEmpty() ) g_pFontSmall->SetNativeFontInfo( config );

    pConf->Read( _T("SpeedometerMax"), &g_iDashSpeedMax, 12 );
    pConf->Read( _T("COGDamp"), &g_iDashCOGDamp, 0);
    pConf->Read( _T("SpeedUnit"), &g_iDashSpeedUnit, 0 );
    pConf->Read( _T("SOGDamp"), &g_iDashSOGDamp, 0);
    pConf->Read( _T("DepthUnit"), &g_iDashDepthUnit, 3 );
    g_iDashDepthUnit = wxMax(g_iDashDepthUnit, 3);

    pConf->Read( _T("DepthOffset"), &g_dDashDBTOffset, 0 );

    pConf->Read( _T("DistanceUnit"), &g_iDashDistanceUnit, 0 );
    pConf->Read( _T("WindSpeedUnit"), &g_iDashWindSpeedUnit, 0 );

    pConf->Read( _T("UTCOffset"), &g_iUTCOffset, 0 );
    pConf->Read( _T("TemperatureUnit"), &g_iDashTemperatureUnit, 0 );
}

void dashboard_pi::LoadDashboardInstruments( wxFileConfig *pConf )
{        
    wxString version;
    pConf->Read( _T("Version"), &version, wxEmptyString );
    int d_cnt;
    pConf->Read( _T("DashboardCount"), &d_cnt, -1 );
    // TODO: Memory leak? We should destroy everything first
    m_ArrayOfDashboardWindow.Clear();
    if( version.IsEmpty() && d_cnt == -1 ) {
        m_config_version = 1;
        // Let's load version 1 or default settings.
        int i_cnt;
        pConf->Read( _T("InstrumentCount"), &i_cnt, -1 );
        wxArrayInt ar;
        wxArrayString idar;
        if( i_cnt != -1 ) {
            for( int i = 0; i < i_cnt; i++ ) {
                int id;
                wxString ids;
                pConf->Read( wxString::Format( _T("Instrument%d"), i + 1 ), &id, -1 );
                pConf->Read( wxString::Format( _T("InstrumentID%d"), i + 1 ), &ids, _T("") );
                if( id != -1 ) {
                    ar.Add( id );
                    idar.Add ( ids );
                }
            }
        } else {
            // This is the default instrument list
            ar.Add( ID_DBP_I_POS ); idar.Add( _T("") );
            ar.Add( ID_DBP_D_COG ); idar.Add( _T("") );
            ar.Add( ID_DBP_D_GPS ); idar.Add( _T("") );
        }

        DashboardWindowContainer *cont = new DashboardWindowContainer( NULL, MakeName(),
                                                                       _("DashT"),
                                                                       _T("V"), ar, idar );
        cont->m_bPersVisible = true;
        m_ArrayOfDashboardWindow.Add(cont);

    } else {
        // Version 2
        m_config_version = 2;
        bool b_onePersisted = false;
        for( int i = 0; i < d_cnt; i++ ) {
            pConf->SetPath( wxString::Format( _T("/PlugIns/DashT/Dashboard%d"), i + 1 ) );
            wxString name;
            pConf->Read( _T("Name"), &name, MakeName() );
            wxString caption;
            pConf->Read( _T("Caption"), &caption,
                         _T("DashT") );
            wxString orient;
            pConf->Read( _T("Orientation"), &orient, _T("V") );
            int i_cnt;
            pConf->Read( _T("InstrumentCount"), &i_cnt, -1 );
            bool b_persist;
            pConf->Read( _T("Persistence"), &b_persist, 1 );

            wxArrayInt ar;
            wxArrayString idar;
            for( int j = 0; j < i_cnt; j++ ) {
                int id;
                wxString ids;
                pConf->Read( wxString::Format( _T("Instrument%d"), j + 1 ), &id, -1 );
                pConf->Read( wxString::Format( _T("InstrumentID%d"), j + 1 ), &ids, _T("") );
                if( id != -1 ) {
                    ar.Add( id );
                    idar.Add ( ids );
                }
            }
            // TODO: Do not add if GetCount == 0
            DashboardWindowContainer *cont = new DashboardWindowContainer( NULL, name, caption, orient, ar, idar );
            cont->m_bPersVisible = b_persist;

            if(b_persist)
                b_onePersisted = true;

            m_ArrayOfDashboardWindow.Add(cont);

        }

        // Make sure at least one dashboard is scheduled to be visible
        if( m_ArrayOfDashboardWindow.Count() && !b_onePersisted){
            DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item(0);
            if(cont)
                cont->m_bPersVisible = true;
        }

    }
}

void dashboard_pi::LoadDialSettings( wxFileConfig *pConf )
{
    pConf->Read( _T("BackgroundColor"),        &g_sDialColorBackground,       _T("DASHB") );
    pConf->Read( _T("ForeroundColor"),         &g_sDialColorForeground,       _T("DASHF") );
    pConf->Read( _T("LabelColor"),             &g_sDialColorLabel,            _T("DASHL") );
    pConf->Read( _T("RedColor"),               &g_sDialColorRed,              _T("DASHR") );
    pConf->Read( _T("GreenColor"),             &g_sDialColorGreen,            _T("DASHG") );
    pConf->Read( _T("IllustrationsColor1"),    &g_sDialColorIs1,              _T("DASH1") );
    pConf->Read( _T("IllustrationsColor2"),    &g_sDialColorIs2,              _T("DASH2") );
    pConf->Read( _T("NeedleColor"),            &g_sDialNeedleColor,           _T("DASHN") );
    pConf->Read( _T("SecondNeedleColor"),      &g_sDialSecondNeedleColor,     _T("BLUE3") );
    pConf->Read( _T("CentralCircleColor"),     &g_sDialCentralCircleColor,    _T("UBLCK") );
    pConf->Read( _T("CompassBackgroundColor"), &g_sDialColorCompassBackgound, _T("COMP1") );
    pConf->Read( _T("EmbossedNeedle"),         &g_bDialNeedleEmbossed,        true        );
    pConf->Read( _T("EmbosNeedleContourColor"),&g_sDialNeedleContourColor,    _T("UBLCK") );
    pConf->Read( _T("ShowRedGreenFace"),       &g_bDialShowRedGreen,          true        );
}

bool dashboard_pi::SaveConfig( void )
{
    wxFileConfig *pConf = static_cast <wxFileConfig *>(m_pconfig);
    if( pConf ) {
        pConf->SetPath( _T("/PlugIns/DashT") );
        SaveDashboardBasePart( pConf );
        SaveDashboardInstruments( pConf );
        pConf->SetPath( _T("/PlugIns/DashT/Dials") );
        SaveDialSettings( pConf );
        pConf->SetPath( _T("/PlugIns/DashT") );
        return true;
    }
    else
        return false;
}

void dashboard_pi::SaveDashboardBasePart( wxFileConfig *pConf )
{
    pConf->Write( _T("Version"), _T("2") );
    pConf->Write( _T("FontTitle"), g_pFontTitle->GetNativeFontInfoDesc() );
    pConf->Write( _T("FontData"), g_pFontData->GetNativeFontInfoDesc() );
    pConf->Write( _T("FontLabel"), g_pFontLabel->GetNativeFontInfoDesc() );
    pConf->Write( _T("FontSmall"), g_pFontSmall->GetNativeFontInfoDesc() );

    pConf->Write( _T("SpeedometerMax"), g_iDashSpeedMax );
    pConf->Write( _T("COGDamp"), g_iDashCOGDamp );
    pConf->Write( _T("SpeedUnit"), g_iDashSpeedUnit );
    pConf->Write( _T("SOGDamp"), g_iDashSOGDamp );
    pConf->Write( _T("DepthUnit"), g_iDashDepthUnit );
    pConf->Write( _T("DepthOffset"), g_dDashDBTOffset );
    pConf->Write( _T("DistanceUnit"), g_iDashDistanceUnit );
    pConf->Write( _T("WindSpeedUnit"), g_iDashWindSpeedUnit );
    pConf->Write( _T("TemperatureUnit"), g_iDashTemperatureUnit );
    pConf->Write( _T("UTCOffset"), g_iUTCOffset );
}

void dashboard_pi::SaveDashboardInstruments( wxFileConfig *pConf )
{
    pConf->Write( _T("DashboardCount" ), (int) m_ArrayOfDashboardWindow.GetCount() );
    for( unsigned int i = 0; i < m_ArrayOfDashboardWindow.GetCount(); i++ ) {
        DashboardWindowContainer *cont = m_ArrayOfDashboardWindow.Item( i );
        pConf->SetPath( wxString::Format( _T("/PlugIns/DashT/Dashboard%d"), i + 1 ) );
        pConf->Write( _T("Name"), cont->m_sName );
        pConf->Write( _T("Caption"), cont->m_sCaption );
        pConf->Write( _T("Orientation"), cont->m_sOrientation );
        pConf->Write( _T("Persistence"), cont->m_bPersVisible );

        pConf->Write( _T("InstrumentCount"), (int) cont->m_aInstrumentList.GetCount() );
        for( unsigned int j = 0; j < cont->m_aInstrumentList.GetCount(); j++ ) {
            pConf->Write( wxString::Format( _T("Instrument%d"), j + 1 ),
                          cont->m_aInstrumentList.Item( j ) );
            pConf->Write( wxString::Format( _T("InstrumentID%d"), j + 1 ),
                          cont->m_aInstrumentIDs.Item( j ) );
        }
    }
}

void dashboard_pi::SaveDialSettings( wxFileConfig *pConf )
{
    pConf->Write( _T("BackgroundColor"),        g_sDialColorBackground );
    pConf->Write( _T("ForeroundColor"),         g_sDialColorForeground );
    pConf->Write( _T("LabelColor"),             g_sDialColorLabel );
    pConf->Write( _T("RedColor"),               g_sDialColorRed );
    pConf->Write( _T("GreenColor"),             g_sDialColorGreen );
    pConf->Write( _T("IllustrationsColor1"),    g_sDialColorIs1 );
    pConf->Write( _T("IllustrationsColor2"),    g_sDialColorIs2 );
    pConf->Write( _T("NeedleColor"),            g_sDialNeedleColor );
    pConf->Write( _T("SecondNeedleColor"),      g_sDialSecondNeedleColor );
    pConf->Write( _T("CentralCircleColor"),     g_sDialCentralCircleColor );
    pConf->Write( _T("CompassBackgroundColor"), g_sDialColorCompassBackgound);
    pConf->Write( _T("EmbossedNeedle"),         g_bDialNeedleEmbossed );
    pConf->Write( _T("EmbosNeedleContourColor"),g_sDialNeedleContourColor );
    pConf->Write( _T("ShowRedGreenFace"),       g_bDialShowRedGreen );
}
