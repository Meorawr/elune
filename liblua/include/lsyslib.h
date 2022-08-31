/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */

#ifndef lsyslib_h
#define lsyslib_h

#include "lua.h"

#include <stdio.h>

/**
 * The below APIs are for internal use only within the
 */

LUA_API lua_Clock luaI_clocktime (lua_State *L);
LUA_API lua_Clock luaI_clockrate (lua_State *L);
LUA_API lua_Number luaI_securerandom (lua_State *L);
LUA_API int luaI_stdin_is_tty (lua_State *L);
LUA_API int luaI_tmpname (lua_State *L);
LUA_API void luaI_writestring (const char *s, size_t sz);
LUA_API void luaI_writestringerror (const char *s, ...);
LUA_API void luaI_writeline (void);

/**
 * popen abstraction layer
 */

LUA_API FILE *luaI_popen (lua_State *L, const char *cmd, const char *mode);
LUA_API int luaI_pclose (lua_State *L, FILE *p);
LUA_API int luaI_checkpmode (lua_State *L, const char *mode);

/**
 * readline abstraction layer
 */

LUA_API int luaI_readline (lua_State *L, const char *prompt);
LUA_API void luaI_saveline (lua_State *L, const char *line);

#endif
