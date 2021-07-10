cmake_minimum_required(VERSION 3.16)
project(lua C)

option(BUILD_SHARED_LIBS "Build shared libraries?" ON)
option(BUILD_VERSIONED_NAMES "Build with versions in output names?" ON)

add_subdirectory(src)
