cmake_minimum_required(VERSION 3.22)

project(
  Elune
  VERSION 2
  DESCRIPTION "Customized Lua 5.1 distribution aimed at replicating the World of Warcraft addon environment"
  HOMEPAGE_URL "https://github.com/Meorawr/elune"
  LANGUAGES C
)

list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

include(elune-configure-target)
include(elune-target-copy-file)
include(elune-target-public-headers)

#
# Platform Detection
#

# Configure stems for Lua library search paths that'll be used to initialize
# user-configurable cache variables later.

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(LUA_ROOT_INIT "!")
  set(LUA_LDIR_INIT "\\lua")
  set(LUA_CDIR_INIT "")
  set(LUA_DIRSEP "\\")
else()
  set(LUA_ROOT_INIT "/usr/local")
  set(LUA_LDIR_INIT "/${CMAKE_INSTALL_DATADIR}/lua/5.1")
  set(LUA_CDIR_INIT "/${CMAKE_INSTALL_LIBDIR}/lua/5.1")
  set(LUA_DIRSEP "/")
endif()

# Lua has an interesting history of output names across systems; we supply
# sensible defaults here that can be overridden by cache variables if needed.

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(LIBLUA_OUTPUT_NAME_INIT "lua51")
  set(LUA_OUTPUT_NAME_INIT "lua")
  set(LUAC_OUTPUT_NAME_INIT "luac")
else()
  set(LIBLUA_OUTPUT_NAME_INIT "lua5.1")
  set(LUA_OUTPUT_NAME_INIT "lua5.1")
  set(LUAC_OUTPUT_NAME_INIT "luac5.1")
endif()

#
# Project Options
#

option(BUILD_SHARED_LIBS ON "Build components as shared libraries?")
option(BUILD_TESTING "Build test executables?" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_INSTALL "Enable the generation of installation targets?" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_PACKAGE "Enable the generation of packaging targets?" ${PROJECT_IS_TOP_LEVEL})

option(BUILD_LUA "Build the Lua interpreter target?" ON)
option(BUILD_LUAC "Build the Lua compiler target?" ON)

set(LIBLUA_OUTPUT_NAME ${LIBLUA_OUTPUT_NAME_INIT} CACHE STRING "Base output name of the Lua library target")
set(LUA_OUTPUT_NAME ${LUA_OUTPUT_NAME_INIT} CACHE STRING "Base output name of the Lua interpreter target")
set(LUAC_OUTPUT_NAME ${LUAC_OUTPUT_NAME_INIT} CACHE STRING "Base output name of the Lua compiler target")

set(LUA_ROOT "${LUA_ROOT_INIT}" CACHE STRING "Base path prefix from which Lua will search for Lua and C libraries")
set(LUA_CDIR "${LUA_CDIR_INIT}" CACHE STRING "Subdirectory within LUA_ROOT from which Lua will search for C libraries")
set(LUA_LDIR "${LUA_LDIR_INIT}" CACHE STRING "Subdirectory within LUA_ROOT from which Lua will search for Lua libraries")
set(LUA_CPATH_DEFAULT "" CACHE STRING "Custom default search path for C libraries")
set(LUA_PATH_DEFAULT "" CACHE STRING "Custom default search path for Lua libraries")

set(LUA_ASSERT_CONFIGS "Debug" CACHE STRING "List of build configurations where Lua assertions are enabled")
set(LUA_APICHECK_CONFIGS "Debug" CACHE STRING "List of build configurations where Lua API checks are enabled")

#
# Project Configuration
#

set(LUA_USE_SHARED ${BUILD_SHARED_LIBS})

# For library paths we support two avenues of configuration.
#
# The 'LUA_PATH_DEFAULT' and 'LUA_CPATH_DEFAULT' cache variables can be
# set to semicolon-delimited list of path templates. By default these
# cache variables are empty strings.
#
# If the above path variables are empty, the 'LUA_ROOT', 'LUA_LDIR', and
# 'LUA_CDIR' cache variables will instead be used to configure a sensible
# pair of search paths.

if(NOT LUA_PATH_DEFAULT)
  set(LUA_PATH_DEFAULT)
  list(APPEND LUA_PATH_DEFAULT ".${LUA_DIRSEP}?.lua")
  list(APPEND LUA_PATH_DEFAULT "${LUA_ROOT}${LUA_LDIR}${LUA_DIRSEP}?.lua")
  list(APPEND LUA_PATH_DEFAULT "${LUA_ROOT}${LUA_LDIR}${LUA_DIRSEP}?${LUA_DIRSEP}init.lua")
  list(APPEND LUA_PATH_DEFAULT "${LUA_ROOT}${LUA_CDIR}${LUA_DIRSEP}?.lua")
  list(APPEND LUA_PATH_DEFAULT "${LUA_ROOT}${LUA_CDIR}${LUA_DIRSEP}?${LUA_DIRSEP}init.lua")
endif()

if(NOT LUA_CPATH_DEFAULT)
  set(LUA_CPATH_DEFAULT)
  list(APPEND LUA_CPATH_DEFAULT ".${LUA_DIRSEP}?${CMAKE_SHARED_LIBRARY_SUFFIX}")
  list(APPEND LUA_CPATH_DEFAULT "${LUA_ROOT}${LUA_CDIR}${LUA_DIRSEP}?${CMAKE_SHARED_LIBRARY_SUFFIX}")
  list(APPEND LUA_CPATH_DEFAULT "${LUA_ROOT}${LUA_CDIR}${LUA_DIRSEP}loadall${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

# The Lua search paths are interpolated into a header file as strings; ensure
# any backslashes (such as directory separators) are appropriately escaped.
#
# For added insanity - the "list(TRANSFORM var REPLACE ...)" call is regex
# based - so we need to escape the slashes here as well!

string(REPLACE "\\" "\\\\" LUA_DIRSEP ${LUA_DIRSEP})
list(TRANSFORM LUA_PATH_DEFAULT REPLACE "\\\\" "\\\\\\\\")
list(TRANSFORM LUA_CPATH_DEFAULT REPLACE "\\\\" "\\\\\\\\")

# Lua needs to know the bit-width of integers for this platform; as we're
# not targetting C23 we can't just use the 'INT_WIDTH' define in <limits.h>.

set(CMAKE_EXTRA_INCLUDE_FILES "limits.h")
check_type_size("char[sizeof(int) * CHAR_BIT]" LUA_INT_WIDTH)
set(CMAKE_EXTRA_INCLUDE_FILES)

# Check for platform-specific APIs; these are typically going to always be
# desirable but can be disabled individually by manually defining the
# equivalent LUA_USE_* cache variables to false.

# Optional Linux APIs

check_symbol_exists(getrandom "sys/random.h" LUA_HAVE_GETRANDOM)
cmake_dependent_option(LUA_USE_GETRANDOM "Use Linux 'getrandom' API for random number generation?" ON "LUA_HAVE_GETRANDOM" OFF)

# Optional macOS APIs

check_symbol_exists(arc4random "stdlib.h" LUA_HAVE_ARC4RANDOM)
cmake_dependent_option(LUA_USE_ARC4RANDOM "Use 'arc4random' API for random number generation?" ON "LUA_HAVE_ARC4RANDOM" OFF)

# Optional POSIX APIs

list(APPEND CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE")
check_symbol_exists(clock_gettime "time.h" LUA_HAVE_CLOCK_GETTIME)
check_symbol_exists(CLOCK_MONOTONIC "time.h" LUA_HAVE_CLOCK_MONOTONIC)
check_symbol_exists(CLOCK_MONOTONIC_RAW "time.h" LUA_HAVE_CLOCK_MONOTONIC_RAW)
check_symbol_exists(mkstemp "stdlib.h" LUA_HAVE_MKSTEMP)
check_symbol_exists(isatty "unistd.h" LUA_HAVE_POSIX_ISATTY)
check_symbol_exists(popen "stdio.h" LUA_HAVE_POSIX_POPEN)
check_symbol_exists(_longjmp "setjmp.h" LUA_HAVE_ULONGJMP)
list(POP_BACK CMAKE_REQUIRED_DEFINITIONS)

if(NOT DEFINED LUA_CLOCK_ID)
  if(LUA_HAVE_CLOCK_MONOTONIC_RAW)
    set(LUA_CLOCK_ID CLOCK_MONOTONIC_RAW)
  elseif(LUA_HAVE_CLOCK_MONOTONIC)
    set(LUA_CLOCK_ID CLOCK_MONOTONIC)
  else()
    set(LUA_CLOCK_ID)
  endif()
endif()

cmake_dependent_option(LUA_USE_CLOCK_GETTIME "Use POSIX 'clock_gettime' API for high resolution timer support?" ON "LUA_HAVE_CLOCK_GETTIME AND LUA_CLOCK_ID" OFF)
cmake_dependent_option(LUA_USE_MKSTEMP "Use POSIX 'mkstemp' API for 'os.tempname' library function?" ON "LUA_HAVE_MKSTEMP" OFF)
cmake_dependent_option(LUA_USE_POSIX_ISATTY "Use POSIX 'isatty' API for interpreter TTY detection?" ON "LUA_HAVE_POSIX_ISATTY" OFF)
cmake_dependent_option(LUA_USE_POSIX_POPEN "Use POSIX 'popen' API for 'io.popen' library function?" ON "LUA_HAVE_POSIX_POPEN" OFF)
cmake_dependent_option(LUA_USE_ULONGJMP "Use POSIX '_longjmp' and '_setjmp' APIs for error handling?" ON "LUA_HAVE_ULONGJMP" OFF)

# Optional Windows APIs

list(APPEND CMAKE_REQUIRED_LIBRARIES "bcrypt")
check_symbol_exists(BCryptGenRandom "windows.h;bcrypt.h" LUA_HAVE_BCRYPTGENRANDOM)
list(POP_BACK CMAKE_REQUIRED_LIBRARIES)
check_symbol_exists(GetProcAddress "windows.h" LUA_HAVE_GETPROCADDRESS)
check_symbol_exists(QueryPerformanceCounter "windows.h" LUA_HAVE_QUERYPERFORMANCECOUNTER)
check_symbol_exists(_isatty "io.h" LUA_HAVE_WINDOWS_ISATTY)
check_symbol_exists(_popen "stdio.h" LUA_HAVE_WINDOWS_POPEN)

cmake_dependent_option(LUA_USE_BCRYPTGENRANDOM "Use Windows 'BCryptGenRandom' API for random number generation?" ON "LUA_HAVE_BCRYPTGENRANDOM" OFF)
cmake_dependent_option(LUA_USE_GETPROCADDRESS "Use Windows 'GetProcAddress' API for dynamic module loading?" ON "LUA_HAVE_GETPROCADDRESS" OFF)
cmake_dependent_option(LUA_USE_QUERYPERFORMANCECOUNTER "Use Windows 'QueryPerformanceCounter' API for high resolution timer support?" ON "LUA_HAVE_QUERYPERFORMANCECOUNTER" OFF)
cmake_dependent_option(LUA_USE_WINDOWS_ISATTY "Use Windows '_isatty' API for interpreter TTY detection?" ON "LUA_HAVE_WINDOWS_ISATTY" OFF)
cmake_dependent_option(LUA_USE_WINDOWS_POPEN "Use Windows '_popen' API for 'io.popen' library function?" ON "LUA_HAVE_WINDOWS_POPEN" OFF)

# Optional libraries

check_library_exists(dl dlopen "" LUA_HAVE_DL)
check_library_exists(readline readline "" LUA_HAVE_READLINE)

cmake_dependent_option(LUA_USE_DL "Link against the 'dl' library for dynamic module loading?" ON "LUA_HAVE_DL" OFF)
cmake_dependent_option(LUA_USE_READLINE "Link against the 'readline' library for the standalone interpreter?" ON "BUILD_LUA AND LUA_HAVE_READLINE" OFF)


# Generate the luaconf.h header file from all of the above. This header is used
# by multiple targets, so it always need to be generated even if ultimately not
# installed.

configure_file(
  "${PROJECT_SOURCE_DIR}/include/lua5.1/luaconf.h.in"
  "${PROJECT_BINARY_DIR}/include/lua5.1/luaconf.h"
  USE_SOURCE_PERMISSIONS
  ESCAPE_QUOTES
)

#
# Project Targets
#

add_subdirectory(src/liblua)

if(BUILD_LUA)
  add_subdirectory(src/lua)
endif()

if(BUILD_LUAC)
  add_subdirectory(src/luac)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if(BUILD_INSTALL)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/elune-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion
  )

  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/elune-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/elune-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/elune"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )

  install(
    FILES
      "${CMAKE_CURRENT_BINARY_DIR}/elune-config-version.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/elune-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/elune"
    COMPONENT Development
  )
endif()

if(BUILD_INSTALL AND BUILD_PACKAGE)
  set(CPACK_PACKAGE_VENDOR "Meorawr")
  string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CPACK_SYSTEM_NAME)
  set(CPACK_GENERATOR "TXZ;ZIP")
  set(CPACK_STRIP_FILES ON)

  include(CPack)
endif()
