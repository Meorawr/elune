#ifndef LUAT_TEST_H
#define LUAT_TEST_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/** Global Lua state initialized at the start of eacn new test. */
extern lua_State *LT;

/** Reusable taint info object that can be applied during testing. */
extern lua_TaintInfo luaT_taint;

/**
 * Utility Functions
 */

#define luaT_extentof(a) (sizeof(a) / sizeof(a[0]))

extern int luaT_pushnumber(lua_State *L);
extern int luaT_pushinteger(lua_State *L);
extern int luaT_pushnil(lua_State *L);
extern int luaT_pushstring(lua_State *L);
extern int luaT_pushboolean(lua_State *L);
extern int luaT_pushtable(lua_State *L);
extern int luaT_pushlightuserdata(lua_State *L);
extern int luaT_pushuserdata(lua_State *L);
extern int luaT_pushcclosure(lua_State *L);
extern int luaT_pushthread(lua_State *L);

extern int luaT_loadfixtures(lua_State *L);

/**
 * Test Vectors
 */

struct LuaValueVector {
  const char *name;
  lua_CFunction func;
};

extern struct LuaValueVector luaT_value_vectors[];
extern struct LuaValueVector luaT_object_vectors[];

/**
 * Precompiled Script Fixtures
 *
 * Add any fixtures to the luaT_fixtures macro to automatically create a
 * declaration and enable them for use with luaT_loadfixtures.
 */

typedef struct luaT_Fixture {
  const char *data;
  size_t size;
} luaT_Fixture;

#define luaT_fixtures(X) \
  X(luac_mixin) \
  X(luac_rectanglemixin)

#define luaT_declarefixture(name) extern const luaT_Fixture name;
luaT_fixtures(luaT_declarefixture);
#undef luaT_declarefixture

/**
 * Test Initialization/Teardown
 */

extern void luaT_test_init(void);
extern void luaT_test_reinit(void);
extern void luaT_test_cleanup(void);

#ifndef TEST_INIT
#define TEST_INIT luaT_test_init();
#endif
#ifndef TEST_FINI
#define TEST_FINI luaT_test_cleanup();
#endif

#endif
