##******************************************************************************
##  Copyright(C) 2012 Intel Corporation. All Rights Reserved.
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

if( Linux )
  set( os_arch "lin" )
elseif( Darwin )
  set( os_arch "darwin" )
elseif( Windows )
  set( os_arch "win" )
endif()

if( __ARCH MATCHES ia32)
  set( os_arch "${os_arch}_ia32" )
else()
  set( os_arch "${os_arch}_x64" )
endif()

if( CMAKE_MFX_HOME )
  set( MFX_API_HOME ${CMAKE_MFX_HOME} )
else()
  set( MFX_API_HOME $ENV{MFX_HOME} )
endif()

find_path( MFX_INCLUDE mfxdefs.h PATHS ${MFX_API_HOME}/include )
find_library( MFX_LIBRARY libmfx.a PATHS ${MFX_API_HOME}/lib PATH_SUFFIXES ${os_arch} )

if( NOT MFX_INCLUDE MATCHES NOTFOUND )
  include_directories( ${MFX_API_HOME}/mediasdk_structures )
  include_directories( ${MFX_INCLUDE} )
endif()

if( NOT MFX_LIBRARY MATCHES NOTFOUND )
  get_filename_component( MFX_LIBRARY_PATH ${MFX_LIBRARY} PATH )
  link_directories( ${MFX_LIBRARY_PATH} )
endif()

if( NOT MFX_INCLUDE MATCHES NOTFOUND )
  set( MFX_FOUND TRUE )
  include_directories( ${MFX_INCLUDE} )
endif()

if( NOT DEFINED MFX_FOUND )
  message( FATAL_ERROR "Intel(R) Media SDK was not found (required)! Set/check MFX_HOME environment variable!")
else()
  message( STATUS "Intel(R) Media SDK was found here $ENV{MFX_HOME}")
endif()

if( NOT MFX_LIBRARY MATCHES NOTFOUND )
  get_filename_component(MFX_LIBRARY_PATH ${MFX_LIBRARY} PATH )
  link_directories( ${MFX_LIBRARY_PATH} )
endif()

# Potential source of confusion here. Environment $MFX_VERSION translates to product name (strings libmfxhw64.so | grep mediasdk),
# but macro definition MFX_VERSION should contain API version i.e. 1025 for API 1.25
if( NOT DEFINED API OR $API STREQUAL "master")
  set(API_FLAGS "")  
else( )
  set( VERSION_REGEX "[0-9]+\\.[0-9]+" )

  # Breaks up a string in the form maj.min into two parts and stores
  # them in major, minor.  version should be a value, not a
  # variable, while major and minor should be variables.
  macro( split_api_version version major minor )
    if(${version} MATCHES ${VERSION_REGEX})
      string(REGEX REPLACE "^([0-9]+)\\.[0-9]+" "\\1" ${major} "${version}")
      string(REGEX REPLACE "^[0-9]+\\.([0-9]+)" "\\1" ${minor} "${version}")
    else(${version} MATCHES ${VERSION_REGEX})
      message("macro( split_api_version ${version} ${major} ${minor} ")
      message(FATAL_ERROR "Problem parsing API version string.")
    endif(${version} MATCHES ${VERSION_REGEX})
  endmacro( split_api_version )

  split_api_version(${API} major_vers minor_vers)
    # Compute a version number
  math(EXPR version_number "${major_vers} * 1000 + ${minor_vers}" )
  set(API_FLAGS -DMFX_VERSION=${version_number})
endif()

message(STATUS "Enabling API ${major_vers}.${minor_vers} feature set with flags ${API_FLAGS}")

if( Linux )
  set( MFX_LDFLAGS "-Wl,--default-symver" )
endif()
