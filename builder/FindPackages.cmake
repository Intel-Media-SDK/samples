##******************************************************************************
##  Copyright(C) 2012-2015 Intel Corporation. All Rights Reserved.
##
##  The source code, information  and  material ("Material") contained herein is
##  owned  by Intel Corporation or its suppliers or licensors, and title to such
##  Material remains  with Intel Corporation  or its suppliers or licensors. The
##  Material  contains proprietary information  of  Intel or  its  suppliers and
##  licensors. The  Material is protected by worldwide copyright laws and treaty
##  provisions. No  part  of  the  Material  may  be  used,  copied, reproduced,
##  modified, published, uploaded, posted, transmitted, distributed or disclosed
##  in any way  without Intel's  prior  express written  permission. No  license
##  under  any patent, copyright  or  other intellectual property rights  in the
##  Material  is  granted  to  or  conferred  upon  you,  either  expressly,  by
##  implication, inducement,  estoppel or  otherwise.  Any  license  under  such
##  intellectual  property  rights must  be express  and  approved  by  Intel in
##  writing.
##
##  *Third Party trademarks are the property of their respective owners.
##
##  Unless otherwise  agreed  by Intel  in writing, you may not remove  or alter
##  this  notice or  any other notice embedded  in Materials by Intel or Intel's
##  suppliers or licensors in any way.
##
##******************************************************************************
##  Content: Intel(R) Media SDK Samples projects creation and build
##******************************************************************************

#
# Usage: append_property( <target> <property_name> <property>)
#  Appends settings to the target property.
#
function( append_property target property_name property )
  get_target_property( property ${ARGV0} ${ARGV1} )
  if( property MATCHES NOTFOUND)
    set( property "" )
  endif( )
  string( REPLACE ";" " " property "${ARGV2} ${property}" )
  set_target_properties( ${ARGV0} PROPERTIES ${ARGV1} "${property}" )
endfunction( )

#
# Usage: configure_build_variant( <target> <variant> )
#   Sets compilation and link flags for the specified target according to the
# specified variant.
#
function( configure_build_variant target variant )
  if( Linux )
    configure_build_variant_linux( ${ARGV0} ${ARGV1} )
  elseif( Darwin )
    configure_build_variant_darwin( ${ARGV0} ${ARGV1} )
  endif( )
endfunction( )

function(configure_target target cflags libdirs)
  set(SCOPE_CFLAGS "${SCOPE_CFLAGS} ${ARGV1}" PARENT_SCOPE)
  foreach(libpath ${ARGV2})
    set(SCOPE_LINKFLAGS "${SCOPE_LINKFLAGS} -L${libpath}" PARENT_SCOPE)
  endforeach()
endfunction()

function(configure_libmfx_target target)
  configure_target(${ARGV0} "${LIBMFX_CFLAGS}" "${LIBMFX_LIBRARY_DIRS}")

  set(SCOPE_CFLAGS ${SCOPE_CFLAGS} PARENT_SCOPE)
  set(SCOPE_LINKFLAGS "${SCOPE_LINKFLAGS} -Wl,--default-symver" PARENT_SCOPE)
  if (CMAKE_BUILD_TYPE STREQUAL debug)
    set(SCOPE_LIBS ${SCOPE_LIBS} mfx PARENT_SCOPE)
  else()
    set(SCOPE_LIBS ${SCOPE_LIBS} mfx PARENT_SCOPE)
  endif()
endfunction()

function(configure_libdrm_target target)
  configure_target(${ARGV0} "${PKG_LIBDRM_CFLAGS}" "${PKG_LIBDRM_LIBRARY_DIRS}")

  set(SCOPE_CFLAGS ${SCOPE_CFLAGS} PARENT_SCOPE)
  set(SCOPE_LINKFLAGS ${SCOPE_LINKFLAGS} PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} drm PARENT_SCOPE)
endfunction()

function(configure_libva_target target)
  configure_target(${ARGV0} "${PKG_LIBVA_CFLAGS}" "${PKG_LIBVA_LIBRARY_DIRS}")

  set(SCOPE_CFLAGS "${SCOPE_CFLAGS} -DLIBVA_SUPPORT" PARENT_SCOPE)
  set(SCOPE_LINKFLAGS ${SCOPE_LINKFLAGS} PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} va PARENT_SCOPE)
endfunction()

function(configure_libva_drm_target target)
  configure_target(${ARGV0} "${PKG_LIBVA_DRM_CFLAGS}" "${PKG_LIBVA_DRM_LIBRARY_DIRS}")

  set(SCOPE_CFLAGS "${SCOPE_CFLAGS} -DLIBVA_DRM_SUPPORT" PARENT_SCOPE)
  set(SCOPE_LINKFLAGS ${SCOPE_LINKFLAGS} PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} va-drm PARENT_SCOPE)
endfunction()

function(configure_wayland_target target)
  configure_target(${ARGV0} "${PKG_WAYLAND_CLIENT_CFLAGS}" "${PKG_WAYLAND_CLIENT_LIBRARY_DIRS}")

  set(SCOPE_CFLAGS "${SCOPE_CFLAGS} -DLIBVA_SUPPORT -DLIBVA_WAYLAND_SUPPORT" PARENT_SCOPE)
  set(SCOPE_LINKFLAGS ${SCOPE_LINKFLAGS} PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} drm_intel drm wayland-client PARENT_SCOPE)
endfunction()

function(configure_libva_x11_target target)
  configure_target(${ARGV0} "${PKG_LIBVA_X11_CFLAGS}" "${PKG_LIBVA_X11_LIBRARY_DIRS}")

  set(SCOPE_CFLAGS "${SCOPE_CFLAGS} -DLIBVA_X11_SUPPORT" PARENT_SCOPE)
  set(SCOPE_LINKFLAGS ${SCOPE_LINKFLAGS} PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} va-x11 PARENT_SCOPE)
endfunction()

function(configure_x11_target target)
  configure_target(${ARGV0} "${PKG_X11_CFLAGS}" "${PKG_X11_LIBRARY_DIRS}")

  set(SCOPE_CFLAGS ${SCOPE_CFLAGS} PARENT_SCOPE)
  set(SCOPE_LINKFLAGS ${SCOPE_LINKFLAGS} PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} X11 PARENT_SCOPE)
endfunction()

function(configure_universal_target target)
  set(LOCAL_CFLAGS "")

  if(PKG_LIBVA_FOUND)
    configure_target(${ARGV0} "${PKG_LIBVA_CFLAGS}" "${PKG_LIBVA_LIBRARY_DIRS}")
    set(LOCAL_CFLAGS "${LOCAL_CFLAGS} -DLIBVA_SUPPORT")
    if( ${PKG_LIBVA_VERSION} LESS 0.36 )
       set(LOCAL_CFLAGS "${LOCAL_CFLAGS} -DDISABLE_VAAPI_BUFFER_EXPORT" )
    endif()
  endif()

  if(PKG_LIBVA_DRM_FOUND AND PKG_LIBDRM_FOUND)
    configure_target(${ARGV0} "${PKG_LIBDRM_CFLAGS}" "${PKG_LIBDRM_LIBRARY_DIRS}" "${PKG_LIBVA_DRM_CFLAGS}" "${PKG_LIBVA_DRM_LIBRARY_DIRS}")
    set(LOCAL_CFLAGS "${LOCAL_CFLAGS} -DLIBVA_DRM_SUPPORT")
  endif()

  if (PKG_LIBVA_X11_FOUND AND PKG_X11_FOUND)
    configure_target(${ARGV0} "${PKG_X11_CFLAGS}" "${PKG_X11_LIBRARY_DIRS}" "${PKG_LIBVA_X11_CFLAGS}" "${PKG_LIBVA_X11_LIBRARY_DIRS}")
    set(LOCAL_CFLAGS "${LOCAL_CFLAGS} -DLIBVA_X11_SUPPORT")
  endif()

  if (PKG_WAYLAND_CLIENT_FOUND)
    configure_target(${ARGV0} "${PKG_WAYLAND_CLIENT_CFLAGS}" "${PKG_WAYLAND_CLIENT_LIBRARY_DIRS}")
    set(LOCAL_CFLAGS "${LOCAL_CFLAGS} -DLIBVA_WAYLAND_SUPPORT")
  endif()

  set(SCOPE_CFLAGS "${SCOPE_CFLAGS} ${LOCAL_CFLAGS}" PARENT_SCOPE)
endfunction()

function(configure_dl_target target)
  set(SCOPE_LIBS ${SCOPE_LIBS} dl PARENT_SCOPE)
endfunction()

function(configure_pthread_target target)
  set(SCOPE_LIBS ${SCOPE_LIBS} pthread PARENT_SCOPE)
endfunction()

function(configure_libavutil_target target)
  configure_target(${ARGV0} "${PKG_LIBAVUTIL_CFLAGS}" "${PKG_LIBAVUTIL_LIBRARY_DIRS}")

  set(SCOPE_CFLAGS ${SCOPE_CFLAGS} PARENT_SCOPE)
  set(SCOPE_LINKFLAGS ${SCOPE_LINKFLAGS} PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} avutil PARENT_SCOPE)
endfunction()

function(configure_libavcodec_target target)
  configure_target(${ARGV0} "${PKG_LIBAVCODEC_CFLAGS}" "${PKG_LIBAVCODEC_LIBRARY_DIRS}")

  set(SCOPE_CFLAGS ${SCOPE_CFLAGS} PARENT_SCOPE)
  set(SCOPE_LINKFLAGS ${SCOPE_LINKFLAGS} PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} avcodec PARENT_SCOPE)
endfunction()

function(configure_libavformat_target target)
  configure_target(${ARGV0} "${PKG_LIBAVFORMAT_CFLAGS}" "${PKG_LIBAVFORMAT_LIBRARY_DIRS}")

  set(SCOPE_CFLAGS ${SCOPE_CFLAGS} PARENT_SCOPE)
  set(SCOPE_LINKFLAGS ${SCOPE_LINKFLAGS} PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} avformat PARENT_SCOPE)
endfunction()

function( configure_opencl_target target )
  set(SCOPE_CFLAGS "${SCOPE_CFLAGS} -I${OPENCL_INCLUDE}" PARENT_SCOPE)
  set(SCOPE_LINKFLAGS "${SCOPE_LINKFLAGS} -L${OPENCL_LIBRARY_PATH}" PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} OpenCL PARENT_SCOPE)
endfunction()

function( configure_corevideo_target target )
  set(SCOPE_CFLAGS "${SCOPE_CFLAGS} -I/System/Library/Frameworks/CoreVideo.framework/Headers" PARENT_SCOPE)
  set(SCOPE_LINKFLAGS "${SCOPE_LINKFLAGS} -framework CoreVideo" PARENT_SCOPE)
endfunction()


function(configure_dependencies target dependencies variant)
  if (${ARGV2} MATCHES universal)
    configure_universal_target (${ARGV0})
  endif()

  foreach(dependency ${ARGV1})
    if(${dependency} STREQUAL libmfx)
      configure_libmfx_target(${ARGV0})
    elseif(${dependency} STREQUAL _enable_sw)
      # that's fake dependency to check --enable-sw=yes|no option
      if(NOT ENABLE_SW)
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    elseif(${dependency} STREQUAL _force_skipping)
      # that's fake dependency to force skipping the target
      report_add_target(SKIPPING ${ARGV0})
      return()
    elseif(${dependency} STREQUAL _force_not_configured)
      # that's fake dependency to force marking the target as not configured
      report_add_target(NOT_CONFIGURED ${ARGV0})
      return()
    elseif(${dependency} STREQUAL libdrm)
      if(ENABLE_DRM)
        if(PKG_LIBDRM_FOUND)
          configure_libdrm_target(${ARGV0})
        else()
          report_add_target(SKIPPING ${ARGV0})
          return()
        endif()
      else()
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    elseif(${dependency} STREQUAL libva-drm)
      if(ENABLE_DRM)
        if(PKG_LIBVA_DRM_FOUND)
          configure_libva_drm_target(${ARGV0})
        else()
          report_add_target(SKIPPING ${ARGV0})
          return()
        endif()
      else()
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    elseif(${dependency} STREQUAL libva-x11)
      if(ENABLE_X11)
        if(PKG_LIBVA_X11_FOUND)
          configure_libva_x11_target(${ARGV0})
        else()
          report_add_target(SKIPPING ${ARGV0})
          return()
        endif()
      else()
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    elseif(${dependency} STREQUAL wayland-client)
      if(ENABLE_WAYLAND)
        if(PKG_WAYLAND_CLIENT_FOUND)
          configure_wayland_target(${ARGV0})
        else()
          report_add_target(SKIPPING ${ARGV0})
          return()
        endif()
      else()
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    elseif(${dependency} STREQUAL libva)
      if(ENABLE_DRM OR ENABLE_X11)
        if(PKG_LIBVA_FOUND)
          configure_libva_target(${ARGV0})
        else()
          report_add_target(SKIPPING ${ARGV0})
          return()
        endif()
      else()
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    elseif(${dependency} STREQUAL x11)
      if(ENABLE_X11)
        if(PKG_X11_FOUND)
          configure_x11_target(${ARGV0})
        else()
          report_add_target(SKIPPING ${ARGV0})
          return()
        endif()
      else()
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    elseif(${dependency} STREQUAL dl)
      configure_dl_target(${ARGV0})
    elseif(${dependency} STREQUAL pthread)
      configure_pthread_target(${ARGV0})
    elseif(${dependency} STREQUAL libavutil)
      if(ENABLE_FFMPEG)
        if(PKG_LIBAVUTIL_FOUND)
          configure_libavutil_target(${ARGV0})
        else()
          report_add_target(SKIPPING ${ARGV0})
          return()
        endif()
      else()
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    elseif(${dependency} STREQUAL libavcodec)
      if(ENABLE_FFMPEG)
        if(PKG_LIBAVCODEC_FOUND)
          configure_libavcodec_target(${ARGV0})
        else()
          report_add_target(SKIPPING ${ARGV0})
          return()
        endif()
      else()
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    elseif(${dependency} STREQUAL libavformat)
      if(ENABLE_FFMPEG)
        if(PKG_LIBAVFORMAT_FOUND)
          configure_libavformat_target(${ARGV0})
        else()
          report_add_target(SKIPPING ${ARGV0})
          return()
        endif()
      else()
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    elseif(${dependency} STREQUAL opencl)
      if(ENABLE_OPENCL)
        if(OPENCL_FOUND)
          configure_opencl_target(${ARGV0})
        else()
          report_add_target(SKIPPING ${ARGV0})
          return()
        endif()
      else()
        report_add_target(NOT_CONFIGURED ${ARGV0})
        return()
      endif()
    else()
      message( FATAL_ERROR "Unknown dependency '${dependency}' for the target '${ARGV0}'" )
      return()
    endif()
  endforeach()

  set(SCOPE_CFLAGS ${SCOPE_CFLAGS} PARENT_SCOPE)
  set(SCOPE_LINKFLAGS "${SCOPE_LINKFLAGS} -Wl,--no-undefined,-z,relro,-z,now,-z,noexecstack -fstack-protector" PARENT_SCOPE)
  set(SCOPE_LIBS ${SCOPE_LIBS} PARENT_SCOPE)
endfunction()

if( Linux )
  find_package(PkgConfig REQUIRED)

  if( ENABLE_DRM )
    pkg_check_modules(PKG_LIBDRM    REQUIRED libdrm)
    pkg_check_modules(PKG_LIBVA     REQUIRED libva>=0.33)
    pkg_check_modules(PKG_LIBVA_DRM REQUIRED libva-drm>=0.33 )
  endif()
  if( ENABLE_X11 )
    pkg_check_modules(PKG_LIBVA     REQUIRED libva>=0.33)
    pkg_check_modules(PKG_LIBVA_X11 REQUIRED libva-x11>=0.33)
    pkg_check_modules(PKG_X11       REQUIRED x11)
  endif()
  if( ENABLE_WAYLAND )
    pkg_check_modules(PKG_LIBDRM    REQUIRED libdrm)
    pkg_check_modules(PKG_LIBVA     REQUIRED libva>=0.33)
    pkg_check_modules(PKG_LIBVA_DRM REQUIRED libva-drm>=0.33 )
    pkg_check_modules(PKG_DRM_INTEL REQUIRED libdrm_intel )
    pkg_check_modules(PKG_WAYLAND_CLIENT REQUIRED wayland-client )
  endif()

endif( )
