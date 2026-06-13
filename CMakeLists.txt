cmake_minimum_required(VERSION ${CMAKE_VERSION})
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  add_compile_definitions(PALATINE_PROD=true)
  add_compile_options("-O3")
else()
  add_compile_definitions(PALATINE_PROD=false)
endif()

project(PalatineBackend)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)
set(CPM_SOURCE_CACHE ${CMAKE_BINARY_DIR}/cpmcache)

add_subdirectory(src)
