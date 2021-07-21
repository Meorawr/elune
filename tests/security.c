#include "common.h"

#include <acutest.h>

/**
 * Basic State Taint Tests
 */

void test_startup_taint(void) {
  TEST_CHECK(lua_gettaint(LT) == NULL);
  TEST_CHECK(luaL_issecure(LT));
}

void test_tainted(void) {
  lua_settaint(LT, &luaT_taint);
  TEST_CHECK(lua_gettaint(LT) == &luaT_taint);
  TEST_CHECK(!luaL_issecure(LT));
}

void test_forced_taint(void) {
  luaL_forcetaint(LT);
  TEST_CHECK(lua_gettaint(LT) != NULL);
}

void test_forced_taint_override(void) {
  lua_settaint(LT, &luaT_taint);
  luaL_forcetaint(LT);
  TEST_CHECK(lua_gettaint(LT) == &luaT_taint);
}

/**
 * State => Value and Object Taint Propagation Tests
 */

void test_push_values_with_taint(void) {
  for (struct LuaValueVector *vec = luaT_value_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    luaT_test_reinit();
    lua_settaint(LT, &luaT_taint);
    vec->func(LT);
    TEST_CHECK(lua_getvaluetaint(LT, 1) != NULL);
    TEST_CHECK(!luaL_issecurevalue(LT, 1));
  }
}

void test_push_values_without_taint(void) {
  for (struct LuaValueVector *vec = luaT_value_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    luaT_test_reinit();
    vec->func(LT);
    TEST_CHECK(lua_getvaluetaint(LT, 1) == NULL);
    TEST_CHECK(luaL_issecurevalue(LT, 1));
  }
}

void test_push_objects_with_taint(void) {
  for (struct LuaValueVector *vec = luaT_object_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    luaT_test_reinit();
    lua_settaint(LT, &luaT_taint);
    vec->func(LT);
    TEST_CHECK(lua_getobjecttaint(LT, 1) != NULL);
    TEST_CHECK(!luaL_issecureobject(LT, 1));
  }
}

void test_push_objects_without_taint(void) {
  for (struct LuaValueVector *vec = luaT_object_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    luaT_test_reinit();
    vec->func(LT);
    TEST_CHECK(lua_getobjecttaint(LT, 1) == NULL);
    TEST_CHECK(luaL_issecureobject(LT, 1));
  }
}

void test_tainted_table_structures(void) {
  lua_settaint(LT, &luaT_taint);

  // Create a color-like table on the stack.
  lua_createtable(LT, 0, 3);
  lua_pushnumber(LT, 0.3);
  lua_setfield(LT, 1, "r");
  lua_pushnumber(LT, 0.6);
  lua_setfield(LT, 1, "g");
  lua_pushnumber(LT, 0.9);
  lua_setfield(LT, 1, "b");

  // Expect all fields to be tainted. This is to mimic behaviors like calling
  // C_ClassColor.GetClassColor ingame which returns a table with tainted
  // keys.

  lua_settaint(LT, NULL);
  lua_getfield(LT, 1, "r");
  TEST_CHECK(!luaL_issecure(LT));
  TEST_CHECK(!luaL_issecurevalue(LT, -1));
  lua_pop(LT, 1);

  lua_settaint(LT, NULL);
  lua_getfield(LT, 1, "g");
  TEST_CHECK(!luaL_issecure(LT));
  TEST_CHECK(!luaL_issecurevalue(LT, -1));
  lua_pop(LT, 1);

  lua_settaint(LT, NULL);
  lua_getfield(LT, 1, "b");
  TEST_CHECK(!luaL_issecure(LT));
  TEST_CHECK(!luaL_issecurevalue(LT, -1));
  lua_pop(LT, 1);
}

/**
 * Value => State Taint Propagation Tests
 */

void test_secure_value_reads(void) {
  lua_pushboolean(LT, 1);
  lua_setfield(LT, LUA_GLOBALSINDEX, "SecureGlobal");

  luaL_forcetaint(LT);
  lua_getfield(LT, LUA_GLOBALSINDEX, "SecureGlobal");

  TEST_CHECK(!luaL_issecure(LT));
  TEST_CHECK(!luaL_issecurevalue(LT, 1));
}

/**
 * Test Listing
 */

TEST_LIST = {
  { "initially secure on state creation", test_startup_taint },
  { "taint can be applied to state", test_tainted },
  { "taint can be forced", test_forced_taint },
  { "forced taint doesn't override existing taint", test_forced_taint_override },
  { "pushed values inherit state taint", test_push_values_with_taint },
  { "pushed values aren't tainted by default", test_push_values_without_taint },
  { "pushed objects inherit state taint", test_push_objects_with_taint },
  { "pushed objects aren't tainted by default", test_push_objects_without_taint },
  { "tainted table structures", test_tainted_table_structures },
  { "read secure values while tainted", test_secure_value_reads },
  { NULL, NULL }
};
