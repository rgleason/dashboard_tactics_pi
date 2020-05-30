#include "doctest.h"

#include "SkData.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/string.h"

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
    SUBCASE ( "Default paths and copy constructor" ) {
        wxJSONValue pRetJSON;
        pRetJSON["context"] = L"vessels.self";
        wxString defSchemas = skdata->getAllSubscriptionsJSON( pRetJSON );
        CHECK( !defSchemas.IsEmpty() );
        CHECK( pRetJSON.HasMember("context") );
        // wxJSONValue pSchema = pRetJSON["environment"];
        // CHECK( pSchema.IsArray() );
        // CHECK( pSchema.Size() == 8 );
        // SkData *copyskdata = new SkData( (*skdata) );
        // wxJSONValue pCopyRetJSON( wxJSONTYPE_NULL );
        // wxString defCopySchemas = copyskdata->getAllSubscriptionsJSON( pCopyRetJSON );
        // CHECK( pCopyRetJSON.Size() == 24 );
    }
}

