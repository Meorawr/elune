add_executable(lua)
add_executable(elune::lua ALIAS lua)
elune_configure_target(lua)

set_target_properties(
  lua
  PROPERTIES
    OUTPUT_NAME ${LUA_OUTPUT_NAME}
)

target_link_libraries(
  lua
  PRIVATE
    elune::liblua
    $<$<PLATFORM_ID:Darwin,Linux>:readline>
)

target_sources(
  lua
  PRIVATE
    lua.c
)
