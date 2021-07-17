cmake_minimum_required(VERSION 3.20)

option(BUILD_SHARED_LIBS "Build Lua as a shared library?" ON)

project(tainted-lua
  VERSION "1"
  DESCRIPTION "Lua 5.1 with support for World of Warcraft-style tainted execution"
  HOMEPAGE_URL "https://github.com/Meorawr/tainted-lua"
  LANGUAGES C
)

add_subdirectory(src)

# Packaging integration.

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LUA_PACKAGE_SYSTEM_NAME "win64")
  else()
    set(LUA_PACKAGE_SYSTEM_NAME "win32")
  endif()
else()
  string(TOLOWER ${CMAKE_SYSTEM_NAME} LUA_PACKAGE_SYSTEM_NAME)
endif()

set(CPACK_PACKAGE_VENDOR "Meorawr")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-v${PROJECT_VERSION}-${LUA_PACKAGE_SYSTEM_NAME}")
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_GENERATOR "TXZ;ZIP")
set(CPACK_STRIP_FILES ON)

include(CPack)
