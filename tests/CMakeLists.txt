add_executable(tainted-luatest)
add_executable(meorawr::tainted-luatest ALIAS tainted-luatest)

set_target_properties(
  tainted-luatest
  PROPERTIES
    OUTPUT_NAME "luatest"
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

target_link_libraries(
  tainted-luatest
  PRIVATE
    meorawr::tainted-liblua
    mity::acutest
)

target_sources(
  tainted-luatest
  PRIVATE
    luatest.c
    luatest_scriptcases.lua
)

# The scripted test cases need copying to the build directory as they're not
# embedded automatically.

get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
get_property(RUNTIME_OUTPUT_DIRECTORY TARGET tainted-luatest PROPERTY RUNTIME_OUTPUT_DIRECTORY)

set(luatest_script_source "${CMAKE_CURRENT_SOURCE_DIR}/luatest_scriptcases.lua")
set(luatest_script_output "${RUNTIME_OUTPUT_DIRECTORY}/$<$<BOOL:${GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>/>luatest_scriptcases.lua")

add_custom_command(
  OUTPUT ${luatest_script_output}
  DEPENDS ${luatest_script_source}
  COMMAND ${CMAKE_COMMAND} -E copy ${luatest_script_source} ${luatest_script_output}
  VERBATIM
)

set_source_files_properties(
  ${luatest_script_source}
  PROPERTIES
    HEADER_FILE_ONLY ON
)

target_sources(
  tainted-luatest
  PRIVATE
    ${luatest_script_source}
    ${luatest_script_output}
)
