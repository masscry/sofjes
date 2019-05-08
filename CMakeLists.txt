cmake_minimum_required(VERSION 3.9)

project(sofjes CXX)

include(CheckIPOSupported)
check_ipo_supported(RESULT LTO_SUPPORT OUTPUT LTO_ERROR_TEXT)

if(LTO_SUPPORT)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "IPO / LTO enabled")
else(LTO_SUPPORT)
    message(STATUS "IPO / LTO not supported: <${LTO_ERROR_TEXT}>")
endif(LTO_SUPPORT)

set(CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED TRUE)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options (-fcolor-diagnostics)
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

find_package(SDL2 REQUIRED)

add_executable(sofjes
  include/world.h
  include/targa.h
  include/texture_t.h
  include/vecmath.h
  src/entry.cpp
  src/world.cpp
  src/targa.cpp
  src/texture_t.cpp
)

target_include_directories(sofjes PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(sofjes PRIVATE ./include)
target_link_libraries(sofjes PUBLIC ${SDL2_LIBRARIES})