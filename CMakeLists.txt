cmake_minimum_required(VERSION 3.22)

project(
  elune
  VERSION 2.1
  DESCRIPTION "Customized Lua 5.1 distribution aimed at replicating the World of Warcraft addon environment"
  HOMEPAGE_URL "https://github.com/Meorawr/elune"
  LANGUAGES C
)

list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CheckTypeSize)
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

include(elune-configure-target)
include(elune-target-copy-file)
include(elune-target-headers)

#
# Project Configuration
#

find_package(readline)

option(BUILD_SHARED_LIBS ON "Build components as shared libraries?")
option(BUILD_TESTING "Build test executables?" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_INSTALL "Enable the generation of installation targets?" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_PACKAGE "Enable the generation of packaging targets?" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_SUMMARY "Print a summary of configuration details?" ${PROJECT_IS_TOP_LEVEL})

option(BUILD_CXX "Build with a C++ compiler?" OFF)
option(BUILD_LUA "Build the Lua interpreter target?" ON)
option(BUILD_LUAC "Build the Lua compiler target?" ON)

set(LUA_ASSERT_CONFIGS "Debug" CACHE STRING "List of build configurations where Lua assertions are enabled")
set(LUA_APICHECK_CONFIGS "Debug" CACHE STRING "List of build configurations where Lua API checks are enabled")

cmake_dependent_option(LUA_USE_CXX_LINKAGE "Build the Lua interface with C++ linkage?" ON "BUILD_CXX" OFF)
cmake_dependent_option(LUA_USE_CXX_EXCEPTIONS "Allow the use of C++ exceptions for error handling?" ON "BUILD_CXX" OFF)
cmake_dependent_option(LUA_USE_READLINE "Allow linking to 'libreadline' for the interpreter and debug library?" ON "TARGET readline::readline" OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(LUA_ROOT_INIT "!")
  set(LUA_LDIR_INIT "\\lua")
  set(LUA_CDIR_INIT "")
  set(LUA_DIRSEP "\\")
else()
  set(LUA_ROOT_INIT "/usr/local")
  set(LUA_LDIR_INIT "/${CMAKE_INSTALL_DATADIR}/lua/5.1")
  set(LUA_CDIR_INIT "/${CMAKE_INSTALL_LIBDIR}/lua/5.1")
  set(LUA_DIRSEP "/")
endif()

set(LUA_ROOT "${LUA_ROOT_INIT}" CACHE STRING "Base path prefix from which Lua will search for Lua and C libraries")
set(LUA_CDIR "${LUA_CDIR_INIT}" CACHE STRING "Subdirectory within LUA_ROOT from which Lua will search for C libraries")
set(LUA_LDIR "${LUA_LDIR_INIT}" CACHE STRING "Subdirectory within LUA_ROOT from which Lua will search for Lua libraries")
set(LUA_CPATH_CUSTOM "" CACHE STRING "Custom default search path for C libraries")
set(LUA_PATH_CUSTOM "" CACHE STRING "Custom default search path for Lua libraries")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(LIBLUA_OUTPUT_NAME_INIT "lua51")
  set(LUA_OUTPUT_NAME_INIT "lua")
  set(LUAC_OUTPUT_NAME_INIT "luac")
else()
  set(LIBLUA_OUTPUT_NAME_INIT "lua5.1")
  set(LUA_OUTPUT_NAME_INIT "lua5.1")
  set(LUAC_OUTPUT_NAME_INIT "luac5.1")
endif()

set(LIBLUA_OUTPUT_NAME "${LIBLUA_OUTPUT_NAME_INIT}" CACHE STRING "Base output name of the Lua library target")
set(LUA_OUTPUT_NAME "${LUA_OUTPUT_NAME_INIT}" CACHE STRING "Base output name of the Lua interpreter target")
set(LUAC_OUTPUT_NAME "${LUAC_OUTPUT_NAME_INIT}" CACHE STRING "Base output name of the Lua compiler target")

#
# Config Header Generation
#

if(BUILD_CXX)
  enable_language(CXX)
endif()

# Configure detected platform.

set(LUA_USE_WINDOWS OFF)
set(LUA_USE_LINUX OFF)
set(LUA_USE_POSIX OFF)
set(LUA_USE_MACOS OFF)
set(LUA_USE_SHARED ${BUILD_SHARED_LIBS})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(LUA_USE_WINDOWS ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(LUA_USE_LINUX ON)
  set(LUA_USE_POSIX ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(LUA_USE_MACOS ON)
  set(LUA_USE_POSIX ON)
endif()

# Detect bit-width of integers on this platform.

set(CMAKE_EXTRA_INCLUDE_FILES "limits.h")
check_type_size("char[sizeof(int) * CHAR_BIT]" LUAI_BITSINT)
set(CMAKE_EXTRA_INCLUDE_FILES)

# Configure library search paths.

if(LUA_PATH_CUSTOM)
  set(LUA_PATH_DEFAULT ${LUA_PATH_CUSTOM})
else()
  set(LUA_PATH_DEFAULT)
  list(APPEND LUA_PATH_DEFAULT ".${LUA_DIRSEP}?.lua")
  list(APPEND LUA_PATH_DEFAULT "${LUA_ROOT}${LUA_LDIR}${LUA_DIRSEP}?.lua")
  list(APPEND LUA_PATH_DEFAULT "${LUA_ROOT}${LUA_LDIR}${LUA_DIRSEP}?${LUA_DIRSEP}init.lua")
  list(APPEND LUA_PATH_DEFAULT "${LUA_ROOT}${LUA_CDIR}${LUA_DIRSEP}?.lua")
  list(APPEND LUA_PATH_DEFAULT "${LUA_ROOT}${LUA_CDIR}${LUA_DIRSEP}?${LUA_DIRSEP}init.lua")
endif()

if(LUA_CPATH_CUSTOM)
  set(LUA_CPATH_DEFAULT ${LUA_CPATH_CUSTOM})
else()
  set(LUA_CPATH_DEFAULT)
  list(APPEND LUA_CPATH_DEFAULT ".${LUA_DIRSEP}?${CMAKE_SHARED_LIBRARY_SUFFIX}")
  list(APPEND LUA_CPATH_DEFAULT "${LUA_ROOT}${LUA_CDIR}${LUA_DIRSEP}?${CMAKE_SHARED_LIBRARY_SUFFIX}")
  list(APPEND LUA_CPATH_DEFAULT "${LUA_ROOT}${LUA_CDIR}${LUA_DIRSEP}loadall${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

# The Lua search paths are interpolated into a header file as strings; ensure
# any backslashes (such as directory separators) are appropriately escaped.
#
# For added insanity - the "list(TRANSFORM var REPLACE ...)" call is regex
# based - so we need to escape the slashes here as well!

string(REPLACE "\\" "\\\\" LUA_DIRSEP_ESCAPED ${LUA_DIRSEP})
list(TRANSFORM LUA_PATH_DEFAULT REPLACE "\\\\" "\\\\\\\\" OUTPUT_VARIABLE LUA_PATH_ESCAPED)
list(TRANSFORM LUA_CPATH_DEFAULT REPLACE "\\\\" "\\\\\\\\" OUTPUT_VARIABLE LUA_CPATH_ESCAPED)

# Generate the configuration header; this is used implicitly by multiple targets
# so needs to be done early and can't really be skipped.

configure_file(
  "${PROJECT_SOURCE_DIR}/liblua/include/luaconf.h.in"
  "${PROJECT_BINARY_DIR}/liblua/include/luaconf.h"
  USE_SOURCE_PERMISSIONS
  ESCAPE_QUOTES
)

#
# Project Targets
#

add_subdirectory(liblua)

if(BUILD_LUA)
  add_subdirectory(lua)
endif()

if(BUILD_LUAC)
  add_subdirectory(luac)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if(BUILD_INSTALL)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/elune-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion
  )

  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/elune-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/elune-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/elune"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )

  install(
    FILES
      "${CMAKE_CURRENT_BINARY_DIR}/elune-config-version.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/elune-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/elune"
    COMPONENT Development
  )
endif()

if(BUILD_INSTALL AND BUILD_PACKAGE)
  set(CPACK_PACKAGE_VENDOR "Meorawr")
  string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CPACK_SYSTEM_NAME)
  set(CPACK_GENERATOR "TXZ;ZIP")
  set(CPACK_STRIP_FILES ON)

  include(CPack)
endif()

if(BUILD_SUMMARY)
  message(STATUS "")
  message(STATUS "Elune has been configured with the following options:")
  message(STATUS "")
  message(STATUS "Platform:           ${CMAKE_SYSTEM_NAME}")
  message(STATUS "Architecture:       ${CMAKE_SYSTEM_PROCESSOR}")
  message(STATUS "Compiler:           ${CMAKE_C_COMPILER}")
  message(STATUS "Lua library path:   ${LUA_PATH_DEFAULT}")
  message(STATUS "Lua C library path: ${LUA_CPATH_DEFAULT}")

  get_property(_variable_names DIRECTORY PROPERTY CACHE_VARIABLES)
  list(SORT _variable_names)

  set(_enabled_features)

  foreach(_name IN LISTS _variable_names)
    if(_name MATCHES "^LUA_USE_")
      if(${${_name}})
        list(APPEND _enabled_features ${_name})
      endif()
    endif()
  endforeach()

  list(SORT _enabled_features)

  if(_enabled_features)
    message(STATUS "")
    message(STATUS "Configured Features:")

    foreach(_name IN LISTS _enabled_features)
      message(STATUS " * ${_name}")
    endforeach()
  endif()

  message(STATUS "")
endif()
