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

file(GLOB_RECURSE dirsAndFiles src/*)
set(includeDirs "")
foreach(item ${dirsAndFiles})
    if(IS_DIRECTORY ${item})
        list(APPEND includeDirs ${item})
    endif()
endforeach()


add_executable(TclCaplParserCMake
    ${sources}
    ${headers}
    )

target_include_directories(TclCaplParserCMake
    PRIVATE

    )
include_directories(src ${includeDirs})

target_link_libraries(TclCaplParserCMake
    PRIVATE
    Qt6::Widgets
    Qt6::Core
    )

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET TclCaplParserCMake PROPERTY WIN32_EXECUTABLE true)
endif()
