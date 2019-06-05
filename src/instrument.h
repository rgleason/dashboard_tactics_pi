/***************************************************************************
 * $Id: instrument.h, v1.0 2010/08/30 SethDart Exp $
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

#ifndef _INSTRUMENT_H_
#define _INSTRUMENT_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#if !wxUSE_GRAPHICS_CONTEXT
#define wxGCDC wxDC
#endif

// Required GetGlobalColor
#include "ocpn_plugin.h"

#ifdef _INCLUDE_TACTICS_PI_
#ifndef _TACTICSPI_H_
#define _TACTICSPI_H_ // compatibility adjustements: more instruments - extended capacity flags (64-bit)
#endif // _TACTICSPI_H_
#endif // _INCLUDE_TACTICS_PI_

#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>         // supplemental, for Mac

const wxString DEGREE_SIGN = wxString::Format(_T("%c"), 0x00B0); // This is the degree sign in UTF8. It should be correctly handled on both Win & Unix
#define DefaultWidth 150

extern wxFont *g_pFontTitle;
extern wxFont *g_pFontData;
extern wxFont *g_pFontLabel;
extern wxFont *g_pFontSmall;

wxString toSDMM ( int NEflag, double a );

class DashboardInstrument;
class DashboardInstrument_Single;
class DashboardInstrument_Position;
class DashboardInstrument_Sun;

#ifndef _TACTICSPI_H_
enum {
#else
enum eSentenceType : unsigned long long {
#endif // _TACTICSPI_H_
    OCPN_DBP_STC_LAT   = 1 << 0,
    OCPN_DBP_STC_LON   = 1 << 1,
    OCPN_DBP_STC_SOG   = 1 << 2,
    OCPN_DBP_STC_COG   = 1 << 3,
    OCPN_DBP_STC_STW   = 1 << 4,
    OCPN_DBP_STC_HDM   = 1 << 5,
    OCPN_DBP_STC_HDT   = 1 << 6,
    OCPN_DBP_STC_HMV   = 1 << 7, // Magnetic variation
    OCPN_DBP_STC_BRG   = 1 << 8,
    OCPN_DBP_STC_AWA   = 1 << 9,
    OCPN_DBP_STC_AWS   = 1 << 10,
    OCPN_DBP_STC_TWA   = 1 << 11,
    OCPN_DBP_STC_TWS   = 1 << 12,
    OCPN_DBP_STC_DPT   = 1 << 13,
    OCPN_DBP_STC_TMP   = 1 << 14,
    OCPN_DBP_STC_VMG   = 1 << 15,
    OCPN_DBP_STC_RSA   = 1 << 16,
    OCPN_DBP_STC_SAT   = 1 << 17,
    OCPN_DBP_STC_GPS   = 1 << 18,
    OCPN_DBP_STC_PLA   = 1 << 19,  // Cursor latitude
    OCPN_DBP_STC_PLO   = 1 << 20,  // Cursor longitude
    OCPN_DBP_STC_CLK   = 1 << 21,
    OCPN_DBP_STC_MON   = 1 << 22,
    OCPN_DBP_STC_ATMP  = 1 << 23,  //AirTemp
    OCPN_DBP_STC_TWD   = 1 << 24,
    OCPN_DBP_STC_TWS2  = 1 << 25,
    OCPN_DBP_STC_VLW1  = 1 << 26,  // Trip Log
    OCPN_DBP_STC_VLW2  = 1 << 27,  // Sum Log
    OCPN_DBP_STC_MDA   = 1 << 28,  // Bareometic pressure
    OCPN_DBP_STC_MCOG  = 1 << 29,  // Magnetic Course over Ground
	OCPN_DBP_STC_PITCH = 1 << 30,  //Pitch
#ifndef _TACTICSPI_H_
	OCPN_DBP_STC_HEEL  = 1 << 31  //Heel
#else
    // >32 sentence capacity identifiers needs 64 bits enumeration
	OCPN_DBP_STC_HEEL    = 1ULL << 31,  //Heel
    OCPN_DBP_STC_LEEWAY  = 1ULL << 32,
    OCPN_DBP_STC_CURRDIR = 1ULL << 33,
    OCPN_DBP_STC_CURRSPD = 1ULL << 34,
    OCPN_DBP_STC_DTW     = 1ULL << 35,
    OCPN_DBP_STC_BC      = 1ULL << 36,
    OCPN_DBP_STC_TWAMARK = 1ULL << 37, // TWA to a Waypoint
    OCPN_DBP_STC_POLPERF = 1ULL << 38  // Polar Performance
#endif // _TACTICSPI_H_
};

class DashboardInstrument : public wxControl
{
public:
    DashboardInstrument(wxWindow *pparent, wxWindowID id, wxString title,
#ifdef _TACTICSPI_H_
                        unsigned long long cap_flag
#else
                        int cap_flag
#endif // _TACTICSPI_H_
        );
    ~DashboardInstrument(){}

#ifdef _TACTICSPI_H_
    unsigned long long GetCapacity(void);
#else
    int GetCapacity();
#endif // _TACTICSPI_H_
    void OnEraseBackground(wxEraseEvent& WXUNUSED(evt));
    virtual wxSize GetSize( int orient, wxSize hint ) = 0;
    void OnPaint(wxPaintEvent& WXUNUSED(event));
    virtual void SetData(
#ifdef _TACTICSPI_H_
        unsigned long long st,
#else
        int st,
#endif // _TACTICSPI_H_
        double data, wxString unit) = 0;
    void SetDrawSoloInPane(bool value);
    void MouseEvent( wxMouseEvent &event );
      
    int               instrumentTypeId;

protected:
#ifdef _TACTICSPI_H_
    unsigned long long m_cap_flag;
#else
    int m_cap_flag;
#endif // _TACTICSPI_H_
    int               m_TitleHeight;
    wxString          m_title;

    virtual void Draw(wxGCDC* dc) = 0;
private:
    bool m_drawSoloInPane;
};

class DashboardInstrument_Single : public DashboardInstrument
{
public:
    DashboardInstrument_Single(wxWindow *pparent, wxWindowID id, wxString title,
#ifdef _TACTICSPI_H_
                               unsigned long long cap,
#else
                               int cap,
#endif // _TACTICSPI_H_
                               wxString format);
    ~DashboardInstrument_Single(){}

    wxSize GetSize( int orient, wxSize hint );
    void SetData(
#ifdef _TACTICSPI_H_
        unsigned long long st,
#else
        int st,
#endif // _TACTICSPI_H_
        double data, wxString unit);

protected:
    wxString          m_data;
    wxString          m_format;
    int               m_DataHeight;

    void Draw(wxGCDC* dc);
};

class DashboardInstrument_Position : public DashboardInstrument
{
public:
      DashboardInstrument_Position(wxWindow *pparent, wxWindowID id, wxString title,
#ifdef _TACTICSPI_H_
                                   unsigned long long cap_flag1=OCPN_DBP_STC_LAT,
                                   unsigned long long cap_flag2=OCPN_DBP_STC_LON
#else
                                   int cap_flag1=OCPN_DBP_STC_LAT,
                                   int cap_flag2=OCPN_DBP_STC_LON
#endif // _TACTICSPI_H_
          );
      ~DashboardInstrument_Position(){}

      wxSize GetSize( int orient, wxSize hint );
      void SetData(
#ifdef _TACTICSPI_H_
          unsigned long long st,
#else
          int st,
#endif // _TACTICSPI_H_
          double data, wxString unit);

protected:
      wxString          m_data1;
      wxString          m_data2;
#ifdef _TACTICSPI_H_
      unsigned long long     m_cap_flag1;
      unsigned long long     m_cap_flag2;
#else
      int               m_cap_flag1;
      int               m_cap_flag2;
#endif // _TACTICSPI_H_
      int               m_DataHeight;

      void Draw(wxGCDC* dc);
};

#endif
