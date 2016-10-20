cmake_minimum_required( VERSION 2.6.2 FATAL_ERROR )
project( mediasdk-samples )

option( CMAKE_MFX_HOME "Where is Media SDK?" OFF )
option( CMAKE_VTUNE_HOME "Where is VTune?" OFF )

option( ENABLE_ITT "Build targets with ITT instrumentation support (requires VTune)?" OFF )

option( ENABLE_SW "Build SW versions of the targets?" OFF )

if( ${CMAKE_SYSTEM_NAME} MATCHES Linux )
option( ENABLE_DRM "Build DRM versions of the targets?" ON )
option( ENABLE_X11 "Build X11 versions of the targets?" ON )
option( ENABLE_X11_DRI3 "Build X11 DRI3 versions of the targets?" OFF )
option( ENABLE_WAYLAND "Build WAYLAND versions of the targets?" OFF )
endif()

option( ENABLE_FFMPEG "Build targets dependent on FFmpeg?" ON )
option( ENABLE_OPENCL "Build targets dependent on OpenCL?" ON )

include( ../builder/FindFunctions.cmake )
include( ../builder/FindGlobals.cmake )
include( ../builder/FindMFX.cmake )
include( ../builder/FindPackages.cmake )
include( ../builder/FindVTune.cmake )

set( NOT_CONFIGURED "" CACHE INTERNAL "" )
set( SKIPPING "" CACHE INTERNAL "" )
set( BUILDING "" CACHE INTERNAL "" )

create_build( )

report_targets("The following targets were NOT configured:" "${NOT_CONFIGURED}")
report_targets("The following targets will be ${BoldRed}SKIPPED${EndColor}:" "${SKIPPING}")
report_targets("The following targets will be ${BoldGreen}BUILT${EndColor}:" "${BUILDING}")
