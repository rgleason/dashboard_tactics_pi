#include "doctest.h"

#include <cmath>

int g_iDashTemperatureUnit;

#include "DashboardFunctions.h"

#include <wx/tokenzr.h>

TEST_CASE( "Random number generator" ) {
    SUBCASE( "C++2011 marco definition compatitbility" ) {
        CHECK( GetRandomNumber( INT_MAX,   INT_MAX )   == INT_MAX );
        CHECK( GetRandomNumber( USHRT_MAX, USHRT_MAX ) == USHRT_MAX );
        CHECK( GetRandomNumber( UCHAR_MAX, UCHAR_MAX ) == UCHAR_MAX );
        CHECK( GetRandomNumber( INT_MAX,   INT_MAX )   == INT_MAX );
        CHECK( GetRandomNumber( INT_MAX,   INT_MAX )   == INT_MAX );
    }
    SUBCASE( "Integer range" ) {
        int randret = GetRandomNumber( INT_MIN, INT_MAX );
        CHECK( ( (randret >= INT_MIN ) && (randret <= INT_MAX) ) );
    }
}
TEST_CASE( "RFC4122 Unique Identifier generator" ) {
    wxString sRet1 = wxEmptyString;
    sRet1 = GetUUID();
    CHECK( !sRet1.IsEmpty() );
    wxStringTokenizer tokenizer( sRet1, "-" );
    CHECK( static_cast<int>(tokenizer.CountTokens()) == 5 );
    wxString elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 8 );
    elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 4 );
    elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 4 );
    elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 4 );
    elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 12 );
    wxString sRet2 = wxEmptyString;
    sRet2 = GetUUID();
    CHECK( sRet1 != sRet2 );
}
TEST_CASE( "Make name containing unique name but with constant identifier prefix for this plug-in") {
    wxString sName1 = wxEmptyString;
    sName1 = MakeName();
    CHECK( !sName1.IsEmpty() );
    wxStringTokenizer tokenizer( sName1, "-" );
    CHECK( static_cast<int>(tokenizer.CountTokens()) == 6 );
    wxString elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 5 );
    CHECK( elem == L"DASHT" );
    elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 8 );
    elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 4 );
    elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 4 );
    elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 4 );
    elem = tokenizer.GetNextToken();
    CHECK( elem.Len() == 12 );
    wxString sName2 = wxEmptyString;
    sName2 = GetUUID();
    CHECK( sName1 != sName2 );
}
TEST_CASE( "Temperature unit conversion" ) {
    double tempRefValue = 20.;
    SUBCASE( "Configuration selection is Celsius and instrument provides Celsius units" ) {
        g_iDashTemperatureUnit = 0;
        double tempValue = tempRefValue;
        wxString tempUnit( L"C" );
        checkNMEATemperatureDataAndUnit(tempValue, tempUnit);
        CHECK( tempValue == tempRefValue );
    }
    SUBCASE( "Configuration selection is Celsius and instrument provides Fahrenheit units" ) {
        g_iDashTemperatureUnit = 0;
        double tempValue = (tempRefValue * 9. / 5.) + 32.;
        wxString tempUnit( L"F" );
        checkNMEATemperatureDataAndUnit(tempValue, tempUnit);
        CHECK( tempValue == tempRefValue );
    }
    SUBCASE( "Configuration selection is Fahrenheit and instrument provides Celsius units" ) {
        g_iDashTemperatureUnit = 1;
        double tempValue = tempRefValue;
        double tempRefValueF = (tempRefValue * 9. / 5.) + 32.;
        wxString tempUnit( L"C" );
        checkNMEATemperatureDataAndUnit(tempValue, tempUnit);
        CHECK( tempValue == tempRefValueF );
    }
    SUBCASE( "Configuration selection is Fahrenheit and instrument provides Fahrenheit units" ) {
        g_iDashTemperatureUnit = 1;
        double tempRefValue = 68.;
        double tempValue = tempRefValue;
        wxString tempUnit( L"F" );
        checkNMEATemperatureDataAndUnit(tempValue, tempUnit);
        CHECK( tempValue == tempRefValue );
    }
}
/*
  In order not to require opencpn-executable being actually tested (it could
  take some...), emulate here a few of its pluginmanger.cpp constructors.
*/
PlugIn_Waypoint::PlugIn_Waypoint() {};
PlugIn_Waypoint::PlugIn_Waypoint(double lat, double lon,
                const wxString& icon_ident, const wxString& wp_name,
                const wxString& GUID) {
    m_lat = lat;
    m_lon = lon;
    m_IconName = icon_ident;
    m_MarkName = wp_name;
    m_GUID = GUID;
    m_MarkDescription = wxEmptyString;
    m_CreateTime = wxDateTime::Now();
    m_IsVisible = true;
    m_HyperlinkList = nullptr;
}
TEST_CASE( "PlugIn_Waypoint object copy" ) {
    double lat = 35.244;
    double lon = -129.99;
    const wxString icon = _T("testiconname1.png");
    const wxString name = _T("testwpname1");
    const wxString guid = _T("123-abc-222");
    PlugIn_Waypoint *src = new PlugIn_Waypoint( lat, lon, icon, name, guid );
    src->m_MarkDescription = _T("testdescription1");
    src->m_CreateTime = wxDateTime::Now();
    src->m_IsVisible = false;
    src->m_HyperlinkList = nullptr;
    PlugIn_Waypoint *dst = new PlugIn_Waypoint();
    CopyPlugInWaypointWithoutHyperlinks( src, dst );
    CHECK( lat == dst->m_lat );
    CHECK( lon == dst->m_lon );
    CHECK( guid == dst->m_GUID );
    CHECK( name == dst->m_MarkName );
    CHECK( src->m_MarkDescription == dst->m_MarkDescription );
    CHECK( src->m_CreateTime.IsSameTime( dst->m_CreateTime ) );
    CHECK( src->m_IsVisible == dst->m_IsVisible );
    CHECK( icon == dst->m_IconName );
    CHECK( dst->m_HyperlinkList == nullptr );
};
TEST_CASE( "Get FontFamily" ) {
    int testVal = (int) GetFontFamily( _T("SWISS") );
    CHECK( testVal == (int) wxFONTFAMILY_SWISS );
    testVal = (int) GetFontFamily( _T("swiss") );
    CHECK( testVal == (int) wxFONTFAMILY_SWISS );
    testVal = (int) GetFontFamily( _T("Swiss") );
    CHECK( testVal == (int) wxFONTFAMILY_SWISS );
    testVal = (int) GetFontFamily( _T("TELETYPE") );
    CHECK( testVal == (int) wxFONTFAMILY_TELETYPE );
    testVal = (int) GetFontFamily( _T("MODERN") );
    CHECK( testVal == (int) wxFONTFAMILY_MODERN );
    testVal = (int) GetFontFamily( _T("ROMAN") );
    CHECK( testVal == (int) wxFONTFAMILY_ROMAN );
    testVal = (int) GetFontFamily( _T("DECORATIVE") );
    CHECK( testVal == (int) wxFONTFAMILY_DECORATIVE );
    testVal = (int) GetFontFamily( _T("SCRIPT") );
    CHECK( testVal == (int) wxFONTFAMILY_SCRIPT );
    testVal = (int) GetFontFamily( _T("") );
    CHECK( testVal == (int) wxFONTFAMILY_DEFAULT );
};
TEST_CASE( "Get FontStyle" ) {
    int testVal = (int) GetFontStyle( _T("NORMAL") );
    CHECK( testVal == (int) wxFONTSTYLE_NORMAL );
    testVal = (int) GetFontStyle( _T("normal") );
    CHECK( testVal == (int) wxFONTSTYLE_NORMAL );
    testVal = (int) GetFontStyle( _T("Normal") );
    CHECK( testVal == (int) wxFONTSTYLE_NORMAL );
    testVal = (int) GetFontStyle( _T("ITALIC") );
    CHECK( testVal == (int) wxFONTSTYLE_ITALIC );
    testVal = (int) GetFontStyle( _T("SLANT") );
    CHECK( testVal == (int) wxFONTSTYLE_SLANT );
    testVal = (int) GetFontStyle( _T("MAX") );
    CHECK( testVal == (int) wxFONTSTYLE_MAX );
    testVal = (int) GetFontStyle( _T("") );
    CHECK( testVal == (int) wxFONTSTYLE_NORMAL );
};
TEST_CASE( "Get FontWeight" ) {
    int testVal = (int) GetFontWeight( _T("NORMAL") );
    CHECK( testVal == (int) wxFONTWEIGHT_NORMAL );
    testVal = (int) GetFontWeight( _T("normal") );
    CHECK( testVal == (int) wxFONTWEIGHT_NORMAL );
    testVal = (int) GetFontWeight( _T("Normal") );
    CHECK( testVal == (int) wxFONTWEIGHT_NORMAL );
    testVal = (int) GetFontWeight( _T("LIGHT") );
    CHECK( testVal == (int) wxFONTWEIGHT_LIGHT );
    testVal = (int) GetFontWeight( _T("BOLD") );
    CHECK( testVal == (int) wxFONTWEIGHT_BOLD );
    testVal = (int) GetFontWeight( _T("MAX") );
    CHECK( testVal == (int) wxFONTWEIGHT_MAX );
    testVal = (int) GetFontWeight( _T("") );
    CHECK( testVal == (int) wxFONTWEIGHT_NORMAL );
};
TEST_CASE( "RFC3359 format date/time string parsing." ) {
    wxString testval1 = L"2014-08-15T19:06:37.688Z";
    wxLongLong testexcp1 = 1408129597688;
    bool parseError = true;
    wxDateTime testtime1 = parseRfc3359UTC( &testval1, parseError );
    CHECK ( parseError == false );
    wxLongLong testretval1 = testtime1.GetValue();
    CHECK ( testretval1 == testexcp1 );
    wxString testval2 = L"2014-08-15T19:06:37.68800Z";
    wxLongLong testexcp2 = 1408129597688;
    parseError = true;
    wxDateTime testtime2 = parseRfc3359UTC( &testval2, parseError );
    CHECK ( parseError == false );
    wxLongLong testretval2 = testtime2.GetValue();
    CHECK ( testretval2 == testexcp2 );
};
