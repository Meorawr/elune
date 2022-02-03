# Changelog

## [Unreleased]
### Added
- Added Lua debug APIs for function utilities:
  - Added `c = debug.newcfunction(f)` which wraps a function `f` in a C function `c`.
  - Added `isC = debug.iscfunction(f)` which returns true if `f` is a C function.
- Added Lua debug APIs for profiling and memory measurement:
  - Added `size = debug.getobjectsize(o)` which returns the approximate size of a collectable object.
  - Added `stats = debug.getglobalstats()` which returns global execution and memory statistics.
  - Added `stats = debug.getfunctionstats(f)` which returns execution statistics for a function `f`.
  - Added `stats = debug.getsourcestats(name)` which returns execution and memory statistics for objects and functions tainted by `name`, or untainted ones if `name` is nil.
  - Added `debug.enablestats([enable])` which controls the measurement of timing statistics.
  - Added `debug.collectstats()` which will can be used to flush and update cached statistics.
  - Added `debug.resetstats()` which will flush cached statistics.
- Added Lua debug APIs for high-precision timers:
  - Added `time = debug.microclock()` which returns the time-since-startup in microseconds.
- Added `security` library for querying and manipulating taint.
- Added a Lua API for secure random number generation - `securerandom()`. This accepts the same arguments as `math.random`.
- Added support for the `__environment` metatable key to `getfenv`.
- Added a `-t` flag to the interpreter to run inline scripts (`-e`) or load files insecurely.
  - When this flag is specified, any modules loaded via `-l` will still be loaded securely.

### Changed
- Made significant changes to how taint is propagated throughout the system, particularly with respect to coroutines.
- Build process now requires CMake 3.21 and makes use of CMake presets for common build targets. See README for more information.

### Removed
- Removed the taint field on the `lua_Debug` to fix potential binary incompatibilities.
- Removed the `"s"` field from `debug.getinfo`. Use `security.getstacktaint()` instead.
- Removed `debug.forcesecure()`. Use `security.setstacktaint(nil)` instead.

## [v1]
### Added
- Added basic taint propagation system.
- Added the following security extensions to the base library:
  - `forceinsecure()`
  - `geterrorhandler()`
  - `hooksecurefunc([table,] "name", func)`
  - `issecure()`
  - `issecurevariable([table,] "variable")`
  - `scrub(...)`
  - `seterrorhandler(errfunc)`
- Added the following security extensions to the debug library:
  - `debug.forcesecure()`
- Added a new hook mask (`"s"`) which is invoked whenever the VM transitions between security states. This occurs after processing of the current instruction.
- Added a debug trap system that allows configuring errors under certain runtime conditions.
  - This can be controlled through the `debug.settrapmask("mask")` and `debug.gettrapmask()` functions.
  - The `"s"` mask enables integer overflow errors in `string.format` falls for signed format specifiers.
  - The `"u"` mask enables integer overflow errors in `string.format` calls for unsigned format specifiers.
  - The `"z"` mask enables divide-by-zero errors for division and modulo operations.
  - The default trap mask matches that of a live retail client environment and is set to `"s"`.
- Added all string library extensions present in the in-game environment as well as their global aliases.
- Added all table library extensions present in the in-game environment as well as their global aliases.
- Added all math library extensions present in the in-game environment as well as their global aliases.
- Added all global aliases to the OS libary functions as present in the in-game environment.

[Unreleased]: https://github.com/Meorawr/tainted-lua/compare/v1...HEAD
[v1]: https://github.com/Meorawr/tainted-lua/releases/tag/v1
