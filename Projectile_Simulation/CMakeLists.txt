cmake_minimum_required(VERSION 3.20)
project(Projectile_Simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(projectile_app
  src/main.cpp
  src/ProjectileSim.cpp
)

target_include_directories(projectile_app PRIVATE src)

target_link_libraries(projectile_app PRIVATE
  SFML::Graphics
  SFML::Window
  SFML::System
)
