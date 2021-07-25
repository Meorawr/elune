# Changelog

## [Unreleased]
### Added
- Added a `-t` flag to the interpreter to run inline scripts (`-e`) or load files insecurely.
  - When this flag is specified, any modules loaded via `-l` will still be loaded securely.
- Added a `debug.gettaintsource([thread])` function which returns the string identifier of any taint source current for the supplied thread, or the current if none is supplied.
  - This replaces the `"s"` field usable with `debug.getinfo`, which has now been removed.
- Added a `debug.issecurelocal([thread,] level, local)` function that works similarly to `issecurevariable`.
- Added a `debug.issecureupvalue(func, up)` function that works similarly to `issecurevariable`.

### Changed
- Reworked taint propagation to work at a C API/VM opcode level instead of object macro level.
- Fixed various initialization issues with taint that could cause some values to incorrectly be considered tainted upon creation.
- Build process now requires CMake 3.20 and makes use of CMake presets for common build targets. See README for more information.

### Removed
- Removed the taint field on the `lua_Debug` to fix potential binary incompatibilities.
- Removed the `"s"` field from `debug.getinfo` to return taint information.

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
