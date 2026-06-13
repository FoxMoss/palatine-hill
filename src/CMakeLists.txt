add_executable(palatine
 main.cpp
 error.cpp
 dbclient.cpp
 curlutil.cpp
)

include(FetchContent)
set(OATPP_BUILD_TESTS OFF)
FetchContent_Declare(
  oatpp
  GIT_REPOSITORY https://github.com/oatpp/oatpp/
  GIT_TAG "f83d648fd82dc222ef88aabbafb68efbd7d7bf50"
)
FetchContent_MakeAvailable(oatpp)

list(APPEND CMAKE_PREFIX_PATH ${oatpp_BUILD_DIR}/src/)

set(OATPP_MODULES_LOCATION "EXTERNAL")
FetchContent_Declare(
  oatpp-sqlite
  GIT_REPOSITORY https://github.com/oatpp/oatpp-sqlite/
  GIT_TAG "b84df87f1119660f27ccc7aa1ae7fa9f240c5377"
)
FetchContent_MakeAvailable(oatpp-sqlite)

CPMAddPackage(
  URI "gh:curl/curl#curl-8_20_0"
  OPTIONS "BUILD_STATIC_LIBS ON"
          "BUILD_SHARED_LIBS OFF"
          "CURL_USE_OPENSSL OFF"
          "CURL_USE_WOLFSSL OFF"
          "CURL_USE_BEARSSL OFF"
          "CURL_USE_MBEDTLS ON"
          "CURL_USE_LIBPSL OFF"
          "CURL_USE_LIBSSH2 OFF"
          "CURL_ZSTD OFF"
          "CURL_BROTLI OFF"
          "USE_NGHTTP2 OFF"
          "USE_LIBIDN2 OFF"
          "USE_LIBIDN2 OFF"
          "CURL_ZLIB OFF"
)

FetchContent_Declare(json 
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)

target_link_libraries(palatine oatpp oatpp-sqlite CURL::libcurl_static nlohmann_json::nlohmann_json)

add_custom_command(TARGET palatine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Add clangd to the base directory")

install(TARGETS palatine DESTINATION bin)
