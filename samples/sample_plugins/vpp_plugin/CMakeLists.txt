include_directories (
  ${CMAKE_SOURCE_DIR}/sample_common/include
  ${CMAKE_SOURCE_DIR}/sample_plugins/vpp_plugin/include
)

make_library( shortname none static )
