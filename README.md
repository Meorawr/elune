# tainted-lua

tainted-lua is a customized Lua 5.1 implementation which extends the virtual machine with an implementation of tainted execution, similar to that found in the UI APIs of World of Warcraft.

This is mostly just a toy project and shouldn't be used in any serious capacity. It's the product wanting to see if theories on the implementation of the taint mechanism found ingame match up with reality. The current state of this project is bound to have an extremely large number of inaccuracies compared to the real environment. Things may improve over time, or this may just get dropped as only having been a temporary curiosity.

## Building

The original Makefiles have been replaced with a CMake-based build. Note that only builds on Linux (GCC), macOS (Clang) and Windows (MSVC) are supported. These are implemented as presets in the `CMakePresets.json` file.

For a build with all components enabled in a release configuration, the following CMake commands will configure and build the project in a `build/<preset>` directory. The resulting binaries for Lua can be found in `bin/Release/` and `lib/Release/` subdirectories of that folder.

```sh
cmake --preset <linux|macos|windows>
cmake --build --preset <linux|windows> [--config <Debug|Release>]
```

Installation of the compiled artifacts to a specified target directory can be performed with the following command.

```sh
cmake --install build/<preset> [--config <Debug|Release>] [--prefix <path to install to>] [--strip]
```

To generate a fully packaged release the following can be executed to create a set of `.tar.xz` and `.zip` files in the build directory for the selected preset.

```sh
cmake --build --preset <linux|macos|windows> [--config <Debug|Release>] --target package
```

## License

This project is based upon the original Lua 5.1 source which is available under the MIT license as documented in the `LICENSE` file at the root of the repository. All modifications atop the original source are also covered under the same license terms.

## Contributors

- [Daniel "Meorawr" Yates](https://github.com/meorawr)
