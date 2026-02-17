#pragma once

#include <chrono>

namespace ChronoUtility {

using LocalTimeSeconds = std::chrono::time_point<std::chrono::local_t, std::chrono::seconds>;

LocalTimeSeconds NowLocalSecond();

};
