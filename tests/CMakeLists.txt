include(FetchContent)

FetchContent_Declare(
  acutest
  GIT_REPOSITORY "https://github.com/mity/acutest.git"
  GIT_TAG "cce300734bfe5c3879b0449ac283a872633e615c"
)

FetchContent_GetProperties(acutest)

if(NOT acutest_POPULATED)
  FetchContent_Populate(acutest)

  add_library(acutest INTERFACE IMPORTED GLOBAL)
  add_library(mity::acutest ALIAS acutest)
  target_include_directories(acutest INTERFACE "${acutest_SOURCE_DIR}/include")
endif()


add_executable(luatest)
add_executable(elune::luatest ALIAS luatest)
elune_configure_target(luatest)

target_link_libraries(
  luatest
  PRIVATE
    elune::liblua
    mity::acutest
)

target_sources(
  luatest
  PRIVATE
    luatest.c
)

elune_target_copy_file(
  luatest
  SOURCE luatest_scriptcases.lua
  OUTPUT luatest_scriptcases.lua
)

elune_target_copy_file(
  luatest
  SOURCE luatest_coroutine.lua
  OUTPUT luatest_coroutine.lua
)

elune_target_copy_file(
  luatest
  SOURCE luatest_profiling.lua
  OUTPUT luatest_profiling.lua
)
