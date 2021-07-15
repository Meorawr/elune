cmake_minimum_required(VERSION 3.16)

option(BUILD_SHARED_LIBS "Build lua as a shared library?" ON)
option(BUILD_LUA_WITH_CXX "Build with a C++ compiler instead of C?" OFF)
option(BUILD_LUA_WITH_FAST_MATH "Build with fast math compiler flags?" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin" CACHE STRING "Directory to place runtime artifacts")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib" CACHE STRING "Directory to place library artifacts")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib" CACHE STRING "Directory to place archive artifacts")
set(CMAKE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin" CACHE STRING "Directory to place PDB artifacts")

# The language for the build is stored in string form as LUA_BUILD_LANGUAGE
# and can be referenced internally as needed.

if(BUILD_LUA_WITH_CXX)
  set(LUA_BUILD_LANGUAGE CXX)
  include(CheckCXXCompilerFlag)
else()
  set(LUA_BUILD_LANGUAGE C)
  include(CheckCCompilerFlag)
endif()

project(tainted-lua
  VERSION       "5.1.5"
  DESCRIPTION   "Lua 5.1 with support for World of Warcraft-style tainted execution"
  HOMEPAGE_URL  "https://github.com/Meorawr/tainted-lua"
  LANGUAGES     ${LUA_BUILD_LANGUAGE}
)

# Configure fast math flags if enabled for this build, detecting based on
# whatever is supported by the current compiler.

if(BUILD_LUA_WITH_FAST_MATH)
  if(BUILD_LUA_WITH_CXX)
    check_cxx_compiler_flag("-ffast-math" LUA_COMPILER_HAS_FFAST_MATH)
    check_cxx_compiler_flag("/fp:fast" LUA_COMPILER_HAS_FP_FAST)
  else()
    check_c_compiler_flag("-ffast-math" LUA_COMPILER_HAS_FFAST_MATH)
    check_c_compiler_flag("/fp:fast" LUA_COMPILER_HAS_FP_FAST)
  endif()

  if(LUA_COMPILER_HAS_FFAST_MATH)
    add_compile_options(-ffast-math)
  elseif(LUA_COMPILER_HAS_FP_FAST)
    add_compile_options(/fp:fast)
  else()
    message(WARNING "Unable to find a supported fast math compiler flag.")
  endif()
endif()

add_subdirectory(src)
