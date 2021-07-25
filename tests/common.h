#ifndef LUAT_TEST_H
#define LUAT_TEST_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/** Global Lua state initialized at the start of eacn new test. */
extern lua_State *LT;

/** Reusable taint object that can be applied during testing. */
extern lua_Taint luaT_taint;

/**
 * Utility Functions
 */

#define luaT_extentof(a) (sizeof(a) / sizeof(a[0]))

#define luaT_checkcall(L, nargs, nresults) \
  do { \
    if (!TEST_CHECK(lua_pcall(L, nargs, nresults, 0) == 0)) { \
      TEST_MSG("call failure: %s", lua_tostring(L, -1)); \
      lua_pop(L, 1); \
    } \
  } while(0)

#define luaT_loadfixture(L, name) \
  do { \
    const luaT_Fixture *fx_ = &name; \
    if (!TEST_CHECK(luaL_loadbuffer(L, fx_->data, fx_->size, NULL) == 0) || !TEST_CHECK(lua_pcall(L, 0, 0, 0) == 0)) { \
      TEST_MSG("load failure: %s", lua_tostring(L, -1)); \
      lua_pop(L, 1); \
    } \
  } while(0)

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
 */

typedef struct luaT_Fixture {
  const char *data;
  size_t size;
} luaT_Fixture;

extern const luaT_Fixture luac_mixin;
extern const luaT_Fixture luac_rectanglemixin;
extern const luaT_Fixture luac_issecurevariable;
extern const luaT_Fixture luac_securecallaux;
extern const luaT_Fixture luac_lvmutil;
extern const luaT_Fixture luac_lvmfields;
extern const luaT_Fixture luac_lvmglobals;
extern const luaT_Fixture luac_lvmlocals;
extern const luaT_Fixture luac_lvmupvalues;

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
