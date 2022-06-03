include(FetchContent)

FetchContent_Declare(
  utf8h
  GIT_REPOSITORY "https://github.com/sheredom/utf8.h.git"
  GIT_TAG "ce48f0eda6cac8f365837edaf8298ad5c03f7f2e"
)

# The target repository doesn't include a CMake project so we'll declare our
# own interface target to give access to its headers.

FetchContent_GetProperties(utf8h)

if(NOT utf8h_POPULATED)
  FetchContent_Populate(utf8h)

  add_library(utf8h INTERFACE)
  add_library(sheredom::utf8h ALIAS utf8h)
  target_include_directories(utf8h INTERFACE ${utf8h_SOURCE_DIR})
endif()
