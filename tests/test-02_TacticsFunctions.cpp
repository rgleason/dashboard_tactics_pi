#include "doctest.h"

#include <math.h>

#include "TacticsFunctions.h"

TEST_CASE( "testing degress to radians conversions" ) {
    CHECK( deg2rad( 0. ) == 0. );
    CHECK( deg2rad( 90.  ) == 90./180.*M_PI );
    CHECK( deg2rad( 180. ) == 180./180.*M_PI );
    CHECK( deg2rad( 270. ) == 270./180.*M_PI );
    CHECK( deg2rad( 360. ) == 360./180.*M_PI );
}
TEST_CASE( "testing radians to conversions" ) {
    CHECK( rad2deg( 0.) == 0. );
    CHECK( rad2deg( M_PI/2.) == 90. );
    CHECK( rad2deg( M_PI) == 180. );
    CHECK( rad2deg( deg2rad( 270. ) ) == 270. );
    CHECK( rad2deg( 2.*M_PI ) == 360. );
}

TEST_CASE( "Testing rotation direction on quadrants (Q)" ) {
    SUBCASE( "Q1 and Q2" ) {
        wxRealPoint p1(.5, .5);
        wxRealPoint p0(0.,1.);
        wxRealPoint p2(-.5,.5);
        SUBCASE ( "from Q1 to Q2" ) {
            CHECK( tacticsCCW( p1, p0, p2 ) == 1 );
        }
        SUBCASE ( "from Q2 to Q1" ) {
            CHECK( tacticsCCW( p2, p0, p1 ) == -1 );
        }
    }
    SUBCASE( "Q2 and Q3" ) {
        wxRealPoint p1(-.5, .5);
        wxRealPoint p0(-1.,0.);
        wxRealPoint p2(-.5,-.5);
        SUBCASE ( "from Q2 to Q3" ) {
            CHECK( tacticsCCW( p1, p0, p2 ) == 1 );
        }
        SUBCASE ( "from Q3 to Q2" ) {
            CHECK( tacticsCCW( p2, p0, p1 ) == -1 );
        }
    }
    SUBCASE( "Q3 and Q4" ) {
        wxRealPoint p1(-.5, -.5);
        wxRealPoint p0(0.,-1.);
        wxRealPoint p2(.5,-.5);
        SUBCASE ( "from Q3 to Q4" ) {
            CHECK( tacticsCCW( p1, p0, p2 ) == 1 );
        }
        SUBCASE ( "from Q4 to Q3" ) {
            CHECK( tacticsCCW( p2, p0, p1 ) == -1 );
        }
    }
    SUBCASE( "Q4 and Q1" ) {
        wxRealPoint p1(.5, -.5);
        wxRealPoint p0(1.,0.);
        wxRealPoint p2(.5,.5);
        SUBCASE ( "from Q4 to Q1" ) {
            CHECK( tacticsCCW( p1, p0, p2 ) == 1 );
        }
        SUBCASE ( "from Q1 to Q4" ) {
            CHECK( tacticsCCW( p2, p0, p1 ) == -1 );
        }
    }
}

TEST_CASE( "Testing line intersection detection" ) {
    SUBCASE( "No line intersection" ) {
        wxRealPoint p1(.5, .5);
        wxRealPoint p2(0.,1.);
        wxRealPoint p3(.5,-.5);
        wxRealPoint p4(-.5,-.5);
        CHECK( !IsLineIntersect( p1, p2, p3, p4 ) );
    }
    SUBCASE( "Line intersection" ) {
        wxRealPoint p1(.5, .5);
        wxRealPoint p2(-1.,0.);
        wxRealPoint p3(-.5,-.5);
        wxRealPoint p4(0.,1.);
        CHECK( IsLineIntersect( p1, p2, p3, p4 ) );
    }
}

TEST_CASE( "Calculate line intersection point" ) {
    SUBCASE( "No line intersection" ) {
        wxRealPoint p1(.5, .5);
        wxRealPoint p2(0.,1.);
        wxRealPoint p3(.5,-.5);
        wxRealPoint p4(-.5,-.5);
        wxRealPoint er(-999.,-999.);
        CHECK( GetLineIntersection( p1, p2, p3, p4 ) == er );
    }
    SUBCASE( "Line intersection" ) {
        wxRealPoint p1(1., .25);
        wxRealPoint p2(-1., .25);
        wxRealPoint p3(.5,-.5);
        wxRealPoint p4(.5, .5);
        wxRealPoint re(.5, .25);
        CHECK( GetLineIntersection( p1, p2, p3, p4 ) == re );
    }
}

TEST_CASE( "Get True Wind Angle TWA to Course CTM" ) {
    SUBCASE( "Non-360-degree use case, TWA on port side" ) {
        double twd = 170.;
        double ctm = 190.;
        CHECK( getMarkTWA( twd, ctm ) == 20. );
    }
    SUBCASE( "Non-360-degree use case, TWA on starboard side" ) {
        double twd = 190.;
        double ctm = 170.;
        CHECK( getMarkTWA( twd, ctm ) == 20. );
    }
    SUBCASE( "360-degree use case, TWA on port side" ) {
        double twd = 350.;
        double ctm =  20.;
        CHECK( getMarkTWA( twd, ctm ) == 30. );
    }
    SUBCASE( "360-degree use case, TWA on starboard side" ) {
        double twd =  20.;
        double ctm = 350.;
        CHECK( getMarkTWA( twd, ctm ) == 30. );
    }
}

TEST_CASE( "Get True Wind Direction TWD from Heading HDT and True Wind Angle TWA" ) {
    SUBCASE( "Non-360-degree use case, TWA on port side" ) {
        double twa =  40.;
        double hdt = 190.;
        bool   stb = false;
        CHECK( getTWD( twa, hdt, stb ) == 150. );
    }
    SUBCASE( "Non-360-degree use case, TWA on starboard side" ) {
        double twa =  40.;
        double hdt = 170.;
        bool   stb = true;
        CHECK( getTWD( twa, hdt, stb ) == 210. );
    }
    SUBCASE( "360-degree use case, TWA on port side" ) {
        double twa = 40.;
        double hdt = 10.;
        bool   stb = false;
        CHECK( getTWD( twa, hdt, stb ) == 330. );
    }
    SUBCASE( "360-degree use case, TWA on starboard side" ) {
        double twa =  40.;
        double hdt = 330.;
        bool   stb = true;
        CHECK( getTWD( twa, hdt, stb ) == 10. );
    }
}

TEST_CASE( "Get angular degree range without sign" ) {
    SUBCASE( "Non-360-degree use case, min lower angle than max" ) {
        double min = 170.;
        double max = 190.;
        CHECK( getDegRange( max, min ) == 20. );
    }
    SUBCASE( "Non-360-degree use case, min higher angle than max" ) {
        double min = 190.;
        double max = 170.;
        CHECK( getDegRange( max, min ) == 20. );
    }
    SUBCASE( "360-degree use case, min = NE, max NW" ) {
        double min =  10.;
        double max = 350.;
        CHECK( getDegRange( max, min ) == 20. );
    }
    SUBCASE( "360-degree use case, min = NW, max NE" ) {
        double min = 350.;
        double max =  10.;
        CHECK( getDegRange( max, min ) == 20. );
    }
}

TEST_CASE( "Get smallest angular degree range with signed angles where rose turning CW is +" ) {
    SUBCASE( "Non-360-degree use case, fra lower angle than toa" ) {
        double fra = 170.;
        double toa = -170.;
        CHECK( getDegRange( toa, fra ) == 20. );
    }
    SUBCASE( "Non-360-degree use case, fra higher angle than toa" ) {
        double fra = -170.;
        double toa = 170.;
        CHECK( getDegRange( toa, fra ) == 340. );
    }
    SUBCASE( "360-degree use case, fra = NE, toa NW" ) {
        double fra =  10.;
        double toa = -30.;
        CHECK( getDegRange( toa, fra ) == 40. );
    }
    SUBCASE( "360-degree use case, fra = NW, toa NE" ) {
        double fra = -10.;
        double toa =  10.;
        CHECK( getDegRange( toa, fra ) == 20. );
    }
}

