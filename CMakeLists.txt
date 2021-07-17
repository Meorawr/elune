cmake_minimum_required(VERSION 3.20)

option(BUILD_SHARED_LIBS "Build Lua as a shared library?" ON)

project(tainted-lua
  VERSION "5.1.5"
  DESCRIPTION "Lua 5.1 with support for World of Warcraft-style tainted execution"
  HOMEPAGE_URL "https://github.com/Meorawr/tainted-lua"
  LANGUAGES C
)

add_subdirectory(src)

# Packaging integration.

set(CPACK_PACKAGE_VENDOR "Meorawr")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_GENERATOR "TXZ;ZIP")

include(CPack)
