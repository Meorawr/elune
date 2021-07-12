cmake_minimum_required(VERSION 3.20)

include(CheckCompilerFlag)

#
# Build Configuration
#
# The defaults set below are set up to match the WoW-provided Lua environment
# as closely as possible, as a result the following will occur:
#
#   - Lua will be compiled with C++ instead of C, which use exceptions
#     instead of setjmp/longjmp to handle protected call jumps.
#
#   - The compilation flags will enable fast math (-ffast-math or /fp:fast)
#     operations to match the compiled game client. This can result in odd
#     behaviours around handling of NaN values.
#

option(BUILD_SHARED_LIBS "Build shared libraries?" ON)
option(LUA_BUILD_VERSIONED_NAMES "Build with versions in output names?" ON)
option(LUA_BUILD_WITH_CXX "Build with a C++ compiler instead of C?" ON)
option(LUA_BUILD_WITH_FAST_MATH "Build with fast math compiler flags?" ON)
option(LUA_BUILD_INTERPRETER "Build lua interpreter executable?" ON)
option(LUA_BUILD_COMPILER "Build luac compiler executable?" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin" CACHE STRING "Directory to place runtime artifacts")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib" CACHE STRING "Directory to place library artifacts")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib" CACHE STRING "Directory to place archive artifacts")
set(CMAKE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin" CACHE STRING "Directory to place PDB artifacts")
set(CMAKE_C_STANDARD 90 CACHE STRING "C standard version")
set(CMAKE_C_STANDARD_REQUIRED ON CACHE STRING "Require C standard version?")
set(CMAKE_C_EXTENSIONS OFF CACHE STRING "Enable C compiler extensions?")
set(CMAKE_CXX_STANDARD 98 CACHE STRING "C++ standard version")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE STRING "Require C++ standard version?")
set(CMAKE_CXX_EXTENSIONS OFF CACHE STRING "Enable C++ compiler extensions?")
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE STRING "Compile with support for position independent code?")

# The language for the build is stored in string form as LUA_COMPILER_LANG
# and can be referenced internally as needed.

if(LUA_BUILD_WITH_CXX)
  set(LUA_COMPILER_LANG CXX)
else()
  set(LUA_COMPILER_LANG C)
endif()

project(lua LANGUAGES ${LUA_COMPILER_LANG})

# Configure fast math flags if enabled for this build, detecting based on
# whatever is supported by the current compiler.

if(LUA_BUILD_WITH_FAST_MATH)
  check_compiler_flag(${LUA_COMPILER_LANG} "-ffast-math" LUA_COMPILER_HAS_FFAST_MATH)
  check_compiler_flag(${LUA_COMPILER_LANG} "/fp:fast" LUA_COMPILER_HAS_FP_FAST)

  if(LUA_COMPILER_HAS_FFAST_MATH)
    add_compile_options(-ffast-math)
  elseif(LUA_COMPILER_HAS_FP_FAST)
    add_compile_options(/fp:fast)
  else()
    message(WARNING "Unable to find a supported fast math compiler flag.")
  endif()
endif()

# If versioned artifact names are enabled, LUA_OUTPUT_SUFFIX will evaluate
# to a non-empty string like "5.1".

if(LUA_BUILD_VERSIONED_NAMES)
  set(LUA_OUTPUT_SUFFIX "5.1")
else()
  set(LUA_OUTPUT_SUFFIX "")
endif()

add_subdirectory(src)
