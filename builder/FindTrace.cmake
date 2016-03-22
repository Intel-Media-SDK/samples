if(__TRACE)
  message( STATUS "Enabled tracing: ${__TRACE}" )
endif()

if(__TRACE MATCHES itt OR __TRACE MATCHES all)
  set(__ITT TRUE)
  include (${CMAKE_CURRENT_LIST_DIR}/FindVTune.cmake)
endif()

if(__TRACE MATCHES all)
  append("-DMFX_TRACE_ENABLE_TEXTLOG -DMFX_TRACE_ENABLE_STAT" CMAKE_C_FLAGS)
  append("-DMFX_TRACE_ENABLE_TEXTLOG -DMFX_TRACE_ENABLE_STAT" CMAKE_CXX_FLAGS)
endif()
