include(GNUInstallDirs)

set(liblua_HEADERS
  "lua.h"
  "lauxlib.h"
  "lualib.h"
  "luaconf.h"
  "lua.hpp"
)

set(liblua_SOURCES
  # Core sources
  "lapi.c"        "lapi.h"
  "lcode.c"       "lcode.h"
  "ldebug.c"      "ldebug.h"
  "ldo.c"         "ldo.h"
  "ldump.c"
  "lfunc.c"       "lfunc.h"
  "lgc.c"         "lgc.h"
  "llex.c"        "llex.h"
                  "llimits.h"
                  "lmanip.h"
  "lmem.c"        "lmem.h"
  "lobject.c"     "lobject.h"
  "lopcodes.c"    "lopcodes.h"
  "lparser.c"     "lparser.h"
  "lstate.c"      "lstate.h"
  "lstring.c"     "lstring.h"
  "ltable.c"      "ltable.h"
  "ltm.c"         "ltm.h"
  "lundump.c"     "lundump.h"
  "lvm.c"         "lvm.h"
  "lzio.c"        "lzio.h"

  # Library sources
  "lauxlib.c"
  "lbaselib.c"
  "lbitlib.c"
  "lcompatlib.c"
  "ldblib.c"
  "liolib.c"
  "lmathlib.c"
  "loadlib.c"
  "loslib.c"
  "lseclib.c"
  "lstrlib.c"
  "ltablib.c"
  "linit.c"
)

add_library(liblua)

set_target_properties(liblua
  PROPERTIES
    OUTPUT_NAME $<IF:$<PLATFORM_ID:Windows>,lua51,lua5.1>
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
)

target_compile_definitions(liblua
  PUBLIC
    $<$<PLATFORM_ID:Linux>:LUA_USE_LINUX>
    $<$<PLATFORM_ID:Darwin>:LUA_USE_MACOSX>
    $<$<PLATFORM_ID:Windows>:LUA_USE_WINDOWS>
    $<$<AND:$<PLATFORM_ID:Windows>,$<STREQUAL:$<TARGET_PROPERTY:liblua,TYPE>,SHARED_LIBRARY>>:LUA_BUILD_AS_DLL>
  PRIVATE
    $<$<CONFIG:Debug>:LUA_USE_ASSERT>     # Enable internal assertions on debug builds.
    $<$<CONFIG:Debug>:LUA_USE_APICHECK>   # Enable public API assertions on debug builds.
)

target_include_directories(liblua
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(liblua
  PRIVATE
    $<$<PLATFORM_ID:Darwin,Linux>:dl>
    $<$<PLATFORM_ID:Darwin,Linux>:m>
    $<$<PLATFORM_ID:Windows>:bcrypt>
    sheredom::utf8h
)

target_sources(liblua
  PUBLIC
    ${liblua_HEADERS}
  PRIVATE
    ${liblua_SOURCES}
)

source_group(
  TREE    "${PROJECT_SOURCE_DIR}"
  PREFIX  "Header Files"
  FILES   ${liblua_HEADERS}
)

set_target_properties(liblua
  PROPERTIES
    PUBLIC_HEADER "${liblua_HEADERS}"  # Must be a string representation of a list.
)

add_executable(lua)

set_target_properties(lua
  PROPERTIES
    OUTPUT_NAME $<IF:$<PLATFORM_ID:Windows>,lua,lua5.1>
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    BUILD_RPATH_USE_ORIGIN ON
    INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
)

target_link_libraries(lua
  PRIVATE
    liblua
    $<$<PLATFORM_ID:Darwin,Linux>:readline>
)

target_sources(lua
  PRIVATE
    "lua.c"
)

add_executable(luac)

set_target_properties(luac
  PROPERTIES
    OUTPUT_NAME $<IF:$<PLATFORM_ID:Windows>,luac,luac5.1>
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
)

target_link_libraries(luac
  PRIVATE
    $<TARGET_GENEX_EVAL:liblua,$<TARGET_PROPERTY:liblua,LINK_LIBRARIES>>
)

target_sources(luac
  PRIVATE
    "luac.c"
    "print.c"
    $<TARGET_OBJECTS:liblua>
)

# Add install target support that'll populate a destination prefix with a
# standard distribution of bin/, lib/, and include/ directories.

if(TL_ENABLE_INSTALL)
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
