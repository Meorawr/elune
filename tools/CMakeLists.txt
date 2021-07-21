# mkembed is used by the test targets to generate a C source/header file with
# embedded static data fetched from a file.

add_executable(mkembed)

set_target_properties(mkembed
  PROPERTIES
    C_EXTENSIONS OFF
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
    EXCLUDE_FROM_DEFAULT_BUILD ON
)

target_sources(mkembed
  PRIVATE
    "mkembed.c"
)
