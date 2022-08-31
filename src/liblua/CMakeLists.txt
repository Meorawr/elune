include(FetchContent)

FetchContent_Declare(
  utf8h
  GIT_REPOSITORY "https://github.com/sheredom/utf8.h.git"
  GIT_TAG "ce48f0eda6cac8f365837edaf8298ad5c03f7f2e"
)

FetchContent_GetProperties(utf8h)

if(NOT utf8h_POPULATED)
  FetchContent_Populate(utf8h)

  add_library(utf8h INTERFACE IMPORTED GLOBAL)
  add_library(sheredom::utf8h ALIAS utf8h)
  target_include_directories(utf8h INTERFACE ${utf8h_SOURCE_DIR})
endif()

add_library(liblua)
add_library(elune::liblua ALIAS liblua)
elune_configure_target(liblua)

set_target_properties(
  liblua
  PROPERTIES
    DEFINE_SYMBOL "LUA_BUILD_EXPORT"
    OUTPUT_NAME ${LIBLUA_OUTPUT_NAME}
    SOVERSION 5.1.5
)

target_compile_definitions(
  liblua
  PRIVATE
    $<$<IN_LIST:$<CONFIG>,${LUA_ASSERT_CONFIGS}>:LUA_USE_ASSERT>
    $<$<IN_LIST:$<CONFIG>,${LUA_APICHECK_CONFIGS}>:LUA_USE_APICHECK>
)

target_include_directories(
  liblua
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/lua5.1>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/lua5.1>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  liblua
  PRIVATE
    sheredom::utf8h
    $<$<PLATFORM_ID:Darwin,Linux>:m>
    $<$<BOOL:${LUA_USE_BCRYPTGENRANDOM}>:bcrypt>
    $<$<BOOL:${LUA_USE_DL}>:dl>
    $<$<BOOL:${LUA_USE_READLINE}>:readline>
)

elune_target_headers(
  liblua
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/lua5.1/luaconf.h>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/lua5.1/lauxlib.h>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/lua5.1/lua.h>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/lua5.1/lua.hpp>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/lua5.1/lualib.h>
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include/lua5.1/lsyslib.h
)

target_sources(
  liblua
  PRIVATE
    # Core sources
    lapi.c            lapi.h
    lcode.c           lcode.h
    ldebug.c          ldebug.h
    ldo.c             ldo.h
    ldump.c
    lfunc.c           lfunc.h
    lgc.c             lgc.h
    llex.c            llex.h
                      llimits.h
                      lmanip.h
    lmem.c            lmem.h
    lobject.c         lobject.h
    lopcodes.c        lopcodes.h
    lparser.c         lparser.h
    lstate.c          lstate.h
    lstring.c         lstring.h
    lsyslib.c
    ltable.c          ltable.h
    ltm.c             ltm.h
    lundump.c         lundump.h
    lvm.c             lvm.h
    lzio.c            lzio.h

    # Library sources
    lauxlib.c
    lbaselib.c
    lbitlib.c
    lcompatlib.c
    lcorolib.c
    ldblib.c
    liolib.c
    lmathlib.c
    loadlib.c
    loslib.c
    lseclib.c
    lstatslib.c
    lstrlib.c
    ltablib.c
    lwowlib.c
    linit.c
)

if(BUILD_INSTALL)
  install(
    TARGETS liblua
    EXPORT elune-liblua-targets
    RUNTIME
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      COMPONENT Runtime
    LIBRARY
      DESTINATION ${CMAKE_INSTALL_LIBDIR}
      COMPONENT Runtime
    ARCHIVE
      DESTINATION ${CMAKE_INSTALL_LIBDIR}
      COMPONENT Development
    PUBLIC_HEADER
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      COMPONENT Development
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(
    EXPORT elune-liblua-targets
    FILE elune-liblua-targets.cmake
    NAMESPACE elune::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/elune"
    COMPONENT Development
  )
endif()
