/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */

#include <string.h>

#define ltm_c
#define LUA_CORE

#include "lua.h"

#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"

const char *const luaT_typenames[] = {
    "nil", "boolean", "userdata", "number", "string", "table", "function", "userdata", "thread", "proto", "upval",
};

static const char *const luaT_eventname[] = {
    /* ORDER TM */
    "__index", "__newindex", "__gc",  "__mode", "__eq", "__add",    "__sub",  "__mul",         "__div", "__mod",
    "__pow",   "__unm",      "__len", "__lt",   "__le", "__concat", "__call", "__environment", "__name"
};

void luaT_init (lua_State *L) {
    int i;

    for (i = 0; i < TM_N; i++) {
        G(L)->tmname[i] = luaS_new(L, luaT_eventname[i]);
        luaS_fix(G(L)->tmname[i]); /* never collect these names */
    }
}

/*
** function to be used with macro "fasttm": optimized for absence of
** tag methods
*/
const TValue *luaT_gettm (Table *events, TMS event, TString *ename) {
    const TValue *tm = luaH_getstr(events, ename);
    lua_assert(event <= TM_EQ);
    if (ttisnil(tm)) { /* no tag method? */
        events->flags |= cast_byte(1u << event); /* cache this fact */
        return NULL;
    } else {
        return tm;
    }
}

const TValue *luaT_gettmbyobj (lua_State *L, const TValue *o, TMS event) {
    Table *mt;
    switch (ttype(o)) {
        case LUA_TTABLE:
            mt = hvalue(o)->metatable;
            break;
        case LUA_TUSERDATA:
            mt = uvalue(o)->metatable;
            break;
        default:
            mt = G(L)->mt[ttype(o)];
    }
    return (mt ? luaH_getstr(mt, G(L)->tmname[event]) : luaO_nilobject);
}
