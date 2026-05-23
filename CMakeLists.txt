cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(CMAKE_BUILD_TYPE EQUAL "Release")
  add_compile_definitions(PALATINE_PROD=true)
else()
  add_compile_definitions(PALATINE_PROD=false)
endif()

project(PalatineBackend)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
