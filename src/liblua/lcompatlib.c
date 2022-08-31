/* Licensed under the terms of the MIT License; see full copyright information
 * in the "LICENSE" file or at <http://www.lua.org/license.html> */

#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

const char script[] = "\n\
-- Table library\n\
local tab = table\n\
foreach = tab.foreach\n\
foreachi = tab.foreachi\n\
getn = tab.getn\n\
tinsert = tab.insert\n\
tremove = tab.remove\n\
sort = tab.sort\n\
wipe = tab.wipe\n\
\n\
-------------------------------------------------------------------\n\
-- math library\n\
local math = math\n\
abs = math.abs\n\
acos = function (x) return math.deg(math.acos(x)) end\n\
asin = function (x) return math.deg(math.asin(x)) end\n\
atan = function (x) return math.deg(math.atan(x)) end\n\
atan2 = function (x,y) return math.deg(math.atan2(x,y)) end\n\
ceil = math.ceil\n\
cos = function (x) return math.cos(math.rad(x)) end\n\
deg = math.deg\n\
exp = math.exp\n\
floor = math.floor\n\
frexp = math.frexp\n\
ldexp = math.ldexp\n\
log = math.log\n\
log10 = math.log10\n\
max = math.max\n\
min = math.min\n\
mod = math.fmod\n\
PI = math.pi\n\
--??? pow = math.pow\n\
rad = math.rad\n\
random = math.random\n\
sin = function (x) return math.sin(math.rad(x)) end\n\
sqrt = math.sqrt\n\
tan = function (x) return math.tan(math.rad(x)) end\n\
\n\
-------------------------------------------------------------------\n\
-- string library\n\
local str = string\n\
strbyte = str.byte\n\
strchar = str.char\n\
strfind = str.find\n\
format = str.format\n\
gmatch = str.gmatch\n\
gsub = str.gsub\n\
strlen = str.len\n\
strlower = str.lower\n\
strmatch = str.match\n\
strrep = str.rep\n\
strrev = str.reverse\n\
strsub = str.sub\n\
strupper = str.upper\n\
-------------------------------------------------------------------\n\
-- Add custom string functions to the string table\n\
str.trim = strtrim\n\
str.split = strsplit\n\
str.join = strjoin\n\
";


LUALIB_API int luaopen_compat (lua_State *L) {
  if (luaL_loadbuffer(L, script, sizeof(script) - 1, "compat.lua") != 0) {
    lua_error(L);
  } else {
    lua_call(L, 0, 0);
  }

  return 0;
}
