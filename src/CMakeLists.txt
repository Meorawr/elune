# The lua-core target is the core of the VM without the interpreter or
# compiler bits included and is set up as an object library target which is
# shared between the liblua, lua, and luac targets.

add_library(lua-core OBJECT)

set_target_properties(lua-core
    PROPERTIES
        C_EXTENSIONS OFF
        C_STANDARD 90
        C_STANDARD_REQUIRED OFF
        CXX_EXTENSIONS OFF
        DEFINE_SYMBOL ""
        OUTPUT_NAME $<IF:$<BOOL:${BUILD_VERSIONED_NAMES}>,lua5.1,lua>
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
        PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
        POSITION_INDEPENDENT_CODE ON
)

target_compile_definitions(lua-core
    PUBLIC
        $<$<AND:$<PLATFORM_ID:Windows>,$<STREQUAL:$<TARGET_PROPERTY:liblua,TYPE>,SHARED_LIBRARY>>:LUA_BUILD_AS_DLL>
    PRIVATE
        $<$<PLATFORM_ID:Linux>:LUA_USE_LINUX>
)

target_link_libraries(lua-core
    PRIVATE
        $<$<PLATFORM_ID:Linux>:dl>
        $<$<PLATFORM_ID:Linux>:m>
)

target_sources(lua-core
    PUBLIC
        "lua.h"
        "lauxlib.h"
        "lualib.h"
        "luaconf.h"
    PRIVATE
        "lapi.c"        "lapi.h"
        "lcode.c"       "lcode.h"
        "ldebug.c"      "ldebug.h"
        "ldo.c"         "ldo.h"
        "ldump.c"
        "lfunc.c"       "lfunc.h"
        "lgc.c"         "lgc.h"
        "llex.c"        "llex.h"
                        "llimits.h"
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

        "lauxlib.c"
        "lbaselib.c"
        "ldblib.c"
        "liolib.c"
        "lmathlib.c"
        "loadlib.c"
        "loslib.c"
        "lstrlib.c"
        "ltablib.c"
        "linit.c"
)

# The liblua target is a shared or static library target controllable through
# the BUILD_SHARED_LIBS CMake variable which packages up the lua-core target.
#
# The prefix is forced on all platforms to avoid name collision issues with
# the interpreter binary, particularly with regards to MSVC's PDB files.

add_library(liblua)

set_target_properties(liblua
    PROPERTIES
        PREFIX "lib"
        IMPORT_PREFIX "lib"
        OUTPUT_NAME $<IF:$<BOOL:${BUILD_VERSIONED_NAMES}>,lua5.1,lua>
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
        PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
)

target_link_libraries(liblua PRIVATE lua-core)

# The lua interpreter target will link against liblua, so if you've built it
# as a shared library target then you'll need to ensure it's locatable at
# runtime.

add_executable(lua)

set_target_properties(lua
    PROPERTIES
        OUTPUT_NAME $<IF:$<BOOL:${BUILD_VERSIONED_NAMES}>,lua5.1,lua>
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
        PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
)

target_link_libraries(lua
    PRIVATE
        liblua
)

target_sources(lua
    PRIVATE
        "lua.c"
)

# The luac compiler target always uses the lua-core object library since
# in Lua 5.1 luac won't build if liblua is a shared library.

add_executable(luac)

set_target_properties(luac
    PROPERTIES
        OUTPUT_NAME $<IF:$<BOOL:${BUILD_VERSIONED_NAMES}>,luac5.1,luac>
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
        PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
)

target_link_libraries(luac
    PRIVATE
        lua-core
        $<$<PLATFORM_ID:Linux>:m>
)

target_sources(luac
    PRIVATE
        "luac.c"
        "print.c"
)
