# The liblua target is a shared or static library build of the core of the VM
# without the interpreter or compiler frontends. Whether this is a static or
# shared build depends upon the state of the ambient BUILD_SHARED_LIBS
# variable.

add_library(liblua)

set_target_properties(liblua
  PROPERTIES
    DEFINE_SYMBOL ""
    OUTPUT_NAME "lua${LUA_OUTPUT_SUFFIX}"
    IMPORT_PREFIX "lib"
    PREFIX "lib"
)

target_compile_definitions(liblua
  PUBLIC
    $<$<AND:$<PLATFORM_ID:Windows>,$<STREQUAL:$<TARGET_PROPERTY:liblua,TYPE>,SHARED_LIBRARY>>:LUA_BUILD_AS_DLL>
  PRIVATE
    $<$<PLATFORM_ID:Linux>:LUA_USE_LINUX>
)

target_link_libraries(liblua
  PRIVATE
    $<$<PLATFORM_ID:Linux>:dl>
    $<$<PLATFORM_ID:Linux>:m>
)

target_sources(liblua
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
    "lbitlib.c"
    "ldblib.c"
    "liolib.c"
    "lmathlib.c"
    "loadlib.c"
    "loslib.c"
    "lstrlib.c"
    "ltablib.c"
    "linit.c"
)

# The lua interpreter target will link against liblua, so if you've built it
# as a shared library target then you'll need to ensure it's locatable at
# runtime.

if(LUA_BUILD_INTERPRETER)
  add_executable(lua)

  set_target_properties(lua
    PROPERTIES
      OUTPUT_NAME "lua${LUA_OUTPUT_SUFFIX}"
  )

  target_compile_definitions(lua
    PRIVATE
      $<$<PLATFORM_ID:Linux>:LUA_USE_LINUX>
  )

  target_link_libraries(lua
    PRIVATE
      liblua
      $<$<PLATFORM_ID:Linux>:readline>
  )

  target_sources(lua
    PRIVATE
      "lua.c"
  )
endif()

# The luac compiler target always pulls in and links to a static rebuild
# of the core library since in Lua 5.1 luac won't compile or link against a
# shared lua build.
#
# If liblua was built statically, the library will be reused and no
# recompilation of its sources is necessary.

if(LUA_BUILD_COMPILER)
  add_executable(luac)

  set_target_properties(luac
    PROPERTIES
      OUTPUT_NAME "luac${LUA_OUTPUT_SUFFIX}"
  )

  target_compile_definitions(luac
    PRIVATE
      $<$<PLATFORM_ID:Linux>:LUA_USE_LINUX>
  )

  target_link_libraries(luac
    PRIVATE
      # Reuse the liblua target if it was already built as a static library.
      $<$<STREQUAL:$<TARGET_PROPERTY:liblua,TYPE>,STATIC_LIBRARY>:liblua>
      $<$<PLATFORM_ID:Linux>:dl>
      $<$<PLATFORM_ID:Linux>:m>
  )

  target_sources(luac
    PRIVATE
      "luac.c"
      "print.c"
      # If liblua was built as a shared library, import its source list.
      $<$<STREQUAL:$<TARGET_PROPERTY:liblua,TYPE>,SHARED_LIBRARY>:$<TARGET_PROPERTY:liblua,SOURCES>>
  )
endif()

# The sources of each target are fixed up with an explicit LANGUAGE and header
# file property to work around issues if attempting to compile with a C++
# compiler, as CMake associates the ".c" and ".h" extensions with the C
# language and will cause build generation and/or linker issues.

get_target_property(liblua_SOURCES liblua SOURCES)

if(TARGET lua)
  get_target_property(lua_SOURCES lua SOURCES)
else()
  set(lua_SOURCES "")
endif()

if(TARGET luac)
  get_target_property(luac_SOURCES luac SOURCES)
else()
  set(luac_SOURCES "")
endif()

foreach(source IN LISTS liblua_SOURCES lua_SOURCES luac_SOURCES)
  if(source MATCHES "\\.c$")
    set_source_files_properties(${source} PROPERTIES LANGUAGE ${LUA_COMPILER_LANG})
  elseif(source MATCHES "\\.h$")
    set_source_files_properties(${source} PROPERTIES HEADER_FILE_ONLY ON)
  endif()
endforeach()
