add_executable(lua)
add_executable(elune::lua ALIAS lua)

set_target_properties(
  lua
  PROPERTIES
    OUTPUT_NAME ${LUA_OUTPUT_NAME}
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    BUILD_RPATH_USE_ORIGIN ON
    INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
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
