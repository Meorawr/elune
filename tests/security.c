#include "common.h"

#include <acutest.h>

/**
 * Basic State Taint Tests
 */

static void test_startup_taint(void) {
  TEST_CHECK(lua_gettaint(LT) == NULL);
  TEST_CHECK(luaL_issecure(LT));
}

static void test_tainted(void) {
  lua_settaint(LT, &luaT_taint);
  TEST_CHECK(lua_gettaint(LT) == &luaT_taint);
  TEST_CHECK(!luaL_issecure(LT));
}

static void test_forced_taint(void) {
  luaL_forcetaint(LT);
  TEST_CHECK(lua_gettaint(LT) != NULL);
}

static void test_forced_taint_override(void) {
  lua_settaint(LT, &luaT_taint);
  luaL_forcetaint(LT);
  TEST_CHECK(lua_gettaint(LT) == &luaT_taint);
}

/**
 * State => Value and Object Taint Propagation Tests
 */

static void test_push_values_with_taint(void) {
  for (struct LuaValueVector *vec = luaT_value_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    luaT_test_reinit();
    lua_settaint(LT, &luaT_taint);
    vec->func(LT);
    TEST_CHECK(lua_getvaluetaint(LT, 1) != NULL);
    TEST_CHECK(!luaL_issecurevalue(LT, 1));
  }
}

static void test_push_values_without_taint(void) {
  for (struct LuaValueVector *vec = luaT_value_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    luaT_test_reinit();
    vec->func(LT);
    TEST_CHECK(lua_getvaluetaint(LT, 1) == NULL);
    TEST_CHECK(luaL_issecurevalue(LT, 1));
  }
}

static void test_push_objects_with_taint(void) {
  for (struct LuaValueVector *vec = luaT_object_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    luaT_test_reinit();
    lua_settaint(LT, &luaT_taint);
    vec->func(LT);
    TEST_CHECK(lua_getobjecttaint(LT, 1) != NULL);
    TEST_CHECK(!luaL_issecureobject(LT, 1));
  }
}

static void test_push_objects_without_taint(void) {
  for (struct LuaValueVector *vec = luaT_object_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    luaT_test_reinit();
    vec->func(LT);
    TEST_CHECK(lua_getobjecttaint(LT, 1) == NULL);
    TEST_CHECK(luaL_issecureobject(LT, 1));
  }
}

static void test_tainted_table_structures(void) {
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

static void test_secure_value_reads(void) {
  lua_pushboolean(LT, 1);
  lua_setfield(LT, LUA_GLOBALSINDEX, "SecureGlobal");

  luaL_forcetaint(LT);
  lua_getfield(LT, LUA_GLOBALSINDEX, "SecureGlobal");

  TEST_CHECK(!luaL_issecure(LT));
  TEST_CHECK(!luaL_issecurevalue(LT, 1));
}

/**
 * Script Constant Tests
 */

static void test_secure_constants(void) {
  /**
   * This test is a bit of an anomaly in the whole taint handling process
   * and largely dictates why things have been implemented the way that they
   * are.
   *
   * The core behavior we're attempting to replicate here is that given a
   * securely loaded script which defines a function that assigns a constant
   * value to a fixed table key, if the call to that function occurs
   * insecurely then taint is _not_ applied to the field.
   *
   * This can be seen ingame with the following insecurely executed script:
   *
   *    local rect = CreateFromMixins(RectangleMixin)
   *    rect:OnLoad()
   *    assert(rect.left == 0, "expected 'rect.left' to be zero")
   *    assert(not issecurevariable(rect, "left"), "expected 'rect.left' to be insecure")
   *    rect:Reset()
   *    assert(rect.left == 0, "expected 'rect.left' to be zero")
   *    assert(issecurevariable(rect, "left"), "expected 'rect.left' to be secure")
   *
   * The Reset method in question does the following assignments:
   *
	 *    self.left = 0.0;
	 *    self.right = 0.0;
	 *    self.top = 0.0;
	 *    self.bottom = 0.0;
   *
   * These end up securely overwriting any existing fields of the same name
   * in `self` always, even if called from an insecure context.
   *
   * The behavior seen here implies that either taint isn't "strict"; if we
   * implement taint through modifications to the setobj macro(s) to taint
   * values based on the taint present in the state, these would end up
   * insecurely assigned. Similarly this also means that taint shouldn't
   * be propagated in the luaV_settable function, since the above snippet
   * only generates SETTABLE opcodes.
   */

  luaT_loadfixture(LT, luac_mixin);
  luaT_loadfixture(LT, luac_rectanglemixin);

  lua_settaint(LT, &luaT_taint);

  lua_getfield(LT, LUA_GLOBALSINDEX, "CreateFromMixins");
  lua_getfield(LT, LUA_GLOBALSINDEX, "RectangleMixin");
  luaT_checkcall(LT, 1, 1);                         /* rect = CreateFromMixins(RectangleMixin) */

  lua_getfield(LT, -1, "OnLoad");
  lua_pushvalue(LT, -2);
  luaT_checkcall(LT, 1, 0);                         /* rect:OnLoad() */
  TEST_CHECK(!luaL_issecurefield(LT, -1, "left"));  /* rect.left should be insecure */

  lua_getfield(LT, -1, "Reset");
  lua_pushvalue(LT, -2);
  luaT_checkcall(LT, 1, 0);                         /* rect:Reset() */
  TEST_CHECK(luaL_issecurefield(LT, -1, "left"));   /* rect.left should be secure */
}

/**
 * Script Tests
 */

static void test_issecurevariable(void) {
  luaT_loadfixture(LT, luac_issecurevariable);
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
  { "secure constants don't inherit state taint", &test_secure_constants },
  { "script: issecurevariable", &test_issecurevariable },
  { NULL, NULL }
};
