#include "common.h"

#define TEST_NO_MAIN
#include <acutest.h>

lua_State *LT = NULL;
lua_TaintInfo luaT_taint = {NULL, NULL};

struct LuaValueVector luaT_value_vectors[] = {
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

struct LuaValueVector luaT_object_vectors[] = {
  { "string", luaT_pushstring },
  { "table", luaT_pushtable },
  { "userdata", luaT_pushuserdata },
  { "function", luaT_pushcclosure },
  { "thread", luaT_pushthread },
  { NULL, NULL }
};

int luaT_pushnumber(lua_State *L) {
  lua_pushnumber(L, 123.456);
  return 0;
}

int luaT_pushinteger(lua_State *L) {
  lua_pushinteger(L, 100);
  return 0;
}

int luaT_pushnil(lua_State *L) {
  lua_pushnil(L);
  return 0;
}

int luaT_pushstring(lua_State *L) {
  lua_pushliteral(L, "string value");
  return 0;
}

int luaT_pushboolean(lua_State *L) {
  lua_pushboolean(L, 1);
  return 0;
}

int luaT_pushtable(lua_State *L) {
  lua_createtable(L, 0, 0);
  return 0;
}

int luaT_pushlightuserdata(lua_State *L) {
  lua_pushlightuserdata(L, &luaT_taint);
  return 0;
}

int luaT_pushuserdata(lua_State *L) {
  lua_newuserdata(L, 0);
  return 0;
}

int luaT_pushcclosure(lua_State *L) {
  lua_pushcclosure(L, &luaT_pushcclosure, 0);
  return 0;
}

int luaT_pushthread(lua_State *L) {
  lua_newthread(L);
  return 0;
}

int luaT_loadfixtures(lua_State *L) {
  #define luaT_fixture2addr(name) &name,
  const luaT_Fixture* fixtures[] = { luaT_fixtures(luaT_fixture2addr) };
  #undef luaT_fixture2addr

  for (int i = 0; i < luaT_extentof(fixtures); ++i) {
    const luaT_Fixture *f = fixtures[i];

    int status = luaL_loadbuffer(L, f->data, f->size, NULL);

    if (!TEST_CHECK(status == 0)) {
      TEST_MSG(lua_tostring(L, -1));
      lua_pop(L, 1);
    } else if (!TEST_CHECK(lua_pcall(L, 0, 0, 0) != 0)) {
      TEST_MSG(lua_tostring(L, -1));
      lua_pop(L, 1);
    }
  }

  return 0;
}

void luaT_test_init(void) {
  luaT_taint.source = "lua-tests";
  luaT_taint.data = NULL;

  LT = luaL_newstate();
  luaL_openlibs(LT);
}

void luaT_test_reinit(void) {
  luaT_test_cleanup();
  luaT_test_init();
}

void luaT_test_cleanup(void) {
  lua_close(LT);
}
