#include "doctest.h"

#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

TEST_CASE("sleep a second") {
    chrono::system_clock::time_point timePt =
        chrono::system_clock::now() + chrono::seconds(1);
    this_thread::sleep_until(timePt);
    chrono::system_clock::time_point timeAwaken =
        chrono::system_clock::now();
    CHECK(timeAwaken >= timePt);
}
