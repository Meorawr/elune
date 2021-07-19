cmake_minimum_required(VERSION 3.20)

option(BUILD_SHARED_LIBS "Build Lua as a shared library?" ON)

# Restrict build configurations to debug or release. For all generators we'll
# aim to default to doing release builds if otherwise unspecified.

get_cmake_property(GENERATOR_IS_MULTI_CONFIG GENERATOR_IS_MULTI_CONFIG)
set(CMAKE_CONFIGURATION_TYPES "debug;release" CACHE STRING "List of supported configuration types for building")

if(CMAKE_GENERATOR MATCHES "Ninja" AND GENERATOR_IS_MULTI_CONFIG)
  set(CMAKE_CROSS_CONFIGS "all" CACHE STRING "Configurations available from all build trees")
  set(CMAKE_DEFAULT_BUILD_TYPE  "release" CACHE STRING "Default configuration to build if unspecified")
  set(CMAKE_DEFAULT_CONFIGS "release" CACHE STRING "Default configurations to build if unspecified")
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "release" CACHE STRING "Configuration to build in")
endif()

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
set(CPACK_GENERATOR "TXZ;ZIP")
set(CPACK_STRIP_FILES ON)

include(CPack)
