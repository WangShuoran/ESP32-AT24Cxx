#include "logging.h"
#include "unity.h"

TEST_CASE("test error", "logging") {
    LOGE("TAG", "ERROR");
}
