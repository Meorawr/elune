/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */

#define lmathlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#undef PI
#define PI (3.14159265358979323846)
#define RADIANS_PER_DEGREE (PI / 180.0)

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
    lua_pushnumber(L, luaL_checknumber(L, 1) / RADIANS_PER_DEGREE);
    return 1;
}

static int math_rad (lua_State *L) {
    lua_pushnumber(L, luaL_checknumber(L, 1) * RADIANS_PER_DEGREE);
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
    int n = lua_gettop(L); /* number of arguments */
    lua_Number dmin = luaL_checknumber(L, 1);
    int i;
    for (i = 2; i <= n; i++) {
        lua_Number d = luaL_checknumber(L, i);
        if (d < dmin) {
            dmin = d;
        }
    }
    lua_pushnumber(L, dmin);
    return 1;
}

static int math_max (lua_State *L) {
    int n = lua_gettop(L); /* number of arguments */
    lua_Number dmax = luaL_checknumber(L, 1);
    int i;
    for (i = 2; i <= n; i++) {
        lua_Number d = luaL_checknumber(L, i);
        if (d > dmax) {
            dmax = d;
        }
    }
    lua_pushnumber(L, dmax);
    return 1;
}

static int randomrange (lua_State *L, lua_Number r) {
    switch (lua_gettop(L)) {      /* check number of arguments */
        case 0: {                 /* no arguments */
            lua_pushnumber(L, r); /* Number between 0 and 1 */
            break;
        }
        case 1: { /* only upper limit */
            int u = luaL_checkint(L, 1);
            luaL_argcheck(L, 1 <= u, 1, "interval is empty");
            lua_pushnumber(L, floor(r * u) + 1); /* int between 1 and `u' */
            break;
        }
        case 2: { /* lower and upper limits */
            int l = luaL_checkint(L, 1);
            int u = luaL_checkint(L, 2);
            luaL_argcheck(L, l <= u, 2, "interval is empty");
            lua_pushnumber(L, floor(r * (u - l + 1)) +
                                  l); /* int between `l' and `u' */
            break;
        }
        default:
            return luaL_error(L, "wrong number of arguments");
    }

    return 1;
}

static int math_fastrandom (lua_State *L) {
    /* the `%' avoids the (rare) case of r==1, and is needed also because on
       some systems (SunOS!) `rand()' may return a value larger than RAND_MAX */
    lua_Number r = ((lua_Number) (rand() % RAND_MAX)) / ((lua_Number) RAND_MAX);
    return randomrange(L, r);
}

static int math_securerandom (lua_State *L) {
    return randomrange(L, luaL_securerandom(L));
}

static int math_randomseed (lua_State *L) {
    const int seed = luaL_checkint(L, 1);
    srand(seed);
    return 0;
}

/**
 * Math library registration
 */

static const luaL_Reg mathlib_global[] = {
    { .name = "fastrandom", .func = math_fastrandom },
    { .name = NULL, .func = NULL },
};

static const luaL_Reg mathlib_shared[] = {
    { .name = "abs", .func = math_abs },
    { .name = "acos", .func = math_acos },
    { .name = "asin", .func = math_asin },
    { .name = "atan2", .func = math_atan2 },
    { .name = "atan", .func = math_atan },
    { .name = "ceil", .func = math_ceil },
    { .name = "cosh", .func = math_cosh },
    { .name = "cos", .func = math_cos },
    { .name = "deg", .func = math_deg },
    { .name = "exp", .func = math_exp },
    { .name = "floor", .func = math_floor },
    { .name = "fmod", .func = math_fmod },
    { .name = "frexp", .func = math_frexp },
    { .name = "ldexp", .func = math_ldexp },
    { .name = "log10", .func = math_log10 },
    { .name = "log", .func = math_log },
    { .name = "max", .func = math_max },
    { .name = "min", .func = math_min },
    { .name = "modf", .func = math_modf },
    { .name = "pow", .func = math_pow },
    { .name = "rad", .func = math_rad },
    { .name = "sinh", .func = math_sinh },
    { .name = "sin", .func = math_sin },
    { .name = "sqrt", .func = math_sqrt },
    { .name = "tanh", .func = math_tanh },
    { .name = "tan", .func = math_tan },
    { .name = NULL, .func = NULL },
};

static const luaL_Reg mathlib_lua[] = {
    { .name = "random", .func = math_fastrandom },
    { .name = "randomseed", .func = math_randomseed },
    { .name = "securerandom", .func = math_securerandom },
    { .name = NULL, .func = NULL },
};

static const luaL_Reg mathlib_wow[] = {
    { .name = "random", .func = math_securerandom },
    { .name = NULL, .func = NULL },
};

static void mathlib_openshared (lua_State *L) {
    luaL_setfuncs(L, mathlib_shared, 0);
    lua_pushnumber(L, PI);
    lua_setfield(L, -2, "pi");
    lua_pushnumber(L, HUGE_VAL);
    lua_setfield(L, -2, "huge");
}

LUALIB_API int luaopen_math (lua_State *L) {
    /* open math library */
    luaL_register(L, LUA_MATHLIBNAME, mathlib_lua);
    mathlib_openshared(L);

    /* open global functions */
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_setfuncs(L, mathlib_global, 0);
    lua_pop(L, 1);

    return 1;
}

LUALIB_API int luaopen_wow_math (lua_State *L) {
    /* open math library */
    luaL_getsubtable(L, LUA_ENVIRONINDEX, LUA_MATHLIBNAME);
    luaL_setfuncs(L, mathlib_wow, 0);
    mathlib_openshared(L);

    /* open global functions */
    lua_pushvalue(L, LUA_ENVIRONINDEX);
    luaL_setfuncs(L, mathlib_global, 0);
    lua_pop(L, 1);

    return 1;
}
