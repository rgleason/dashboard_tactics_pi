#include "doctest.h"
#include <unistd.h>

TEST_CASE("sleep a second") {
    CHECK(sleep(1) == 0);
}
