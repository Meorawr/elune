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

if(BUILD_INSTALL)
  install(
    TARGETS lua
    EXPORT elune-lua-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
  )

  install(
    EXPORT elune-lua-targets
    FILE elune-lua-targets.cmake
    NAMESPACE elune::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/elune"
    COMPONENT Development
  )
endif()
