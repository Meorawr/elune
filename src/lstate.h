/*
** $Id: lstate.h,v 2.24.1.2 2008/01/03 15:20:39 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/

#ifndef lstate_h
#define lstate_h

#include "lua.h"

#include "lobject.h"
#include "ltm.h"
#include "lzio.h"



struct lua_longjmp;  /* defined in ldo.c */


/* table of globals */
#define gt(L)	(&L->l_gt)

/* registry */
#define registry(L)	(&G(L)->l_registry)


/* extra stack space to handle TM calls and some other extras */
#define EXTRA_STACK   5


#define BASIC_CI_SIZE           8

#define BASIC_STACK_SIZE        (2*LUA_MINSTACK)



typedef struct stringtable {
  GCObject **hash;
  lu_int32 nuse;  /* number of elements */
  int size;
} stringtable;


/*
** informations about a call
*/
typedef struct CallInfo {
  StkId base;  /* base for this function */
  StkId func;  /* function index in the stack */
  StkId	top;  /* top for this function */
  const Instruction *savedpc;
  TString *savedtaint;  /* saved taint for this call; informational only */
  lua_clock_t entryticks;  /* tick count on first initial entry or resumption of this call */
  lua_clock_t startticks;  /* tick count on last reentry of this function */
  int nresults;  /* expected number of results from this function */
  int tailcalls;  /* number of tail calls lost under this entry */
} CallInfo;


#define curr_func(L)	(clvalue(L->ci->func))
#define ci_func(ci)	(clvalue((ci)->func))
#define f_isLua(ci)	(!ci_func(ci)->c.isC)
#define isLua(ci)	(ttisfunction((ci)->func) && f_isLua(ci))


/*
** Profiling Stats
*/
typedef struct SourceStats {
  TString *owner;
  lua_clock_t execticks;  /* ticks spent executing owned functions */
  lu_mem bytesowned;  /* total size of owned allocations */
  struct SourceStats *next;
} SourceStats;


/*
** `global state', shared by all threads of this state
*/
typedef struct global_State {
  stringtable strt;  /* hash table for strings */
  lua_Alloc frealloc;  /* function to reallocate memory */
  void *ud;         /* auxiliary data to `frealloc' */
  lu_byte enablestats;
  lu_byte currentwhite;
  lu_byte gcstate;  /* state of garbage collector */
  int sweepstrgc;  /* position of sweep in `strt' */
  GCObject *rootgc;  /* list of all collectable objects */
  GCObject **sweepgc;  /* position of sweep in `rootgc' */
  GCObject *gray;  /* list of gray objects */
  GCObject *grayagain;  /* list of objects to be traversed atomically */
  GCObject *weak;  /* list of weak tables (to be cleared) */
  GCObject *tmudata;  /* last element of list of userdata to be GC */
  Mbuffer buff;  /* temporary buffer for string concatentation */
  lu_mem GCthreshold;
  lu_mem totalbytes;  /* number of bytes currently allocated */
  lu_mem estimate;  /* an estimate of number of bytes actually in use */
  lu_mem gcdept;  /* how much GC is `behind schedule' */
  int gcpause;  /* size of pause between successive GCs */
  int gcstepmul;  /* GC `granularity' */
  lua_clock_t startticks;  /* tick count at startup */
  lua_clock_t tickfreq;  /* tick frequency; cached on startup */
  lu_mem bytesallocated;  /* total number of bytes allocated */
  SourceStats *sourcestats;  /* list of source-specific statistics */
  lua_CFunction panic;  /* to be called in unprotected errors */
  TValue l_registry;
  TValue l_errfunc;  /* global error handler */
  struct lua_State *mainthread;
  UpVal uvhead;  /* head of double-linked list of all open upvalues */
  struct Table *mt[NUM_TAGS];  /* metatables for basic types */
  TString *tmname[TM_N];  /* array with tag-method names */
} global_State;



/*
** per-thread taint state
*/
typedef struct TaintState {
  lu_intptr readmask;  /* user-controlled mask applied to taint on reads */
  lu_intptr vmexecmask;  /* read-mask enabled only when executing an insecure Lua closure */
  lu_intptr writemask;  /* user-controlled mask applied to taint on writes */
  TString *stacktaint;  /* current stack taint */
  TString *newgctaint;  /* taint applied to newly allocated objects */
  TString *newcltaint;  /* taint applied to newly allocated closures */
} TaintState;

static const lu_intptr LUA_TAINTALLOWED = UINTPTR_MAX;
static const lu_intptr LUA_TAINTBLOCKED = 0;


/*
** `per thread' state
*/
struct lua_State {
  CommonHeader;
  lu_byte status;
  TaintState ts;
  StkId top;  /* first free slot in the stack */
  StkId base;  /* base of current function */
  global_State *l_G;
  CallInfo *ci;  /* call info for current function */
  const Instruction *savedpc;  /* `savedpc' of current function */
  StkId stack_last;  /* last free slot in the stack */
  StkId stack;  /* stack base */
  CallInfo *end_ci;  /* points after end of ci array*/
  CallInfo *base_ci;  /* array of CallInfo's */
  int stacksize;
  int size_ci;  /* size of array `base_ci' */
  unsigned short nCcalls;  /* number of nested C calls */
  unsigned short baseCcalls;  /* nested C calls when resuming coroutine */
  lu_byte exceptmask;
  lu_byte hookmask;
  lu_byte allowhook;
  int basehookcount;
  int hookcount;
  lua_Hook hook;
  TValue l_gt;  /* table of globals */
  TValue env;  /* temporary place for environments */
  GCObject *openupval;  /* list of open upvalues in this stack */
  GCObject *gclist;
  struct lua_longjmp *errorJmp;  /* current error recover point */
  ptrdiff_t errfunc;  /* current error handling function (stack index) */
};


#define G(L)	(L->l_G)


/*
** Union of all collectable objects
*/
union GCObject {
  GCheader gch;
  union TString ts;
  union Udata u;
  union Closure cl;
  struct Table h;
  struct Proto p;
  struct UpVal uv;
  struct lua_State th;  /* thread */
};


/* macros to convert a GCObject into a specific value */
#define rawgco2ts(o)	check_exp((o)->gch.tt == LUA_TSTRING, &((o)->ts))
#define gco2ts(o)	(&rawgco2ts(o)->tsv)
#define rawgco2u(o)	check_exp((o)->gch.tt == LUA_TUSERDATA, &((o)->u))
#define gco2u(o)	(&rawgco2u(o)->uv)
#define gco2cl(o)	check_exp((o)->gch.tt == LUA_TFUNCTION, &((o)->cl))
#define gco2h(o)	check_exp((o)->gch.tt == LUA_TTABLE, &((o)->h))
#define gco2p(o)	check_exp((o)->gch.tt == LUA_TPROTO, &((o)->p))
#define gco2uv(o)	check_exp((o)->gch.tt == LUA_TUPVAL, &((o)->uv))
#define ngcotouv(o) \
	check_exp((o) == NULL || (o)->gch.tt == LUA_TUPVAL, &((o)->uv))
#define gco2th(o)	check_exp((o)->gch.tt == LUA_TTHREAD, &((o)->th))

/* macro to convert any Lua object into a GCObject */
#define obj2gco(v)	(cast(GCObject *, (v)))


LUAI_FUNC lua_State *luaE_newthread (lua_State *L);
LUAI_FUNC void luaE_freethread (lua_State *L, lua_State *L1);


static inline TString *luaE_maskreadtaint (lua_State *L, TString *taint) {
  return cast(TString *, (cast(lu_intptr, taint) & (L->ts.readmask | L->ts.vmexecmask)));
}


static inline TString *luaE_maskwritetaint (lua_State *L) {
  return cast(TString *, (cast(lu_intptr, L->ts.stacktaint) & (L->ts.writemask)));
}


static inline TString *luaE_maskalloctaint (lua_State *L, int tt) {
  TString *taint = NULL;

  if (L->ts.newgctaint != NULL) {
    taint = L->ts.newgctaint;
  } else if (L->ts.stacktaint != NULL) {
    taint = L->ts.stacktaint;
  } else if (tt == LUA_TFUNCTION) {
    taint = L->ts.newcltaint;
  }

  return cast(TString *, (cast(lu_intptr, taint) & (L->ts.writemask)));
}


static inline int luaE_istaintexpected (lua_State *L) {
  return ((L->ts.readmask | L->ts.vmexecmask) == 0);
}


static inline void luaE_taintstack (lua_State *L, TString *taint) {
  taint = luaE_maskreadtaint(L, taint);

  if (taint != NULL) {
    L->ts.stacktaint = taint;
  }
}


static inline void luaE_taintvalue (lua_State *L, TValue *o) {
  TString *taint = luaE_maskwritetaint(L);

  if (taint != NULL) {
    o->taint = taint;
  }
}


static inline void luaE_taintobject (lua_State *L, GCObject *o) {
  TString *taint = luaE_maskwritetaint(L);

  if (taint != NULL) {
    o->gch.taint = taint;
  }
}


static inline void luaE_taintthread (lua_State *L, const lua_State *from) {
  lua_assert(from->ts.vmexecmask == LUA_TAINTALLOWED);
  L->ts = from->ts;
}


#endif

