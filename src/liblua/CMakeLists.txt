include(CheckSymbolExists)

check_symbol_exists(clock_gettime "time.h" LUA_USE_CLOCK_GETTIME)
check_symbol_exists(CLOCK_MONOTONIC "time.h" LUA_USE_CLOCK_MONOTONIC)
check_symbol_exists(CLOCK_MONOTONIC_RAW "time.h" LUA_USE_CLOCK_MONOTONIC_RAW)
check_symbol_exists(QueryPerformanceCounter "windows.h" LUA_USE_QUERY_PERFORMANCE_COUNTER)

set(CMAKE_REQUIRED_LIBRARIES "bcrypt")
check_symbol_exists(BCryptGenRandom "windows.h;bcrypt.h" LUA_USE_BCRYPT_GEN_RANDOM)
set(CMAKE_REQUIRED_LIBRARIES)

add_library(liblua)
add_library(elune::liblua ALIAS liblua)

set_target_properties(
  liblua
  PROPERTIES
    OUTPUT_NAME ${LIBLUA_OUTPUT_NAME}
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

target_compile_definitions(
  liblua
  PUBLIC
    $<$<PLATFORM_ID:Linux>:LUA_USE_LINUX>
    $<$<PLATFORM_ID:Darwin>:LUA_USE_MACOSX>
    $<$<PLATFORM_ID:Windows>:LUA_USE_WINDOWS>
    $<$<AND:$<PLATFORM_ID:Windows>,$<STREQUAL:$<TARGET_PROPERTY:liblua,TYPE>,SHARED_LIBRARY>>:LUA_BUILD_AS_DLL>
  PRIVATE
    $<$<CONFIG:Debug>:LUA_USE_ASSERT>     # Enable internal assertions on debug builds.
    $<$<CONFIG:Debug>:LUA_USE_APICHECK>   # Enable public API assertions on debug builds.
    $<$<BOOL:${LUA_USE_CLOCK_GETTIME}>:LUA_USE_CLOCK_GETTIME>
    $<$<BOOL:${LUA_USE_CLOCK_MONOTONIC_RAW}>:LUA_USE_CLOCK_MONOTONIC_RAW>
    $<$<BOOL:${LUA_USE_CLOCK_MONOTONIC}>:LUA_USE_CLOCK_MONOTONIC>
    $<$<BOOL:${LUA_USE_BCRYPT_GEN_RANDOM}>:LUA_USE_BCRYPT_GEN_RANDOM>
    $<$<BOOL:${LUA_USE_QUERY_PERFORMANCE_COUNTER}>:LUA_USE_QUERY_PERFORMANCE_COUNTER>
)

target_include_directories(
  liblua
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/lua5.1>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  liblua
  PRIVATE
    $<$<PLATFORM_ID:Darwin,Linux>:dl>
    $<$<PLATFORM_ID:Darwin,Linux>:m>
    $<$<BOOL:${LUA_USE_BCRYPT_GEN_RANDOM}>:bcrypt>
    sheredom::utf8h
)

set(
  liblua_PUBLIC_HEADERS
  "${PROJECT_SOURCE_DIR}/include/lua5.1/lauxlib.h"
  "${PROJECT_SOURCE_DIR}/include/lua5.1/lua.h"
  "${PROJECT_SOURCE_DIR}/include/lua5.1/lua.hpp"
  "${PROJECT_SOURCE_DIR}/include/lua5.1/luaconf.h"
  "${PROJECT_SOURCE_DIR}/include/lua5.1/lualib.h"
)

target_sources(
  liblua
  PRIVATE
    # Public headers
    ${liblua_PUBLIC_HEADERS}

    # Core sources
    lapi.c        lapi.h
    lcode.c       lcode.h
    ldebug.c      ldebug.h
    ldo.c         ldo.h
    ldump.c
    lfunc.c       lfunc.h
    lgc.c         lgc.h
    llex.c        llex.h
    llimits.h
    lmanip.h
    lmem.c        lmem.h
    lobject.c     lobject.h
    lopcodes.c    lopcodes.h
    lparser.c     lparser.h
    lstate.c      lstate.h
    lstring.c     lstring.h
    ltable.c      ltable.h
    ltm.c         ltm.h
    lundump.c     lundump.h
    lvm.c         lvm.h
    lzio.c        lzio.h

    # Library sources
    lauxlib.c
    lbaselib.c
    lbitlib.c
    lcompatlib.c
    ldblib.c
    liolib.c
    lmathlib.c
    loadlib.c
    loslib.c
    lseclib.c
    lstatslib.c
    lstrlib.c
    ltablib.c
    linit.c
)

source_group(
  TREE    ${PROJECT_SOURCE_DIR}
  PREFIX  "Header Files"
  FILES   ${liblua_PUBLIC_HEADERS}
)

set_target_properties(
  liblua
  PROPERTIES
    PUBLIC_HEADER "${liblua_PUBLIC_HEADERS}"  # Must be a string representation of a list.
)
