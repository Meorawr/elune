add_executable(luac)
add_executable(elune::luac ALIAS luac)

set_target_properties(
  luac
  PROPERTIES
    OUTPUT_NAME ${LUAC_OUTPUT_NAME}
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

target_include_directories(
  luac
  PRIVATE
    $<TARGET_GENEX_EVAL:liblua,$<TARGET_PROPERTY:liblua,INCLUDE_DIRECTORIES>>
)

target_link_libraries(
  luac
  PRIVATE
    $<TARGET_GENEX_EVAL:liblua,$<TARGET_PROPERTY:liblua,LINK_LIBRARIES>>
)

target_sources(
  luac
  PRIVATE
    luac.c
    print.c
    $<TARGET_OBJECTS:liblua>
)
