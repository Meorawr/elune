include(FetchContent)

FetchContent_Declare(
  acutest
  GIT_REPOSITORY "https://github.com/mity/acutest.git"
  GIT_TAG "cce300734bfe5c3879b0449ac283a872633e615c"
)

# The target repository doesn't export an interface target for its headers so
# we'll ignore its subdirectory and make our own target with blackjack and
# includes.

FetchContent_GetProperties(acutest)

if(NOT acutest_POPULATED)
  FetchContent_Populate(acutest)

  add_library(acutest INTERFACE)
  add_library(mity::acutest ALIAS acutest)
  target_include_directories(acutest INTERFACE "${acutest_SOURCE_DIR}/include")
endif()
