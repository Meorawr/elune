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

#define bit_checksigned(L, i) ((lua_Signed) luaL_checkinteger(L, i))
#define bit_checkunsigned(L, i) ((lua_Unsigned) luaL_checkinteger(L, i))

static int bit_band (lua_State *L) {
  const int    top    = lua_gettop(L);
  int          pos    = 2;
  lua_Unsigned result = bit_checksigned(L, 1);

  for (; pos <= top; ++pos) {
    result &= bit_checksigned(L, pos);
  }

  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_bor (lua_State *L) {
  const int    top    = lua_gettop(L);
  int          pos    = 2;
  lua_Unsigned result = bit_checksigned(L, 1);

  for (; pos <= top; ++pos) {
    result |= bit_checksigned(L, pos);
  }

  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_bxor (lua_State *L) {
  const int    top    = lua_gettop(L);
  int          pos    = 2;
  lua_Unsigned result = bit_checksigned(L, 1);

  for (; pos <= top; ++pos) {
    result ^= bit_checksigned(L, pos);
  }

  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_bnot (lua_State *L) {
  lua_Unsigned result = ~bit_checksigned(L, 1);
  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

static int bit_lshift (lua_State *L) {
  lua_Unsigned result = bit_checksigned(L, 1);
  result <<= bit_checksigned(L, 2);
  lua_pushinteger(L, (lua_Integer) result);
  return 1;
}

/**
 * rshift/arshift rely on compiler implementations doing sensible things;
 * namely that a right shift of an unsigned value does a logical shift and
 * zero-fills, whereas a right shift on a signed integer should result in an
 * arithmetic shift with sign extension.
 */

static int bit_rshift (lua_State *L) {
  lua_Unsigned result = bit_checksigned(L, 1);
  result >>= bit_checksigned(L, 2);
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
  { "band",     bit_band    },
  { "bor",      bit_bor     },
  { "bxor",     bit_bxor    },
  { "bnot",     bit_bnot    },
  { "lshift",   bit_lshift  },
  { "rshift",   bit_rshift  },
  { "arshift",  bit_arshift },
  { "mod",      bit_mod     },
  {NULL, NULL}
};

LUALIB_API int luaopen_bit (lua_State *L) {
  luaL_register(L, LUA_BITLIBNAME, bitlib);
  return 1;
}
