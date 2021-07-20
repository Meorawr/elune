if(NOT BUILD_TESTING)
  return()
endif()

add_executable(lua-tests)

set_target_properties(lua-tests
  PROPERTIES
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
)

target_include_directories(lua-tests
  PRIVATE
    "../extern/acutest/include"
)

target_link_libraries(lua-tests
  PRIVATE
    liblua
)

target_sources(lua-tests
  PRIVATE
    "security.c"
)
