cmake_minimum_required(VERSION 3.5)

project(TclCaplParserCMake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Core REQUIRED)

file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE headers src/*.hpp)

add_executable(TclCaplParserCMake
    src/TCL_CAPL_Parser/main.cpp
    ${sources}
    ${headers}
    )

target_include_directories(TclCaplParserCMake
    PRIVATE

    )
include_directories(src)

target_link_libraries(TclCaplParserCMake
    PRIVATE
    Qt6::Widgets
    Qt6::Core
    )
