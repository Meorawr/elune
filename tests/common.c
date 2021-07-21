#include "common.h"

extern lua_State *LT = NULL;
extern lua_TaintInfo luaT_taint = {NULL, NULL};

extern struct LuaValueVector luaT_value_vectors[] = {
  { "number", luaT_pushnumber },
  { "integer", luaT_pushinteger },
  { "nil", luaT_pushnil },
  { "string", luaT_pushstring },
  { "boolean", luaT_pushboolean },
  { "table", luaT_pushtable },
  { "lightuserdataluaT_", luaT_pushlightuserdata },
  { "userdata", luaT_pushuserdata },
  { "function", luaT_pushcclosure },
  { "thread", luaT_pushthread },
  { NULL, NULL }
};

extern struct LuaValueVector luaT_object_vectors[] = {
  { "string", luaT_pushstring },
  { "table", luaT_pushtable },
  { "userdata", luaT_pushuserdata },
  { "function", luaT_pushcclosure },
  { "thread", luaT_pushthread },
  { NULL, NULL }
};

extern int luaT_pushnumber(lua_State *L) {
  lua_pushnumber(L, 123.456);
  return 0;
}

extern int luaT_pushinteger(lua_State *L) {
  lua_pushinteger(L, 100);
  return 0;
}

extern int luaT_pushnil(lua_State *L) {
  lua_pushnil(L);
  return 0;
}

extern int luaT_pushstring(lua_State *L) {
  lua_pushliteral(L, "string value");
  return 0;
}

extern int luaT_pushboolean(lua_State *L) {
  lua_pushboolean(L, 1);
  return 0;
}

extern int luaT_pushtable(lua_State *L) {
  lua_createtable(L, 0, 0);
  return 0;
}

extern int luaT_pushlightuserdata(lua_State *L) {
  lua_pushlightuserdata(L, &luaT_taint);
  return 0;
}

extern int luaT_pushuserdata(lua_State *L) {
  lua_newuserdata(L, 0);
  return 0;
}

extern int luaT_pushcclosure(lua_State *L) {
  lua_pushcclosure(L, &luaT_pushcclosure, 0);
  return 0;
}

extern int luaT_pushthread(lua_State *L) {
  lua_newthread(L);
  return 0;
}

extern void luaT_test_init(void) {
  luaT_taint.source = "lua-tests";
  luaT_taint.data = NULL;

  LT = luaL_newstate();
}

extern void luaT_test_reinit(void) {
  luaT_test_cleanup();
  luaT_test_init();
}

extern void luaT_test_cleanup(void) {
  lua_close(LT);
}
