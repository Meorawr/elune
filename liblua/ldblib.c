/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */

#include <stdlib.h>
#include <string.h>

#define ldblib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int db_getregistry (lua_State *L) {
  lua_pushvalue(L, LUA_REGISTRYINDEX);
  return 1;
}

static int db_getmetatable (lua_State *L) {
  luaL_checkany(L, 1);
  if (!lua_getmetatable(L, 1)) {
    lua_pushnil(L); /* no metatable */
  }
  return 1;
}

static int db_setmetatable (lua_State *L) {
  int t = lua_type(L, 2);
  luaL_argcheck(L, t == LUA_TNIL || t == LUA_TTABLE, 2,
                "nil or table expected");
  lua_settop(L, 2);
  lua_pushboolean(L, lua_setmetatable(L, 1));
  return 1;
}

static int db_getfenv (lua_State *L) {
  luaL_checkany(L, 1);
  lua_getfenv(L, 1);
  return 1;
}

static int db_setfenv (lua_State *L) {
  luaL_checktype(L, 2, LUA_TTABLE);
  lua_settop(L, 2);
  if (lua_setfenv(L, 1) == 0) {
    luaL_error(L, "'setfenv' cannot change environment of given object");
  }
  return 1;
}

static void settabss (lua_State *L, const char *i, const char *v) {
  lua_pushstring(L, v);
  lua_setfield(L, -2, i);
}

static void settabsi (lua_State *L, const char *i, int v) {
  lua_pushinteger(L, v);
  lua_setfield(L, -2, i);
}

static lua_State *getthread (lua_State *L, int *arg) {
  if (lua_isthread(L, 1)) {
    *arg = 1;
    return lua_tothread(L, 1);
  } else {
    *arg = 0;
    return L;
  }
}

static void treatstackoption (lua_State *L, lua_State *L1, const char *fname) {
  if (L == L1) {
    lua_pushvalue(L, -2);
    lua_remove(L, -3);
  } else {
    lua_xmove(L1, L, 1);
  }
  lua_setfield(L, -2, fname);
}

static int db_getinfo (lua_State *L) {
  lua_Debug ar;
  int arg;
  lua_State *L1 = getthread(L, &arg);
  const char *options = luaL_optstring(L, arg + 2, "flnSu");
  if (lua_isnumber(L, arg + 1)) {
    if (!lua_getstack(L1, lua_toint(L, arg + 1), &ar)) {
      lua_pushnil(L); /* level out of range */
      return 1;
    }
  } else if (lua_isfunction(L, arg + 1)) {
    lua_pushfstring(L, ">%s", options);
    options = lua_tostring(L, -1);
    lua_pushvalue(L, arg + 1);
    lua_xmove(L, L1, 1);
  } else {
    return luaL_argerror(L, arg + 1, "function or level expected");
  }
  if (!lua_getinfo(L1, options, &ar)) {
    return luaL_argerror(L, arg + 2, "invalid option");
  }
  lua_createtable(L, 0, 2);
  if (strchr(options, 'S')) {
    settabss(L, "source", ar.source);
    settabss(L, "short_src", ar.short_src);
    settabsi(L, "linedefined", ar.linedefined);
    settabsi(L, "lastlinedefined", ar.lastlinedefined);
    settabss(L, "what", ar.what);
  }
  if (strchr(options, 'l')) {
    settabsi(L, "currentline", ar.currentline);
  }
  if (strchr(options, 'u')) {
    settabsi(L, "nups", ar.nups);
  }
  if (strchr(options, 'n')) {
    settabss(L, "name", ar.name);
    settabss(L, "namewhat", ar.namewhat);
  }
  if (strchr(options, 'L')) {
    treatstackoption(L, L1, "activelines");
  }
  if (strchr(options, 'f')) {
    treatstackoption(L, L1, "func");
  }
  return 1; /* return table */
}

static int db_getlocal (lua_State *L) {
  int arg;
  lua_State *L1 = getthread(L, &arg);
  lua_Debug ar;
  const char *name;
  if (!lua_getstack(L1, luaL_checkint(L, arg + 1), &ar)) { /* out of range? */
    return luaL_argerror(L, arg + 1, "level out of range");
  }
  name = lua_getlocal(L1, &ar, luaL_checkint(L, arg + 2));
  if (name) {
    lua_xmove(L1, L, 1);
    lua_pushstring(L, name);
    lua_pushvalue(L, -2);
    return 2;
  } else {
    lua_pushnil(L);
    return 1;
  }
}

static int db_setlocal (lua_State *L) {
  int arg;
  lua_State *L1 = getthread(L, &arg);
  lua_Debug ar;
  if (!lua_getstack(L1, luaL_checkint(L, arg + 1), &ar)) { /* out of range? */
    return luaL_argerror(L, arg + 1, "level out of range");
  }
  luaL_checkany(L, arg + 3);
  lua_settop(L, arg + 3);
  lua_xmove(L, L1, 1);
  lua_pushstring(L, lua_setlocal(L1, &ar, luaL_checkint(L, arg + 2)));
  return 1;
}

static int auxupvalue (lua_State *L, int get) {
  const char *name;
  int n = luaL_checkint(L, 2);
  luaL_checktype(L, 1, LUA_TFUNCTION);
  if (lua_iscfunction(L, 1)) {
    return 0; /* cannot touch C upvalues from Lua */
  }
  name = get ? lua_getupvalue(L, 1, n) : lua_setupvalue(L, 1, n);
  if (name == NULL) {
    return 0;
  }
  lua_pushstring(L, name);
  lua_insert(L, -(get + 1));
  return get + 1;
}

static int db_getupvalue (lua_State *L) {
  return auxupvalue(L, 1);
}

static int db_setupvalue (lua_State *L) {
  luaL_checkany(L, 3);
  return auxupvalue(L, 0);
}

static const char KEY_HOOK = 'h';

static void hookf (lua_State *L, lua_Debug *ar) {
  static const char *const hooknames[] = {
    "call", "return", "line", "count", "tail return",
  };

  lua_pushlightuserdata(L, (void *) &KEY_HOOK);
  lua_rawget(L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L, L);
  lua_rawget(L, -2);
  if (lua_isfunction(L, -1)) {
    lua_pushstring(L, hooknames[(int) ar->event]);
    if (ar->currentline >= 0) {
      lua_pushinteger(L, ar->currentline);
    } else {
      lua_pushnil(L);
    }
    lua_assert(lua_getinfo(L, "lS", ar));
    lua_call(L, 2, 0);
  }
}

static int makemask (const char *smask, int count) {
  int mask = 0;
  if (strchr(smask, 'c')) {
    mask |= LUA_MASKCALL;
  }
  if (strchr(smask, 'r')) {
    mask |= LUA_MASKRET;
  }
  if (strchr(smask, 'l')) {
    mask |= LUA_MASKLINE;
  }
  if (count > 0) {
    mask |= LUA_MASKCOUNT;
  }
  return mask;
}

static char *unmakemask (int mask, char *smask) {
  int i = 0;
  if (mask & LUA_MASKCALL) {
    smask[i++] = 'c';
  }
  if (mask & LUA_MASKRET) {
    smask[i++] = 'r';
  }
  if (mask & LUA_MASKLINE) {
    smask[i++] = 'l';
  }
  smask[i] = '\0';
  return smask;
}

static void gethooktable (lua_State *L) {
  lua_pushlightuserdata(L, (void *) &KEY_HOOK);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    lua_createtable(L, 0, 1);
    lua_pushlightuserdata(L, (void *) &KEY_HOOK);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
  }
}

static int db_sethook (lua_State *L) {
  int arg;
  int mask;
  int count;
  lua_Hook func;
  lua_State *L1 = getthread(L, &arg);
  if (lua_isnoneornil(L, arg + 1)) {
    lua_settop(L, arg + 1);
    func = NULL;
    mask = 0;
    count = 0; /* turn off hooks */
  } else {
    const char *smask = luaL_checkstring(L, arg + 2);
    luaL_checktype(L, arg + 1, LUA_TFUNCTION);
    count = luaL_optint(L, arg + 3, 0);
    func = hookf;
    mask = makemask(smask, count);
  }
  gethooktable(L);
  lua_pushlightuserdata(L, L1);
  lua_pushvalue(L, arg + 1);
  lua_rawset(L, -3);                  /* set new hook */
  lua_pop(L, 1);                      /* remove hook table */
  lua_sethook(L1, func, mask, count); /* set hooks */
  return 0;
}

static int db_gethook (lua_State *L) {
  int arg;
  lua_State *L1 = getthread(L, &arg);
  char buff[4];
  int mask = lua_gethookmask(L1);
  lua_Hook hook = lua_gethook(L1);
  if (hook != NULL && hook != hookf) { /* external hook? */
    lua_pushliteral(L, "external hook");
  } else {
    gethooktable(L);
    lua_pushlightuserdata(L, L1);
    lua_rawget(L, -2); /* get hook */
    lua_remove(L, -2); /* remove hook table */
  }
  lua_pushstring(L, unmakemask(mask, buff));
  lua_pushinteger(L, lua_gethookcount(L1));
  return 3;
}

static int db_debug (lua_State *L) {
  for (;;) {
    const char *line;
    size_t len;

    if (luaL_readline(L, "lua_debug> ") != 0) {
      return 0; /* error reading line */
    }

    line = lua_tolstring(L, -1, &len);

    if (strcmp(line, "cont") == 0) {
      return 0; /* explicit break */
    }

    if (luaL_loadbuffer(L, line, len, "=(debug command)") ||
        lua_pcall(L, 0, 0, 0)) {
      luaL_writestringerror("%s\n", lua_tostring(L, -1));
    }

    if (len > 0) {
      luaL_saveline(L, line);
    }

    lua_settop(L, 0); /* pop line and any errors */
  }
}

static int db_traceback (lua_State *L) {
  int arg;
  lua_State *L1 = getthread(L, &arg);
  const char *msg = lua_tostring(L, arg + 1);
  if (msg == NULL && !lua_isnoneornil(L, arg + 1)) { /* non-string 'msg'? */
    lua_pushvalue(L, arg + 1);                       /* return it untouched */
  } else {
    int level = luaL_optint(L, arg + 2, (L == L1) ? 1 : 0);
    luaL_traceback(L, L1, msg, level);
  }
  return 1;
}

static int db_geterrorhandler (lua_State *L) {
  lua_pushvalue(L, LUA_ERRORHANDLERINDEX);
  return 1;
}

static int db_seterrorhandler (lua_State *L) {
  luaL_checkany(L, 1);
  lua_replace(L, LUA_ERRORHANDLERINDEX);
  return 0;
}

static int db_getobjectsize (lua_State *L) {
  luaL_checkany(L, 1);
  lua_pushinteger(L, lua_objsize(L, 1));
  return 1;
}

static int db_iscfunction (lua_State *L) {
  lua_pushboolean(L, lua_iscfunction(L, 1));
  return 1;
}

static int db_newcfunction (lua_State *L) {
  luaL_checkany(L, 1);
  luaL_createdelegate(L);
  return 1;
}

static int db_ref (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checkany(L, 2);
  lua_pushinteger(L, luaL_ref(L, 1));
  return 1;
}

static int db_unref (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_unref(L, 1, luaL_checkint(L, 2));
  return 0;
}

static int db_getexceptmask (lua_State *L) {
  lua_State *L1;
  int arg;
  int mask;
  char smask[4];

  L1 = getthread(L, &arg);
  mask = lua_getexceptmask(L);

  int i = 0;
  if (mask & LUA_EXCEPTFPECOERCE) {
    smask[i++] = 'f';
  }
  if (mask & LUA_EXCEPTFPESTRICT) {
    smask[i++] = 'F';
  }
  if (mask & LUA_EXCEPTOVERFLOW) {
    smask[i++] = 'o';
  }
  smask[i] = '\0';

  lua_pushstring(L1, smask);
  return 1;
}

static int db_setexceptmask (lua_State *L) {
  lua_State *L1;
  const char *smask;
  int arg;
  int mask;

  L1 = getthread(L, &arg);
  smask = luaL_checkstring(L, arg + 1);

  mask = 0;
  if (strchr(smask, 'f')) {
    mask |= LUA_EXCEPTFPECOERCE;
  }
  if (strchr(smask, 'F')) {
    mask |= LUA_EXCEPTFPESTRICT;
  }
  if (strchr(smask, 'o')) {
    mask |= LUA_EXCEPTOVERFLOW;
  }

  lua_setexceptmask(L1, mask);
  return 0;
}

static int db_getscripttimeout (lua_State *L) {
  lua_ScriptTimeout timeout;
  lua_getscripttimeout(L, &timeout);

  lua_pushnumber(L, (lua_Number) timeout.ticks / lua_clockrate(L));
  lua_pushinteger(L, timeout.instructions);
  return 2;
}

static int db_setscripttimeout (lua_State *L) {
  lua_ScriptTimeout timeout;
  timeout.ticks = (lua_Clock) (luaL_checknumber(L, 1) * lua_clockrate(L));
  timeout.instructions = luaL_checkint(L, 2);
  lua_setscripttimeout(L, &timeout);
  return 0;
}

static int db_debugprofilestart (lua_State *L) {
  lua_Clock *start = lua_touserdata(L, lua_upvalueindex(1));
  *start = lua_clocktime(L);
  return 0;
}

static int db_debugprofilestop (lua_State *L) {
  lua_Clock stop = lua_clocktime(L);
  lua_Clock start = *((lua_Clock *) lua_touserdata(L, lua_upvalueindex(1)));
  lua_Clock rate = lua_clockrate(L);

  lua_pushnumber(L, (((lua_Number) stop - start) * 1e3) / rate);
  return 1;
}

/**
 * Debug library registration
 */

static const luaL_Reg dblib_lua[] = {
  { .name = "debug", .func = db_debug },
  { .name = "geterrorhandler", .func = db_geterrorhandler },
  { .name = "getexceptmask", .func = db_getexceptmask },
  { .name = "getfenv", .func = db_getfenv },
  { .name = "gethook", .func = db_gethook },
  { .name = "getinfo", .func = db_getinfo },
  { .name = "getlocal", .func = db_getlocal },
  { .name = "getmetatable", .func = db_getmetatable },
  { .name = "getobjectsize", .func = db_getobjectsize },
  { .name = "getregistry", .func = db_getregistry },
  { .name = "getscripttimeout", .func = db_getscripttimeout },
  { .name = "getupvalue", .func = db_getupvalue },
  { .name = "iscfunction", .func = db_iscfunction },
  { .name = "newcfunction", .func = db_newcfunction },
  { .name = "ref", .func = db_ref },
  { .name = "seterrorhandler", .func = db_seterrorhandler },
  { .name = "setexceptmask", .func = db_setexceptmask },
  { .name = "setfenv", .func = db_setfenv },
  { .name = "sethook", .func = db_sethook },
  { .name = "setlocal", .func = db_setlocal },
  { .name = "setmetatable", .func = db_setmetatable },
  { .name = "setscripttimeout", .func = db_setscripttimeout },
  { .name = "setupvalue", .func = db_setupvalue },
  { .name = "traceback", .func = db_traceback },
  { .name = "unref", .func = db_unref },
  { .name = NULL, .func = NULL },
};

static const luaL_Reg dblib_global[] = {
  { .name = "debugstack", .func = NULL },  /* TODO: Implement me! */
  { .name = "debuglocals", .func = NULL }, /* TODO: Implement me! */
  { .name = NULL, .func = NULL },
};

static void dblib_opendebugprofile (lua_State *L) {
  /**
   * The debugprofilestart and debugprofilestop functions act as paired calls
   * and need to share a "start" time as state between the two. We store this
   * in some full-userdata stored as upvalues on each closure.
   *
   * On creation the start time is left as zero so that any calls to the
   * debugprofilestop API will return time-since-state-creation until a
   * call to debugprofilestart is made.
   */

  lua_Clock *start = lua_newuserdata(L, sizeof(lua_Clock));
  *start = 0;

  lua_pushvalue(L, -1);
  lua_pushcclosure(L, db_debugprofilestart, 1);
  lua_setfield(L, -3, "debugprofilestart");
  lua_pushcclosure(L, db_debugprofilestop, 1);
  lua_setfield(L, -2, "debugprofilestop");
}

LUALIB_API int luaopen_debug (lua_State *L) {
  luaL_register(L, "_G", dblib_global);
  dblib_opendebugprofile(L);
  luaL_register(L, LUA_DBLIBNAME, dblib_lua);
  return 1;
}

LUALIB_API int luaopen_wow_debug (lua_State *L) {
  lua_pushvalue(L, LUA_ENVIRONINDEX);
  luaL_setfuncs(L, dblib_global, 0);
  dblib_opendebugprofile(L);
  return 0;
}
