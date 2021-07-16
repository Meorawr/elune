cmake_minimum_required(VERSION 3.16)

option(BUILD_SHARED_LIBS "Build lua as a shared library?" ON)

project(tainted-lua
  VERSION       "5.1.5"
  DESCRIPTION   "Lua 5.1 with support for World of Warcraft-style tainted execution"
  HOMEPAGE_URL  "https://github.com/Meorawr/tainted-lua"
  LANGUAGES     C
)

add_subdirectory(src)
