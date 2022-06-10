# Changelog

## [Unreleased]
### Added
- Reimplemented taint propagation from the ground-up, fixing a large amount of bugs and unimplemented aspects particularly around coroutines.
- Added C and Lua APIs for creating and testing C closures.
- Added C and Lua APIs for profiling and memory measurement.
- Added C and Lua APIs for high-precision timers.
- Added C and Lua APIs for querying and manipulating taint.
- Added a Lua API for secure random number generation.
  - At present this is only implemented for Windows; on other platforms this will fall back to `rand()`.
- Added support for the `__environment` metatable key to `getfenv`.
- Added a `-p` flag to the interpreter to enable performance accounting.
- Added a `-t` flag to the interpreter to run inline scripts (`-e`) or load files insecurely.
  - When this flag is specified, any modules loaded via `-l` will still be loaded securely.

### Changed
- Build process now requires CMake 3.21 and makes use of CMake presets for common build targets. See README for more information.

### Removed
- Removed the taint field on the `lua_Debug` to fix potential binary incompatibilities.
- Removed the `"s"` field from `debug.getinfo`. Use `debug.getstacktaint()` instead.
- Removed `debug.forcesecure()`. Use `debug.setstacktaint(nil)` instead.

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
