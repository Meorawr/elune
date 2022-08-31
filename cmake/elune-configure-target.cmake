# Configures TARGET with common properties used throughout the project.
macro(elune_configure_target TARGET)
  set_target_properties(
    ${TARGET}
    PROPERTIES
      C_EXTENSIONS OFF
      C_STANDARD 11
      C_STANDARD_REQUIRED ON
      C_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN ON
      BUILD_RPATH_USE_ORIGIN ON
      INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
  )
endmacro()
