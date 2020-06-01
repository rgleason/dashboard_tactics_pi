#include "doctest.h"

#include "SkData.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/string.h"
#include <wx/tokenzr.h>

TEST_CASE( "Constructor" ) {
    SkData *skdata = new SkData();
    SUBCASE ( "DB Schema Recording and subscription states" ) {
        CHECK( !skdata->isRecordingAllDbSchemas() );
        skdata->recordAllDbSchemas();
        CHECK( skdata->isRecordingAllDbSchemas() );
        skdata->stopRecordingAllDbSchemas();
        CHECK( !skdata->isRecordingAllDbSchemas() );
        CHECK( !skdata->isSubscribedToAllPaths() );
        skdata->subscribeToAllPaths();
        CHECK( skdata->isSubscribedToAllPaths() );
        skdata->subscribeToSubscriptionList();
        CHECK( !skdata->isSubscribedToAllPaths() );
    }
    SUBCASE ( "wxJSONValue usage in SK path subscription management" ) {
        wxString sl ( L"A long string" );
        wxJSONValue v( L"A long string" );
        wxString s = v.AsString();
        CHECK( s.IsSameAs( sl ) );
        v = 12;
        int i = v.AsInt();
        CHECK ( i == 12 );
        wxJSONValue pTestJSON;
        wxString sself( L"vessels.self" );
        pTestJSON["context"] = sself;
        wxString spath( L"test.path.test" );
        pTestJSON["subscribe"][0]["path"] = spath;
        CHECK( pTestJSON["subscribe"][0].HasMember("path") );
        CHECK( pTestJSON["subscribe"][0]["path"].IsString() );
        wxString s2 = pTestJSON["subscribe"][0]["path"].AsString();
        CHECK( s2.IsSameAs( spath ) );
    }
    SUBCASE ( "Default subscription paths, copy constructor" ) {
        wxJSONValue pRetJSON;
        wxString sself( L"vessels.self" );
        pRetJSON["context"] = sself;
        wxString defSchemas = skdata->getAllSubscriptionsJSON( pRetJSON );
        CHECK( !defSchemas.IsEmpty() );
        CHECK( pRetJSON["subscribe"][0].HasMember("path") );
        CHECK( pRetJSON["subscribe"].Size() == 25 );
        SkData *copyskdata = new SkData( (*skdata) );
        wxJSONValue pCopyRetJSON;
        pCopyRetJSON["context"] = sself;
        wxString defCopySchemas = skdata->getAllSubscriptionsJSON( pCopyRetJSON );
        CHECK( !defCopySchemas.IsEmpty() );
        CHECK( pCopyRetJSON["subscribe"][0].HasMember("path") );
        CHECK( pCopyRetJSON["subscribe"].Size() == 25 );
    }
}
TEST_CASE( "NMEA0183, NMEA2000 and Sk path lists" ) {
    SkData *skdata = new SkData();
    wxString stpath1( L"navigation.position" );
    wxString stkey11( L"longitude" );
    wxString stkey12( L"latitude"  );
    wxString stpath2( L"environment.depth.belowTransducer" );
    wxString stkey21( L"" );
    wxString stpath3( L"environment.wind.speedApparent" );
    SUBCASE ( "NMEA0183 path list" ) {
        skdata->UpdateNMEA0183PathList( &stpath1, &stkey11 );
        skdata->UpdateNMEA0183PathList( &stpath1, &stkey12 );
        skdata->UpdateNMEA0183PathList( &stpath2, &stkey21 );
        skdata->UpdateNMEA0183PathList( &stpath2, NULL ); // duplicate
        skdata->UpdateNMEA0183PathList( &stpath3, NULL );
        wxString testOrderedList = skdata->getAllNMEA0183JsOrderedList();
        CHECK( !testOrderedList.IsEmpty() );
        wxStringTokenizer tokenizer( testOrderedList, "," );
        CHECK( static_cast<int>(tokenizer.CountTokens()) == 4 );
        wxString elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"environment.depth.belowTransducer" ) );
        elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"environment.wind.speedApparent" ) );
        elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"navigation.position.latitude" ) );
        elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"navigation.position.longitude" ) );
    }
    SUBCASE ( "NMEA2000 path list" ) {
        skdata->UpdateNMEA2000PathList( &stpath1, &stkey11 );
        skdata->UpdateNMEA2000PathList( &stpath1, &stkey12 );
        skdata->UpdateNMEA2000PathList( &stpath2, &stkey21 );
        skdata->UpdateNMEA2000PathList( &stpath2, NULL ); // duplicate
        skdata->UpdateNMEA2000PathList( &stpath3, NULL );
        wxString testOrderedList = skdata->getAllNMEA2000JsOrderedList();
        CHECK( !testOrderedList.IsEmpty() );
        wxStringTokenizer tokenizer( testOrderedList, "," );
        CHECK( static_cast<int>(tokenizer.CountTokens()) == 4 );
        wxString elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"environment.depth.belowTransducer" ) );
        elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"environment.wind.speedApparent" ) );
        elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"navigation.position.latitude" ) );
        elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"navigation.position.longitude" ) );
    }
    SUBCASE ( "SK path list" ) {
        skdata->UpdateSubscriptionList( &stpath1, &stkey11 ); // duplicate
        skdata->UpdateSubscriptionList( &stpath1, &stkey12 ); // duplicate
        skdata->UpdateSubscriptionList( &stpath2, &stkey21 ); // duplicate
        skdata->UpdateSubscriptionList( &stpath2, NULL );     // duplicate x2
        skdata->UpdateSubscriptionList( &stpath3, NULL );     // duplicate
        wxJSONValue noJSONret( wxJSONTYPE_NULL );
        wxString testOrderedList = skdata->getAllSubscriptionsJSON( noJSONret );
        CHECK( !testOrderedList.IsEmpty() );
        wxStringTokenizer tokenizer( testOrderedList, "," );
        CHECK( static_cast<int>(tokenizer.CountTokens()) == 25 ); // default paths!
        wxString elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"environment.depth.belowKeel" ) );
        elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"environment.depth.belowTransducer" ) );
        elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"environment.outside.pressure" ) );
        elem = tokenizer.GetNextToken();
        CHECK( elem.IsSameAs( L"environment.water.temperature" ) );
    }
}
TEST_CASE( "Database schema lists" ) {
    SkData *skdata = new SkData();
    wxString url( L"http://localhost:9999" );
    wxString org( L"myboat" );
    wxString token( L"iupP8J7WFVwcOV2Bk1P1nuHrOMYhSVi8whJIQnLGrIhyiobX3LpMhN0bgffEh5av2SQfvpk-9H_UHT2Z15u6zw==" );
    wxString bucket( L"nmea");
    StreamoutSchema schema1;
    schema1.sMeasurement = L"navigation";
    schema1.sProp1       = L"";
    schema1.sProp2       = L"";
    schema1.sProp3       = L"";
    schema1.sField1      = L"speedOverGround";
    schema1.sField2      = L"";
    schema1.sField3      = L"";
    schema1.sSkpathe     = L"navigation.speedOverGround";
    skdata->UpdateStreamoutSchemaList( &url, &org, &token, &bucket, &schema1 );
    StreamoutSchema schema2;
    schema2.sMeasurement = L"chart";
    schema2.sProp1       = L"cursor";
    schema2.sProp2       = L"latitude";
    schema2.sProp3       = L"";
    schema2.sField1      = L"position";
    schema2.sField2      = L"";
    schema2.sField3      = L"";
    schema2.sSkpathe     = L"chart.position.cursor.latitude";
    skdata->UpdateStreamoutSchemaList( &url, &org, &token, &bucket, &schema2 );
    wxString getAllAsJsOrdered = skdata->getAllDbSchemasJsOrderedList();
    wxStringTokenizer tokenizer( getAllAsJsOrdered, "," );
    CHECK( static_cast<int>(tokenizer.CountTokens()) == 2 );
    wxString elem = tokenizer.GetNextToken();
    CHECK( elem.IsSameAs( L"chart.position.cursor.latitude" ) );
    elem = tokenizer.GetNextToken();
    CHECK( elem.IsSameAs( L"navigation.speedOverGround" ) );
    wxString schemaJsStart( L"{\"path\":\"" );
    wxString schemaJsEnd(   L"\"}" );
    wxString searchSchemaPath = "chart.position.cursor.latitude";
    wxString getSchemaAsJs = skdata->getDbSchemaJs( &searchSchemaPath );
    CHECK( getSchemaAsJs.StartsWith( schemaJsStart ) );
    CHECK( getSchemaAsJs.EndsWith(   schemaJsEnd   ) );
    CHECK( getSchemaAsJs.Contains( schema2.sMeasurement ) );
    CHECK( getSchemaAsJs.Contains( schema2.sProp1 ) );
    searchSchemaPath = "navigation.speedOverGround";
    getSchemaAsJs = skdata->getDbSchemaJs( &searchSchemaPath );
    CHECK( getSchemaAsJs.StartsWith( schemaJsStart ) );
    CHECK( getSchemaAsJs.EndsWith(   schemaJsEnd   ) );
    CHECK( getSchemaAsJs.Contains( schema1.sMeasurement ) );
    CHECK( getSchemaAsJs.Contains( schema1.sField1 ) );
}

