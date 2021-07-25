if(NOT BUILD_TESTING)
  return()
endif()

# generate_test_fixture generates a C source and header file for a precompiled
# Lua script which is linked into the final test binaries.

function(generate_test_fixture)
  cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "NAME;SOURCE" "")

  if(NOT ARG_NAME)
    message(FATAL_ERROR "generate_test_fixture: no symbol NAME specified")
  elseif(NOT ARG_SOURCE)
    message(FATAL_ERROR "generate_test_fixture: no SOURCE file specified")
  endif()

  # Path operations look a bit odd here; the debug information from luac
  # includes the full source file path so we make it absolute from the
  # current source directory then relative from the project root.

  cmake_path(SET "SOURCE_FILE" NORMALIZE ${ARG_SOURCE})
  cmake_path(ABSOLUTE_PATH "SOURCE_FILE" BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  cmake_path(RELATIVE_PATH "SOURCE_FILE" BASE_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE "LUAC_SOURCE_FILE")

  # For the output file(s) we just need to transform the extension.

  cmake_path(SET "LUAC_OUTPUT_FILE" NORMALIZE ${ARG_SOURCE})
  cmake_path(ABSOLUTE_PATH "LUAC_OUTPUT_FILE" BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  cmake_path(REPLACE_EXTENSION "LUAC_OUTPUT_FILE" LAST_ONLY "luac")

  cmake_path(SET "C_OUTPUT_FILE" NORMALIZE ${ARG_SOURCE})
  cmake_path(ABSOLUTE_PATH "C_OUTPUT_FILE" BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  cmake_path(REPLACE_EXTENSION "C_OUTPUT_FILE" LAST_ONLY "c")

  add_custom_command(
    OUTPUT
      ${LUAC_OUTPUT_FILE}
      ${C_OUTPUT_FILE}
    COMMAND
      luac -o ${LUAC_OUTPUT_FILE} ${LUAC_SOURCE_FILE}
    COMMAND
      mkembed ${ARG_NAME} ${LUAC_OUTPUT_FILE} ${C_OUTPUT_FILE}
    WORKING_DIRECTORY
      ${PROJECT_SOURCE_DIR}
    DEPENDS
      mkembed
      ${SOURCE_FILE}
    COMMENT
      "Generating test fixture ${ARG_NAME}"
    VERBATIM
  )

  target_sources(lua-tests
    PRIVATE
      ${C_OUTPUT_FILE}
  )
endfunction()

# lua-tests is the main target for testing containing our test suite for
# extensions implemented as part of the project.

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
    ${CMAKE_CURRENT_SOURCE_DIR}  # Required for generated fixtures.
)

target_link_libraries(lua-tests
  PRIVATE
    liblua
)

target_sources(lua-tests
  PRIVATE
    "common.c"
    "common.h"
    "security.c"
)

generate_test_fixture(NAME "luac_mixin" SOURCE "fixtures/mixin.lua")
generate_test_fixture(NAME "luac_rectanglemixin" SOURCE "fixtures/rectanglemixin.lua")
generate_test_fixture(NAME "luac_issecurevariable" SOURCE "fixtures/issecurevariable.lua")
generate_test_fixture(NAME "luac_securecallaux" SOURCE "fixtures/securecallaux.lua")
generate_test_fixture(NAME "luac_ldblib" SOURCE "fixtures/ldblib.lua")
generate_test_fixture(NAME "luac_lvmutil" SOURCE "fixtures/lvmutil.lua")
generate_test_fixture(NAME "luac_lvmarith" SOURCE "fixtures/lvmarith.lua")
generate_test_fixture(NAME "luac_lvmfields" SOURCE "fixtures/lvmfields.lua")
generate_test_fixture(NAME "luac_lvmglobals" SOURCE "fixtures/lvmglobals.lua")
generate_test_fixture(NAME "luac_lvmlocals" SOURCE "fixtures/lvmlocals.lua")
generate_test_fixture(NAME "luac_lvmupvalues" SOURCE "fixtures/lvmupvalues.lua")
