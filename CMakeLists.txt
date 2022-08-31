cmake_minimum_required(VERSION 3.21)

project(
  elune
  DESCRIPTION "Customized Lua 5.1 distribution aimed at replicating the World of Warcraft addon environment"
  HOMEPAGE_URL "https://github.com/Meorawr/elune"
  LANGUAGES C
)

include(GNUInstallDirs)

option(BUILD_SHARED_LIBS "Build Lua as a shared library?" ON)
option(BUILD_TESTING "Build tests?" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_INSTALL "Generate installation rules for build targets?" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_PACKAGE "Generate packaging rules for build targets?" ${PROJECT_IS_TOP_LEVEL})

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

add_subdirectory(ports/utf8)
add_subdirectory(src/liblua)
add_subdirectory(src/lua)
add_subdirectory(src/luac)

if(BUILD_TESTING)
  add_subdirectory(ports/acutest)
  add_subdirectory(tests)
endif()

if(BUILD_INSTALL)
  install(
    TARGETS
      liblua
      lua
      luac
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    OPTIONAL
  )
endif()

if(BUILD_INSTALL AND BUILD_PACKAGE)
  set(CPACK_PACKAGE_VENDOR "Meorawr")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
  string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CPACK_SYSTEM_NAME)
  set(CPACK_GENERATOR "TXZ;ZIP")
  set(CPACK_STRIP_FILES ON)

  include(CPack)
endif()
