cmake_minimum_required(VERSION 3.20.0)
project(hash VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(PkgConfig REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(${PROJECT_NAME} main.c)

if( OPENSSL_FOUND )
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()

target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
