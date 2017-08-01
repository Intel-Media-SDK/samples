##******************************************************************************
##  Copyright(C) 2014 Intel Corporation. All Rights Reserved.
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

if (Linux)

	pkg_check_modules(PKG_LIBVAUTIL libavutil>=52.38.100)
	pkg_check_modules(PKG_LIBAVCODEC libavcodec>=55.18.102)
	pkg_check_modules(PKG_LIBAVFORMAT libavformat>=55.12.100)

	if(PKG_LIBVAUTIL_FOUND AND
	   PKG_LIBAVCODEC_FOUND AND
	   PKG_LIBAVFORMAT_FOUND)
	    set( FFMPEG_FOUND TRUE )
	    message( STATUS "FFmpeg headers and libraries were found." )
	endif()

	if(NOT DEFINED FFMPEG_FOUND)
	  message( STATUS "FFmpeg headers and libraries were not found (optional). The following will not be built: sample_spl_mux." )
	endif()

else()
   message( STATUS "FFmpeg headers and libraries were serached at all (optional). The following will not be built: sample_spl_mux." )
endif()
