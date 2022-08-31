/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */

#ifndef lsys_h
#define lsys_h


/*
** {==================================================================
** l_readline defines how to show a prompt and then read a line from
** the standard input.
** l_saveline defines how to "save" a read line in a "history".
** l_freeline defines how to free a line read by l_readline.
** ===================================================================
*/
#if !defined(l_readline)      /* { */
#if defined(LUA_USE_READLINE)   /* { */

#include <readline/readline.h>
#include <readline/history.h>
#define l_initreadline(L)       ((void)L, rl_readline_name="lua")
#define l_readline(L,b,p)       ((void)L, ((b)=readline(p)) != NULL)
#define l_saveline(L,line)      ((void)L, add_history(line))
#define l_freeline(L,b)         ((void)L, free(b))

#else                           /* }{ */

#define l_initreadline(L)       ((void)L)
#define l_readline(L,b,p) \
        ((void)L, fputs(p, stdout), fflush(stdout),  /* show prompt */ \
        fgets(b, LUA_MAXINPUT, stdin) != NULL)  /* get line */
#define l_saveline(L,line)      { (void)L; (void)line; }
#define l_freeline(L,b)         { (void)L; (void)b; }

#endif                          /* } */
#endif                        /* } */
/* }================================================================== */


/*
** {==================================================================
** l_stdin_is_tty detects whether the standard input is a 'tty' (that
** is, whether we're running lua interactively).
** ===================================================================
*/
#if !defined(l_stdin_is_tty)          /* { */
#if defined(LUA_USE_POSIX_ISATTY)       /* { */

#include <unistd.h>
#define l_stdin_is_tty()        isatty(0)

#elif defined(LUA_USE_WINDOWS_ISATTY)   /* }{ */

#include <io.h>
#include <windows.h>

#define l_stdin_is_tty()        _isatty(_fileno(stdin))

#else                                   /* }{ */

/* ISO C definition */
#define l_stdin_is_tty()        1  /* assume stdin is a tty */

#endif                                  /* } */
#endif                                /* } */
/* }================================================================== */


/*
** {==================================================================
** l_tmpnam generates a name for a temporary file
** ===================================================================
*/
#if !defined(l_tmpnam)          /* { */
#if defined(LUA_USE_MKSTEMP)      /* { */

#include <unistd.h>

#define LUA_TMPNAMBUFSIZE       32

#if !defined(LUA_TMPNAMTEMPLATE)
#define LUA_TMPNAMTEMPLATE      "/tmp/lua_XXXXXX"
#endif

#define l_tmpnam(b,e) { \
        strcpy(b, LUA_TMPNAMTEMPLATE); \
        e = mkstemp(b); \
        if (e != -1) close(e); \
        e = (e == -1); }

#else                           /* }{ */

/* ISO C definitions */
#define LUA_TMPNAMBUFSIZE       L_tmpnam
#define l_tmpnam(b,e)           { e = (tmpnam(b) == NULL); }

#endif                          /* } */
#endif                        /* } */
/* }================================================================== */


/*
** {==================================================================
** l_popen spawns a new process connected to the current
** one through the file streams.
** ===================================================================
*/

#if !defined(l_popen)               /* { */
#if defined(LUA_USE_POSIX_POPEN)      /* { */

#define l_popen(L,c,m)          (fflush(NULL), popen(c,m))
#define l_pclose(L,file)        (pclose(file))

#elif defined(LUA_USE_WINDOWS_POPEN)  /* }{ */

#define l_popen(L,c,m)          (_popen(c,m))
#define l_pclose(L,file)        (_pclose(file))

#if !defined(l_checkmodep)
/* Windows accepts "[rw][bt]?" as valid modes */
#define l_checkmodep(m) ((m[0] == 'r' || m[0] == 'w') && \
  (m[1] == '\0' || ((m[1] == 'b' || m[1] == 't') && m[2] == '\0')))
#endif

#else                                 /* }{ */

/* ISO C definitions */
#define l_popen(L,c,m)  \
          ((void)c, (void)m, \
          luaL_error(L, "'popen' not supported"), \
          (FILE*)0)
#define l_pclose(L,file)                ((void)L, (void)file, -1)

#endif                                /* } */
#endif                              /* } */


#if !defined(l_checkmodep)
/* By default, Lua accepts only "r" or "w" as valid modes */
#define l_checkmodep(m)        ((m[0] == 'r' || m[0] == 'w') && m[1] == '\0')
#endif

/* }====================================================== */


/*
** {==================================================================
** l_securerandom generates an unsigned 32-bit random integer from
** any secure random number source available on this platform.
** ===================================================================
*/

#if !defined(l_securerandom)            /* { */
#if defined(LUA_USE_GETRANDOM)            /* { */

#include <errno.h>
#include <string.h>
#include <sys/random.h>

static inline uint32_t l_securerandom (lua_State *L) {
  uint32_t i;
  ssize_t read = 0;

  do {
    ssize_t result = getrandom((&i) + read, sizeof(i) - read, 0);

    if (result < 0) {
      lua_pushfstring(L, "%s", strerror(errno));
      return lua_error(L);
    } else {
      read += result;
    }
  } while (read != sizeof(i));

  return i;
}

#elif defined(LUA_USE_ARC4RANDOM)         /* }{ */

#include <stdlib.h>

static inline uint32_t l_securerandom (lua_State *L) {
  lua_unused(L);
  return arc4random();
}

#elif defined(LUA_USE_BCRYPTGENRANDOM)    /* }{ */

#include <windows.h>
#include <bcrypt.h>

static inline uint32_t l_securerandom (lua_State *L) {
  uint32_t i;
  lua_unused(L);
  BCryptGenRandom(NULL, (PUCHAR) &i, sizeof(i), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
  return i;
}

#else

static inline uint32_t l_securerandom (lua_State *L) {
  luaL_error(L, "secure random generator not available on this platform");
}

#endif                                    /* } */
#endif                                  /* } */

/* }================================================================== */



/*
** {==================================================================
** l_gettickcount returns high resolution timestamp in 'ticks'
** l_gettickfrequency returns the number of 'ticks' in a single second
** ===================================================================
*/

#if !defined(l_gettickcount)                    /* { */
#if defined(LUA_USE_CLOCK_GETTIME)                /* { */

#include <time.h>

static inline uint_least64_t l_gettickcount (void) {
  struct timespec ts;
  uint64_t ticks;
  clock_gettime(LUA_CLOCK_ID, &ts);
  ticks = (ts.tv_sec * 1e9) + ts.tv_nsec;
  return ticks;
}

static inline uint_least64_t l_gettickfrequency (void) {
  return (uint_least64_t) 1e9;
}

#elif defined(LUA_USE_QUERYPERFORMANCECOUNTER)    /* }{ */

#include <windows.h>

static inline uint_least64_t l_gettickcount (void) {
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return counter.QuadPart;
}

static inline uint_least64_t l_gettickfrequency (void) {
  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);
  return frequency.QuadPart;
}

#else

static inline uint_least64_t l_gettickcount (void) {
  return clock();
}

static inline uint_least64_t l_gettickfrequency (void) {
  return CLOCKS_PER_SEC;
}

#endif                                            /* } */
#endif                                          /* } */

/* }================================================================== */


#endif
