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
