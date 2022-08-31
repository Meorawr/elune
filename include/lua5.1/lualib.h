/**
 * Lua - An Extensible Extension Language
 *
 * Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html>
 */


#ifndef lualib_h
#define lualib_h

#include "lua.h"


#define LUA_BASELIBNAME "_G"
#define LUA_BITLIBNAME "bit"
#define LUA_COLIBNAME "coroutine"
#define LUA_DBLIBNAME "debug"
#define LUA_IOLIBNAME "io"
#define LUA_LOADLIBNAME "package"
#define LUA_MATHLIBNAME "math"
#define LUA_OSLIBNAME "os"
#define LUA_STRLIBNAME "string"
#define LUA_TABLIBNAME "table"
#define LUA_WOWLIBNAME "wow"


LUALIB_API int (luaopen_base) (lua_State *L);
LUALIB_API int (luaopen_bit) (lua_State *L);
LUALIB_API int (luaopen_coroutine) (lua_State *L);
LUALIB_API int (luaopen_debug) (lua_State *L);
LUALIB_API int (luaopen_io) (lua_State *L);
LUALIB_API int (luaopen_math) (lua_State *L);
LUALIB_API int (luaopen_os) (lua_State *L);
LUALIB_API int (luaopen_package) (lua_State *L);
LUALIB_API int (luaopen_string) (lua_State *L);
LUALIB_API int (luaopen_table) (lua_State *L);

LUALIB_API void (luaL_openlibs) (lua_State *L);


/*
** {======================================================================
** Extension Libraries
** =======================================================================
*/

LUALIB_API int (luaopen_wow_base) (lua_State *L);
LUALIB_API int (luaopen_wow_bit) (lua_State *L);
LUALIB_API int (luaopen_wow_debug) (lua_State *L);
LUALIB_API int (luaopen_wow_coroutine) (lua_State *L);
LUALIB_API int (luaopen_wow_os) (lua_State *L);
LUALIB_API int (luaopen_wow_math) (lua_State *L);
LUALIB_API int (luaopen_wow_string) (lua_State *L);
LUALIB_API int (luaopen_wow_table) (lua_State *L);
LUALIB_API int (luaopen_wow) (lua_State *L);

LUALIB_API void (luaL_openwowlibs) (lua_State *L);


/* }====================================================================== */

#endif
