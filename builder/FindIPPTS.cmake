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
  set( ipp_ts_arch 32 )
else()
  set( ipp_ts_arch em64t )
endif()

set( ipp_ts_root $ENV{MEDIASDK_ROOT}/tools/ts )
if( Windows )
  set( ipp_ts_lib ${ipp_ts_root}/lib/windows${ipp_ts_arch} )
elseif( Linux )
  set( ipp_ts_lib ${ipp_ts_root}/lib/linux${ipp_ts_arch} )
elseif( Darwin )
  set( ipp_ts_lib ${ipp_ts_root}/lib/darwin${ipp_arch} )
endif()

find_path( IPPTS_INCLUDE ts.h PATHS ${ipp_ts_root}/include )

if(NOT IPPTS_INCLUDE MATCHES NOTFOUND)
  set( IPPTS_FOUND TRUE)
  include_directories( ${IPPTS_INCLUDE} )
  link_directories( ${ipp_ts_lib} )
endif()

if(NOT DEFINED IPPTS_FOUND)
  message( STATUS "Intel(R) IPP TS was not found (optional)!" )
endif()
