/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */

#ifndef lmanip_h
#define lmanip_h

#include "lgc.h"
#include "lobject.h"
#include "lsec.h"
#include "lstate.h"

/**
 * Functions to set values
 */

inline void setnilvalue (lua_State *L, TValue *dst) {
    dst->tt = LUA_TNIL;
    dst->taint = L->writetaint;
}

inline void setnvalue (lua_State *L, TValue *dst, lua_Number n) {
    dst->value.n = n;
    dst->tt = LUA_TNUMBER;
    dst->taint = L->writetaint;
}

inline void setpvalue (lua_State *L, TValue *dst, void *p) {
    dst->value.p = p;
    dst->tt = LUA_TLIGHTUSERDATA;
    dst->taint = L->writetaint;
}

inline void setbvalue (lua_State *L, TValue *dst, int b) {
    dst->value.b = b;
    dst->tt = LUA_TBOOLEAN;
    dst->taint = L->writetaint;
}

inline void setsvalue (lua_State *L, TValue *dst, TString *s) {
    dst->value.gc = cast(GCObject *, s);
    dst->tt = LUA_TSTRING;
    dst->taint = L->writetaint;
    checkliveness(G(L), dst);
}

inline void setuvalue (lua_State *L, TValue *dst, Udata *u) {
    dst->value.gc = cast(GCObject *, u);
    dst->tt = LUA_TUSERDATA;
    dst->taint = L->writetaint;
    checkliveness(G(L), dst);
}

inline void setthvalue (lua_State *L, TValue *dst, lua_State *th) {
    dst->value.gc = cast(GCObject *, th);
    dst->tt = LUA_TTHREAD;
    dst->taint = L->writetaint;
    checkliveness(G(L), dst);
}

inline void setclvalue (lua_State *L, TValue *dst, Closure *cl) {
    dst->value.gc = cast(GCObject *, cl);
    dst->tt = LUA_TFUNCTION;
    dst->taint = L->writetaint;
    checkliveness(G(L), dst);
}

inline void sethvalue (lua_State *L, TValue *dst, Table *h) {
    dst->value.gc = cast(GCObject *, h);
    dst->tt = LUA_TTABLE;
    dst->taint = L->writetaint;
    checkliveness(G(L), dst);
}

inline void setptvalue (lua_State *L, TValue *dst, Proto *pt) {
    dst->value.gc = cast(GCObject *, pt);
    dst->tt = LUA_TPROTO;
    dst->taint = L->writetaint;
    checkliveness(G(L), dst);
}

inline void setobj (lua_State *L, TValue *dst, const TValue *src) {
    dst->value = src->value;
    dst->tt = src->tt;
    dst->taint = src->taint;
    checkliveness(G(L), dst);
}

/**
 * Different types of sets according to destination
 */

inline void setobj2s (lua_State *L, StkId dst, const TValue *src) {
    dst->value = src->value;
    dst->tt = src->tt;
    dst->taint = src->taint;

    if (dst->taint == NULL) {
        dst->taint = L->writetaint;
    } else {
        luaR_taintstack(L, src->taint);
    }

    checkliveness(G(L), dst);
}

inline void setobj2n (lua_State *L, TValue *dst, const TValue *src) {
    setobj(L, dst, src);
}

inline void setobj2t (lua_State *L, TValue *dst, const TValue *src) {
    setobj(L, dst, src);
}

inline void setobjs2s (lua_State *L, StkId dst, const TValue *src) {
    setobj2s(L, dst, src);
}

inline void setobjt2t (lua_State *L, TValue *dst, const TValue *src) {
    setobj(L, dst, src);
}

inline void sethvalue2s (lua_State *L, StkId dst, Table *src) {
    sethvalue(L, dst, src);
}

inline void setptvalue2s (lua_State *L, StkId dst, Proto *src) {
    setptvalue(L, dst, src);
}

inline void setsvalue2n (lua_State *L, TValue *dst, TString *src) {
    setsvalue(L, dst, src);
}

inline void setsvalue2s (lua_State *L, StkId dst, TString *src) {
    setsvalue(L, dst, src);
}

inline void rawsetnilvalue (TValue *dst) {
    dst->tt = LUA_TNIL;
    dst->taint = NULL;
}

#endif
