/******************************************************************************
 * $Id: instrument.cpp, v1.0 2010/08/30 SethDart Exp $
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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <cmath>

#include "instrument.h"
#include "wx28compat.h"

#ifdef _TACTICSPI_H_
#include "plugin_ids.h"
wxBEGIN_EVENT_TABLE (DashboardInstrument,wxControl)
    EVT_TIMER (myID_DBP_I_TIMER_TICK, DashboardInstrument::OnDPBITimerTick)
   EVT_CLOSE (DashboardInstrument::OnClose)
wxEND_EVENT_TABLE ()
#endif // _TACTICSPI_H_

//----------------------------------------------------------------
//
//    Generic DashboardInstrument Implementation
//
//----------------------------------------------------------------

DashboardInstrument::DashboardInstrument( wxWindow *pparent, wxWindowID id, wxString title,
#ifdef _TACTICSPI_H_
                                          unsigned long long cap_flag, bool drawSoloInPane ) :
    wxControl( pparent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE )
#else
                                          int cap_flag ) :
    wxControl( pparent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE )
#endif // _TACTICSPI_H_
{

#ifdef _TACTICSPI_H_
      m_title = title.wc_str();
#else
      m_title = title;
#endif // _TACTICSPI_H_
      m_cap_flag = cap_flag;
      instrumentTypeId = 0;
      SetBackgroundStyle( wxBG_STYLE_CUSTOM );
#ifdef _TACTICSPI_H_
      SetDrawSoloInPane( drawSoloInPane );
#else
      SetDrawSoloInPane(false);
#endif // _TACTICSPI_H_
      wxClientDC dc(this);
      int width;
      dc.GetTextExtent(m_title, &width, &m_TitleHeight, 0, 0, g_pFontTitle);

      Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(DashboardInstrument::OnEraseBackground));
      Connect(wxEVT_PAINT, wxPaintEventHandler(DashboardInstrument::OnPaint));
      
      //  On OSX, there is an orphan mouse event that comes from the automatic
      //  exEVT_CONTEXT_MENU synthesis on the main wxWindow mouse handler.
      //  The event goes to an instrument window (here) that may have been deleted by the
      //  preferences dialog.  Result is NULL deref.
      //  Solution:  Handle right-click here, and DO NOT skip()
      //  Strangely, this does not work for GTK...
      //  See: http://trac.wxwidgets.org/ticket/15417
      
#ifdef __WXOSX__
      Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(DashboardInstrument::MouseEvent), NULL, this);
#endif
#ifdef _TACTICSPI_H_
      m_DPBITickTimer = new wxTimer( this, myID_DBP_I_TIMER_TICK );
      previousTimestamp = 0LL;
      m_DPBITickTimer->Start( DBP_I_TIMER_TICK, wxTIMER_CONTINUOUS );
#endif // _TACTICSPI_H_
}

#ifdef _TACTICSPI_H_
DashboardInstrument::~DashboardInstrument()
{
    if ( this->m_DPBITickTimer != NULL ) {
        this->m_DPBITickTimer->Stop();
        delete this->m_DPBITickTimer;
    }
}
void DashboardInstrument::OnClose( wxCloseEvent &event )
{
    if ( this->m_DPBITickTimer != NULL ) {
        this->m_DPBITickTimer->Stop();
    }
    event.Skip(); // Destroy() must be called
}
void DashboardInstrument::setTimestamp( long long ts )
{
    previousTimestamp = ts;
}
long long DashboardInstrument::getTimestamp()
{
    return previousTimestamp;
}
void DashboardInstrument::OnDPBITimerTick( wxTimerEvent &event )
{
    if (previousTimestamp == 0LL)
        return;
    wxLongLong wxllNowMs = wxGetUTCTimeMillis();
    long long  llNowMs = wxllNowMs.GetValue();
    if ( (llNowMs - previousTimestamp) >= (DBP_I_TIMER_TICK * DBP_I_DATA_TIMEOUT) ) {
        this->timeoutEvent();
        previousTimestamp = llNowMs;
    }
}

#endif // _TACTICSPI_H_

void DashboardInstrument::MouseEvent( wxMouseEvent &event )
{
    if ( event.GetEventType() == wxEVT_RIGHT_DOWN )
    {
       wxContextMenuEvent evtCtx(wxEVT_CONTEXT_MENU,
                                  this->GetId(),
                                  this->ClientToScreen(event.GetPosition()));
        evtCtx.SetEventObject(this);
        GetParent()->GetEventHandler()->AddPendingEvent(evtCtx );
    }
}

#ifdef _TACTICSPI_H_
    unsigned long long DashboardInstrument::GetCapacity()
#else
    int DashboardInstrument::GetCapacity()
#endif // _TACTICSPI_H_
{
      return m_cap_flag;
}
void DashboardInstrument::SetDrawSoloInPane(bool value)
{
    m_drawSoloInPane = value;
}
void DashboardInstrument::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
        // intentionally empty
}

void DashboardInstrument::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxAutoBufferedPaintDC pdc( this );
    if( !pdc.IsOk() ) {
        wxLogMessage( _T("DashboardInstrument::OnPaint() fatal: wxAutoBufferedPaintDC.IsOk() false.") );
        return;
    }

    wxSize size = GetClientSize();
    if( size.x == 0 || size.y == 0 ) {
        wxLogMessage( _T("DashboardInstrument::OnPaint() fatal: Zero size DC.") );
        return;
    }

#if wxUSE_GRAPHICS_CONTEXT
    wxGCDC dc( pdc );
#else
    wxDC &dc( pdc );
#endif

    wxColour cl;
    GetGlobalColor( _T("DASHB"), &cl );
    dc.SetBackground( cl );
#ifdef __WXGTK__
    dc.SetBrush( cl );
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.DrawRectangle( 0, 0, size.x, size.y );
#endif
    dc.Clear();

    Draw( &dc );

    if(!m_drawSoloInPane) {

    //  Windows GCDC does a terrible job of rendering small texts
    //  Workaround by using plain old DC for title box if text size is too small
#ifdef __WXMSW__
        if( g_pFontTitle->GetPointSize() > 12 )
#endif
        {
            wxPen pen;
            pen.SetStyle( wxPENSTYLE_SOLID );
            GetGlobalColor( _T("DASHL"), &cl );
            pen.SetColour( cl );
            dc.SetPen( pen );
            dc.SetBrush( cl );
            dc.DrawRoundedRectangle( 0, 0, size.x, m_TitleHeight, 3 );

            dc.SetFont( *g_pFontTitle );
            GetGlobalColor( _T("DASHF"), &cl );
            dc.SetTextForeground( cl );
            dc.DrawText( m_title, 5, 0 );
        }

#ifdef __WXMSW__
        if( g_pFontTitle->GetPointSize() <= 12 ) {
            wxColour cl;
            GetGlobalColor( _T("DASHB"), &cl );
            pdc.SetBrush(cl);
            pdc.DrawRectangle(0, 0, size.x, m_TitleHeight);

            wxPen pen;
            pen.SetStyle( wxPENSTYLE_SOLID );
            GetGlobalColor( _T("DASHL"), &cl );
            pen.SetColour( cl );
            pdc.SetPen( pen );
            pdc.SetBrush( cl );
            pdc.DrawRoundedRectangle( 0, 0, size.x, m_TitleHeight, 3 );

            pdc.SetFont( *g_pFontTitle );
            GetGlobalColor( _T("DASHF"), &cl );
            pdc.SetTextForeground( cl );
            pdc.DrawText( m_title, 5, 0 );
        }
#endif
    }
}

//----------------------------------------------------------------
//
//    DashboardInstrument_Simple Implementation
//
//----------------------------------------------------------------

DashboardInstrument_Single::DashboardInstrument_Single(wxWindow *pparent, wxWindowID id, wxString title,
#ifdef _TACTICSPI_H_
                                                       unsigned long long cap_flag,
#else
                                                       int cap_flag,
#endif // _TACTICSPI_H_
                                                       wxString format)
      :DashboardInstrument(pparent, id, title, cap_flag)
{
      m_format = format;
      m_data = _T("---");
      m_DataHeight = 0;
}

wxSize DashboardInstrument_Single::GetSize( int orient, wxSize hint )
{
      wxClientDC dc(this);
      int w;
      dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
      dc.GetTextExtent(_T("000"), &w, &m_DataHeight, 0, 0, g_pFontData);

      if( orient == wxHORIZONTAL ) {
          return wxSize( DefaultWidth, wxMax(hint.y, m_TitleHeight+m_DataHeight) );
      } else {
          return wxSize( wxMax(hint.x, DefaultWidth), m_TitleHeight+m_DataHeight );
      }
}

void DashboardInstrument_Single::Draw(wxGCDC* dc)
{
      wxColour cl;
#ifdef __WXMSW__
      wxBitmap tbm( dc->GetSize().x, m_DataHeight, -1 );
      wxMemoryDC tdc( tbm );
      wxColour c2;
      GetGlobalColor( _T("DASHB"), &c2 );
      tdc.SetBackground( c2 );
      tdc.Clear();

      tdc.SetFont(*g_pFontData );
      GetGlobalColor( _T("DASHF"), &cl );
      tdc.SetTextForeground( cl );

      tdc.DrawText(m_data, 10, 0);

      tdc.SelectObject( wxNullBitmap );

      dc->DrawBitmap(tbm, 0, m_TitleHeight, false);
#else
      dc->SetFont(*g_pFontData );
      GetGlobalColor( _T("DASHF"), &cl );
      dc->SetTextForeground( cl );

      dc->DrawText(m_data, 10, m_TitleHeight);

#endif

}

void DashboardInstrument_Single::SetData(
#ifdef _TACTICSPI_H_
    unsigned long long st,
#else
    int st,
#endif // _TACTICSPI_H_
    double data, wxString unit
#ifdef _TACTICSPI_H_
    , long long timestamp
#endif // _TACTICSPI_H_
    )
{
#ifdef _TACTICSPI_H_
    setTimestamp( timestamp );
    // units strings shall allow passing long format strings
    unit = unit.wc_str();
#endif // _TACTICSPI_H_
      if (m_cap_flag & st){
            if(!std::isnan(data) && (data < 9999)){
                if (unit == _T("C"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_T("C");
#ifdef _TACTICSPI_H_
                else if (unit == _T("F"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_T("F");
#endif // _TACTICSPI_H_
                else if (unit == _T("\u00B0"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN;
                else if (unit == _T("\u00B0T"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_(" true");
                else if (unit == _T("\u00B0M"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_(" mag");
#ifdef _TACTICSPI_H_
                else if (unit == _T("\u00B0l")){
                    if (data < 0) data = -data;
                    m_data = L"\u2190" +
                        wxString::Format(m_format, data) + DEGREE_SIGN;
                }
                else if (unit == _T("\u00B0rl")){ // wind arrow on starboard side
                  if (data < 0) data = -data;
                  m_data = wxString::Format(m_format, data) +
                      DEGREE_SIGN + L"\u2190";
                }
                else if (unit == _T("\u00B0r")){
                  if (data < 0) data = -data;
                  m_data = wxString::Format(m_format, data) +
                      DEGREE_SIGN + L"\u2192";
                }
                else if (unit == _T("\u00B0lr")){ // wind arrow on port side
                    if (data < 0) data = -data;
                    m_data = L"\u2192" +
                        wxString::Format(m_format, data) + DEGREE_SIGN;
                }
                else if (unit == _T("\u00B0u")){
                  if (data < 0) data = -data;
                  m_data = wxString::Format(m_format, data) +
                      DEGREE_SIGN + L"\u2191";
                }
                else if (unit == _T("\u00B0d")){
                  if (data < 0) data = -data;
                  m_data = wxString::Format(m_format, data) +
                      DEGREE_SIGN + L"\u2193";
                }
#else
                else if (unit == _T("\u00B0L"))
                  m_data = _T(">")+ wxString::Format(m_format, data)+DEGREE_SIGN;
                else if (unit == _T("\u00B0R"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_T("<");
#endif // _TACTICSPI_H_
                else if (unit == _T("N")) //Knots
                  m_data = wxString::Format(m_format, data)+_T(" Kts");
/* maybe in the future ...
                else if (unit == _T("M")) // m/s
                  m_data = wxString::Format(m_format, data)+_T(" m/s");
                else if (unit == _T("K")) // km/h
                  m_data = wxString::Format(m_format, data)+_T(" km/h");
 ... to be completed
 */
                else
                  m_data = wxString::Format(m_format, data)+_T(" ")+unit;
            }
            else
                m_data = _T("---");
      }
}
#ifdef _TACTICSPI_H_
void DashboardInstrument_Single::timeoutEvent()
{
    m_data = _T("---");
    derivedTimeoutEvent();
}
#endif // _TACTICSPI_H_
//----------------------------------------------------------------
//
//    DashboardInstrument_Position Implementation
//
//----------------------------------------------------------------

DashboardInstrument_Position::DashboardInstrument_Position(wxWindow *pparent, wxWindowID id, wxString title,
#ifdef _TACTICSPI_H_
                                                           unsigned long long cap_flag1,
                                                           unsigned long long cap_flag2
#else
                                                           int cap_flag1,
                                                           int cap_flag2
#endif // _TACTICSPI_H_
    )
      :DashboardInstrument(pparent, id, title, cap_flag1 | cap_flag2)
{

      m_data1 = _T("---");
      m_data2 = _T("---");
      m_cap_flag1 = cap_flag1;
      m_cap_flag2 = cap_flag2;
      m_DataHeight = 0;
}

wxSize DashboardInstrument_Position::GetSize( int orient, wxSize hint )
{
      wxClientDC dc(this);
      int w;
      dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
      dc.GetTextExtent(_T("000  00.0000 W"), &w, &m_DataHeight, 0, 0, g_pFontData);

      if( orient == wxHORIZONTAL ) {
          return wxSize( w+10, wxMax(hint.y, m_TitleHeight+m_DataHeight*2) );
      } else {
          return wxSize( wxMax(hint.x, w+10), m_TitleHeight+m_DataHeight*2 );
      }
}

void DashboardInstrument_Position::Draw(wxGCDC* dc)
{
      wxColour cl;

#ifdef __WXMSW__
      wxBitmap tbm( dc->GetSize().x, m_DataHeight * 2, -1 );
      wxMemoryDC tdc( tbm );
      wxColour c2;
      GetGlobalColor( _T("DASHB"), &c2 );
      tdc.SetBackground( c2 );
      tdc.Clear();

      tdc.SetFont(*g_pFontData );
      GetGlobalColor( _T("DASHF"), &cl );
      tdc.SetTextForeground( cl );

      tdc.DrawText(m_data1, 10, 0);
      tdc.DrawText(m_data2, 10, m_DataHeight);

      tdc.SelectObject( wxNullBitmap );

      dc->DrawBitmap(tbm, 0, m_TitleHeight, false);
#else
      dc->SetFont(*g_pFontData );
      GetGlobalColor( _T("DASHF"), &cl );
      dc->SetTextForeground( cl );

      dc->DrawText(m_data1, 10, m_TitleHeight);
      dc->DrawText(m_data2, 10, m_TitleHeight + m_DataHeight);
#endif


}

void DashboardInstrument_Position::SetData(
#ifdef _TACTICSPI_H_
    unsigned long long st,
#else
    int st,
#endif // _TACTICSPI_H_
    double data, wxString unit
#ifdef _TACTICSPI_H_
    , long long timestamp
#endif // _TACTICSPI_H_
    )
{
#ifdef _TACTICSPI_H_
    setTimestamp( timestamp );
#endif // _TACTICSPI_H_
    if (st == m_cap_flag1)
    {
        m_data1 = toSDMM(1, data);
        m_data1[0] = ' ';
    }
    else if (st == m_cap_flag2)
    {
        m_data2 = toSDMM(2, data);
    }
    else return;
    Refresh();
}
#ifdef _TACTICSPI_H_
void DashboardInstrument_Position::timeoutEvent()
{
      m_data1 = _T("---");
      m_data2 = _T("---");
}
#endif // _TACTICSPI_H_

/**************************************************************************/
/*          Some assorted utilities                                       */
/**************************************************************************/

wxString toSDMM ( int NEflag, double a )
{
      short neg = 0;
      int d;
      long m;

      if ( a < 0.0 )
      {
            a = -a;
            neg = 1;
      }
      d = ( int ) a;
      m = ( long ) ( ( a - ( double ) d ) * 60000.0 );

      if ( neg )
            d = -d;

      wxString s;

      if ( !NEflag )
            s.Printf ( _T ( "%d %02ld.%03ld'" ), d, m / 1000, m % 1000 );
      else
      {
            if ( NEflag == 1 )
            {
                  char c = 'N';

                  if ( neg )
                  {
                        d = -d;
                        c = 'S';
                  }

                  s.Printf ( _T ( "%03d %02ld.%03ld %c" ), d, m / 1000, ( m % 1000 ), c );
            }
            else if ( NEflag == 2 )
            {
                  char c = 'E';

                  if ( neg )
                  {
                        d = -d;
                        c = 'W';
                  }
                  s.Printf ( _T ( "%03d %02ld.%03ld %c" ), d, m / 1000, ( m % 1000 ), c );
            }
      }
      return s;
}
