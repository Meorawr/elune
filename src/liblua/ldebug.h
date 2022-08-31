/*
** $Id: ldebug.h,v 2.3.1.1 2007/12/27 13:02:25 roberto Exp $
** Auxiliary functions from Debug Interface module
** See Copyright Notice in lua.h
*/

#ifndef ldebug_h
#define ldebug_h


#include "lstate.h"


#define pcRel(pc, p)	(cast(int, (pc) - (p)->code) - 1)

#define getline(f,pc)	(((f)->lineinfo) ? (f)->lineinfo[pc] : 0)

#define resethookcount(L)	(L->hookcount = L->basehookcount)


LUAI_FUNC LUA_NORETURN void luaG_typeerror (lua_State *L, const TValue *o, const char *opname);
LUAI_FUNC LUA_NORETURN void luaG_concaterror (lua_State *L, StkId p1, StkId p2);
LUAI_FUNC LUA_NORETURN void luaG_aritherror (lua_State *L, const TValue *p1, const TValue *p2);
LUAI_FUNC LUA_NORETURN void luaG_ordererror (lua_State *L, const TValue *p1, const TValue *p2);
LUAI_FUNC LUA_NORETURN void luaG_overflowerror (lua_State *L, lua_Number n);
LUAI_FUNC LUA_NORETURN void luaG_runerror (lua_State *L, const char *fmt, ...);
LUAI_FUNC LUA_NORETURN void luaG_errormsg (lua_State *L);
LUAI_FUNC int luaG_checkcode (const Proto *pt);
LUAI_FUNC int luaG_checkopenop (Instruction i);
LUAI_FUNC int luaG_getinfo (lua_State *L, CallInfo *ci, const char *what, lua_Debug *ar);

LUAI_FUNC void luaG_init (global_State *g);
LUAI_FUNC void luaG_profileenter (lua_State *L);
LUAI_FUNC void luaG_profileleave (lua_State *L);
LUAI_FUNC void luaG_profileresume (lua_State *L);
LUAI_FUNC lua_clock_t luaG_gettickcount (const global_State *g);
LUAI_FUNC lua_clock_t luaG_gettickfrequency (const global_State *g);

#endif
