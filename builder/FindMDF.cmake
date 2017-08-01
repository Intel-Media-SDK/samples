##******************************************************************************
##  Copyright(C) 2013 Intel Corporation. All Rights Reserved.
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
  set( mdf_arch x86)
  set( mdf_lib_suffix 86 )
else()
  set( mdf_arch x64 )
  set( mdf_lib_suffix 64 )
endif()

set( mdf_root $ENV{MFX_MDF_PATH} )
if( NOT EXISTS ${mdf_root} )
  if( Windows )
    message( FATAL_ERROR "MDF was not found (required)! Set/check MFX_MDF_PATH environment variable!")
  else()
    message( STATUS "!!! There is no MFX_MDF_PATH, looking into $ENV{MEDIASDK_ROOT}/tools/linuxem64t/mdf" )
    set( mdf_root $ENV{MEDIASDK_ROOT}/tools/linuxem64t/mdf )
  endif()
endif()

find_path( CMRT_INCLUDE cm_rt.h PATHS ${mdf_root}/runtime/include )
find_path( CMDF_INCLUDE cm_def.h PATHS ${mdf_root}/compiler/include ${mdf_root}/compiler/include/cm )

if(NOT CMRT_INCLUDE MATCHES NOTFOUND AND
   NOT CMDF_INCLUDE MATCHES NOTFOUND)
  set( MDF_FOUND TRUE )
  include_directories( ${CMRT_INCLUDE} ${CMDF_INCLUDE} )

  link_directories( ${mdf_root}/runtime/lib/${mdf_arch} )
endif()

if(NOT DEFINED MDF_FOUND)
  message( FATAL_ERROR "Intel(R) MDF was not found (required)! Set/check MFX_MDF_PATH environment variable!" )
else ()
  message( STATUS "Intel(R) MDF was found in ${mdf_root}" )
endif()

