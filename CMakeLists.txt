cmake_minimum_required(VERSION 3.24)
project(ace_artillery1_0)

set(CMAKE_CXX_STANDARD 23)

add_executable(ace_artillery1_0 main.cpp Gun.h Data.cpp Data.h dependencies.h obsolete/obsolete.h libs/rapidcsv.h libs/csv.h Gun.cpp Mil.cpp Mil.h Process.cpp Process.h)

add_subdirectory(libs/json)
include_directories(libs/json/include)
target_link_libraries(ace_artillery1_0 PRIVATE nlohmann_json::nlohmann_json)
