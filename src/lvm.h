/*
** $Id: lvm.h,v 2.5.1.1 2007/12/27 13:02:25 roberto Exp $
** Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef lvm_h
#define lvm_h


#include "ldo.h"
#include "lobject.h"
#include "ltm.h"


#define tostring(L,o) ((ttype(o) == LUA_TSTRING) || (luaV_tostring(L, o)))

#define tonumber(L,o,n)	(ttype(o) == LUA_TNUMBER || \
                         (((o) = luaV_tonumber(L,o,n)) != NULL))

#define equalobj(L,o1,o2) \
	(ttype(o1) == ttype(o2) && luaV_equalval(L, o1, o2))


LUAI_FUNC int luaV_lessthan (lua_State *L, const TValue *l, const TValue *r);
LUAI_FUNC int luaV_equalval (lua_State *L, const TValue *t1, const TValue *t2);
LUAI_FUNC const TValue *luaV_tonumber (lua_State *L, const TValue *obj, TValue *n);
LUAI_FUNC int luaV_tostring (lua_State *L, StkId obj);
LUAI_FUNC void luaV_gettable (lua_State *L, const TValue *t, TValue *key,
                                            StkId val);
LUAI_FUNC void luaV_settable (lua_State *L, const TValue *t, TValue *key,
                                            StkId val);
LUAI_FUNC void luaV_execute (lua_State *L, int nexeccalls);
LUAI_FUNC void luaV_concat (lua_State *L, int total, int last);

#define luaV_taintbarrier(L, x) \
  do { \
    lua_Taint *lt_ = L->taint; \
    { x; } \
    L->taint = lt_; \
  } while(0)

static inline void luaV_taint (lua_State *L, TValue *v) {
  if (!v->taint) v->taint = L->taint;
  else if (!L->taint) L->taint = v->taint;
}

static inline void luaV_readtaint (lua_State *L, TValue *v) {
  if (!L->taint) L->taint = v->taint;
}

static inline void luaV_writetaint (lua_State *L, TValue *v) {
  if (!v->taint) v->taint = L->taint;
}

static inline void luaV_gcotaint (lua_State *L, GCObject *gco) {
  GCheader *gch = &gco->gch;
  if (!gch->taint) gch->taint = L->taint;
  else if (!L->taint) L->taint = gch->taint;
}

static inline void luaV_readgcotaint (lua_State *L, GCObject *gco) {
  GCheader *gch = &gco->gch;
  if (!L->taint) L->taint = gch->taint;
}

static inline void luaV_writegcotaint (lua_State *L, GCObject *gco) {
  GCheader *gch = &gco->gch;
  if (!gch->taint) gch->taint = L->taint;
}

#endif
