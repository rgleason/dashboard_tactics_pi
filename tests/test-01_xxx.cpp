#include "doctest.h"

#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

SCENARIO("sleep a second") {

    GIVEN("a one second sleep duration") {

        WHEN("call sleep with this duration") {

            chrono::system_clock::time_point timePt =
                chrono::system_clock::now() + chrono::seconds(60);
            this_thread::sleep_until(timePt);
            chrono::system_clock::time_point timeAwaken =
                chrono::system_clock::now();
            
            THEN("it's expected nobody interupted our rest") {
                CHECK(timeAwaken >= timePt);
            }
        }
    }
}
