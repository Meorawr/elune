#include "common.h"

#include <acutest.h>

/**
 * Basic State Taint Tests
 */

static void test_startup_taint(void) {
  TEST_CHECK(lua_getthreadtaint(LT) == NULL);
  TEST_CHECK(luaL_issecurethread(LT));
}

static void test_tainted(void) {
  lua_setthreadtaint(LT, &luaT_taint);
  TEST_CHECK(lua_getthreadtaint(LT) == &luaT_taint);
  TEST_CHECK(!luaL_issecurethread(LT));
}

static void test_forced_taint(void) {
  luaL_forcetaintthread(LT);
  TEST_CHECK(lua_getthreadtaint(LT) != NULL);
}

static void test_forced_taint_override(void) {
  lua_setthreadtaint(LT, &luaT_taint);
  luaL_forcetaintthread(LT);
  TEST_CHECK(lua_getthreadtaint(LT) == &luaT_taint);
}

/**
 * State => Value and Object Taint Propagation Tests
 */

static void test_push_values_with_taint(void) {
  for (struct LuaValueVector *vec = luaT_value_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    luaT_test_reinit();
    lua_setthreadtaint(LT, &luaT_taint);
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
    lua_setthreadtaint(LT, &luaT_taint);
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
  lua_setthreadtaint(LT, &luaT_taint);

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

  lua_setthreadtaint(LT, NULL);
  lua_getfield(LT, 1, "r");
  TEST_CHECK(!luaL_issecurethread(LT));
  TEST_CHECK(!luaL_issecurevalue(LT, -1));
  lua_pop(LT, 1);

  lua_setthreadtaint(LT, NULL);
  lua_getfield(LT, 1, "g");
  TEST_CHECK(!luaL_issecurethread(LT));
  TEST_CHECK(!luaL_issecurevalue(LT, -1));
  lua_pop(LT, 1);

  lua_setthreadtaint(LT, NULL);
  lua_getfield(LT, 1, "b");
  TEST_CHECK(!luaL_issecurethread(LT));
  TEST_CHECK(!luaL_issecurevalue(LT, -1));
  lua_pop(LT, 1);
}

/**
 * Value => State Taint Propagation Tests
 */

static void test_secure_value_reads(void) {
  lua_pushboolean(LT, 1);
  lua_setfield(LT, LUA_GLOBALSINDEX, "SecureGlobal");

  luaL_forcetaintthread(LT);
  lua_getfield(LT, LUA_GLOBALSINDEX, "SecureGlobal");

  TEST_CHECK(!luaL_issecurethread(LT));
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

  lua_setthreadtaint(LT, &luaT_taint);

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
 * Lua Security API tests
 */

static void test_securecall_initiallysecure(void) {
  luaT_loadfixture(LT, luac_securecallaux);

  lua_pushliteral(LT, "securecall_identity");
  luaT_pushstring(LT);
  luaT_pushnumber(LT);
  luaT_pushboolean(LT);
  luaT_pushtable(LT);
  TEST_CHECK(lua_securecall(LT, 4, LUA_MULTRET, 0) == 0);

  TEST_CHECK(luaL_issecurethread(LT));      /* Should still be secure. */
  TEST_CHECK(lua_gettop(LT) == 4);          /* Should have four return values. */
  TEST_CHECK(luaL_issecurevalue(LT, -1));   /* All function returns should be secure. */
  TEST_CHECK(luaL_issecurevalue(LT, -1));
  TEST_CHECK(luaL_issecurevalue(LT, -1));
  TEST_CHECK(luaL_issecurevalue(LT, -1));
}

static void test_securecall_initiallyinsecure(void) {
  luaT_loadfixture(LT, luac_securecallaux);

  lua_setthreadtaint(LT, &luaT_taint);      /* Taint execution */
  lua_pushliteral(LT, "securecall_identity");
  luaT_pushstring(LT);
  luaT_pushnumber(LT);
  luaT_pushboolean(LT);
  luaT_pushtable(LT);
  TEST_CHECK(lua_securecall(LT, 4, LUA_MULTRET, 0) == 0);

  TEST_CHECK(!luaL_issecurethread(LT));     /* Should not be secure. */
  TEST_CHECK(lua_gettop(LT) == 4);          /* Should have four return values. */
  TEST_CHECK(!luaL_issecurevalue(LT, -1));  /* All function returns should be insecure. */
  TEST_CHECK(!luaL_issecurevalue(LT, -1));
  TEST_CHECK(!luaL_issecurevalue(LT, -1));
  TEST_CHECK(!luaL_issecurevalue(LT, -1));
}

static void test_securecall_insecurefunction(void) {
  lua_setthreadtaint(LT, &luaT_taint);            /* Load fixture insecurely. */
  luaT_loadfixture(LT, luac_securecallaux);
  lua_setthreadtaint(LT, NULL);

  TEST_CHECK(luaL_issecurethread(LT));      /* Should be secure at this point. */

  lua_pushliteral(LT, "securecall_identity");
  luaT_pushstring(LT);
  luaT_pushnumber(LT);
  luaT_pushboolean(LT);
  luaT_pushtable(LT);
  TEST_CHECK(lua_securecall(LT, 4, LUA_MULTRET, 0) == 0);

  TEST_CHECK(luaL_issecurethread(LT));      /* Should still be secure. */
  TEST_CHECK(lua_gettop(LT) == 4);          /* Should have four return values. */
  TEST_CHECK(luaL_issecurevalue(LT, -1));   /* All function returns should be secure. */
  TEST_CHECK(luaL_issecurevalue(LT, -1));
  TEST_CHECK(luaL_issecurevalue(LT, -1));
  TEST_CHECK(luaL_issecurevalue(LT, -1));
}

static void test_securecall_forceinsecure(void) {
  luaT_loadfixture(LT, luac_securecallaux);

  lua_pushliteral(LT, "securecall_forceinsecure");
  luaT_pushstring(LT);
  luaT_pushnumber(LT);
  luaT_pushboolean(LT);
  luaT_pushtable(LT);
  TEST_CHECK(lua_securecall(LT, 4, LUA_MULTRET, 0) == 0);

  TEST_CHECK(luaL_issecurethread(LT));      /* Should still be secure. */
  TEST_CHECK(lua_gettop(LT) == 4);          /* Should have four return values. */
  TEST_CHECK(luaL_issecurevalue(LT, -1));   /* All function returns should be secure. */
  TEST_CHECK(luaL_issecurevalue(LT, -1));
  TEST_CHECK(luaL_issecurevalue(LT, -1));
  TEST_CHECK(luaL_issecurevalue(LT, -1));
}

static void test_securecall_directsecurefunction(void) {
  luaT_loadfixture(LT, luac_securecallaux);

  TEST_CHECK(luaL_issecurethread(LT));      /* Should be secure at this point. */

  lua_pushvalue(LT, LUA_GLOBALSINDEX);
  lua_pushliteral(LT, "securecall_identity");
  lua_gettable(LT, -2);
  lua_remove(LT, 1);                        /* Leave only the function on the stack. */

  luaT_pushstring(LT);
  luaT_pushnumber(LT);
  luaT_pushboolean(LT);
  luaT_pushtable(LT);
  TEST_CHECK(lua_securecall(LT, 4, LUA_MULTRET, 0) == 0);

  TEST_CHECK(luaL_issecurethread(LT));      /* Should still be secure. */
  TEST_CHECK(lua_gettop(LT) == 4);          /* Should have four return values. */
  TEST_CHECK(luaL_issecurevalue(LT, -1));   /* All function returns should be secure. */
  TEST_CHECK(luaL_issecurevalue(LT, -1));
  TEST_CHECK(luaL_issecurevalue(LT, -1));
  TEST_CHECK(luaL_issecurevalue(LT, -1));
}

static void test_securecall_directinsecurefunction(void) {
  lua_setthreadtaint(LT, &luaT_taint);            /* Load fixture insecurely. */
  luaT_loadfixture(LT, luac_securecallaux);

  lua_pushvalue(LT, LUA_GLOBALSINDEX);
  lua_pushliteral(LT, "securecall_identity");
  lua_gettable(LT, -2);
  lua_remove(LT, 1);                        /* Leave only the function on the stack. */
  lua_setthreadtaint(LT, NULL);
  TEST_CHECK(luaL_issecurethread(LT));      /* Should be secure at this point. */
  TEST_CHECK(!luaL_issecurevalue(LT, -1));  /* Function value should be insecure. */

  luaT_pushstring(LT);
  luaT_pushnumber(LT);
  luaT_pushboolean(LT);
  luaT_pushtable(LT);
  TEST_CHECK(lua_securecall(LT, 4, LUA_MULTRET, 0) == 0);

  TEST_CHECK(luaL_issecurethread(LT));      /* Should still be secure. */
  TEST_CHECK(lua_gettop(LT) == 4);          /* Should have four return values. */
  TEST_CHECK(luaL_issecurevalue(LT, -1));   /* All function returns should be secure. */
  TEST_CHECK(luaL_issecurevalue(LT, -1));
  TEST_CHECK(luaL_issecurevalue(LT, -1));
  TEST_CHECK(luaL_issecurevalue(LT, -1));
}

static int secureerrorhandler(lua_State *L) {
  TEST_CHECK(lua_gettop(L) == 1);          /* Expect only error information on the stack */
  TEST_CHECK(luaL_issecurethread(L));      /* Expect to have been called securely... */
  TEST_CHECK(luaL_issecurevalue(L, 1));    /* ...And that our one argument is secure. */
  return 1;
}

static void test_securecall_secureerror(void) {
  luaT_loadfixture(LT, luac_securecallaux);

  lua_pushcclosure(LT, secureerrorhandler, 0);    /* See securerrrorhandler for additional checks. */
  lua_pushliteral(LT, "securecall_error");
  lua_pushliteral(LT, "test error message");
  TEST_CHECK(lua_securecall(LT, 1, 0, 1) != 0);   /* Expect call to fail. */
  TEST_CHECK(luaL_issecurethread(LT));                  /* Expect to remain secure. */
  TEST_CHECK(lua_gettop(LT) == 2);                /* Expect two values on stack; errfunc and err */
  TEST_CHECK(luaL_issecurevalue(LT, -1));         /* Error object should be secure */
}

static int insecureerrorhandler(lua_State *L) {
  TEST_CHECK(lua_gettop(L) == 1);          /* Expect only error information on the stack */
  TEST_CHECK(!luaL_issecurethread(L));     /* Expect to have been called insecurely... */
  TEST_CHECK(!luaL_issecurevalue(L, 1));   /* ...And that our one argument is insecure. */
  return 1;
}

static void test_securecall_insecureerror(void) {
  luaT_loadfixture(LT, luac_securecallaux);

  lua_setthreadtaint(LT, &luaT_taint);
  lua_pushcclosure(LT, insecureerrorhandler, 0);  /* See insecurerrorhandler for additional checks. */
  lua_pushliteral(LT, "securecall_error");
  lua_pushliteral(LT, "test error message");
  TEST_CHECK(lua_securecall(LT, 1, 0, 1) != 0);   /* Expect call to fail. */
  TEST_CHECK(!luaL_issecurethread(LT));            /* Expect to remain insecure. */
  TEST_CHECK(lua_gettop(LT) == 2);                /* Expect two values on stack; errfunc and err */
  TEST_CHECK(!luaL_issecurevalue(LT, -1));        /* Error object should be insecure */
}

static void test_securecall_forceinsecureerror(void) {
  luaT_loadfixture(LT, luac_securecallaux);

  lua_pushcclosure(LT, insecureerrorhandler, 0);  /* See insecurerrorhandler for additional checks. */
  lua_pushliteral(LT, "securecall_insecureerror");
  lua_pushliteral(LT, "test error message");
  TEST_CHECK(lua_securecall(LT, 1, 0, 1) != 0);   /* Expect call to fail. */
  TEST_CHECK(luaL_issecurethread(LT));            /* Expect to remain secure. */
  TEST_CHECK(lua_gettop(LT) == 2);                /* Expect two values on stack; errfunc and err */
  TEST_CHECK(luaL_issecurevalue(LT, -1));         /* Error object should be secure */
}

/**
 * VM Script Tests
 */

static void test_vm_arith(void) {
  luaT_loadfixture(LT, luac_lvmutil);
  luaT_loadfixture(LT, luac_lvmarith);
}

static void test_vm_fields(void) {
  luaT_loadfixture(LT, luac_lvmutil);
  luaT_loadfixture(LT, luac_lvmfields);
}

static void test_vm_globals(void) {
  luaT_loadfixture(LT, luac_lvmutil);
  luaT_loadfixture(LT, luac_lvmglobals);
}

static void test_vm_locals(void) {
  luaT_loadfixture(LT, luac_lvmutil);
  luaT_loadfixture(LT, luac_lvmlocals);
}

static void test_vm_upvalues(void) {
  luaT_loadfixture(LT, luac_lvmutil);
  luaT_loadfixture(LT, luac_lvmupvalues);
}

/**
 * Library Extension Tests
 */

static void test_script_dblib(void) {
  luaT_loadfixture(LT, luac_lvmutil);
  luaT_loadfixture(LT, luac_ldblib);
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
  { "lua_securecall: named call while secure", &test_securecall_initiallysecure },
  { "lua_securecall: named call while insecure", &test_securecall_initiallyinsecure },
  { "lua_securecall: named call insecure function", &test_securecall_insecurefunction },
  { "lua_securecall: named call forceinsecure", &test_securecall_forceinsecure },
  { "lua_securecall: direct call secure function value", &test_securecall_directsecurefunction },
  { "lua_securecall: direct call insecure function value", &test_securecall_directinsecurefunction },
  { "lua_securecall: secure error handling", &test_securecall_secureerror },
  { "lua_securecall: insecure error handling", &test_securecall_insecureerror },
  { "lua_securecall: forceinsecure error handling", &test_securecall_forceinsecureerror },
  { "vm: arithmetic ops", &test_vm_arith },
  { "vm: field taint", &test_vm_fields },
  { "vm: global variable taint", &test_vm_globals },
  { "vm: local value taint", &test_vm_locals },
  { "vm: upvalue taint", &test_vm_upvalues },
  { "lib: debug library extensions", &test_script_dblib },
  { NULL, NULL }
};
