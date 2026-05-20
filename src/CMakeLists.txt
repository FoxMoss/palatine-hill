add_executable(palatine
 main.cpp
 error.cpp
)


include(FetchContent)
set(OATPP_BUILD_TESTS OFF)
FetchContent_Declare(
  oatpp
  GIT_REPOSITORY https://github.com/oatpp/oatpp/
  GIT_TAG "f83d648fd82dc222ef88aabbafb68efbd7d7bf50"
)
FetchContent_MakeAvailable(oatpp)
set(OATPP_MODULES_LOCATION "EXTERNAL")
FetchContent_Declare(
  oatpp-sqlite
  GIT_REPOSITORY https://github.com/oatpp/oatpp-sqlite/
  GIT_TAG "b84df87f1119660f27ccc7aa1ae7fa9f240c5377"
)
FetchContent_MakeAvailable(oatpp-sqlite)

target_link_libraries(palatine oatpp oatpp-sqlite)

add_compile_options(-ggdb)

add_custom_command(TARGET palatine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Add clangd to the base directory")

install(TARGETS palatine DESTINATION bin)
