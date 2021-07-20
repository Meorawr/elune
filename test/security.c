#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <acutest.h>

static lua_TaintInfo test_taint = { "Test Taint", NULL };

void test_startup_taint(void) {
  lua_State *L = luaL_newstate();
  TEST_CHECK(lua_gettaint(L) == NULL);
  TEST_CHECK(luaL_issecure(L));
  lua_close(L);
}

void test_tainted(void) {
  lua_State *L = luaL_newstate();
  lua_settaint(L, &test_taint);
  TEST_CHECK(lua_gettaint(L) == &test_taint);
  TEST_CHECK(!luaL_issecure(L));
  lua_close(L);
}

void test_forced_taint(void) {
  lua_State *L = luaL_newstate();
  luaL_forcetaint(L);
  TEST_CHECK(lua_gettaint(L) != NULL);
  lua_close(L);
}

void test_forced_taint_override(void) {
  lua_State *L = luaL_newstate();
  lua_settaint(L, &test_taint);
  luaL_forcetaint(L);
  TEST_CHECK(lua_gettaint(L) == &test_taint);
  lua_close(L);
}

TEST_LIST = {
  { "initially secure on state creation", test_startup_taint },
  { "taint can be applied to state", test_tainted },
  { "taint can be forced", test_forced_taint },
  { "forced taint doesn't override existing taint", test_forced_taint_override },
  { NULL, NULL }
};
