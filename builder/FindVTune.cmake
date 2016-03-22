if(__ITT)
  if( Linux )
    if( __ARCH MATCHES intel64 )
      set( arch "64" )
    elseif()
      set( arch "32" )
    endif( )

    find_path( VTUNE_INCLUDE ittnotify.h PATHS $ENV{MFX_VTUNE_PATH}/include )
    find_library( VTUNE_LIBRARY libittnotify.a PATHS $ENV{MFX_VTUNE_PATH}/lib${arch}/ )

    if(NOT VTUNE_INCLUDE MATCHES NOTFOUND)
      if(NOT VTUNE_LIBRARY MATCHES NOTFOUND)
        set( VTUNE_FOUND TRUE )
        include_directories( ${VTUNE_INCLUDE} )

        get_filename_component( VTUNE_LIBRARY_PATH ${VTUNE_LIBRARY} PATH )
        link_directories( ${VTUNE_LIBRARY_PATH} )

      endif( )
    endif( )

    if(NOT DEFINED VTUNE_FOUND)
      message( FATAL_ERROR "ITT was not found! Set/check MFX_VTUNE_PATH environment variable!" )
    else ( )
      message( STATUS "ITT was found here $ENV{MFX_VTUNE_PATH}" )
    endif( )

    append("-DMFX_TRACE_ENABLE_ITT" CMAKE_C_FLAGS)
    append("-DMFX_TRACE_ENABLE_ITT" CMAKE_CXX_FLAGS)
    
    set( ITT_LIBS "" )
    list( APPEND ITT_LIBS
      mfx_trace
      ittnotify
      dl
    )

  else()
    message( FATAL_ERROR "MFX tracing is supported only for linux!")
  endif()
endif()
