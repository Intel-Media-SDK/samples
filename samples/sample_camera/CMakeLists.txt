if(0)
include_directories (
  ${CMAKE_SOURCE_DIR}/sample_common/include
  ${CMAKE_SOURCE_DIR}/sample_camera/include
)

list( APPEND LIBS_NOVARIANT sample_common )

set(DEPENDENCIES libmfx dl pthread)
make_executable( shortname none )
endif(0)