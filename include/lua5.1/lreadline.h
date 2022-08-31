/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */

#ifndef lreadline_h
#define lreadline_h

/*
** lua_readline defines how to show a prompt and then read a line from
** the standard input.
** lua_saveline defines how to "save" a read line in a "history".
** lua_freeline defines how to free a line read by lua_readline.
*/
#if !defined(lua_readline)      /* { */

#if defined(LUA_USE_READLINE)   /* { */

#include <readline/readline.h>
#include <readline/history.h>
#define lua_initreadline(L)     ((void)L, rl_readline_name="lua")
#define lua_readline(L,b,p)     ((void)L, ((b)=readline(p)) != NULL)
#define lua_saveline(L,line)    ((void)L, add_history(line))
#define lua_freeline(L,b)       ((void)L, free(b))

#else                           /* }{ */

#define lua_initreadline(L)  ((void)L)
#define lua_readline(L,b,p) \
        ((void)L, fputs(p, stdout), fflush(stdout),  /* show prompt */ \
        fgets(b, LUA_MAXINPUT, stdin) != NULL)  /* get line */
#define lua_saveline(L,line)    { (void)L; (void)line; }
#define lua_freeline(L,b)       { (void)L; (void)b; }

#endif                          /* } */

#endif                          /* } */

#endif
