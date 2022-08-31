cmake_minimum_required(VERSION 3.22)

project(
  Elune
  VERSION 2
  DESCRIPTION "Customized Lua 5.1 distribution aimed at replicating the World of Warcraft addon environment"
  HOMEPAGE_URL "https://github.com/Meorawr/elune"
  LANGUAGES C
)

list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

include(elune-configure-target)
include(elune-target-copy-file)
include(elune-target-public-headers)

option(BUILD_SHARED_LIBS "Build Lua as a shared library?" ON)
option(BUILD_TESTING "Build tests?" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_INSTALL "Generate installation rules for build targets?" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_PACKAGE "Generate packaging rules for build targets?" ${PROJECT_IS_TOP_LEVEL})

option(BUILD_LUA "Build the Lua interpreter target?" ON)
option(BUILD_LUAC "Build the Lua compiler target?" ON)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(LIBLUA_OUTPUT_NAME_INIT "lua51")
  set(LUA_OUTPUT_NAME_INIT "lua")
  set(LUAC_OUTPUT_NAME_INIT "luac")
else()
  set(LIBLUA_OUTPUT_NAME_INIT "lua5.1")
  set(LUA_OUTPUT_NAME_INIT "lua5.1")
  set(LUAC_OUTPUT_NAME_INIT "luac5.1")
endif()

set(LIBLUA_OUTPUT_NAME ${LIBLUA_OUTPUT_NAME_INIT} CACHE STRING "Output name of the Lua library target, without any prefixes or extensions")
set(LUA_OUTPUT_NAME ${LUA_OUTPUT_NAME_INIT} CACHE STRING "Output name of the Lua interpreter target, without any prefixes or extensions")
set(LUAC_OUTPUT_NAME ${LUAC_OUTPUT_NAME_INIT} CACHE STRING "Output name of the Lua compiler target, without any prefixes or extensions")

#
# Project Targets
#

add_subdirectory(src/liblua)

if(BUILD_LUA)
  add_subdirectory(src/lua)
endif()

if(BUILD_LUAC)
  add_subdirectory(src/luac)
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
