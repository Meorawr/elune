/*
** $Id: llimits.h,v 1.69.1.1 2007/12/27 13:02:25 roberto Exp $
** Limits, basic types, and some other `installation-dependent' definitions
** See Copyright Notice in lua.h
*/

#ifndef llimits_h
#define llimits_h


#include <limits.h>
#include <stddef.h>
#include <stdint.h>


#include "lua.h"


/* Chars used as small naturals (so that `char' is reserved for characters). */
typedef unsigned char lu_byte;

/* Type to ensure maximum alignment. */
typedef struct { LUA_NUMBER d; void *p; LUA_INTEGER i; } L_Umaxalign;

/* Result of a `usual argument conversion' over lua_Number. */
typedef LUAI_UACNUMBER l_uacNumber;

/* Type used for VM instructions. Must be an unsigned with (at least) 4 bytes;
 * see details in lopcodes.h. */
typedef uint_least32_t Instruction;

/* Integer maximums; these are subtracted for safety per the original defines. */

#define LUA_SIZE_MAX ((size_t) (SIZE_MAX - 2))
#define LUA_PTRDIFF_MAX	((size_t) (PTRDIFF_MAX - 2))
#define LUA_INT_MAX ((int) (INT_MAX - 2))

/* Maximum stack for a Lua function */
#define LUAI_MAXSTACK	250
/* Minimum size for the string table (must be power of 2) */
#define LUAI_MINSTRTABSIZE 32
/* Minimum size for string buffer */
#define LUAI_MINBUFFER 32

/*
** conversion of pointer to integer
** this is for hashing only; there is no problem if the integer
** cannot hold the whole pointer value
*/
#define IntPoint(p)  ((unsigned int)(size_t)(p))


/* internal assertions for in-house debugging */
#define check_exp(c,e)		(e)
#define api_check		luai_apicheck


#ifndef UNUSED
#define UNUSED(x)	((void)(x))	/* to avoid warnings */
#endif


#ifndef cast
#define cast(t, exp)	((t)(exp))
#endif

#define cast_byte(i)	cast(lu_byte, (i))
#define cast_num(i)	cast(lua_Number, (i))
#define cast_int(i)	cast(int, (i))


#ifndef lua_lock
#define lua_lock(L)     ((void) 0)
#define lua_unlock(L)   ((void) 0)
#endif

#ifndef luai_threadyield
#define luai_threadyield(L)     {lua_unlock(L); lua_lock(L);}
#endif


/*
** macro to control inclusion of some hard tests on stack reallocation
*/
#ifndef HARDSTACKTESTS
#define condhardstacktests(x)	((void)0)
#else
#define condhardstacktests(x)	x
#endif

#endif
