cmake_minimum_required(VERSION 3.21)

project(
  tainted-lua
  DESCRIPTION "Lua 5.1 with support for World of Warcraft-style tainted execution"
  HOMEPAGE_URL "https://github.com/Meorawr/tainted-lua"
  LANGUAGES C
)

include(GNUInstallDirs)

option(TL_BUILD_SHARED_LIBS "Build Lua as a shared library?" ${BUILD_SHARED_LIBS})
option(TL_BUILD_TESTS "Build tests?" ${PROJECT_IS_TOP_LEVEL})
option(TL_BUILD_INSTALL "Generate installation rules for build targets?" ${PROJECT_IS_TOP_LEVEL})
option(TL_BUILD_PACKAGE "Generate packaging rules for build targets?" ${PROJECT_IS_TOP_LEVEL})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(TL_LIBLUA_OUTPUT_NAME_INIT "lua51")
  set(TL_LUA_OUTPUT_NAME_INIT "lua")
  set(TL_LUAC_OUTPUT_NAME_INIT "luac")
else()
  set(TL_LIBLUA_OUTPUT_NAME_INIT "lua5.1")
  set(TL_LUA_OUTPUT_NAME_INIT "lua5.1")
  set(TL_LUAC_OUTPUT_NAME_INIT "luac5.1")
endif()

set(TL_LIBLUA_OUTPUT_NAME ${TL_LIBLUA_OUTPUT_NAME_INIT} CACHE STRING "Output name of the Lua library target, without any prefixes or extensions")
set(TL_LUA_OUTPUT_NAME ${TL_LUA_OUTPUT_NAME_INIT} CACHE STRING "Output name of the Lua interpreter target, without any prefixes or extensions")
set(TL_LUAC_OUTPUT_NAME ${TL_LUAC_OUTPUT_NAME_INIT} CACHE STRING "Output name of the Lua compiler target, without any prefixes or extensions")

set(BUILD_SHARED_LIBS ${TL_BUILD_SHARED_LIBS})

add_subdirectory(ports/utf8)
add_subdirectory(src)

if(TL_BUILD_TESTS)
  add_subdirectory(ports/acutest)
  add_subdirectory(tests)
endif()

if(TL_BUILD_INSTALL)
  install(
    TARGETS
      tainted-liblua
      tainted-lua
      tainted-luac
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    OPTIONAL
  )
endif()

if(TL_BUILD_PACKAGE)
  set(CPACK_PACKAGE_VENDOR "Meorawr")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
  string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CPACK_SYSTEM_NAME)
  set(CPACK_GENERATOR "TXZ;ZIP")
  set(CPACK_STRIP_FILES ON)

  include(CPack)
endif()
