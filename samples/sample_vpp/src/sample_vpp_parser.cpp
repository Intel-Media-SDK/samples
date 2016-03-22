/******************************************************************************\
Copyright (c) 2005-2016, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

This sample was distributed or derived from the Intel's Media Samples package.
The original version of this sample may be obtained from https://software.intel.com/en-us/intel-media-server-studio
or https://software.intel.com/en-us/media-client-solutions-support.
\**********************************************************************************/

#include "mfx_samples_config.h"

#include "sample_vpp_utils.h"

#define SIZE_CHECK(cond) \
    if (cond) \
    { \
        msdk_printf(MSDK_STRING("Buffer is too small\n")); \
        return MFX_ERR_UNSUPPORTED; \
    }

void vppPrintHelp(msdk_char *strAppName, const msdk_char *strErrorMessage)
{

    msdk_printf(MSDK_STRING("VPP Sample Version %s\n\n"), MSDK_SAMPLE_VERSION);
    if (strErrorMessage)
    {
        msdk_printf(MSDK_STRING("Error: %s\n"), strErrorMessage);
    }

    msdk_printf(MSDK_STRING("Usage1: %s [Options] -i InputFile -o OutputFile\n"), strAppName);

    msdk_printf(MSDK_STRING("Options: \n"));
    msdk_printf(MSDK_STRING("   [-lib  type]     - type of used library. sw, hw (def: auto)\n\n"));

    msdk_printf(MSDK_STRING("   [-extapi]        - use RunFrameVPPAsyncEx instead of RunFrameVPPAsync. Need for PTIR.\n\n"));
    msdk_printf(MSDK_STRING("   [-p guid]        - use VPP plug-in with specified 32-character hexadecimal guid string\n"));
    msdk_printf(MSDK_STRING("   [-sw  width]     - width  of src video (def: 352)\n"));
    msdk_printf(MSDK_STRING("   [-sh  height]    - height of src video (def: 288)\n"));
    msdk_printf(MSDK_STRING("   [-scrX  x]       - cropX  of src video (def: 0)\n"));
    msdk_printf(MSDK_STRING("   [-scrY  y]       - cropY  of src video (def: 0)\n"));
    msdk_printf(MSDK_STRING("   [-scrW  w]       - cropW  of src video (def: width)\n"));
    msdk_printf(MSDK_STRING("   [-scrH  h]       - cropH  of src video (def: height)\n"));
    msdk_printf(MSDK_STRING("   [-sf  frameRate] - frame rate of src video (def: 30.0)\n"));
    msdk_printf(MSDK_STRING("   [-scc format]    - format (FourCC) of src video (def: nv12. support nv12|yv12|yuy2|rgb3|rgb4|uyvy)\n"));

    msdk_printf(MSDK_STRING("   [-spic value]    - picture structure of src video\n"));
    msdk_printf(MSDK_STRING("                       0 = interlaced top    field first\n"));
    msdk_printf(MSDK_STRING("                       2 = interlaced bottom field first\n"));
    msdk_printf(MSDK_STRING("                       1 = progressive (default)\n\n"));


    msdk_printf(MSDK_STRING("   [-dw  width]     - width  of dst video (def: 352)\n"));
    msdk_printf(MSDK_STRING("   [-dh  height]    - height of dst video (def: 288)\n"));
    msdk_printf(MSDK_STRING("   [-dcrX  x]       - cropX  of dst video (def: 0)\n"));
    msdk_printf(MSDK_STRING("   [-dcrY  y]       - cropY  of dst video (def: 0)\n"));
    msdk_printf(MSDK_STRING("   [-dcrW  w]       - cropW  of dst video (def: width)\n"));
    msdk_printf(MSDK_STRING("   [-dcrH  h]       - cropH  of dst video (def: height)\n"));
    msdk_printf(MSDK_STRING("   [-df  frameRate] - frame rate of dst video (def: 30.0)\n"));
    msdk_printf(MSDK_STRING("   [-dcc format]    - format (FourCC) of dst video (def: nv12. support nv12|yuy2|rgb4)\n"));

    msdk_printf(MSDK_STRING("   [-dpic value]    - picture structure of dst video\n"));
    msdk_printf(MSDK_STRING("                       0 = interlaced top    field first\n"));
    msdk_printf(MSDK_STRING("                       2 = interlaced bottom field first\n"));
    msdk_printf(MSDK_STRING("                       1 = progressive (default)\n\n"));


    msdk_printf(MSDK_STRING("   Video Enhancement Algorithms\n"));
    msdk_printf(MSDK_STRING("   [-vanalysis]                 - enable video analysis algorithm \n"));
    msdk_printf(MSDK_STRING("   [-denoise (level)]           - enable denoise algorithm. Level is optional \n"));
    msdk_printf(MSDK_STRING("                                  range of  noise level is [0, 100]\n"));
    msdk_printf(MSDK_STRING("   [-detail  (level)]           - enable detail enhancement algorithm. Level is optional \n"));
    msdk_printf(MSDK_STRING("                                  range of detail level is [0, 100]\n\n"));
    msdk_printf(MSDK_STRING("   [-pa_hue  hue]               - procamp hue property.         range [-180.0, 180.0] (def: 0.0)\n"));
    msdk_printf(MSDK_STRING("   [-pa_sat  saturation]        - procamp satursation property. range [   0.0,  10.0] (def: 1.0)\n"));
    msdk_printf(MSDK_STRING("   [-pa_con  contrast]          - procamp contrast property.    range [   0.0,  10.0] (def: 1.0)\n"));
    msdk_printf(MSDK_STRING("   [-pa_bri  brightness]        - procamp brightness property.  range [-100.0, 100.0] (def: 0.0)\n\n"));
    msdk_printf(MSDK_STRING("   [-istab (mode)]              - enable Image Stabilization algorithm.  Mode is optional \n"));
    msdk_printf(MSDK_STRING("                                  mode of istab can be [1, 2] (def: 2)\n"));
    msdk_printf(MSDK_STRING("                                  where: 1 means upscale mode, 2 means cropping mode\n\n"));
    msdk_printf(MSDK_STRING("   [-frc:interp]                - enable FRC based on frame interpolation algorithm\n\n"));

    msdk_printf(MSDK_STRING("   [-n frames]                  - number of frames to VPP process\n\n"));
#if D3D_SURFACES_SUPPORT
    msdk_printf(MSDK_STRING("\n   [-d3d]                - use d3d9 surfaces\n"));
#endif
#if MFX_D3D11_SUPPORT
    msdk_printf(MSDK_STRING("\n   [-d3d11]              - use d3d11 surfaces\n"));
#endif
#ifdef LIBVA_SUPPORT
    msdk_printf(MSDK_STRING("\n   [-vaapi]                - work with vaapi surfaces\n"));
#endif
    msdk_printf(MSDK_STRING("   [-composite parameters_file] - composite several input files in one output. The syntax of the parameters file is:\n"));
    msdk_printf(MSDK_STRING("                                  stream=<video file name>\n"));
    msdk_printf(MSDK_STRING("                                  width=<input video width>\n"));
    msdk_printf(MSDK_STRING("                                  height=<input video height>\n"));
    msdk_printf(MSDK_STRING("                                  cropx=<input cropX (def: 0)>\n"));
    msdk_printf(MSDK_STRING("                                  cropy=<input cropY (def: 0)>\n"));
    msdk_printf(MSDK_STRING("                                  cropw=<input cropW (def: width)>\n"));
    msdk_printf(MSDK_STRING("                                  croph=<input cropH (def: height)>\n"));
    msdk_printf(MSDK_STRING("                                  framerate=<input frame rate (def: 30.0)>\n"));
    msdk_printf(MSDK_STRING("                                  fourcc=<format (FourCC) of input video (def: nv12. support nv12|rgb4)>\n"));
    msdk_printf(MSDK_STRING("                                  picstruct=<picture structure of input video,\n"));
    msdk_printf(MSDK_STRING("                                             0 = interlaced top    field first\n"));
    msdk_printf(MSDK_STRING("                                             2 = interlaced bottom field first\n"));
    msdk_printf(MSDK_STRING("                                             1 = progressive (default)>\n"));
    msdk_printf(MSDK_STRING("                                  dstx=<X coordinate of input video located in the output (def: 0)>\n"));
    msdk_printf(MSDK_STRING("                                  dsty=<Y coordinate of input video located in the output (def: 0)>\n"));
    msdk_printf(MSDK_STRING("                                  dstw=<width of input video located in the output (def: width)>\n"));
    msdk_printf(MSDK_STRING("                                  dsth=<height of input video located in the output (def: height)>\n\n"));
    msdk_printf(MSDK_STRING("                                  stream=<video file name>\n"));
    msdk_printf(MSDK_STRING("                                  width=<input video width>\n"));
    msdk_printf(MSDK_STRING("                                  GlobalAlphaEnable=1\n"));
    msdk_printf(MSDK_STRING("                                  GlobalAlpha=<Alpha value>\n"));
    msdk_printf(MSDK_STRING("                                  LumaKeyEnable=1\n"));
    msdk_printf(MSDK_STRING("                                  LumaKeyMin=<Luma key min value>\n"));
    msdk_printf(MSDK_STRING("                                  LumaKeyMax=<Luma key max value>\n"));
    msdk_printf(MSDK_STRING("                                  ...\n"));
    msdk_printf(MSDK_STRING("                                  The parameters file may contain up to 64 streams.\n\n"));
    msdk_printf(MSDK_STRING("                                  For example:\n"));
    msdk_printf(MSDK_STRING("                                  stream=input_720x480.yuv\n"));
    msdk_printf(MSDK_STRING("                                  width=720\n"));
    msdk_printf(MSDK_STRING("                                  height=480\n"));
    msdk_printf(MSDK_STRING("                                  cropx=0\n"));
    msdk_printf(MSDK_STRING("                                  cropy=0\n"));
    msdk_printf(MSDK_STRING("                                  cropw=720\n"));
    msdk_printf(MSDK_STRING("                                  croph=480\n"));
    msdk_printf(MSDK_STRING("                                  dstx=0\n"));
    msdk_printf(MSDK_STRING("                                  dsty=0\n"));
    msdk_printf(MSDK_STRING("                                  dstw=720\n"));
    msdk_printf(MSDK_STRING("                                  dsth=480\n\n"));
    msdk_printf(MSDK_STRING("                                  stream=input_480x320.yuv\n"));
    msdk_printf(MSDK_STRING("                                  width=480\n"));
    msdk_printf(MSDK_STRING("                                  height=320\n"));
    msdk_printf(MSDK_STRING("                                  cropx=0\n"));
    msdk_printf(MSDK_STRING("                                  cropy=0\n"));
    msdk_printf(MSDK_STRING("                                  cropw=480\n"));
    msdk_printf(MSDK_STRING("                                  croph=320\n"));
    msdk_printf(MSDK_STRING("                                  dstx=100\n"));
    msdk_printf(MSDK_STRING("                                  dsty=100\n"));
    msdk_printf(MSDK_STRING("                                  dstw=320\n"));
    msdk_printf(MSDK_STRING("                                  dsth=240\n"));
    msdk_printf(MSDK_STRING("                                  GlobalAlphaEnable=1\n"));
    msdk_printf(MSDK_STRING("                                  GlobalAlpha=128\n"));
    msdk_printf(MSDK_STRING("                                  LumaKeyEnable=1\n"));
    msdk_printf(MSDK_STRING("                                  LumaKeyMin=250\n"));
    msdk_printf(MSDK_STRING("                                  LumaKeyMax=255\n"));

    msdk_printf(MSDK_STRING("\n"));

    msdk_printf(MSDK_STRING("Usage2: %s -lib sw -sw 352 -sh 144 -scc rgb3 -dw 320 -dh 240 -dcc nv12 -denoise 32 -vanalysis -i in.rgb -o out.yuv\n"), strAppName);

    msdk_printf(MSDK_STRING("\n"));

} // void vppPrintHelp(msdk_char *strAppName, msdk_char *strErrorMessage)

static mfxU32 Str2FourCC( msdk_char* strInput )
{
    mfxU32 fourcc = MFX_FOURCC_YV12;//default

    if ( 0 == msdk_strcmp(strInput, MSDK_STRING("yv12")) )
    {
        fourcc = MFX_FOURCC_YV12;
    }
    else if ( 0 == msdk_strcmp(strInput, MSDK_STRING("rgb3")) )
    {
        fourcc = MFX_FOURCC_RGB3;
    }
    else if ( 0 == msdk_strcmp(strInput, MSDK_STRING("rgb4")) )
    {
        fourcc = MFX_FOURCC_RGB4;
    }
    else if ( 0 == msdk_strcmp(strInput, MSDK_STRING("yuy2")) )
    {
        fourcc = MFX_FOURCC_YUY2;
    }
    else if ( 0 == msdk_strcmp(strInput, MSDK_STRING("uyvy")) )
    {
        fourcc = MFX_FOURCC_UYVY;
    }
    else if ( 0 == msdk_strcmp(strInput, MSDK_STRING("nv12")) )
    {
        fourcc = MFX_FOURCC_NV12;
    }

    return fourcc;

} // mfxU32 Str2FourCC( msdk_char* strInput )

static
mfxU8 GetPicStruct( mfxU8 picStruct )
{
    if ( 0 == picStruct )
    {
        return MFX_PICSTRUCT_FIELD_TFF;
    }
    else if( 2 == picStruct )
    {
        return MFX_PICSTRUCT_FIELD_BFF;
    }
    else
    {
        return MFX_PICSTRUCT_PROGRESSIVE;
    }

} // mfxU8 GetPicStruct( mfxU8 picStruct )

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

void getPair(std::string line, std::string &key, std::string &value)
{
    std::istringstream iss(line);
    getline(iss, key,   '=');
    getline(iss, value, '=');
    trim(key);
    trim(value);
}

mfxStatus ParseCompositionParfile(const msdk_char* parFileName, sInputParams* pParams)
{
    mfxStatus sts = MFX_ERR_NONE;
    if(msdk_strlen(parFileName) == 0)
        return MFX_ERR_UNKNOWN;

    std::string line;
    std::string key, value;
    mfxU8 nStreamInd = 0;
    mfxU8 firstStreamFound = 0;
    std::ifstream stream(parFileName);
    if (stream.fail())
        return MFX_ERR_UNKNOWN;

    while (getline(stream, line) && nStreamInd < MAX_INPUT_STREAMS)
    {
        getPair(line, key, value);
        if (key.compare("width") == 0)
        {
            pParams->inFrameInfo[nStreamInd].nWidth = (mfxU16) MSDK_ALIGN16(atoi(value.c_str()));
        }
        else if (key.compare("height") == 0)
        {
            pParams->inFrameInfo[nStreamInd].nHeight = (MFX_PICSTRUCT_PROGRESSIVE == pParams->inFrameInfo[nStreamInd].PicStruct)?
                                                       (mfxU16) MSDK_ALIGN16(atoi(value.c_str())) : (mfxU16) MSDK_ALIGN32(atoi(value.c_str()));
        }
        else if (key.compare("cropx") == 0)
        {
            pParams->inFrameInfo[nStreamInd].CropX = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("cropy") == 0)
        {
            pParams->inFrameInfo[nStreamInd].CropY = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("cropw") == 0)
        {
            pParams->inFrameInfo[nStreamInd].CropW = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("croph") == 0)
        {
            pParams->inFrameInfo[nStreamInd].CropH = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("framerate") == 0)
        {
            pParams->inFrameInfo[nStreamInd].dFrameRate = (mfxF64) atof(value.c_str());
        }
        else if (key.compare("fourcc") == 0)
        {
            const mfxU16 len_size = 5;
            msdk_char fourcc[len_size];
            for (mfxU16 i = 0; i < (value.size() > len_size ? len_size : value.size()); i++)
                fourcc[i] = value.at(i);
            fourcc[len_size-1]=0;
            pParams->inFrameInfo[nStreamInd].FourCC = Str2FourCC(fourcc);
        }
        else if (key.compare("picstruct") == 0)
        {
            pParams->inFrameInfo[nStreamInd].PicStruct = (mfxU8) atoi(value.c_str());
            pParams->inFrameInfo[nStreamInd].PicStruct = GetPicStruct(pParams->inFrameInfo[nStreamInd].PicStruct);
        }
        else if (key.compare("dstx") == 0)
        {
            pParams->compositionParam.streamInfo[nStreamInd].compStream.DstX = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("dsty") == 0)
        {
            pParams->compositionParam.streamInfo[nStreamInd].compStream.DstY = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("dstw") == 0)
        {
            pParams->compositionParam.streamInfo[nStreamInd].compStream.DstW = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("dsth") == 0)
        {
            pParams->compositionParam.streamInfo[nStreamInd].compStream.DstH = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("GlobalAlphaEnable") == 0)
        {
            pParams->compositionParam.streamInfo[nStreamInd].compStream.GlobalAlphaEnable = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("GlobalAlpha") == 0)
        {
            pParams->compositionParam.streamInfo[nStreamInd].compStream.GlobalAlpha = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("PixelAlphaEnable") == 0)
        {
            pParams->compositionParam.streamInfo[nStreamInd].compStream.PixelAlphaEnable = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("LumaKeyEnable") == 0)
        {
            pParams->compositionParam.streamInfo[nStreamInd].compStream.LumaKeyEnable = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("LumaKeyMin") == 0)
        {
            pParams->compositionParam.streamInfo[nStreamInd].compStream.LumaKeyMin = (mfxU16) atoi(value.c_str());
        }
        else if (key.compare("LumaKeyMax") == 0)
        {
            pParams->compositionParam.streamInfo[nStreamInd].compStream.LumaKeyMax = (mfxU16) atoi(value.c_str());
        }
        else if ((key.compare("stream") == 0 || key.compare("primarystream") == 0) && nStreamInd < (MAX_INPUT_STREAMS - 1))
        {
            const mfxU16 len_size = MSDK_MAX_FILENAME_LEN;

            if (firstStreamFound == 1)
            {
                nStreamInd ++;
            }
            else
            {
                nStreamInd = 0;
                firstStreamFound = 1;
            }
            pParams->inFrameInfo[nStreamInd].CropX = 0;
            pParams->inFrameInfo[nStreamInd].CropY = 0;
            pParams->inFrameInfo[nStreamInd].CropW = NOT_INIT_VALUE;
            pParams->inFrameInfo[nStreamInd].CropH = NOT_INIT_VALUE;
            for (mfxU16 i = 0; i < (value.size() > len_size ? len_size : value.size()); i++)
                pParams->compositionParam.streamInfo[nStreamInd].streamName[i] = value.at(i);
            pParams->compositionParam.streamInfo[nStreamInd].streamName[len_size-1]=0;
        }
    }
    if (pParams->inFrameInfo[0].nWidth > pParams->inFrameInfo[0].CropW)
    {
        /* This case means alignment for Width was done */
        pParams->outFrameInfo.nWidth = pParams->inFrameInfo[0].CropW;
    }
    else
    {
        pParams->outFrameInfo.nWidth = pParams->inFrameInfo[0].nWidth;
    }
    if (pParams->inFrameInfo[0].nHeight > pParams->inFrameInfo[0].CropH)
    {
        /* This case means alignment for Height was done */
        pParams->outFrameInfo.nHeight = pParams->inFrameInfo[0].CropH;
    }
    else
    {
        pParams->outFrameInfo.nHeight = pParams->inFrameInfo[0].nHeight;
    }
    pParams->numStreams = nStreamInd + 1;

    return sts;
}

mfxStatus vppParseInputString(msdk_char* strInput[], mfxU8 nArgNum, sInputParams* pParams)
{
    MSDK_CHECK_POINTER(pParams,  MFX_ERR_NULL_PTR);
    MSDK_CHECK_POINTER(strInput, MFX_ERR_NULL_PTR);

    mfxU32 readData;

    mfxU8 i;

    if (nArgNum < 4)
    {
        vppPrintHelp(strInput[0], MSDK_STRING("Please, specify all necessary parameters"));

        return MFX_ERR_MORE_DATA;
    }

    pParams->inFrameInfo[VPP_IN].CropX = 0;
    pParams->inFrameInfo[VPP_IN].CropY = 0;
    pParams->inFrameInfo[VPP_IN].CropW = NOT_INIT_VALUE;
    pParams->inFrameInfo[VPP_IN].CropH = NOT_INIT_VALUE;
    // zeroize destination crops
    pParams->outFrameInfo.CropX = 0;
    pParams->outFrameInfo.CropY = 0;
    pParams->outFrameInfo.CropW = NOT_INIT_VALUE;
    pParams->outFrameInfo.CropH = NOT_INIT_VALUE;
    for (i = 1; i < nArgNum; i++ )
    {
        MSDK_CHECK_POINTER(strInput[i], MFX_ERR_NULL_PTR);
        {
            if ( 0 == msdk_strcmp(strInput[i], MSDK_STRING("-sw")) )
            {
                i++;
                msdk_opt_read(strInput[i], pParams->inFrameInfo[VPP_IN].nWidth);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-sh")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->inFrameInfo[VPP_IN].nHeight);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-scrX")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->inFrameInfo[VPP_IN].CropX);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-scrY")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->inFrameInfo[VPP_IN].CropY);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-scrW")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->inFrameInfo[VPP_IN].CropW);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-scrH")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->inFrameInfo[VPP_IN].CropH);
            }
            else if(0 == msdk_strcmp(strInput[i], MSDK_STRING("-spic")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->inFrameInfo[VPP_IN].PicStruct);
                pParams->inFrameInfo[VPP_IN].PicStruct = GetPicStruct(pParams->inFrameInfo[VPP_IN].PicStruct);
            }
            else if(0 == msdk_strcmp(strInput[i], MSDK_STRING("-sf")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->inFrameInfo[VPP_IN].dFrameRate);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-dw")) )
            {
                i++;
                msdk_opt_read(strInput[i], pParams->outFrameInfo.nWidth);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-dh")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->outFrameInfo.nHeight);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-dcrX")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->outFrameInfo.CropX);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-dcrY")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->outFrameInfo.CropY);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-dcrW")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->outFrameInfo.CropW);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-dcrH")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->outFrameInfo.CropH);
            }
            else if(0 == msdk_strcmp(strInput[i], MSDK_STRING("-dpic")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->outFrameInfo.PicStruct);
                pParams->outFrameInfo.PicStruct = GetPicStruct(pParams->outFrameInfo.PicStruct);
            }
            else if(0 == msdk_strcmp(strInput[i], MSDK_STRING("-df")))
            {
                i++;
                msdk_opt_read(strInput[i], pParams->outFrameInfo.dFrameRate);
            }
            else if( 0 == msdk_strcmp(strInput[i], MSDK_STRING("-extapi")) )
            {
                pParams->use_extapi = true;
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-p")))
            {
                if (++i < nArgNum) {
                    if (MFX_ERR_NONE == ConvertStringToGuid(strInput[i], pParams->pluginParams.pluginGuid)) {
                        pParams->pluginParams.type = MFX_PLUGINLOAD_TYPE_GUID;
                    }
                    else {
                        vppPrintHelp(strInput[0], MSDK_STRING("Invalid plugin GUID"));
                        return MFX_ERR_UNSUPPORTED;
                    }
                    pParams->need_plugin = true;
                }
            }
            //-----------------------------------------------------------------------------------
            //                   Video Enhancement Algorithms
            //-----------------------------------------------------------------------------------
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-denoise")))
            {
                pParams->denoiseParam.mode = VPP_FILTER_ENABLED_DEFAULT;

                if( i+1 < nArgNum )
                {
                    if (MFX_ERR_NONE == msdk_opt_read(strInput[i+1], readData))
                    {
                        pParams->denoiseParam.factor = (mfxU16)readData;
                        pParams->denoiseParam.mode   = VPP_FILTER_ENABLED_CONFIGURED;
                        i++;
                    }
                }
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-detail")))
            {
                pParams->detailParam.mode = VPP_FILTER_ENABLED_DEFAULT;

                if( i+1 < nArgNum )
                {
                    if (MFX_ERR_NONE == msdk_opt_read(strInput[i+1], readData))
                    {
                        pParams->detailParam.factor = (mfxU16)readData;
                        pParams->detailParam.mode   = VPP_FILTER_ENABLED_CONFIGURED;
                        i++;
                    }
                }
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-vanalysis")))
            {
                pParams->vaParam.mode = VPP_FILTER_ENABLED_DEFAULT;
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-pa_hue")))
            {
                pParams->procampParam.mode = VPP_FILTER_ENABLED_CONFIGURED;
                i++;
                msdk_opt_read(strInput[i], pParams->procampParam.hue);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-pa_bri")))
            {
                pParams->procampParam.mode = VPP_FILTER_ENABLED_CONFIGURED;
                i++;
                msdk_opt_read(strInput[i], pParams->procampParam.brightness);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-pa_con")))
            {
                pParams->procampParam.mode = VPP_FILTER_ENABLED_CONFIGURED;
                i++;
                msdk_opt_read(strInput[i], pParams->procampParam.contrast);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-pa_sat")))
            {
                pParams->procampParam.mode = VPP_FILTER_ENABLED_CONFIGURED;
                i++;
                msdk_opt_read(strInput[i], pParams->procampParam.saturation);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-istab")))
            {
                pParams->istabParam.mode = VPP_FILTER_ENABLED_DEFAULT;

                if( i+1 < nArgNum )
                {
                    if (MFX_ERR_NONE == msdk_opt_read(strInput[i+1], readData))
                    {
                        pParams->istabParam.istabMode = (mfxU8)readData;
                        pParams->istabParam.mode    = VPP_FILTER_ENABLED_CONFIGURED;
                        i++;

                        if( pParams->istabParam.istabMode != 1 && pParams->istabParam.istabMode != 2 )
                        {
                            vppPrintHelp(strInput[0], MSDK_STRING("Invalid IStab configuration"));
                            return MFX_ERR_UNSUPPORTED;
                        }
                    }
                }
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-composite")))
            {
                if( i+1 < nArgNum )
                {
                    if (ParseCompositionParfile(strInput[i+1], pParams) != MFX_ERR_NONE)
                    {
                        vppPrintHelp(strInput[0], MSDK_STRING("Incorrect parfile for -composite"));
                        return MFX_ERR_UNSUPPORTED;
                    }
                    pParams->compositionParam.mode = VPP_FILTER_ENABLED_CONFIGURED;
                }
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-frc:interp")))
            {
                pParams->frcParam.mode = VPP_FILTER_ENABLED_CONFIGURED;
                pParams->frcParam.algorithm = MFX_FRCALGM_FRAME_INTERPOLATION;
            }
            //-----------------------------------------------------------------------------------
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-i")))
            {
                i++;
                SIZE_CHECK((msdk_strlen(strInput[i])+1) > MSDK_ARRAY_LEN(pParams->strSrcFile));
                msdk_opt_read(strInput[i], pParams->strSrcFile);
                pParams->numStreams = 1;
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-o")))
            {
                i++;
                SIZE_CHECK((msdk_strlen(strInput[i])+1) > MSDK_ARRAY_LEN(pParams->strDstFile));
                msdk_opt_read(strInput[i], pParams->strDstFile);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-scc")))
            {
                i++;
                pParams->inFrameInfo[VPP_IN].FourCC = Str2FourCC( strInput[i] );
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-dcc")))
            {
                i++;
                pParams->outFrameInfo.FourCC = Str2FourCC( strInput[i] );
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-n")) )
            {
                i++;
                msdk_opt_read(strInput[i], pParams->requestedFramesCount);
            }
            else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("-lib")) )
            {
                i++;
                if (0 == msdk_strcmp(strInput[i], MSDK_STRING("sw")) )
                {
                    pParams->impLib = MFX_IMPL_SOFTWARE;
                }
                else if (0 == msdk_strcmp(strInput[i], MSDK_STRING("hw")) )
                {
                    pParams->impLib = MFX_IMPL_HARDWARE;
                }

            }
#if D3D_SURFACES_SUPPORT
            else if( 0 == msdk_strcmp(strInput[i], MSDK_STRING("-d3d")) )
            {
                pParams->memType = D3D9_MEMORY;
            }
#endif
#if MFX_D3D11_SUPPORT
            else if( 0 == msdk_strcmp(strInput[i], MSDK_STRING("-d3d11")) )
            {
                 pParams->memType = D3D11_MEMORY;
            }
#endif
#ifdef LIBVA_SUPPORT
            else if( 0 == msdk_strcmp(strInput[i], MSDK_STRING("-vaapi")) )
            {
                pParams->memType = VAAPI_MEMORY;
            }
#endif
        }
    }
    if (NOT_INIT_VALUE == pParams->outFrameInfo.nWidth)
    {
        pParams->outFrameInfo.nWidth = pParams->outFrameInfo.CropW = pParams->inFrameInfo[VPP_IN].nWidth;
    }
    if (NOT_INIT_VALUE == pParams->outFrameInfo.nHeight)
    {
        pParams->outFrameInfo.nHeight = pParams->outFrameInfo.CropH = pParams->inFrameInfo[VPP_IN].nHeight;
    }
    if (NOT_INIT_VALUE == pParams->inFrameInfo[VPP_IN].CropW)
    {
        pParams->inFrameInfo[VPP_IN].CropW = pParams->inFrameInfo[VPP_IN].nWidth;
    }

    if (NOT_INIT_VALUE == pParams->inFrameInfo[VPP_IN].CropH)
    {
        pParams->inFrameInfo[VPP_IN].CropH = pParams->inFrameInfo[VPP_IN].nHeight;
    }

    if (NOT_INIT_VALUE == pParams->outFrameInfo.CropW)
    {
        pParams->outFrameInfo.CropW = pParams->outFrameInfo.nWidth;
    }

    if (NOT_INIT_VALUE == pParams->outFrameInfo.CropH)
    {
        pParams->outFrameInfo.CropH = pParams->outFrameInfo.nHeight;
    }

    if (0 == msdk_strlen(pParams->strSrcFile) && pParams->compositionParam.mode != VPP_FILTER_ENABLED_CONFIGURED)
    {
        vppPrintHelp(strInput[0], MSDK_STRING("Source file name not found"));
        return MFX_ERR_UNSUPPORTED;
    };

    //if (pParams->compositionParam.mode == VPP_FILTER_ENABLED_CONFIGURED)
    //{
    //  for (i = 1; i < pParams->numStreams; i++)
    //    {
    //        pParams->inFrameInfo[i].FourCC = pParams->inFrameInfo[VPP_IN].FourCC;
    //    }
    //};

    if (0 == msdk_strlen(pParams->strDstFile))
    {
        vppPrintHelp(strInput[0], MSDK_STRING("Destination file name not found"));
        return MFX_ERR_UNSUPPORTED;
    };

    return MFX_ERR_NONE;

} // mfxStatus vppParseInputString(...)

/* EOF */
