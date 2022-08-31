/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */

#define linit_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static const luaL_Reg lualibs[] = {
    { LUA_BASELIBNAME, luaopen_base },
    { LUA_BITLIBNAME, luaopen_bit },
    { LUA_DBLIBNAME, luaopen_debug },
    { LUA_DBLIBNAME ".security", luaopen_security },
    { LUA_DBLIBNAME ".stats", luaopen_stats },
    { LUA_IOLIBNAME, luaopen_io },
    { LUA_LOADLIBNAME, luaopen_package },
    { LUA_MATHLIBNAME, luaopen_math },
    { LUA_OSLIBNAME, luaopen_os },
    { LUA_STRLIBNAME, luaopen_string },
    { LUA_TABLIBNAME, luaopen_table },
    { LUA_COMPATLIBNAME, luaopen_compat },
    /* clang-format off */
    { NULL, NULL },
    /* clang-format on */
};

static const luaL_Reg wowlibs[] = {
    { LUA_BASELIBNAME, luaopen_wow_base },
    { LUA_BITLIBNAME, luaopen_wow_bit },
    { LUA_DBLIBNAME, luaopen_wow_debug },
    { LUA_COLIBNAME, luaopen_wow_coroutine },
    { LUA_MATHLIBNAME, luaopen_wow_math },
    { LUA_OSLIBNAME, luaopen_wow_os },
    { LUA_STRLIBNAME, luaopen_wow_string },
    { LUA_TABLIBNAME, luaopen_wow_table },
    { LUA_COMPATLIBNAME, luaopen_compat },
    /* clang-format off */
    { NULL, NULL },
    /* clang-format on */
};

static void openlibs (lua_State *L, const luaL_Reg *lib) {
    for (; lib->func; lib++) {
        lua_pushcclosure(L, lib->func, 0);
        lua_pushstring(L, lib->name);
        lua_call(L, 1, 0);
    }
}

LUALIB_API void luaL_openlibs (lua_State *L) {
    openlibs(L, lualibs);
}

LUALIB_API void luaL_openwowlibs (lua_State *L) {
    openlibs(L, wowlibs);
}
