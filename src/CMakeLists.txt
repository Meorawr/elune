include(CheckSymbolExists)

check_symbol_exists(clock_gettime "time.h" LUA_USE_CLOCK_GETTIME)
check_symbol_exists(CLOCK_MONOTONIC "time.h" LUA_USE_CLOCK_MONOTONIC)
check_symbol_exists(CLOCK_MONOTONIC_RAW "time.h" LUA_USE_CLOCK_MONOTONIC_RAW)
check_symbol_exists(QueryPerformanceCounter "windows.h" LUA_USE_QUERY_PERFORMANCE_COUNTER)

set(CMAKE_REQUIRED_LIBRARIES "bcrypt")
check_symbol_exists(BCryptGenRandom "windows.h;bcrypt.h" LUA_USE_BCRYPT_GEN_RANDOM)
set(CMAKE_REQUIRED_LIBRARIES)

# -----------------------------------------------------------------------------
# Lua library target (elune::liblua)
# -----------------------------------------------------------------------------

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

set(liblua_PUBLIC_HEADERS lua.h lauxlib.h lualib.h luaconf.h lua.hpp)

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

# -----------------------------------------------------------------------------
# Lua interpreter target (elune::lua)
# -----------------------------------------------------------------------------

add_executable(lua)
add_executable(elune::lua ALIAS lua)

set_target_properties(
  lua
  PROPERTIES
    OUTPUT_NAME ${LUA_OUTPUT_NAME}
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    BUILD_RPATH_USE_ORIGIN ON
    INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
)

target_link_libraries(
  lua
  PRIVATE
    elune::liblua
    $<$<PLATFORM_ID:Darwin,Linux>:readline>
)

target_sources(
  lua
  PRIVATE
    lua.c
)

# -----------------------------------------------------------------------------
# Lua compiler target (elune::luac)
# -----------------------------------------------------------------------------

add_executable(luac)
add_executable(elune::luac ALIAS luac)

set_target_properties(
  luac
  PROPERTIES
    OUTPUT_NAME ${LUAC_OUTPUT_NAME}
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

target_link_libraries(
  luac
  PRIVATE
    $<TARGET_GENEX_EVAL:liblua,$<TARGET_PROPERTY:liblua,LINK_LIBRARIES>>
)

target_sources(
  luac
  PRIVATE
    luac.c
    print.c
    $<TARGET_OBJECTS:liblua>
)
