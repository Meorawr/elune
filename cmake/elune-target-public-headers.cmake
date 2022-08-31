# elune_target_public_headers(TARGET [PUBLIC|INTERFACE|PRIVATE <files...>]...)
#
# Adds the supplied source files to TARGET and configures them for installation
# as public header files.
function(elune_target_public_headers TARGET)
  cmake_parse_arguments(PARSE_ARGV 0 "ARG" "" "" "PUBLIC;INTERFACE;PRIVATE")

  target_sources(
    ${TARGET}
    PUBLIC ${ARG_PUBLIC}
    INTERFACE ${ARG_INTERFACE}
    PRIVATE ${ARG_PRIVATE}
  )

  # Ensure CMake is aware that these files are intended to be headers and not
  # actually compiled.

  foreach(_file IN LISTS ARG_PUBLIC ARG_INTERFACE ARG_PRIVATE)
    set_source_files_properties(${_file} PROPERTIES HEADER_FILE_ONLY ON)
  endforeach()

  # Add the headers to the PUBLIC_HEADER property on the target, appending to
  # any existing value set for this property.

  get_target_property(_public_headers ${TARGET} PUBLIC_HEADER)

  # If the property isn't already set then '_public_headers' will be set to
  # "-NOTFOUND", which leads to errors later. Clearing it fixes this.

  if(NOT _public_headers)
    set(_public_headers)
  endif()

  list(APPEND _public_headers ${ARG_PUBLIC} ${ARG_INTERFACE} ${ARG_PRIVATE})
  set_target_properties(${TARGET} PROPERTIES PUBLIC_HEADER "${_public_headers}")
endfunction()
