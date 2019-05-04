cmake_minimum_required(VERSION 3.5)

project(sofjes CXX)

find_package(SDL2 REQUIRED)

add_executable(sofjes
  src/entry.cpp
  src/world.cpp
  resources/bricks.cpp
)

target_include_directories(sofjes PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(sofjes PRIVATE ./include)
target_link_libraries(sofjes PUBLIC ${SDL2_LIBRARIES})