#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <acutest.h>

/****************************************************************************
 * Test Utilities
 ****************************************************************************/

static lua_TaintInfo test_taint = { "Test Taint", NULL };

static int push_number(lua_State *L) { lua_pushnumber(L, 123.456); return 0; }
static int push_integer(lua_State *L) { lua_pushinteger(L, 100); return 0; }
static int push_nil(lua_State *L) { lua_pushnil(L); return 0; }
static int push_string(lua_State *L) { lua_pushliteral(L, "string value"); return 0; }
static int push_boolean(lua_State *L) { lua_pushboolean(L, 1); return 0; }
static int push_table(lua_State *L) { lua_createtable(L, 0, 0); return 0; }
static int push_lightuserdata(lua_State *L) { lua_pushlightuserdata(L, &test_taint); return 0; }
static int push_userdata(lua_State *L) { lua_newuserdata(L, 0); return 0; }
static int push_cclosure(lua_State *L) { lua_pushcclosure(L, &push_cclosure, 0); return 0; }
static int push_thread(lua_State *L) { lua_newthread(L); return 0; }

static lua_State *newtaintedstate() {
  lua_State *L = luaL_newstate();
  lua_settaint(L, &test_taint);
  return L;
}

/****************************************************************************
 * Value Test Vectors
 ****************************************************************************/

struct LuaValueVector {
  const char *name;
  lua_CFunction func;
};

struct LuaValueVector lua_value_vectors[] = {
  { "number", push_number },
  { "integer", push_integer },
  { "nil", push_nil },
  { "string", push_string },
  { "boolean", push_boolean },
  { "table", push_table },
  { "lightuserdata", push_lightuserdata },
  { "userdata", push_userdata },
  { "function", push_cclosure },
  { "thread", push_thread },
  { NULL, NULL }
};

struct LuaValueVector lua_object_vectors[] = {
  { "string", push_string },
  { "table", push_table },
  { "userdata", push_userdata },
  { "function", push_cclosure },
  { "thread", push_thread },
  { NULL, NULL }
};

/****************************************************************************
 * Basic State Taint Tests
 ****************************************************************************/

void test_startup_taint(void) {
  lua_State *L = luaL_newstate();
  TEST_CHECK(lua_gettaint(L) == NULL);
  TEST_CHECK(luaL_issecure(L));
  lua_close(L);
}

void test_tainted(void) {
  lua_State *L = newtaintedstate();
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
  lua_State *L = newtaintedstate();
  luaL_forcetaint(L);
  TEST_CHECK(lua_gettaint(L) == &test_taint);
  lua_close(L);
}

/****************************************************************************
 * State => Value and Object Taint Propagation Tests
 ****************************************************************************/

void test_push_values_with_taint(void) {
  for (struct LuaValueVector *vec = lua_value_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    lua_State *L = newtaintedstate();
    vec->func(L);
    TEST_CHECK(lua_getvaluetaint(L, 1) != NULL);
    TEST_CHECK(!luaL_issecurevalue(L, 1));
    lua_close(L);
  }
}

void test_push_values_without_taint(void) {
  for (struct LuaValueVector *vec = lua_value_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    lua_State *L = luaL_newstate();
    vec->func(L);
    TEST_CHECK(lua_getvaluetaint(L, 1) == NULL);
    TEST_CHECK(luaL_issecurevalue(L, 1));
    lua_close(L);
  }
}

void test_push_objects_with_taint(void) {
  for (struct LuaValueVector *vec = lua_object_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    lua_State *L = newtaintedstate();
    vec->func(L);
    TEST_CHECK(lua_getobjecttaint(L, 1) != NULL);
    TEST_CHECK(!luaL_issecureobject(L, 1));
    lua_close(L);
  }
}

void test_push_objects_without_taint(void) {
  for (struct LuaValueVector *vec = lua_object_vectors; vec->name; ++vec) {
    TEST_CASE(vec->name);
    lua_State *L = luaL_newstate();
    vec->func(L);
    TEST_CHECK(lua_getobjecttaint(L, 1) == NULL);
    TEST_CHECK(luaL_issecureobject(L, 1));
    lua_close(L);
  }
}

void test_tainted_table_structures(void) {
  lua_State *L = newtaintedstate();

  // Create a color-like table on the stack.
  lua_createtable(L, 0, 3);
  lua_pushnumber(L, 0.3);
  lua_setfield(L, 1, "r");
  lua_pushnumber(L, 0.6);
  lua_setfield(L, 1, "g");
  lua_pushnumber(L, 0.9);
  lua_setfield(L, 1, "b");

  // Expect all fields to be tainted. This is to mimic behaviors like calling
  // C_ClassColor.GetClassColor ingame which returns a table with tainted
  // keys.

  lua_settaint(L, NULL);
  lua_getfield(L, 1, "r");
  TEST_CHECK(!luaL_issecure(L));
  TEST_CHECK(!luaL_issecurevalue(L, -1));
  lua_pop(L, 1);

  lua_settaint(L, NULL);
  lua_getfield(L, 1, "g");
  TEST_CHECK(!luaL_issecure(L));
  TEST_CHECK(!luaL_issecurevalue(L, -1));
  lua_pop(L, 1);

  lua_settaint(L, NULL);
  lua_getfield(L, 1, "b");
  TEST_CHECK(!luaL_issecure(L));
  TEST_CHECK(!luaL_issecurevalue(L, -1));
  lua_pop(L, 1);

  lua_close(L);
}

/****************************************************************************
 * Test Listing
 ****************************************************************************/

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
  { NULL, NULL }
};
