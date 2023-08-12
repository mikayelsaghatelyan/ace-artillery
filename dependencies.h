#ifndef ACE_ARTILLERY1_0_DEPENDENCIES_H
#define ACE_ARTILLERY1_0_DEPENDENCIES_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <cmath>
#include <clocale>
#include <iostream>
#include <iomanip>
#include <random>
#include <numbers>
#include <functional>
#include <fstream>
#include <unistd.h>
#include <tuple>
#include <memory>
#include <deque>
#include <algorithm>
#include "libs/rapidcsv.h"
#include "libs/csv.h"
#include "Mil.h"
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace fs = std::filesystem;

namespace constants {
    constexpr double pi = 3.14159265358979323846;
    constexpr double inf = 999999999999999999999.;
}

#endif
