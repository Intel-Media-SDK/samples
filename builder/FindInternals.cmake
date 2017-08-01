##******************************************************************************
##  Copyright(C) 2012-2014 Intel Corporation. All Rights Reserved.
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

set( MSDK_STUDIO_ROOT  ${CMAKE_HOME_DIRECTORY}/mdp_msdk-lib/_studio    )
set( MSDK_TSUITE_ROOT  ${CMAKE_HOME_DIRECTORY}/mdp_msdk-lib/_testsuite )
set( MSDK_LIB_ROOT     ${MSDK_STUDIO_ROOT}/mfx_lib                     )
set( MSDK_UMC_ROOT     ${MSDK_STUDIO_ROOT}/shared/umc                  )
set( MSDK_SAMPLES_ROOT ${CMAKE_HOME_DIRECTORY}/mdp_msdk-lib/samples    )
set( MSDK_BUILDER_ROOT ${CMAKE_HOME_DIRECTORY}/mdp_msdk-lib/builder    )

function( mfx_include_dirs )
  include_directories (
    ${MSDK_STUDIO_ROOT}/shared/include
    ${MSDK_UMC_ROOT}/core/vm/include
    ${MSDK_UMC_ROOT}/core/vm_plus/include
    ${MSDK_UMC_ROOT}/core/umc/include
    ${MSDK_UMC_ROOT}/io/umc_io/include
    ${MSDK_UMC_ROOT}/io/umc_va/include
    ${MSDK_UMC_ROOT}/io/media_buffers/include
    ${MSDK_LIB_ROOT}/shared/include
    ${MSDK_LIB_ROOT}/optimization/h265/include
    ${MSDK_LIB_ROOT}/optimization/h264/include
    ${MSDK_LIB_ROOT}/shared/include
    ${MSDK_LIB_ROOT}/fei/include
    ${MSDK_LIB_ROOT}/fei/h264_la
    ${CMAKE_HOME_DIRECTORY}/mdp_msdk-contrib/SafeStringStaticLibrary/include
  )
endfunction()
