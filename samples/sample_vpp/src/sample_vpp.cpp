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

/* default params */
const sOwnFrameInfo       defaultOwnFrameInfo     = {352, 288, 0, 0, 352, 288, MFX_FOURCC_NV12, MFX_PICSTRUCT_PROGRESSIVE, 30.0};
const sProcAmpParam       defaultProcAmpParam     = {0.0, 1.0, 1.0, 0.0, VPP_FILTER_DISABLED};
const sDetailParam        defaultDetailParam      = {1,  VPP_FILTER_DISABLED};
const sDenoiseParam       defaultDenoiseParam     = {1,  VPP_FILTER_DISABLED};
const sVideoAnalysisParam defaultVAParam          = {VPP_FILTER_DISABLED};
const sIStabParam         defaultImgStabParam     = {MFX_IMAGESTAB_MODE_BOXING, VPP_FILTER_DISABLED};
const sCompositionParam   defaultCompositionParam = {{}, VPP_FILTER_DISABLED};

static
void vppDefaultInitParams( sInputParams* pParams )
{
  for (int i = 0; i < MAX_INPUT_STREAMS; i++)
    pParams->inFrameInfo[i]  = defaultOwnFrameInfo;
  pParams->outFrameInfo = defaultOwnFrameInfo;

  pParams->outFrameInfo.nHeight = pParams->outFrameInfo.nWidth =
      pParams->outFrameInfo.CropH = pParams->outFrameInfo.CropW = NOT_INIT_VALUE;

  // Video Enhancement Algorithms
  pParams->denoiseParam     = defaultDenoiseParam;
  pParams->detailParam      = defaultDetailParam;
  pParams->procampParam     = defaultProcAmpParam;
  pParams->vaParam          = defaultVAParam;
  pParams->istabParam       = defaultImgStabParam;
  pParams->compositionParam = defaultCompositionParam;

  pParams->memType    = SYSTEM_MEMORY;

  pParams->requestedFramesCount = MFX_MAX_32U;

  pParams->impLib = MFX_IMPL_HARDWARE;

  // Use RunFrameVPPAsyncEx
  pParams->use_extapi  = false;
  pParams->need_plugin = false;

  return;

} // void vppDefaultInitParams( sInputParams* pParams )

static
void vppSafeRealInfo( sOwnFrameInfo* in, mfxFrameInfo* out )
{
  out->Width  = in->nWidth;
  out->Height = in->nHeight;
  out->CropX  = 0;
  out->CropY  = 0;
  out->CropW  = out->Width;
  out->CropH  = out->Height;
  out->FourCC = in->FourCC;

  return;

} // void vppSafeRealInfo( sOwnFrameInfo* in, mfxFrameInfo* out )

static
void ownToMfxFrameInfo( sOwnFrameInfo* in, mfxFrameInfo* out )
{
  out->Width  = in->nWidth;
  out->Height = in->nHeight;
  out->CropX  = in->CropX;
  out->CropY  = in->CropY;
  out->CropW  = (in->CropW == NOT_INIT_VALUE) ? in->nWidth : in->CropW;
  out->CropH  = (in->CropH == NOT_INIT_VALUE) ? in->nHeight : in->CropH;
  out->FourCC = in->FourCC;
  out->PicStruct = in->PicStruct;
  ConvertFrameRate(in->dFrameRate, &out->FrameRateExtN, &out->FrameRateExtD);

  return;

} // void ownToMfxFrameInfo( sOwnFrameInfo* in, mfxFrameInfo* out )

#if defined(_WIN32) || defined(_WIN64)
int _tmain(int argc, TCHAR *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  mfxStatus           sts = MFX_ERR_NONE;
  mfxU32              nFrames = 0;
  mfxU16              nInStreamInd = 0;

  CRawVideoReader     yuvReaders[MAX_INPUT_STREAMS];
  CRawVideoWriter     yuvWriter;

  sFrameProcessor     FrameProcessor;
  sMemoryAllocator    Allocator;

  sInputParams        Params;
  mfxVideoParam       mfxParamsVideo;
  sAppResources       Resources;

  // to prevent incorrect read/write of image in case of CropW/H != width/height
  mfxFrameInfo        inFrameInfo[MAX_INPUT_STREAMS];
  mfxFrameInfo        outFrameInfo;

  mfxFrameSurface1*   pInSurf[MAX_INPUT_STREAMS]={0};
  mfxFrameSurface1*   pOutSurf = NULL;
  mfxFrameSurface1*   pWorkSurf = NULL;

  mfxSyncPoint        syncPoint;

  mfxExtVppAuxData    extVPPAuxData;

  mfxU32              readFramesCount = 0;

  mfxU16              i, j;

  mfxU16              argbSurfaceIndex = 0xffff;

  //reset pointers to the all internal resources
  MSDK_ZERO_MEMORY(Resources);
  MSDK_ZERO_MEMORY(mfxParamsVideo);
  MSDK_ZERO_MEMORY(Params);
  MSDK_ZERO_MEMORY(Allocator);
  MSDK_ZERO_MEMORY(outFrameInfo);
  MSDK_ZERO_MEMORY(extVPPAuxData);
//  MSDK_ZERO_MEMORY(FrameProcessor);
  FrameProcessor.pmfxVPP = NULL;
  for (i = 0; i < MAX_INPUT_STREAMS; i++)
  {
      MSDK_ZERO_MEMORY(inFrameInfo[i]);
  }
  MSDK_ZERO_MEMORY(outFrameInfo);
  vppDefaultInitParams( &Params );

  //parse input string
  sts = vppParseInputString(argv, (mfxU8)argc, &Params);
  MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, 1);

  Resources.numSrcFiles = 1;
  if (Params.compositionParam.mode == VPP_FILTER_ENABLED_CONFIGURED)
  {
    Resources.numSrcFiles = Params.numStreams > MAX_INPUT_STREAMS ? MAX_INPUT_STREAMS : Params.numStreams;
    for (i = 0; i < Resources.numSrcFiles; i++)
    {
      ownToMfxFrameInfo( &(Params.inFrameInfo[i]), &(inFrameInfo[i]) );
      sts = yuvReaders[i].Init(Params.compositionParam.streamInfo[i].streamName);
      MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, 1);
    }
  }
  else
  {
    vppSafeRealInfo( &(Params.inFrameInfo[VPP_IN]), &inFrameInfo[VPP_IN] );
    sts = yuvReaders[VPP_IN].Init(Params.strSrcFile);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, 1);
  }

  // to prevent incorrect read/write of image in case of CropW/H != width/height
  // application save real image size
  vppSafeRealInfo( &(Params.outFrameInfo), &outFrameInfo );

  for (i = 0; i < Resources.numSrcFiles; i++)
  {
    Resources.pSrcFileReaders[i] = &yuvReaders[i];
  }
  Resources.pDstFileWriter       = &yuvWriter;
  Resources.pProcessor           = &FrameProcessor;
  Resources.pAllocator           = &Allocator;
  Resources.pVppParams           = &mfxParamsVideo;

  //prepare file writer (YUV file)
  sts = yuvWriter.Init(Params.strDstFile);
  MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, 1, WipeResources(&Resources));

  //prepare mfxParams
  sts = InitParamsVPP(&mfxParamsVideo, &Params);
  MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, 1, WipeResources(&Resources));

  sts = ConfigVideoEnhancementFilters(&Params, &Resources);
  MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, 1, WipeResources(&Resources));

  sts = InitResources(&Resources, &mfxParamsVideo, &Params);
  MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, 1, WipeResources(&Resources));

  // print parameters to console
  PrintInfo(&Params, &mfxParamsVideo, &Resources.pProcessor->mfxSession);

  sts = MFX_ERR_NONE;
  nFrames = 1;

  if (Allocator.response[VPP_IN].NumFrameActual < Resources.numSrcFiles)
    return MFX_ERR_MEMORY_ALLOC;

  if (Params.compositionParam.mode == VPP_FILTER_ENABLED_CONFIGURED)
  {
    for (i = 0; i < Resources.numSrcFiles; i++)
    {
        /* Normal case of processing */
        if (inFrameInfo[i].FourCC != MFX_FOURCC_RGB4)
        {
            MSDK_MEMCPY(&(Allocator.pSurfaces[VPP_IN][i].Info), &(inFrameInfo[i]), sizeof (mfxFrameInfo));
            //Composition need the whole frame reading
            inFrameInfo[i].CropX = 0;
            inFrameInfo[i].CropY = 0;
            //inFrameInfo[i].CropW = inFrameInfo[i].Width;
            //inFrameInfo[i].CropH = inFrameInfo[i].Height;

        }
        else if (inFrameInfo[i].FourCC == MFX_FOURCC_RGB4)
        {
            /* This is case for mixed processing NV12 plus RGB4:
             * all (all surface from pool, does not only current one!)
             * RGB4 surfaces should be properly initialised */
            for (j = 0; j < Allocator.response[VPP_IN_RGB].NumFrameActual; j++)
            {
                MSDK_MEMCPY(&(Allocator.pSurfaces[VPP_IN_RGB][j].Info), &(inFrameInfo[i]), sizeof (mfxFrameInfo));
                Allocator.pSurfaces[VPP_IN_RGB][j].Info.CropH = inFrameInfo[i].CropH;
                Allocator.pSurfaces[VPP_IN_RGB][j].Info.CropW = inFrameInfo[i].CropW;
            }
            inFrameInfo[i].CropX = 0;
            inFrameInfo[i].CropY = 0;
            argbSurfaceIndex = i;
        }
    }
  }
  msdk_printf(MSDK_STRING("VPP started\n"));

  bool bDoNotUpdateIn = false;
  if(Params.use_extapi)
      bDoNotUpdateIn = true;
  while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_DATA == sts || bDoNotUpdateIn )
  {
    if (nInStreamInd >= MAX_INPUT_STREAMS)
    {
        sts = MFX_ERR_UNKNOWN;
        break;
    }

    if( !bDoNotUpdateIn )
    {
        if (nInStreamInd == argbSurfaceIndex)
        {
            sts = GetFreeSurface(Allocator.pSurfaces[VPP_IN_RGB],
                                 Allocator.response[VPP_IN_RGB].NumFrameActual,
                                 &pInSurf[nInStreamInd]);
        }
        else
        {
            sts = GetFreeSurface(Allocator.pSurfaces[VPP_IN],
                                 Allocator.response[VPP_IN].NumFrameActual,
                                 &pInSurf[nInStreamInd]);
        }


        MSDK_BREAK_ON_ERROR(sts);
        if (readFramesCount++ == Params.requestedFramesCount)
        {
            sts = MFX_ERR_MORE_DATA;
            break;
        }

        // if we share allocator with mediasdk we need to call Lock to access surface data and after we're done call Unlock
        if (Allocator.bUsedAsExternalAllocator)
        {
          // get YUV pointers
          sts = Allocator.pMfxAllocator->Lock(Allocator.pMfxAllocator->pthis, pInSurf[nInStreamInd]->Data.MemId, &(pInSurf[nInStreamInd]->Data));
          MSDK_BREAK_ON_ERROR(sts);

          sts = yuvReaders[nInStreamInd].LoadNextFrame( &(pInSurf[nInStreamInd]->Data), &(inFrameInfo[nInStreamInd]));
          MSDK_BREAK_ON_ERROR(sts);

          sts = Allocator.pMfxAllocator->Unlock(Allocator.pMfxAllocator->pthis, pInSurf[nInStreamInd]->Data.MemId, &(pInSurf[nInStreamInd]->Data));
          MSDK_BREAK_ON_ERROR(sts);
        }
        else
        {
          sts = yuvReaders[nInStreamInd].LoadNextFrame( &(pInSurf[nInStreamInd]->Data), &(inFrameInfo[nInStreamInd]));
          MSDK_BREAK_ON_ERROR(sts);
        }
    }

    if ( !Params.use_extapi )
    {
        sts = GetFreeSurface(Allocator.pSurfaces[VPP_OUT],
                             Allocator.response[VPP_OUT].NumFrameActual,
                             &pOutSurf);
    }
    else
    {
        sts = GetFreeSurface(Allocator.pSurfaces[VPP_OUT],
                             Allocator.response[VPP_OUT].NumFrameActual,
                             &pWorkSurf);
    }
    MSDK_BREAK_ON_ERROR(sts);

    // VPP processing
    bDoNotUpdateIn = false;
    if ( !Params.use_extapi )
    {
        sts = FrameProcessor.pmfxVPP->RunFrameVPPAsync( pInSurf[nInStreamInd], pOutSurf,
                                                        (VPP_FILTER_DISABLED != Params.vaParam.mode)? &extVPPAuxData : NULL,
                                                         &syncPoint );
    }
    else
    {
        sts = FrameProcessor.pmfxVPP->RunFrameVPPAsyncEx( pInSurf[nInStreamInd], pWorkSurf, &pOutSurf, &syncPoint );
        if(MFX_ERR_MORE_DATA != sts)
            bDoNotUpdateIn = true;
    }

    nInStreamInd++;
    if (nInStreamInd == Resources.numSrcFiles)
        nInStreamInd = 0;

    if (MFX_ERR_MORE_DATA == sts)
    {
      continue;
    }

    //MFX_ERR_MORE_SURFACE means output is ready but need more surface
    //because VPP produce multiple out. example: Frame Rate Conversion 30->60
    if (MFX_ERR_MORE_SURFACE == sts)
    {
        if ( Params.use_extapi )
        {
            // RunFrameAsyncEx is used
            continue;
        }
        bDoNotUpdateIn = true;
        sts = MFX_ERR_NONE;
    }

    MSDK_BREAK_ON_ERROR(sts);

    sts = Resources.pProcessor->mfxSession.SyncOperation(syncPoint, MSDK_VPP_WAIT_INTERVAL);
    MSDK_BREAK_ON_ERROR(sts);

    // if we share allocator with mediasdk we need to call Lock to access surface data and after we're done call Unlock
    if (Allocator.bUsedAsExternalAllocator)
    {
      // get YUV pointers
      sts = Allocator.pMfxAllocator->Lock(Allocator.pMfxAllocator->pthis, pOutSurf->Data.MemId, &(pOutSurf->Data));
      MSDK_BREAK_ON_ERROR(sts);

      sts = yuvWriter.WriteFrame( &(pOutSurf->Data), &outFrameInfo );
      MSDK_BREAK_ON_ERROR(sts);

      sts = Allocator.pMfxAllocator->Unlock(Allocator.pMfxAllocator->pthis, pOutSurf->Data.MemId, &(pOutSurf->Data));
      MSDK_BREAK_ON_ERROR(sts);
    }
    else
    {
      sts = yuvWriter.WriteFrame( &(pOutSurf->Data), &outFrameInfo );
      MSDK_BREAK_ON_ERROR(sts);
    }

    //VPP progress
    if( VPP_FILTER_DISABLED == Params.vaParam.mode )
    {
      msdk_printf(MSDK_STRING("Frame number: %u\r"), nFrames++);
    }
    else
    {
      msdk_printf(MSDK_STRING("Frame number: %u, spatial: %u, temporal: %u, scd: %u \n"), nFrames++,
                                                            extVPPAuxData.SpatialComplexity,
                                                            extVPPAuxData.TemporalComplexity,
                                                            extVPPAuxData.SceneChangeRate);
    }
  }

  // means that file has ended, need to go to buffering loop
  MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
  // exit in case of other errors
  MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, 1, WipeResources(&Resources));

  // loop to get buffered frames from VPP
  while (MFX_ERR_NONE <= sts)
  {
    if ( !Params.use_extapi )
    {
        sts = GetFreeSurface(Allocator.pSurfaces[VPP_OUT],
                             Allocator.response[VPP_OUT].NumFrameActual,
                             &pOutSurf);
    }
    else
    {
        sts = GetFreeSurface(Allocator.pSurfaces[VPP_OUT],
                             Allocator.response[VPP_OUT].NumFrameActual,
                             &pWorkSurf);
    }
    MSDK_BREAK_ON_ERROR(sts);

    if ( !Params.use_extapi )
    {
        sts = FrameProcessor.pmfxVPP->RunFrameVPPAsync( NULL, pOutSurf,
                                                        (VPP_FILTER_DISABLED != Params.vaParam.mode)? &extVPPAuxData : NULL,
                                                        &syncPoint );
    }
    else
    {
        sts = FrameProcessor.pmfxVPP->RunFrameVPPAsyncEx( NULL, pWorkSurf, &pOutSurf, &syncPoint );
        if (MFX_ERR_MORE_SURFACE == sts)
        {
            sts = MFX_ERR_NONE;
            continue;
        }
    }

    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_SURFACE);
    MSDK_BREAK_ON_ERROR(sts);

    sts = Resources.pProcessor->mfxSession.SyncOperation(syncPoint, MSDK_VPP_WAIT_INTERVAL);
    MSDK_BREAK_ON_ERROR(sts);

    // if we share allocator with mediasdk we need to call Lock to access surface data and after we're done call Unlock
    if (Allocator.bUsedAsExternalAllocator)
    {
      // get YUV pointers
      sts = Allocator.pMfxAllocator->Lock(Allocator.pMfxAllocator->pthis, pOutSurf->Data.MemId, &(pOutSurf->Data));
      MSDK_BREAK_ON_ERROR(sts);

      sts = yuvWriter.WriteFrame( &(pOutSurf->Data), &outFrameInfo );
      MSDK_BREAK_ON_ERROR(sts);

      sts = Allocator.pMfxAllocator->Unlock(Allocator.pMfxAllocator->pthis, pOutSurf->Data.MemId, &(pOutSurf->Data));
      MSDK_BREAK_ON_ERROR(sts);
    }
    else
    {
      sts = yuvWriter.WriteFrame( &(pOutSurf->Data), &outFrameInfo );
      MSDK_BREAK_ON_ERROR(sts);
    }

    //VPP progress
    if( VPP_FILTER_DISABLED == Params.vaParam.mode )
    {
      msdk_printf(MSDK_STRING("Frame number: %u\r"), nFrames++);
    }
    else
    {
      msdk_printf(MSDK_STRING("Frame number: %u, spatial: %u, temporal: %u, scd: %u \n"), nFrames++,
        extVPPAuxData.SpatialComplexity,
        extVPPAuxData.TemporalComplexity,
        extVPPAuxData.SceneChangeRate);
    }
  }

  MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);

  // report any errors that occurred
  MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, 1, WipeResources(&Resources));

  msdk_printf(MSDK_STRING("\nVPP finished\n"));

  WipeResources(&Resources);

  return 0; /* OK */

} // int _tmain(int argc, TCHAR *argv[])
/* EOF */
