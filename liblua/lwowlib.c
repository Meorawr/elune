/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */

#define lwowlib_c
#define LUA_LIB

#include "lua.h"

#include "lualib.h"

LUALIB_API int luaopen_wow (lua_State *L)
{
  lua_createtable(L, 0, 2);
  lua_pushvalue(L, -1);
  lua_replace(L, LUA_ENVIRONINDEX);
  luaL_openwowlibs(L);
  return 1;
}
