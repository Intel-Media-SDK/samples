include_directories (
  ${CMAKE_SOURCE_DIR}/sample_common/include
  ${CMAKE_SOURCE_DIR}/sample_misc/wayland/include
  ${CMAKE_SOURCE_DIR}/sample_decode/include
)

list( APPEND LIBS_VARIANT sample_common )

set(DEPENDENCIES libmfx dl pthread)
make_executable( shortname universal )