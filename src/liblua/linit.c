/**
 * Initialization of libraries for lua.c
 *
 * Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html>
 */


#define linit_c
#define LUA_LIB

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"


static const luaL_Reg lualibs[] = {
  { .name = LUA_BASELIBNAME, .func = luaopen_base },
  { .name = LUA_BITLIBNAME, .func = luaopen_bit },
  { .name = LUA_DBLIBNAME, .func = luaopen_debug },
  { .name = LUA_IOLIBNAME, .func = luaopen_io },
  { .name = LUA_LOADLIBNAME, .func = luaopen_package },
  { .name = LUA_MATHLIBNAME, .func = luaopen_math },
  { .name = LUA_OSLIBNAME, .func = luaopen_os },
  { .name = LUA_STRLIBNAME, .func = luaopen_string },
  { .name = LUA_TABLIBNAME, .func = luaopen_table },
  { .name = LUA_COMPATLIBNAME, .func = luaopen_compat },
  { .name = NULL, .func = NULL },
};


static const luaL_Reg elunelibs[] = {
  /* { .name = LUA_BASELIBNAME, .func = luaopen_elune_base }, */
  /* { .name = LUA_BITLIBNAME, .func = luaopen_elune_bit }, */
  /* { .name = LUA_DBLIBNAME, .func = luaopen_elune_debug }, */
  /* { .name = LUA_COLIBNAME, .func = luaopen_elune_coroutine }, */
  /* { .name = LUA_MATHLIBNAME, .func = luaopen_elune_math }, */
  /* { .name = LUA_OSLIBNAME, .func = luaopen_elune_os }, */
  /* { .name = LUA_STRLIBNAME, .func = luaopen_elune_string }, */
  /* { .name = LUA_TABLIBNAME, .func = luaopen_elune_table }, */
  /* { .name = LUA_COMPATLIBNAME, .func = luaopen_elune_compat }, */
  { .name = NULL, .func = NULL },
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


LUALIB_API void luaL_openelunelibs (lua_State *L) {
  openlibs(L, elunelibs);
}
