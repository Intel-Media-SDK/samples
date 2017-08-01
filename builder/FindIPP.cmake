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

if (__ARCH MATCHES ia32)
  set( ipp_arch ${__ARCH})
else()
  set( ipp_arch em64t )
endif()

set( ipp_root $ENV{MEDIASDK_ROOT}/ipp )
if( Windows )
  set( ipp_root ${ipp_root}/ipp81goldGuard/windows/${ipp_arch} )
elseif( Linux )
  set( ipp_root ${ipp_root}/linux/${ipp_arch} )
elseif( Darwin )
  set( ipp_root ${ipp_root}/darwin/${ipp_arch} )
endif()

message( STATUS "Search IPP in ${ipp_root}" )

find_path( IPP_INCLUDE ippcore.h PATHS ${ipp_root}/include )
if( Windows )
  find_library( IPP_LIBRARY ippcoremt PATHS ${ipp_root}/lib )
else()
  find_library( IPP_LIBRARY ippcore_l PATHS ${ipp_root}/lib )
endif()

if(NOT IPP_INCLUDE MATCHES NOTFOUND)
  if(NOT IPP_LIBRARY MATCHES NOTFOUND)
    set( IPP_FOUND TRUE )
    include_directories( ${IPP_INCLUDE} )

    get_filename_component( IPP_LIBRARY_PATH ${IPP_LIBRARY} PATH )
    link_directories( ${IPP_LIBRARY_PATH} )
  endif()
endif()

if(NOT DEFINED IPP_FOUND)
  message( FATAL_ERROR "Intel(R) IPP was not found (required)! Set/check MEDIASDK_ROOT environment variable!" )
else ()
  message( STATUS "Intel(R) IPP was found here $ENV{MEDIASDK_ROOT}" )
endif()

if( __IPP )
  if( Linux )
    append("-include ${ipp_root}/tools/staticlib/ipp_${__IPP}.h" CMAKE_C_FLAGS)
    append("-include ${ipp_root}/tools/staticlib/ipp_${__IPP}.h" CMAKE_CXX_FLAGS)
  endif()
endif()

