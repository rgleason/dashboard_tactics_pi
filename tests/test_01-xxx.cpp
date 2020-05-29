#include "doctest.h"

#include <unistd.h>


SCENARIO("sleep a second") {

    GIVEN("a one second sleep duration") {
        unsigned int duration =1 ;

        WHEN("call sleep with this duration") {
            int ret = sleep(duration) ;

            THEN("it's expected nobody interupted our rest") {
                CHECK(ret == 0);
            }
        }
    }
}
