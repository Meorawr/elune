#define lsysapi_c
#define LUA_CORE

#include "lua.h"

#include "lauxlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(LUA_USE_CLOCK_GETTIME)
#include <time.h>
#elif defined(LUA_USE_QUERYPERFORMANCECOUNTER)
#include <windows.h>
#else
#endif

LUAI_FUNC lua_Clock luaI_clocktime (lua_State *L) {
  lua_unused(L);

#if defined(LUA_USE_CLOCK_GETTIME)
  /* POSIX implementation */
  struct timespec ts;
  lua_Clock ticks;
  clock_gettime(LUA_CLOCK_ID, &ts);
  ticks = (ts.tv_sec * 1e9) + ts.tv_nsec;
  return ticks;
#elif defined(LUA_USE_QUERYPERFORMANCECOUNTER)
  /* Windows implementation */
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return counter.QuadPart;
#else
  /* Default implementation */
  return clock();
#endif
}

LUAI_FUNC lua_Clock luaI_clockrate (lua_State *L) {
  lua_unused(L);

#if defined(LUA_USE_CLOCK_GETTIME)
  /* POSIX implementation */
  return (uint_least64_t) 1e9;
#elif defined(LUA_USE_QUERYPERFORMANCECOUNTER)
  /* Windows implementation */
  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);
  return frequency.QuadPart;
#else
  /* Default implementation */
  return CLOCKS_PER_SEC;
#endif
}

#if defined(LUA_USE_GETRANDOM)
#include <errno.h>
#include <string.h>

#include <sys/random.h>
#elif defined(LUA_USE_ARC4RANDOM)
#include <stdlib.h>
#elif defined(LUA_USE_BCRYPTGENRANDOM)
#include <bcrypt.h>
#include <windows.h>
#endif

LUALIB_API lua_Number luaL_securerandom (lua_State *L) {
  lua_unused(L);

#if defined(LUA_USE_GETRANDOM)
  /* Linux implementation */
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

  return ((lua_Number) i / UINT32_MAX);
#elif defined(LUA_USE_ARC4RANDOM)
  /* macOS implementation */
  return ((lua_Number) arc4random() / UINT32_MAX);
#elif defined(LUA_USE_BCRYPTGENRANDOM)
  /* Windows implementation */
  uint32_t i;
  BCryptGenRandom(NULL, (PUCHAR) &i, sizeof(i),
                  BCRYPT_USE_SYSTEM_PREFERRED_RNG);
  return ((lua_Number) i / UINT32_MAX);
#else
  /* Default implementation */
  luaL_error(L, "secure random generator not available on this platform");
  return 0;
#endif
}

#if defined(LUA_USE_POSIX_ISATTY)
#include <unistd.h>
#elif defined(LUA_USE_WINDOWS_ISATTY)
#include <io.h>
#include <windows.h>
#endif

LUALIB_API int luaL_stdinistty (lua_State *L) {
  lua_unused(L);

#if defined(LUA_USE_POSIX_ISATTY)
  /* POSIX implementation */
  return isatty(0);
#elif defined(LUA_USE_WINDOWS_ISATTY)
  /* Windows implementation */
  return _isatty(_fileno(stdin));
#else
  /* Default implementation */
  return 1; /* assume stdin is a tty */
#endif
}

#if defined(LUA_USE_MKSTEMP)
#include <unistd.h>
#endif

LUALIB_API int luaL_tmpname (lua_State *L) {
#if defined(LUA_USE_MKSTEMP)
  /* POSIX implementation */
  char buf[32];
  int fd;

  strcpy(buf, "/tmp/lua_XXXXXX");
  fd = mkstemp(buf);

  if (fd != -1) {
    close(fd);
    lua_pushstring(L, buf);
    return 0;
  } else {
    return 1;
  }
#else
  /* Default implementation */
  char buf[L_tmpnam];
  if (tmpnam(buf) != NULL) {
    lua_pushstring(L, buf);
    return 0;
  } else {
    return 1;
  }
#endif
}

LUALIB_API void luaL_writestring (const char *s, size_t sz) {
  fwrite(s, sizeof(char), sz, stdout);
}

LUALIB_API void luaL_writestringerror (const char *s, ...) {
  va_list args;
  va_start(args, s);
  vfprintf(stderr, s, args);
  va_end(args);
  fflush(stderr);
}

LUALIB_API void luaL_writeline (void) {
  luaL_writestring("\n", 1);
  fflush(stdout);
}

LUALIB_API FILE *luaL_popen (lua_State *L, const char *cmd, const char *mode) {
  lua_unused(L);
#if defined(LUA_USE_POSIX_POPEN)
  /* POSIX implementation */
  fflush(NULL);
  return popen(cmd, mode);
#elif defined(LUA_USE_WINDOWS_POPEN)
  /* Windows implementation */
  return _popen(cmd, mode);
#else
  /* Default implementation */
  lua_unused(cmd);
  lua_unused(mode);
  luaL_error(L, "'popen' not supported");
  return NULL;
#endif
}

LUALIB_API int luaL_pclose (lua_State *L, FILE *p) {
  lua_unused(L);

#if defined(LUA_USE_POSIX_POPEN)
  /* POSIX implementation */
  return pclose(p);
#elif defined(LUA_USE_WINDOWS_POPEN)
  /* Windows implementation */
  return _pclose(p);
#else
  /* Default implementation */
  lua_unused(p);
  return -1;
#endif
}

LUALIB_API int luaL_checkpopenmode (lua_State *L, const char *mode) {
  lua_unused(L);

#if defined(LUA_USE_WINDOWS_POPEN)
  /* Windows accepts "[rw][bt]?" as valid modes */
  return ((mode[0] == 'r' || mode[0] == 'w') &&
          (mode[1] == '\0' ||
           ((mode[1] == 'b' || mode[1] == 't') && mode[2] == '\0')));
#else
  /* By default, Lua accepts only "r" or "w" as valid modes */
  return ((mode[0] == 'r' || mode[0] == 'w') && mode[1] == '\0');
#endif
}

#if defined(LUA_USE_READLINE)
#include <readline/history.h>
#include <readline/readline.h>
#endif

LUALIB_API int luaL_readline (lua_State *L, const char *prompt) {
#if defined(LUA_USE_READLINE)
  /* libreadline implementation */
  char *str = readline(prompt);

  if (str != NULL) {
    size_t len = strlen(str);
    lua_pushlstring(L, str, len);
    free(str);
    return 0;
  } else {
    return 1;
  }
#else
  /* Default implementation */
  char buf[LUA_MAXINPUT];
  size_t len = (sizeof(buf) / sizeof(buf[0]));

  luaL_writestring(prompt, strlen(prompt));

  if (fgets(buf, (int) len, stdin) != NULL) {
    len = strlen(buf);

    if (len > 0 && buf[len - 1] == '\n') { /* line ends with newline? */
      buf[--len] = '\0';                   /* remove it */
    }

    lua_pushlstring(L, buf, len);
    return 0;
  } else {
    return 1;
  }
#endif
}

LUALIB_API void luaL_saveline (lua_State *L, const char *line) {
  lua_unused(L);

#if defined(LUA_USE_READLINE)
  /* libreadline implementation */
  add_history(line);
#else
  /* Default implementation */
  lua_unused(line);
#endif
}
