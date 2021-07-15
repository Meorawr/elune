# tainted-lua

tainted-lua is a customized Lua 5.1 implementation which extends the virtual machine with an implementation of tainted execution, similar to that found in the UI APIs of World of Warcraft.

This is mostly just a toy project and shouldn't be used in any serious capacity. It's the product wanting to see if theories on the implementation of the taint mechanism found ingame match up with reality. The current state of this project is bound to have an extremely large number of inaccuracies compared to the real environment. Things may improve over time, or this may just get dropped as only having been a temporary curiosity.

## Building

The original Makefiles have been replaced with a CMake-based build. Note that only builds on Linux (via GCC and Clang) and Windows (via MSVC) are supported. Mac OS X is as yet untested.

```sh
cmake -S <path to checkout> -B build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build/

# To install the generated binaries and libraries run the following.
cmake --install build/ --prefix <path to install prefix>
```

## License

This project is based upon the original Lua 5.1 source which is available under the MIT license as documented in the `LICENSE` file at the root of the repository. All modifications atop the original source are also covered under the same license terms.

## Contributors

- [Daniel "Meorawr" Yates](https://github.com/meorawr)
