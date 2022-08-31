/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */


#define lbaselib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>


static int luaB_print (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  int i;
  lua_getglobal(L, "tostring");
  for (i=1; i<=n; i++) {
    const char *s;
    size_t l;
    lua_pushvalue(L, -1);  /* function to be called */
    lua_pushvalue(L, i);   /* value to print */
    lua_call(L, 1, 1);
    s = lua_tolstring(L, -1, &l);  /* get result */
    if (s == NULL)
      return luaL_error(L, LUA_QL("tostring") " must return a string to " LUA_QL("print"));
    if (i>1) luaL_writestring("\t", 1);
    luaL_writestring(s, l);
    lua_pop(L, 1);  /* pop result */
  }
  luaL_writeline();
  return 0;
}


static int luaB_tonumber (lua_State *L) {
  int base = luaL_optint(L, 2, 10);
  if (base == 10) {  /* standard conversion */
    luaL_checkany(L, 1);
    if (lua_isnumber(L, 1)) {
      lua_pushnumber(L, lua_tonumber(L, 1));
      return 1;
    }
  }
  else {
    const char *s1 = luaL_checkstring(L, 1);
    char *s2;
    unsigned long n;
    luaL_argcheck(L, 2 <= base && base <= 36, 2, "base out of range");
    n = strtoul(s1, &s2, base);
    if (s1 != s2) {  /* at least one valid digit? */
      while (isspace((unsigned char)(*s2))) s2++;  /* skip trailing spaces */
      if (*s2 == '\0') {  /* no invalid trailing characters? */
        lua_pushnumber(L, (lua_Number)n);
        return 1;
      }
    }
  }
  lua_pushnil(L);  /* else not a number */
  return 1;
}


static int luaB_error (lua_State *L) {
  int level = luaL_optint(L, 2, 1);
  lua_settop(L, 1);
  if (lua_isstring(L, 1) && level > 0) {  /* add extra information? */
    luaL_where(L, level);
    lua_pushvalue(L, 1);
    lua_concat(L, 2);
  }
  return lua_error(L);
}


static int luaB_getmetatable (lua_State *L) {
  luaL_checkany(L, 1);
  if (!lua_getmetatable(L, 1)) {
    lua_pushnil(L);
    return 1;  /* no metatable */
  }
  luaL_getmetafield(L, 1, "__metatable");
  return 1;  /* returns either __metatable field (if present) or metatable */
}


static int luaB_setmetatable (lua_State *L) {
  int t = lua_type(L, 2);
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_argcheck(L, t == LUA_TNIL || t == LUA_TTABLE, 2,
                    "nil or table expected");
  if (luaL_getmetafield(L, 1, "__metatable"))
    luaL_error(L, "cannot change a protected metatable");
  lua_settop(L, 2);
  lua_setmetatable(L, 1);
  return 1;
}


static void getfunc (lua_State *L, int opt) {
  if (lua_isfunction(L, 1)) lua_pushvalue(L, 1);
  else {
    lua_Debug ar;
    int level = opt ? luaL_optint(L, 1, 1) : luaL_checkint(L, 1);
    luaL_argcheck(L, level >= 0, 1, "level must be non-negative");
    if (lua_getstack(L, level, &ar) == 0)
      luaL_argerror(L, 1, "invalid level");
    lua_getinfo(L, "f", &ar);
    if (lua_isnil(L, -1))
      luaL_error(L, "no function environment for tail call at level %d",
                    level);
  }
}


static int luaB_getfenv (lua_State *L) {
  getfunc(L, 1);
  if (lua_iscfunction(L, -1))  /* is a C function? */
    lua_pushvalue(L, LUA_GLOBALSINDEX);  /* return the thread's global env. */
  else
    lua_getfenv(L, -1);

  if (luaL_getmetafield(L, -1, "__environment") == 0) {
    lua_taintstack(L, lua_getobjecttaint(L, -2));
  }

  return 1;
}


static int luaB_setfenv (lua_State *L) {
  luaL_checktype(L, 2, LUA_TTABLE);
  getfunc(L, 0);
  lua_taintobject(L, -1);
  lua_pushvalue(L, 2);
  if (lua_isnumber(L, 1) && lua_tonumber(L, 1) == 0) {
    /* change environment of current thread */
    lua_pushthread(L);
    lua_insert(L, -2);
    lua_setfenv(L, -2);
    return 0;
  }
  else if (lua_iscfunction(L, -2) || lua_setfenv(L, -2) == 0)
    luaL_error(L,
          LUA_QL("setfenv") " cannot change environment of given object");
  return 1;
}


static int luaB_rawequal (lua_State *L) {
  luaL_checkany(L, 1);
  luaL_checkany(L, 2);
  lua_pushboolean(L, lua_rawequal(L, 1, 2));
  return 1;
}


static int luaB_rawget (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checkany(L, 2);
  lua_settop(L, 2);
  lua_rawget(L, 1);
  return 1;
}

static int luaB_rawset (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checkany(L, 2);
  luaL_checkany(L, 3);
  lua_settop(L, 3);
  lua_rawset(L, 1);
  return 1;
}


static int luaB_gcinfo (lua_State *L) {
  lua_pushinteger(L, lua_getgccount(L));
  return 1;
}


static int luaB_collectgarbage (lua_State *L) {
  static const char *const opts[] = {"stop", "restart", "collect",
    "count", "step", "setpause", "setstepmul", NULL};
  static const int optsnum[] = {LUA_GCSTOP, LUA_GCRESTART, LUA_GCCOLLECT,
    LUA_GCCOUNT, LUA_GCSTEP, LUA_GCSETPAUSE, LUA_GCSETSTEPMUL};
  int o = luaL_checkoption(L, 1, "collect", opts);
  int ex = luaL_optint(L, 2, 0);
  int res = lua_gc(L, optsnum[o], ex);
  switch (optsnum[o]) {
    case LUA_GCCOUNT: {
      int b = lua_gc(L, LUA_GCCOUNTB, 0);
      lua_pushnumber(L, res + ((lua_Number)b/1024));
      return 1;
    }
    case LUA_GCSTEP: {
      lua_pushboolean(L, res);
      return 1;
    }
    default: {
      lua_pushnumber(L, res);
      return 1;
    }
  }
}


static int luaB_type (lua_State *L) {
  luaL_checkany(L, 1);
  lua_pushstring(L, luaL_typename(L, 1));
  return 1;
}


static int luaB_next (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_settop(L, 2);  /* create a 2nd argument if there isn't one */
  if (lua_next(L, 1))
    return 2;
  else {
    lua_pushnil(L);
    return 1;
  }
}


static int luaB_pairs (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_pushvalue(L, lua_upvalueindex(1));  /* return generator, */
  lua_pushvalue(L, 1);  /* state, */
  lua_pushnil(L);  /* and initial value */
  return 3;
}


static int ipairsaux (lua_State *L) {
  int i = luaL_checkint(L, 2);
  luaL_checktype(L, 1, LUA_TTABLE);
  i++;  /* next value */
  lua_pushinteger(L, i);
  lua_rawgeti(L, 1, i);
  return (lua_isnil(L, -1)) ? 0 : 2;
}


static int luaB_ipairs (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_pushvalue(L, lua_upvalueindex(1));  /* return generator, */
  lua_pushvalue(L, 1);  /* state, */
  lua_pushinteger(L, 0);  /* and initial value */
  return 3;
}


static int load_aux (lua_State *L, int status) {
  if (status == 0)  /* OK? */
    return 1;
  else {
    lua_pushnil(L);
    lua_insert(L, -2);  /* put before error message */
    return 2;  /* return nil plus error message */
  }
}


static int luaB_loadstring (lua_State *L) {
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  const char *chunkname = luaL_optstring(L, 2, s);
  lua_setstacktaint(L, LUA_LOADSTRING_TAINT);
  return load_aux(L, luaL_loadbuffer(L, s, l, chunkname));
}


static int luaB_loadstringuntainted (lua_State *L) {
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  const char *chunkname = luaL_optstring(L, 2, s);
  return load_aux(L, luaL_loadbuffer(L, s, l, chunkname));
}


static int luaB_loadfile (lua_State *L) {
  const char *fname = luaL_optstring(L, 1, NULL);
  return load_aux(L, luaL_loadfile(L, fname));
}


/*
** Reader for generic `load' function: `lua_load' uses the
** stack for internal stuff, so the reader cannot change the
** stack top. Instead, it keeps its resulting string in a
** reserved slot inside the stack.
*/
static const char *generic_reader (lua_State *L, void *ud, size_t *size) {
  (void)ud;  /* to avoid warnings */
  luaL_checkstack(L, 2, "too many nested functions");
  lua_pushvalue(L, 1);  /* get function */
  lua_call(L, 0, 1);  /* call it */
  if (lua_isnil(L, -1)) {
    *size = 0;
    return NULL;
  }
  else if (lua_isstring(L, -1)) {
    lua_replace(L, 3);  /* save string in a reserved stack slot */
    return lua_tolstring(L, 3, size);
  }
  else luaL_error(L, "reader function must return a string");
  return NULL;  /* to avoid warnings */
}


static int luaB_load (lua_State *L) {
  int status;
  const char *cname = luaL_optstring(L, 2, "=(load)");
  luaL_checktype(L, 1, LUA_TFUNCTION);
  lua_settop(L, 3);  /* function, eventual name, plus one reserved slot */
  status = lua_load(L, generic_reader, NULL, cname);
  return load_aux(L, status);
}


static int luaB_dofile (lua_State *L) {
  const char *fname = luaL_optstring(L, 1, NULL);
  int n = lua_gettop(L);
  if (luaL_loadfile(L, fname) != 0) lua_error(L);
  lua_call(L, 0, LUA_MULTRET);
  return lua_gettop(L) - n;
}


static int luaB_assert (lua_State *L) {
  luaL_checkany(L, 1);
  if (!lua_toboolean(L, 1))
    return luaL_error(L, "%s", luaL_optstring(L, 2, "assertion failed!"));
  return lua_gettop(L);
}


static int luaB_unpack (lua_State *L) {
  int i, e, n;
  luaL_checktype(L, 1, LUA_TTABLE);
  i = luaL_optint(L, 2, 1);
  e = luaL_opt(L, luaL_checkint, 3, (int) lua_objlen(L, i));
  if (i > e) return 0;  /* empty range */
  n = e - i + 1;  /* number of elements */
  if (n <= 0 || !lua_checkstack(L, n))  /* n <= 0 means arith. overflow */
    return luaL_error(L, "too many results to unpack");
  lua_rawgeti(L, 1, i);  /* push arg[i] (avoiding overflow problems) */
  while (i++ < e)  /* push arg[i + 1...e] */
    lua_rawgeti(L, 1, i);
  return n;
}


static int luaB_select (lua_State *L) {
  int n = lua_gettop(L);
  if (lua_type(L, 1) == LUA_TSTRING && *lua_tostring(L, 1) == '#') {
    lua_pushinteger(L, n-1);
    return 1;
  }
  else {
    int i = luaL_checkint(L, 1);
    if (i < 0) i = n + i;
    else if (i > n) i = n;
    luaL_argcheck(L, 1 <= i, 1, "index out of range");
    return n - i;
  }
}


static int luaB_pcall (lua_State *L) {
  int status;
  luaL_checkany(L, 1);
  status = lua_pcall(L, lua_gettop(L) - 1, LUA_MULTRET, 0);
  lua_pushboolean(L, (status == 0));
  lua_insert(L, 1);
  return lua_gettop(L);  /* return status + all results */
}


static int luaB_xpcall (lua_State *L) {
  int status;
  int n = lua_gettop(L);
  luaL_checkany(L, 2);
  lua_pushvalue(L, 1);  /* exchange function... */
  lua_copy(L, 2, 1);  /* ...and error handler */
  lua_replace(L, 2);
  status = lua_pcall(L, n - 2, LUA_MULTRET, 1);
  lua_pushboolean(L, (status == 0));
  lua_replace(L, 1);
  return lua_gettop(L);  /* return status + all results */
}


static int luaB_tostring (lua_State *L) {
  luaL_checkany(L, 1);
  if (luaL_callmeta(L, 1, "__tostring"))  /* is there a metafield? */
    return 1;  /* use its value */
  switch (lua_type(L, 1)) {
    case LUA_TNUMBER:
      lua_pushstring(L, lua_tostring(L, 1));
      break;
    case LUA_TSTRING:
      lua_pushvalue(L, 1);
      break;
    case LUA_TBOOLEAN:
      lua_pushstring(L, (lua_toboolean(L, 1) ? "true" : "false"));
      break;
    case LUA_TNIL:
      lua_pushliteral(L, "nil");
      break;
    default:
      lua_pushfstring(L, "%s: %p", luaL_typename(L, 1), lua_topointer(L, 1));
      break;
  }
  return 1;
}


static int luaB_newproxy (lua_State *L) {
  lua_settop(L, 1);
  lua_newuserdata(L, 0);  /* create proxy */
  if (lua_toboolean(L, 1) == 0)
    return 1;  /* no metatable */
  else if (lua_isboolean(L, 1)) {
    lua_newtable(L);  /* create a new metatable `m' ... */
    lua_pushvalue(L, -1);  /* ... and mark `m' as a valid metatable */
    lua_pushboolean(L, 1);
    lua_rawset(L, lua_upvalueindex(1));  /* weaktable[m] = true */
  }
  else {
    int validproxy = 0;  /* to check if weaktable[metatable(u)] == true */
    if (lua_getmetatable(L, 1)) {
      lua_rawget(L, lua_upvalueindex(1));
      validproxy = lua_toboolean(L, -1);
      lua_pop(L, 1);  /* remove value */
    }
    luaL_argcheck(L, validproxy, 1, "boolean or proxy expected");
    lua_getmetatable(L, 1);  /* metatable is valid; get it */
  }
  lua_setmetatable(L, 2);
  return 1;
}


static int luaB_forceinsecure (lua_State *L) {
  luaL_forceinsecure(L);
  return 0;
}


static int luaB_issecure (lua_State *L) {
  lua_settop(L, 0);
  lua_pushboolean(L, luaL_issecure(L));
  return 1;
}


static int luaB_issecurevariable (lua_State *L) {
  if (lua_tostring(L, 1)) {
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    lua_insert(L, 1);
  }

  if (!lua_istable(L, 1) || !lua_tostring(L, 2)) {
    return luaL_error(L, "Usage: issecurevariable([table,] \"variable\")");
  }

  lua_settop(L, 2);
  const char *taint = luaL_gettabletaint(L, 1);

  if (taint) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, taint);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }

  return 2;
}


static int luaB_securecall (lua_State *L) {
  lua_TaintState savedts;
  int status;

  lua_savetaint(L, &savedts);

  /* If the supplied function is a string then look up the function in _G. */
  if (lua_tostring(L, 1)) {
    lua_pushvalue(L, 1);
    lua_rawget(L, LUA_GLOBALSINDEX);
    lua_replace(L, 1);
  }

  status = lua_pcall(L, (lua_gettop(L) - 1), LUA_MULTRET, LUA_ERRORHANDLERINDEX);
  if (status != 0) lua_pop(L, 1);
  lua_restoretaint(L, &savedts);
  lua_settoptaint(L, lua_gettop(L), lua_getstacktaint(L));

  return lua_gettop(L);
}


static int luaB_securecallfunction (lua_State *L) {
  luaL_securecall(L, (lua_gettop(L) - 1), LUA_MULTRET, LUA_ERRORHANDLERINDEX);
  return lua_gettop(L);
}


static int luaB_secureexecuterange (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_settop(L, 2);
  luaL_secureforeach(L, 1, LUA_ERRORHANDLERINDEX);
  return 0;
}


static int luaB_geterrorhandler (lua_State *L) {
  lua_pushvalue(L, LUA_ERRORHANDLERINDEX);
  lua_getfenv(L, -1);
  if (luaL_getmetafield(L, -1, "__environment") == 0) lua_settop(L, 1);
  return 1;
}


static int f_errorhandler (lua_State *L) {
  if (!lua_isstring(L, 1)) {
    lua_pushliteral(L, "UNKNOWN ERROR");
    lua_replace(L, 1);
  }

  lua_settop(L, 1);
  lua_pushvalue(L, lua_upvalueindex(1));
  lua_insert(L, 1);
  lua_call(L, 1, 1);
  return 1;
}


static int luaB_seterrorhandler (lua_State *L) {
  if (!lua_isfunction(L, 1)) {
    return luaL_error(L, "Usage: seterrorhandler(errfunc)");
  }

  lua_settop(L, 1);

  /* Create an environment table that references the supplied function. */
  lua_createtable(L, 0, 0);
  lua_createtable(L, 0, 1);
  lua_pushvalue(L, 1);
  lua_setfield(L, 3, "__environment");
  lua_setmetatable(L, 2);
  lua_insert(L, 1);

  /* Bind the error handler proxy and set its environment. */
  lua_pushcclosure(L, f_errorhandler, 1);
  lua_insert(L, 1);
  lua_setfenv(L, 1);
  lua_replace(L, LUA_ERRORHANDLERINDEX);
  return 0;
}


static void aux_getorigfunc_untainted (lua_State *L, void *ud) {
  lua_unused(ud);
  lua_setstacktaint(L, NULL);
  lua_gettable(L, 1);
}


static int luaB_hooksecurefunc (lua_State *L) {
  lua_TaintState savedts;

  if (!lua_istable(L, 1)) {
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    lua_insert(L, 1);
  }

  if (!lua_tostring(L, 2) || !lua_isfunction(L, 3)) {
    return luaL_error(L, "Usage: hooksecurefunc([table,] \"function\", hookfunc)");
  }

  lua_settop(L, 3);
  lua_savetaint(L, &savedts);
  lua_pushvalue(L, 2);  /* [4] = "function" */
  lua_protecttaint(L, aux_getorigfunc_untainted, NULL);  /* [4] = origfunc */
  lua_exchangetaint(L, &savedts);

  if (!lua_isfunction(L, 4)) {
    return luaL_error(L, "hooksecurefunc(): %s is not a function", lua_tostring(L, 2));
  }

  lua_insert(L, 3);  /* [3] = origfunc, [4] = hookfunc */
  lua_setvaluetaint(L, 3, savedts.stacktaint);  /* lua_insert may taint origfunc */
  luaL_createsecurehook(L);  /* [3] = securehook */
  lua_setvaluetaint(L, 3, NULL);
  lua_rawset(L, 1);  /* table["function"] = securehook */

  return 0;
}


static int luaB_scrub (lua_State *L) {
  const int argc = lua_gettop(L);
  int argi;
  int type;

  for (argi = 1; argi <= argc; ++argi) {
    type = lua_type(L, argi);

    if (type != LUA_TNUMBER && type != LUA_TSTRING && type != LUA_TBOOLEAN) {
      lua_pushnil(L);
      lua_replace(L, argi);
    }
  }

  return argc;
}


static const char lua_getprinthandler[] =
  "local LOCAL_printhandler;\n"
  "\n"
  "return function()\n"
  "  return LOCAL_printhandler;\n"
  "end\n";


static const char lua_setprinthandler[] =
  "local LOCAL_printhandler;\n"
  "local error = error;\n"
  "local type = type;\n"
  "\n"
  "return function(func)\n"
  "  if type(func) ~= 'function' then\n"
  "    error('Invalid print handler');\n"
  "  else\n"
  "    LOCAL_printhandler = func;\n"
  "  end\n"
  "end\n";


static const char lua_print[] =
  "local LOCAL_PrintHandler;\n"
  "local forceinsecure = forceinsecure;\n"
  "local geterrorhandler = geterrorhandler;\n"
  "local pcall = pcall;\n"
  "local securecall = securecall;\n"
  "\n"
  "local function print_inner(...)\n"
  "  forceinsecure();\n"
  "  local ok, err = pcall(LOCAL_PrintHandler, ...);\n"
  "  if not ok then\n"
  "    local func = geterrorhandler()\n"
  "    func(err);\n"
  "  end\n"
  "end\n"
  "\n"
  "return function(...)\n"
  "  securecall(pcall, print_inner, ...);\n"
  "end\n";


static const char lua_tostringall[] =
  "local select = select;\n"
  "local tostring = tostring;\n"
  "local unpack = unpack;\n"
  "\n"
  "return function(...)\n"
  "  local n = select('#', ...);\n"
  "  if n == 1 then\n"
  "    return tostring(...);\n"
  "  elseif n == 2 then\n"
  "    return tostring(a), tostring(b);\n"
  "  elseif n == 3 then\n"
  "    return tostring(a), tostring(b), tostring(c);\n"
  "  elseif n == 0 then\n"
  "    return;\n"
  "  else\n"
  "    local temp = { ... };\n"
  "    for i = 1, n do\n"
  "      temp[i] = tostring(temp[i]);\n"
  "    end\n"
  "    return unpack(temp);\n"
  "  end\n"
  "end\n";


/**
 * Base library registration
 */

static const luaL_Reg baselib_shared[] = {
  { .name = "assert", .func = luaB_assert },
  { .name = "collectgarbage", .func = luaB_collectgarbage },
  { .name = "error", .func = luaB_error },
  { .name = "forceinsecure", .func = luaB_forceinsecure },
  { .name = "gcinfo", .func = luaB_gcinfo },
  { .name = "geterrorhandler", .func = luaB_geterrorhandler },
  { .name = "getfenv", .func = luaB_getfenv },
  { .name = "getmetatable", .func = luaB_getmetatable },
  { .name = "hooksecurefunc", .func = luaB_hooksecurefunc },
  { .name = "issecure", .func = luaB_issecure },
  { .name = "issecurevariable", .func = luaB_issecurevariable },
  { .name = "next", .func = luaB_next },
  { .name = "pcall", .func = luaB_pcall },
  { .name = "rawequal", .func = luaB_rawequal },
  { .name = "rawget", .func = luaB_rawget },
  { .name = "rawset", .func = luaB_rawset },
  { .name = "scrub", .func = luaB_scrub },
  { .name = "securecall", .func = luaB_securecall },
  { .name = "securecallfunction", .func = luaB_securecallfunction },
  { .name = "secureexecuterange", .func = luaB_secureexecuterange },
  { .name = "select", .func = luaB_select },
  { .name = "seterrorhandler", .func = luaB_seterrorhandler },
  { .name = "setfenv", .func = luaB_setfenv },
  { .name = "setmetatable", .func = luaB_setmetatable },
  { .name = "tonumber", .func = luaB_tonumber },
  { .name = "tostring", .func = luaB_tostring },
  { .name = "type", .func = luaB_type },
  { .name = "unpack", .func = luaB_unpack },
  { .name = "xpcall", .func = luaB_xpcall },
  { .name = NULL, .func = NULL },
};



static const luaL_Reg baselib_lua[] = {
  { .name = "dofile", .func = luaB_dofile },
  { .name = "load", .func = luaB_load },
  { .name = "loadfile", .func = luaB_loadfile },
  { .name = "loadstring_untainted", .func = luaB_loadstringuntainted },
  { .name = "loadstring", .func = luaB_loadstringuntainted },
  { .name = "print", .func = luaB_print },
  { .name = NULL, .func = NULL },
};


static const luaL_Reg baselib_wow[] = {
  { .name = "loadstring", .func = luaB_loadstring },
  { .name = NULL, .func = NULL },
};


static void baselib_openshared (lua_State *L) {
  /* register shared functions */
  luaL_setfuncs(L, baselib_shared, 0);

  /* register `_VERSION' field */
  lua_pushliteral(L, LUA_VERSION);
  lua_setglobal(L, "_VERSION");

  /* `ipairs' and `pairs' need auxiliary functions as upvalues */
  lua_pushcclosure(L, ipairsaux, 0);
  lua_pushcclosure(L, luaB_ipairs, 1);
  lua_setfield(L, -2, "ipairs");

  lua_pushcclosure(L, luaB_next, 0);
  lua_pushcclosure(L, luaB_pairs, 1);
  lua_setfield(L, -2, "pairs");

  /* `newproxy' needs a weaktable as upvalue */
  lua_createtable(L, 0, 1);  /* new table `w' */
  lua_pushvalue(L, -1);  /* `w' will be its own metatable */
  lua_setmetatable(L, -2);
  lua_pushliteral(L, "kv");
  lua_setfield(L, -2, "__mode");  /* metatable(w).__mode = "kv" */
  lua_pushcclosure(L, luaB_newproxy, 1);
  lua_setglobal(L, "newproxy");  /* set global `newproxy' */

  /* register Lua functions */
  luaL_dobuffer(L, lua_tostringall, "print.lua");
  lua_setfield(L, -2, "tostringall");
}


LUALIB_API int luaopen_base (lua_State *L) {
  /* set global '_G' */
  lua_pushvalue(L, LUA_GLOBALSINDEX);
  lua_setfield(L, LUA_GLOBALSINDEX, LUA_BASELIBNAME);

  /* register '_G' base library */
  luaL_register(L, LUA_BASELIBNAME, baselib_lua);
  baselib_openshared(L);

  /* open coroutine library for backwards compatibility */
  luaopen_coroutine(L);

  return 2;
}


LUALIB_API int luaopen_wow_base (lua_State *L) {
  /* set field '_G' */
  lua_pushvalue(L, LUA_ENVIRONINDEX);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, LUA_BASELIBNAME);

  /* register '_G' base library */
  luaL_setfuncs(L, baselib_wow, 0);
  baselib_openshared(L);

  /* register print infrastructure */
  luaL_dobuffer(L, lua_getprinthandler, "print.lua");
  lua_pushcclosure(L, luaB_print, 0);
  lua_setupvalue(L, -2, 1);  /* set default print handler */
  luaL_dobuffer(L, lua_setprinthandler, "print.lua");
  luaL_dobuffer(L, lua_print, "print.lua");

  /* join upvalues of 'LOCAL_PrintHandler' between loaded functions */
  lua_getupvalue(L, -1, 3);  /* push 'print_inner' upvalue from 'print' */
  lua_upvaluejoin(L, -1, 3, -4, 1);  /* join 'print_inner' to 'getprinthandler' */
  lua_upvaluejoin(L, -3, 3, -4, 1);  /* join 'setprinthandler' to 'getprinthandler'  */
  lua_pop(L, 1); /* pop 'print_inner' */

  lua_setfield(L, -4, "print");
  lua_setfield(L, -3, "setprinthandler");
  lua_setfield(L, -2, "getprinthandler");

  return 1;
}
