/*
** $Id: lmathlib.c,v 1.67.1.1 2007/12/27 13:02:25 roberto Exp $
** Standard mathematical library
** See Copyright Notice in lua.h
*/


#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define lmathlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


#undef PI
#define PI (3.14159265358979323846)
#define RADIANS_PER_DEGREE (PI/180.0)


/**
 * Written in 2016 by Kaito Udagawa
 * Released under CC0 <http://creativecommons.org/publicdomain/zero/1.0/>
 */

static uint32_t splitmix32 (uint32_t *x) {
  uint32_t z = (*x += 0x9e3779b9);
  z = (z ^ (z >> 16)) * 0x85ebca6b;
  z = (z ^ (z >> 13)) * 0xc2b2ae35;
  return z ^ (z >> 16);
}

/**
 * Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)
 *
 * To the extent possible under law, the author has dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * See <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

static uint32_t rotl (const uint32_t x, int k) {
  return (x << k) | (x >> (32 - k));
}

typedef struct xoroshiro_state {
  uint32_t s[2];
} xoroshiro_state;

static void xoroshiro_init (xoroshiro_state *st, uint32_t seed) {
  st->s[0] = splitmix32(&seed);
  st->s[1] = splitmix32(&seed);
}

static uint32_t xoroshiro_next (xoroshiro_state *st) {
	const uint32_t s0 = st->s[0];
	uint32_t s1 = st->s[1];
	const uint32_t result = rotl(s0 * 0x9E3779BB, 5) * 5;

	s1 ^= s0;
	st->s[0] = rotl(s0, 26) ^ s1 ^ (s1 << 9); // a, b
	st->s[1] = rotl(s1, 13); // c

	return result;
}

static int math_abs (lua_State *L) {
  lua_pushnumber(L, fabs(luaL_checknumber(L, 1)));
  return 1;
}

static int math_sin (lua_State *L) {
  lua_pushnumber(L, sin(luaL_checknumber(L, 1)));
  return 1;
}

static int math_sinh (lua_State *L) {
  lua_pushnumber(L, sinh(luaL_checknumber(L, 1)));
  return 1;
}

static int math_cos (lua_State *L) {
  lua_pushnumber(L, cos(luaL_checknumber(L, 1)));
  return 1;
}

static int math_cosh (lua_State *L) {
  lua_pushnumber(L, cosh(luaL_checknumber(L, 1)));
  return 1;
}

static int math_tan (lua_State *L) {
  lua_pushnumber(L, tan(luaL_checknumber(L, 1)));
  return 1;
}

static int math_tanh (lua_State *L) {
  lua_pushnumber(L, tanh(luaL_checknumber(L, 1)));
  return 1;
}

static int math_asin (lua_State *L) {
  lua_pushnumber(L, asin(luaL_checknumber(L, 1)));
  return 1;
}

static int math_acos (lua_State *L) {
  lua_pushnumber(L, acos(luaL_checknumber(L, 1)));
  return 1;
}

static int math_atan (lua_State *L) {
  lua_pushnumber(L, atan(luaL_checknumber(L, 1)));
  return 1;
}

static int math_atan2 (lua_State *L) {
  lua_pushnumber(L, atan2(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
  return 1;
}

static int math_ceil (lua_State *L) {
  lua_pushnumber(L, ceil(luaL_checknumber(L, 1)));
  return 1;
}

static int math_floor (lua_State *L) {
  lua_pushnumber(L, floor(luaL_checknumber(L, 1)));
  return 1;
}

static int math_fmod (lua_State *L) {
  lua_pushnumber(L, fmod(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
  return 1;
}

static int math_modf (lua_State *L) {
  double ip;
  double fp = modf(luaL_checknumber(L, 1), &ip);
  lua_pushnumber(L, ip);
  lua_pushnumber(L, fp);
  return 2;
}

static int math_sqrt (lua_State *L) {
  lua_pushnumber(L, sqrt(luaL_checknumber(L, 1)));
  return 1;
}

static int math_pow (lua_State *L) {
  lua_pushnumber(L, pow(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
  return 1;
}

static int math_log (lua_State *L) {
  lua_pushnumber(L, log(luaL_checknumber(L, 1)));
  return 1;
}

static int math_log10 (lua_State *L) {
  lua_pushnumber(L, log10(luaL_checknumber(L, 1)));
  return 1;
}

static int math_exp (lua_State *L) {
  lua_pushnumber(L, exp(luaL_checknumber(L, 1)));
  return 1;
}

static int math_deg (lua_State *L) {
  lua_pushnumber(L, luaL_checknumber(L, 1)/RADIANS_PER_DEGREE);
  return 1;
}

static int math_rad (lua_State *L) {
  lua_pushnumber(L, luaL_checknumber(L, 1)*RADIANS_PER_DEGREE);
  return 1;
}

static int math_frexp (lua_State *L) {
  int e;
  lua_pushnumber(L, frexp(luaL_checknumber(L, 1), &e));
  lua_pushinteger(L, e);
  return 2;
}

static int math_ldexp (lua_State *L) {
  lua_pushnumber(L, ldexp(luaL_checknumber(L, 1), luaL_checkint(L, 2)));
  return 1;
}



static int math_min (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmin = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d < dmin)
      dmin = d;
  }
  lua_pushnumber(L, dmin);
  return 1;
}


static int math_max (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmax = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d > dmax)
      dmax = d;
  }
  lua_pushnumber(L, dmax);
  return 1;
}


static int math_random (lua_State *L) {
  lua_getfield(L, LUA_REGISTRYINDEX, "_XOROSTATE");
  xoroshiro_state *state = (xoroshiro_state *) lua_touserdata(L, -1);

  if (!state) {
    return 0;  // Should only happen if called during close.
  }

  const lua_Number r = ldexp((lua_Number) xoroshiro_next(state), -32);
  lua_pop(L, 1);

  switch (lua_gettop(L)) {
    case 0: {  // Random number between 0 and 1
      lua_pushnumber(L, r);
      break;
    }
    case 1: {  // Random integer between 1 and given upper limit
      int u = luaL_checkint(L, 1);
      luaL_argcheck(L, 1 <= u, 1, "interval is empty");
      lua_pushnumber(L, floor(r * u) + 1);
      break;
    }
    case 2: {  // Random integer between given limits
      int l = luaL_checkint(L, 1);
      int u = luaL_checkint(L, 2);
      luaL_argcheck(L, l <= u, 2, "interval is empty");
      lua_pushnumber(L, floor(r * (u - l + 1)) + l);
      break;
    }
    default: {
      return luaL_error(L, "wrong number of arguments");
    }
  }

  return 1;
}


static int math_fastrandom (lua_State *L) {
  /* the `%' avoids the (rare) case of r==1, and is needed also because on
     some systems (SunOS!) `rand()' may return a value larger than RAND_MAX */
  lua_Number r = (lua_Number)(rand()%RAND_MAX) / (lua_Number)RAND_MAX;
  switch (lua_gettop(L)) {  /* check number of arguments */
    case 0: {  /* no arguments */
      lua_pushnumber(L, r);  /* Number between 0 and 1 */
      break;
    }
    case 1: {  /* only upper limit */
      int u = luaL_checkint(L, 1);
      luaL_argcheck(L, 1<=u, 1, "interval is empty");
      lua_pushnumber(L, floor(r*u)+1);  /* int between 1 and `u' */
      break;
    }
    case 2: {  /* lower and upper limits */
      int l = luaL_checkint(L, 1);
      int u = luaL_checkint(L, 2);
      luaL_argcheck(L, l<=u, 2, "interval is empty");
      lua_pushnumber(L, floor(r*(u-l+1))+l);  /* int between `l' and `u' */
      break;
    }
    default: return luaL_error(L, "wrong number of arguments");
  }
  return 1;
}


static int math_randomseed (lua_State *L) {
  const int seed = luaL_checkint(L, 1);

  lua_getfield(L, LUA_REGISTRYINDEX, "_XOROSTATE");
  xoroshiro_state *state = (xoroshiro_state *) lua_touserdata(L, -1);

  if (state) {
    xoroshiro_init(state, (uint32_t) seed);
  }

  srand(seed);
  return 0;
}


static const luaL_Reg mathlib[] = {
  {"abs",   math_abs},
  {"acos",  math_acos},
  {"asin",  math_asin},
  {"atan2", math_atan2},
  {"atan",  math_atan},
  {"ceil",  math_ceil},
  {"cosh",   math_cosh},
  {"cos",   math_cos},
  {"deg",   math_deg},
  {"exp",   math_exp},
  {"floor", math_floor},
  {"fmod",   math_fmod},
  {"frexp", math_frexp},
  {"ldexp", math_ldexp},
  {"log10", math_log10},
  {"log",   math_log},
  {"max",   math_max},
  {"min",   math_min},
  {"modf",   math_modf},
  {"pow",   math_pow},
  {"rad",   math_rad},
  {"random",     math_random},
  {"randomseed", math_randomseed},
  {"sinh",   math_sinh},
  {"sin",   math_sin},
  {"sqrt",  math_sqrt},
  {"tanh",   math_tanh},
  {"tan",   math_tan},
  {NULL, NULL}
};


/*
** Open math library
*/
LUALIB_API int luaopen_math (lua_State *L) {
  luaL_register(L, LUA_MATHLIBNAME, mathlib);
  lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");
#if defined(LUA_COMPAT_MOD)
  lua_getfield(L, -1, "fmod");
  lua_setfield(L, -2, "mod");
#endif

  // Add global compatibility aliases.

  luaL_dostring(L, " \
    abs = math.abs \
    ceil = math.ceil \
    deg = math.deg \
    exp = math.exp \
    floor = math.floor \
    frexp = math.frexp \
    ldexp = math.ldexp \
    log = math.log \
    log10 = math.log10 \
    max = math.max \
    min = math.min \
    mod = math.fmod \
    PI = math.pi \
    rad = math.rad \
    random = math.random \
    sqrt = math.sqrt \
  ");

  // Add global degree => radian compatibility wrappers. These are explicitly
  // loaded via loadstring to ensure that any code which, for whatever unholy
  // reason, is expecting them to be Lua functions and not C ones doesn't
  // get surprised.

  luaL_dostring(L, " \
    acos = function (x) return math.deg(math.acos(x)) end \
    asin = function (x) return math.deg(math.asin(x)) end \
    atan = function (x) return math.deg(math.atan(x)) end \
    atan2 = function (x,y) return math.deg(math.atan2(x,y)) end \
    cos = function (x) return math.cos(math.rad(x)) end \
    sin = function (x) return math.sin(math.rad(x)) end \
    tan = function (x) return math.tan(math.rad(x)) end \
  ");

  // The fastrandom function is meant to alias to what math.random used to
  // be ingame, where math.random actually was changed to use a more secure
  // PRNG during ~Mists of Pandaria.
  //
  // Security isn't a goal here, but we should at least provide two separate
  // sources of RNG so that each function can be different.

  xoroshiro_state *state = (xoroshiro_state *) lua_newuserdata(L, sizeof(xoroshiro_state));
  xoroshiro_init(state, 0);
  lua_setfield(L, LUA_REGISTRYINDEX, "_XOROSTATE");

  lua_pushcclosure(L, math_fastrandom, 0);
  lua_setglobal(L, "fastrandom");

  return 1;
}

