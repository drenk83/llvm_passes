add_library(uafml MODULE
    uafmlAnalysis.cpp
    uafmlPlugin.cpp)

target_include_directories(uafml PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")