/*
** $Id: lualib.h,v 1.36.1.1 2007/12/27 13:02:25 roberto Exp $
** Lua standard libraries
** See Copyright Notice in lua.h
*/


#ifndef lualib_h
#define lualib_h

#include "lua.h"


#define LUA_BASELIBNAME "_G"
#define LUA_BITLIBNAME "bit"
#define LUA_COLIBNAME "coroutine"
#define LUA_DBLIBNAME "debug"
#define LUA_ELUNELIBNAME "elune"
#define LUA_IOLIBNAME "io"
#define LUA_LOADLIBNAME "package"
#define LUA_MATHLIBNAME "math"
#define LUA_OSLIBNAME "os"
#define LUA_STRLIBNAME "string"
#define LUA_TABLIBNAME "table"


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

LUALIB_API int (luaopen_elune_base) (lua_State *L);
LUALIB_API int (luaopen_elune_bit) (lua_State *L);
LUALIB_API int (luaopen_elune_debug) (lua_State *L);
LUALIB_API int (luaopen_elune_coroutine) (lua_State *L);
LUALIB_API int (luaopen_elune_os) (lua_State *L);
LUALIB_API int (luaopen_elune_math) (lua_State *L);
LUALIB_API int (luaopen_elune_string) (lua_State *L);
LUALIB_API int (luaopen_elune_table) (lua_State *L);
LUALIB_API int (luaopen_elune) (lua_State *L);

LUALIB_API void (luaL_openelunelibs) (lua_State *L);


/* }====================================================================== */

#endif
