/**
 * WoW-compatible library for bitwise operations
 * See Copyright Notice in lua.h
 */

#define lbitlib_c
#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

typedef LUAI_UINT32 lua_Unsigned;
typedef LUAI_INT32 lua_Signed;

#define bit_checksigned(L, i) ((lua_Signed) luaL_checknumber(L, i))
#define bit_checkunsigned(L, i) ((lua_Unsigned) luaL_checknumber(L, i))

static int bit_band (lua_State *L) {
  const int top = lua_gettop(L);
  int pos = 2;
  lua_Unsigned result = bit_checkunsigned(L, 1);

  for (; pos <= top; ++pos) {
    result &= bit_checksigned(L, pos);
  }

  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_bor (lua_State *L) {
  const int top = lua_gettop(L);
  int pos = 2;
  lua_Unsigned result = bit_checkunsigned(L, 1);

  for (; pos <= top; ++pos) {
    result |= bit_checksigned(L, pos);
  }

  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_bxor (lua_State *L) {
  const int top = lua_gettop(L);
  int pos = 2;
  lua_Unsigned result = bit_checkunsigned(L, 1);

  for (; pos <= top; ++pos) {
    result ^= bit_checksigned(L, pos);
  }

  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_bnot (lua_State *L) {
  lua_Unsigned result = bit_checkunsigned(L, 1);
  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_lshift (lua_State *L) {
  lua_Unsigned result = bit_checkunsigned(L, 1);
  result <<= bit_checkunsigned(L, 2);
  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_rshift (lua_State *L) {
  lua_Unsigned result = bit_checkunsigned(L, 1);
  result >>= bit_checkunsigned(L, 2);
  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_arshift (lua_State *L) {
  lua_Signed result = bit_checksigned(L, 1);
  result >>= bit_checksigned(L, 2);
  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_mod (lua_State *L) {
  lua_Signed result = bit_checksigned(L, 1);
  result %= bit_checksigned(L, 2);
  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static const luaL_Reg bitlib[] = {
  { .name = "band", .func = bit_band },
  { .name = "bor", .func = bit_bor },
  { .name = "bxor", .func = bit_bxor },
  { .name = "bnot", .func = bit_bnot },
  { .name = "lshift", .func = bit_lshift },
  { .name = "rshift", .func = bit_rshift },
  { .name = "arshift", .func = bit_arshift },
  { .name = "mod", .func = bit_mod },
  { .name = NULL, .func = NULL },
};

LUALIB_API int luaopen_bit (lua_State *L) {
  luaL_register(L, LUA_BITLIBNAME, bitlib);
  return 1;
}
