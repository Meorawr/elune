cmake_minimum_required(VERSION 3.21)

project(tainted-lua
  DESCRIPTION "Lua 5.1 with support for World of Warcraft-style tainted execution"
  HOMEPAGE_URL "https://github.com/Meorawr/tainted-lua"
  LANGUAGES C
)

option(BUILD_SHARED_LIBS "Build Lua as a shared library?" ON)
option(BUILD_TESTING "Build tests?" ON)

add_subdirectory(ports/acutest)
add_subdirectory(ports/utf8)
add_subdirectory(src)
add_subdirectory(tests)

set(CPACK_PACKAGE_VENDOR "Meorawr")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CPACK_SYSTEM_NAME)
set(CPACK_GENERATOR "TXZ;ZIP")
set(CPACK_STRIP_FILES ON)

include(CPack)
