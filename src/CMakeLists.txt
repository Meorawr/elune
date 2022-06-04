# -----------------------------------------------------------------------------
# Lua library target (meorawr::tainted-liblua)
# -----------------------------------------------------------------------------

add_library(tainted-liblua)
add_library(meorawr::tainted-liblua ALIAS tainted-liblua)

set_target_properties(
  tainted-liblua
  PROPERTIES
    OUTPUT_NAME $<IF:$<PLATFORM_ID:Windows>,lua51,lua5.1>
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

target_compile_definitions(
  tainted-liblua
  PUBLIC
    $<$<PLATFORM_ID:Linux>:LUA_USE_LINUX>
    $<$<PLATFORM_ID:Darwin>:LUA_USE_MACOSX>
    $<$<PLATFORM_ID:Windows>:LUA_USE_WINDOWS>
    $<$<AND:$<PLATFORM_ID:Windows>,$<STREQUAL:$<TARGET_PROPERTY:tainted-liblua,TYPE>,SHARED_LIBRARY>>:LUA_BUILD_AS_DLL>
  PRIVATE
    $<$<CONFIG:Debug>:LUA_USE_ASSERT>     # Enable internal assertions on debug builds.
    $<$<CONFIG:Debug>:LUA_USE_APICHECK>   # Enable public API assertions on debug builds.
)

target_include_directories(
  tainted-liblua
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  tainted-liblua
  PRIVATE
    $<$<PLATFORM_ID:Darwin,Linux>:dl>
    $<$<PLATFORM_ID:Darwin,Linux>:m>
    $<$<PLATFORM_ID:Windows>:bcrypt>
    sheredom::utf8h
)

set(liblua_PUBLIC_HEADERS lua.h lauxlib.h lualib.h luaconf.h lua.hpp)

target_sources(
  tainted-liblua
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
  tainted-liblua
  PROPERTIES
    PUBLIC_HEADER "${liblua_PUBLIC_HEADERS}"  # Must be a string representation of a list.
)

# -----------------------------------------------------------------------------
# Lua interpreter target (meorawr::tainted-lua)
# -----------------------------------------------------------------------------

add_executable(tainted-lua)
add_executable(meorawr::tainted-lua ALIAS tainted-lua)

set_target_properties(
  tainted-lua
  PROPERTIES
    OUTPUT_NAME $<IF:$<PLATFORM_ID:Windows>,lua,lua5.1>
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    BUILD_RPATH_USE_ORIGIN ON
    INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
)

target_link_libraries(
  tainted-lua
  PRIVATE
    meorawr::tainted-liblua
    $<$<PLATFORM_ID:Darwin,Linux>:readline>
)

target_sources(
  tainted-lua
  PRIVATE
    lua.c
)

# -----------------------------------------------------------------------------
# Lua compiler target (meorawr::tainted-luac)
# -----------------------------------------------------------------------------

add_executable(tainted-luac)
add_executable(meorawr::tainted-luac ALIAS tainted-luac)

set_target_properties(
  tainted-luac
  PROPERTIES
    OUTPUT_NAME $<IF:$<PLATFORM_ID:Windows>,luac,luac5.1>
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

target_link_libraries(
  tainted-luac
  PRIVATE
    $<TARGET_GENEX_EVAL:tainted-liblua,$<TARGET_PROPERTY:tainted-liblua,LINK_LIBRARIES>>
)

target_sources(
  tainted-luac
  PRIVATE
    luac.c
    print.c
    $<TARGET_OBJECTS:tainted-liblua>
)
