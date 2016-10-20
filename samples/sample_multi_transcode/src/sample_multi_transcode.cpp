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
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include "sample_multi_transcode.h"

#if defined(LIBVA_WAYLAND_SUPPORT)
#include "class_wayland.h"
#endif

using namespace std;
using namespace TranscodingSample;

Launcher::Launcher():
    m_StartTime(0),
    m_eDevType(static_cast<mfxHandleType>(0))
{
} // Launcher::Launcher()

Launcher::~Launcher()
{
    Close();
} // Launcher::~Launcher()

CTranscodingPipeline* CreatePipeline()
{
    MOD_SMT_CREATE_PIPELINE;

    return new CTranscodingPipeline;
}

mfxStatus Launcher::Init(int argc, msdk_char *argv[])
{
    mfxStatus sts;
    mfxU32 i = 0;
    SafetySurfaceBuffer* pBuffer = NULL;
    mfxU32 BufCounter = 0;
    mfxHDL hdl = NULL;
    sInputParams    InputParams;

    //parent transcode pipeline
    CTranscodingPipeline *pParentPipeline = NULL;
    // source transcode pipeline use instead parent in heterogeneous pipeline
    CTranscodingPipeline *pSinkPipeline = NULL;

    // parse input par file
    sts = m_parser.ParseCmdLine(argc, argv);
    MSDK_CHECK_PARSE_RESULT(sts, MFX_ERR_NONE, sts);

    // get parameters for each session from parser
    while(m_parser.GetNextSessionParams(InputParams))
    {
        m_InputParamsArray.push_back(InputParams);
        InputParams.Reset();
    }

    // check correctness of input parameters
    sts = VerifyCrossSessionsOptions();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

#if defined(_WIN32) || defined(_WIN64)
    if (m_eDevType == MFX_HANDLE_D3D9_DEVICE_MANAGER)
    {
        m_pAllocParam.reset(new D3DAllocatorParams);
        m_hwdev.reset(new CD3D9Device());
        /* The last param set in vector always describe VPP+ENCODE or Only VPP
         * So, if we want to do rendering we need to do pass HWDev to CTranscodingPipeline */
        if (m_InputParamsArray[m_InputParamsArray.size() -1].eModeExt == VppCompOnly)
        {
            /* Rendering case */
            sts = m_hwdev->Init(NULL, 1, MSDKAdapter::GetNumber() );
            m_InputParamsArray[m_InputParamsArray.size() -1].m_hwdev = m_hwdev.get();
        }
        else /* NO RENDERING*/
        {
            sts = m_hwdev->Init(NULL, 0, MSDKAdapter::GetNumber() );
        }
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        sts = m_hwdev->GetHandle(MFX_HANDLE_D3D9_DEVICE_MANAGER, (mfxHDL*)&hdl);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        // set Device Manager to external dx9 allocator
        D3DAllocatorParams *pD3DParams = dynamic_cast<D3DAllocatorParams*>(m_pAllocParam.get());
        pD3DParams->pManager =(IDirect3DDeviceManager9*)hdl;
    }
#if MFX_D3D11_SUPPORT
    else if (m_eDevType == MFX_HANDLE_D3D11_DEVICE)
    {

        m_pAllocParam.reset(new D3D11AllocatorParams);
        m_hwdev.reset(new CD3D11Device());
        /* The last param set in vector always describe VPP+ENCODE or Only VPP
         * So, if we want to do rendering we need to do pass HWDev to CTranscodingPipeline */
        if (m_InputParamsArray[m_InputParamsArray.size() -1].eModeExt == VppCompOnly)
        {
            /* Rendering case */
            sts = m_hwdev->Init(NULL, 1, MSDKAdapter::GetNumber() );
            m_InputParamsArray[m_InputParamsArray.size() -1].m_hwdev = m_hwdev.get();
        }
        else /* NO RENDERING*/
        {
            sts = m_hwdev->Init(NULL, 0, MSDKAdapter::GetNumber() );
        }
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        sts = m_hwdev->GetHandle(MFX_HANDLE_D3D11_DEVICE, (mfxHDL*)&hdl);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        // set Device to external dx11 allocator
        D3D11AllocatorParams *pD3D11Params = dynamic_cast<D3D11AllocatorParams*>(m_pAllocParam.get());
        pD3D11Params->pDevice =(ID3D11Device*)hdl;

    }
#endif
#elif defined(LIBVA_X11_SUPPORT) || defined(LIBVA_DRM_SUPPORT)
    if (m_eDevType == MFX_HANDLE_VA_DISPLAY)
    {
        mfxI32  libvaBackend = 0;

        m_pAllocParam.reset(new vaapiAllocatorParams);
        vaapiAllocatorParams *pVAAPIParams = dynamic_cast<vaapiAllocatorParams*>(m_pAllocParam.get());
        /* The last param set in vector always describe VPP+ENCODE or Only VPP
         * So, if we want to do rendering we need to do pass HWDev to CTranscodingPipeline */
        if (m_InputParamsArray[m_InputParamsArray.size() -1].eModeExt == VppCompOnly)
        {
            sInputParams& params = m_InputParamsArray[m_InputParamsArray.size() -1];
            libvaBackend = params.libvaBackend;

            /* Rendering case */
            m_hwdev.reset(CreateVAAPIDevice(params.libvaBackend));
            if(!m_hwdev.get()) {
                msdk_printf(MSDK_STRING("error: failed to initialize VAAPI device\n"));
                return MFX_ERR_DEVICE_FAILED;
            }
            sts = m_hwdev->Init(&params.monitorType, 1, MSDKAdapter::GetNumber() );
            if (params.libvaBackend == MFX_LIBVA_DRM_MODESET) {
                CVAAPIDeviceDRM* drmdev = dynamic_cast<CVAAPIDeviceDRM*>(m_hwdev.get());
                pVAAPIParams->m_export_mode = vaapiAllocatorParams::CUSTOM_FLINK;
                pVAAPIParams->m_exporter = dynamic_cast<vaapiAllocatorParams::Exporter*>(drmdev->getRenderer());

            }
#if defined(LIBVA_WAYLAND_SUPPORT)
            else if (params.libvaBackend == MFX_LIBVA_WAYLAND) {
                VADisplay va_dpy = NULL;
                sts = m_hwdev->GetHandle(MFX_HANDLE_VA_DISPLAY, (mfxHDL *)&va_dpy);
                MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
                hdl = pVAAPIParams->m_dpy =(VADisplay)va_dpy;

                mfxHDL whdl = NULL;
                mfxHandleType hdlw_t = (mfxHandleType)HANDLE_WAYLAND_DRIVER;
                Wayland *wld;
                sts = m_hwdev->GetHandle(hdlw_t, &whdl);
                MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
                wld = (Wayland*)whdl;
                wld->SetRenderWinPos(params.nRenderWinX, params.nRenderWinY);
                wld->SetPerfMode(params.bPerfMode);

                pVAAPIParams->m_export_mode = vaapiAllocatorParams::PRIME;
            }
#endif // LIBVA_WAYLAND_SUPPORT
            params.m_hwdev = m_hwdev.get();
        }
        else /* NO RENDERING*/
        {
            m_hwdev.reset(CreateVAAPIDevice());
            if(!m_hwdev.get()) {
                msdk_printf(MSDK_STRING("error: failed to initialize VAAPI device\n"));
                return MFX_ERR_DEVICE_FAILED;
            }
            sts = m_hwdev->Init(NULL, 0, MSDKAdapter::GetNumber());
        }
        if (libvaBackend != MFX_LIBVA_WAYLAND) {
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        sts = m_hwdev->GetHandle(MFX_HANDLE_VA_DISPLAY, (mfxHDL*)&hdl);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        // set Device to external vaapi allocator
        pVAAPIParams->m_dpy =(VADisplay)hdl;
    }
    }
#endif
    if (!m_pAllocParam.get())
    {
        m_pAllocParam.reset(new SysMemAllocatorParams);
    }

    // each pair of source and sink has own safety buffer
    sts = CreateSafetyBuffers();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    /* One more hint. Example you have 3 dec + 1 enc sessions
    * (enc means vpp_comp call invoked. m_InputParamsArray.size() is 4.
    * You don't need take vpp comp params from last one session as it is enc session.
    * But you need process {0, 1, 2} sessions - totally 3.
    * So, you need start from 0 and end at 2.
    * */
    for(mfxI32 jj = 0; jj<(mfxI32)m_InputParamsArray.size() - 1; jj++)
    {
        /* Save params for VPP composition */
        sVppCompDstRect tempDstRect;
        tempDstRect.DstX = m_InputParamsArray[jj].nVppCompDstX;
        tempDstRect.DstY = m_InputParamsArray[jj].nVppCompDstY;
        tempDstRect.DstW = m_InputParamsArray[jj].nVppCompDstW;
        tempDstRect.DstH = m_InputParamsArray[jj].nVppCompDstH;
        m_VppDstRects.push_back(tempDstRect);
    }

    // create sessions, allocators
    for (i = 0; i < m_InputParamsArray.size(); i++)
    {
        GeneralAllocator* pAllocator = new GeneralAllocator;
        sts = pAllocator->Init(m_pAllocParam.get());
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        m_pAllocArray.push_back(pAllocator);

        std::auto_ptr<ThreadTranscodeContext> pThreadPipeline(new ThreadTranscodeContext);
        // extend BS processing init
        m_InputParamsArray[i].nTimeout == 0 ? m_pExtBSProcArray.push_back(new FileBitstreamProcessor) :
                                        m_pExtBSProcArray.push_back(new FileBitstreamProcessor_WithReset);
        pThreadPipeline->pPipeline.reset(CreatePipeline());

        pThreadPipeline->pBSProcessor = m_pExtBSProcArray.back();
        if (Sink == m_InputParamsArray[i].eMode)
        {
            /* N_to_1 mode */
            if ((VppComp == m_InputParamsArray[i].eModeExt) ||
                (VppCompOnly == m_InputParamsArray[i].eModeExt))
            {
                // Taking buffers from tail because they are stored in m_pBufferArray in reverse order
                // So, by doing this we'll fill buffers properly according to order from par file
                pBuffer = m_pBufferArray[m_pBufferArray.size()-1-BufCounter];
                BufCounter++;
            }
            else /* 1_to_N mode*/
            {
                pBuffer = m_pBufferArray[m_pBufferArray.size() - 1];
            }
            pSinkPipeline = pThreadPipeline->pPipeline.get();
            sts = m_pExtBSProcArray.back()->Init(m_InputParamsArray[i].strSrcFile, NULL);
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
        else if (Source == m_InputParamsArray[i].eMode)
        {
            /* N_to_1 mode */
            if ((VppComp == m_InputParamsArray[i].eModeExt) ||
                (VppCompOnly == m_InputParamsArray[i].eModeExt))
            {
                pBuffer = m_pBufferArray[m_pBufferArray.size() - 1];
            }
            else /* 1_to_N mode*/
            {
                pBuffer = m_pBufferArray[BufCounter];
                BufCounter++;
            }
            sts = m_pExtBSProcArray.back()->Init(NULL, m_InputParamsArray[i].strDstFile);
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
        else
        {
            sts = m_pExtBSProcArray.back()->Init(m_InputParamsArray[i].strSrcFile, m_InputParamsArray[i].strDstFile);
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
            pBuffer = NULL;
        }

        /**/
        /* Vector stored linearly in the memory !*/
        m_InputParamsArray[i].pVppCompDstRects = m_VppDstRects.empty() ? NULL : &m_VppDstRects[0];

        // if session has VPP plus ENCODE only (-i::source option)
        // use decode source session as input
        sts = MFX_ERR_MORE_DATA;
        if (Source == m_InputParamsArray[i].eMode)
        {
            sts = pThreadPipeline->pPipeline->Init(&m_InputParamsArray[i],
                                                   m_pAllocArray[i],
                                                   hdl,
                                                   pSinkPipeline,
                                                   pBuffer,
                                                   m_pExtBSProcArray.back());
        }
        else
        {
            sts =  pThreadPipeline->pPipeline->Init(&m_InputParamsArray[i],
                                                    m_pAllocArray[i],
                                                    hdl,
                                                    pParentPipeline,
                                                    pBuffer,
                                                    m_pExtBSProcArray.back());
        }

        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        if (!pParentPipeline && m_InputParamsArray[i].bIsJoin)
            pParentPipeline = pThreadPipeline->pPipeline.get();

        // set the session's start status (like it is waiting)
        pThreadPipeline->startStatus = MFX_WRN_DEVICE_BUSY;
        // set other session's parameters
        pThreadPipeline->implType = m_InputParamsArray[i].libType;
        m_pSessionArray.push_back(pThreadPipeline.release());

        mfxVersion ver = {{0, 0}};
        sts = m_pSessionArray[i]->pPipeline->QueryMFXVersion(&ver);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        PrintInfo(i, &m_InputParamsArray[i], &ver);
    }

    for (i = 0; i < m_InputParamsArray.size(); i++)
    {
        sts = m_pSessionArray[i]->pPipeline->CompleteInit();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        if (m_pSessionArray[i]->pPipeline->GetJoiningFlag())
            msdk_printf(MSDK_STRING("Session %d was joined with other sessions\n"), i);
        else
            msdk_printf(MSDK_STRING("Session %d was NOT joined with other sessions\n"), i);

        m_pSessionArray[i]->pPipeline->SetPipelineID(i);
    }

    msdk_printf(MSDK_STRING("\n"));

    return sts;

} // mfxStatus Launcher::Init()

void Launcher::Run()
{
    mfxU32 totalSessions;

    msdk_printf(MSDK_STRING("Transcoding started\n"));

    // mark start time
    m_StartTime = GetTick();

    // get parallel sessions parameters
    totalSessions = (mfxU32) m_pSessionArray.size();

    mfxU32 i;
    mfxStatus sts;

    MSDKThread * pthread = NULL;

    for (i = 0; i < totalSessions; i++)
    {
        pthread = new MSDKThread(sts, ThranscodeRoutine, (void *)m_pSessionArray[i]);

        m_HDLArray.push_back(pthread);
    }

    for (i = 0; i < m_pSessionArray.size(); i++)
    {
        m_HDLArray[i]->Wait();
    }

    msdk_printf(MSDK_STRING("\nTranscoding finished\n"));

} // mfxStatus Launcher::Init()

mfxStatus Launcher::ProcessResult()
{
    FILE* pPerfFile = m_parser.GetPerformanceFile();
    msdk_printf(MSDK_STRING("\nCommon transcoding time is  %.2f sec \n"), GetTime(m_StartTime));

    m_parser.PrintParFileName();

    if (pPerfFile)
    {
        msdk_fprintf(pPerfFile, MSDK_STRING("Common transcoding time is  %.2f sec \n"), GetTime(m_StartTime));
    }

    // get result
    bool SuccessTranscode = true;
    mfxU32 i;
    for (i = 0; i < m_pSessionArray.size(); i++)
    {
        mfxStatus sts = m_pSessionArray[i]->transcodingSts;
        if (MFX_ERR_NONE != sts)
        {
            SuccessTranscode = false;
            msdk_printf(MSDK_STRING("MFX session %d transcoding FAILED:\nProcessing time: %.2f sec \nNumber of processed frames: %d\n"),
                i,
                m_pSessionArray[i]->working_time,
                m_pSessionArray[i]->numTransFrames);
            if (pPerfFile)
            {
                msdk_fprintf(pPerfFile, MSDK_STRING("MFX session %d transcoding FAILED:\nProcessing time: %.2f sec \nNumber of processed frames: %d\n"),
                    i,
                    m_pSessionArray[i]->working_time,
                    m_pSessionArray[i]->numTransFrames);
            }

        }
        else
        {
            msdk_printf(MSDK_STRING("MFX session %d transcoding PASSED:\nProcessing time: %.2f sec \nNumber of processed frames: %d\n"),
                i,
                m_pSessionArray[i]->working_time,
                m_pSessionArray[i]->numTransFrames);
            if (pPerfFile)
            {
                msdk_fprintf(pPerfFile, MSDK_STRING("MFX session %d transcoding PASSED:\nProcessing time: %.2f sec \nNumber of processed frames: %d\n"),
                    i,
                    m_pSessionArray[i]->working_time,
                    m_pSessionArray[i]->numTransFrames);
            }
        }

        if (pPerfFile)
        {
            if (Native == m_InputParamsArray[i].eMode || Sink == m_InputParamsArray[i].eMode)
            {
                msdk_fprintf(pPerfFile, MSDK_STRING("Input stream: %s\n"), m_InputParamsArray[i].strSrcFile);
            }
            else
                msdk_fprintf(pPerfFile, MSDK_STRING("Input stream: from parent session\n"));
            msdk_fprintf(pPerfFile, MSDK_STRING("\n"));
        }


    }

    if (SuccessTranscode)
    {
        msdk_printf(MSDK_STRING("\nThe test PASSED\n"));
        if (pPerfFile)
        {
            msdk_fprintf(pPerfFile, MSDK_STRING("\nThe test PASSED\n"));
        }
        return MFX_ERR_NONE;
    }
    else
    {
        msdk_printf(MSDK_STRING("\nThe test FAILED\n"));
        if (pPerfFile)
        {
            msdk_fprintf(pPerfFile, MSDK_STRING("\nThe test FAILED\n"));
        }
        return MFX_ERR_UNKNOWN;
    }
} // mfxStatus Launcher::ProcessResult()

mfxStatus Launcher::VerifyCrossSessionsOptions()
{
    bool IsSinkPresence = false;
    bool IsSourcePresence = false;
    bool IsHeterSessionJoin = false;
    bool IsFirstInTopology = true;
    bool areAllInterSessionsOpaque = true;

    mfxU16 minAsyncDepth = 0;
    bool bUseExternalAllocator = false;
    for (mfxU32 i = 0; i < m_InputParamsArray.size(); i++)
    {
        if (!m_InputParamsArray[i].bUseOpaqueMemory &&
            ((m_InputParamsArray[i].eMode == Source) || (m_InputParamsArray[i].eMode == Sink)))
        {
            areAllInterSessionsOpaque = false;
        }

        if (m_InputParamsArray[i].bOpenCL ||
            m_InputParamsArray[i].EncoderFourCC ||
            m_InputParamsArray[i].DecoderFourCC)
        {
            bUseExternalAllocator = true;
        }

        // All sessions have to know about timeout
        if (m_InputParamsArray[i].nTimeout && (m_InputParamsArray[i].eMode == Sink))
        {
            for (mfxU32 j = 0; j < m_InputParamsArray.size(); j++)
            {
                if (m_InputParamsArray[j].MaxFrameNumber != MFX_INFINITE)
                {
                    msdk_printf(MSDK_STRING("\"-timeout\" option isn't compatible with \"-n\". \"-n\" will be ignored.\n"));
                    for (mfxU32 j = 0; j < m_InputParamsArray.size(); j++)
                        m_InputParamsArray[j].MaxFrameNumber = MFX_INFINITE;
                }
            }
            msdk_printf(MSDK_STRING("Timeout %d seconds has been set to all sessions\n"), m_InputParamsArray[i].nTimeout);
            for (mfxU32 j = 0; j < m_InputParamsArray.size(); j++)
                m_InputParamsArray[j].nTimeout = m_InputParamsArray[i].nTimeout;
        }

        if (Source == m_InputParamsArray[i].eMode)
        {
            if (m_InputParamsArray[i].nAsyncDepth < minAsyncDepth)
            {
                minAsyncDepth = m_InputParamsArray[i].nAsyncDepth;
            }
            // topology definition
            if (!IsSinkPresence)
            {
                PrintError(MSDK_STRING("Error in par file. Decode source session must be declared BEFORE encode sinks \n"));
                return MFX_ERR_UNSUPPORTED;
            }
            IsSourcePresence = true;

            if (IsFirstInTopology)
            {
                if (m_InputParamsArray[i].bIsJoin)
                    IsHeterSessionJoin = true;
                else
                    IsHeterSessionJoin = false;
            }
            else
            {
                if (m_InputParamsArray[i].bIsJoin && !IsHeterSessionJoin)
                {
                    PrintError(MSDK_STRING("Error in par file. All heterogeneous sessions must be joined \n"));
                    return MFX_ERR_UNSUPPORTED;
                }
                if (!m_InputParamsArray[i].bIsJoin && IsHeterSessionJoin)
                {
                    PrintError(MSDK_STRING("Error in par file. All heterogeneous sessions must be NOT joined \n"));
                    return MFX_ERR_UNSUPPORTED;
                }
            }

            if (IsFirstInTopology)
                IsFirstInTopology = false;

        }
        else if (Sink == m_InputParamsArray[i].eMode)
        {
            minAsyncDepth = m_InputParamsArray[i].nAsyncDepth;
            IsSinkPresence = true;

            if (IsFirstInTopology)
            {
                if (m_InputParamsArray[i].bIsJoin)
                    IsHeterSessionJoin = true;
                else
                    IsHeterSessionJoin = false;
            }
            else
            {
                if (m_InputParamsArray[i].bIsJoin && !IsHeterSessionJoin)
                {
                    PrintError(MSDK_STRING("Error in par file. All heterogeneous sessions must be joined \n"));
                    return MFX_ERR_UNSUPPORTED;
                }
                if (!m_InputParamsArray[i].bIsJoin && IsHeterSessionJoin)
                {
                    PrintError(MSDK_STRING("Error in par file. All heterogeneous sessions must be NOT joined \n"));
                    return MFX_ERR_UNSUPPORTED;
                }
            }

            if (IsFirstInTopology)
                IsFirstInTopology = false;
        }
        if (MFX_IMPL_SOFTWARE != m_InputParamsArray[i].libType)
        {
            // TODO: can we avoid ifdef and use MFX_IMPL_VIA_VAAPI?
#if defined(_WIN32) || defined(_WIN64)
            m_eDevType = (MFX_IMPL_VIA_D3D11 == MFX_IMPL_VIA_MASK(m_InputParamsArray[i].libType))?
                MFX_HANDLE_D3D11_DEVICE :
                MFX_HANDLE_D3D9_DEVICE_MANAGER;
#elif defined(LIBVA_SUPPORT)
            m_eDevType = MFX_HANDLE_VA_DISPLAY;
#endif
        }
    }

    if (bUseExternalAllocator)
    {
        for(mfxU32 i = 0; i < m_InputParamsArray.size(); i++)
        {
            m_InputParamsArray[i].bUseOpaqueMemory = false;
        }
        msdk_printf(MSDK_STRING("OpenCL or chroma conversion is present at least in one session. External memory allocator will be used for all sessions .\n"));
    }

    // Async depth between inter-sessions should be equal to the minimum async depth of all these sessions.
    for (mfxU32 i = 0; i < m_InputParamsArray.size(); i++)
    {
        if ((m_InputParamsArray[i].eMode == Source) || (m_InputParamsArray[i].eMode == Sink))
        {
            m_InputParamsArray[i].nAsyncDepth = minAsyncDepth;

            //--- If at least one of inter-session is not using opaque memory, all of them should not use it
            if(!areAllInterSessionsOpaque)
            {
                m_InputParamsArray[i].bUseOpaqueMemory=false;
            }
        }
    }

    if(!areAllInterSessionsOpaque)
    {
        msdk_printf(MSDK_STRING("Some inter-sessions do not use opaque memory (possibly because of -o::raw).\nOpaque memory in all inter-sessions is disabled.\n"));
    }

    if (IsSinkPresence && !IsSourcePresence)
    {
        PrintError(MSDK_STRING("Error: Sink must be defined"));
        return MFX_ERR_UNSUPPORTED;
    }
    return MFX_ERR_NONE;

} // mfxStatus Launcher::VerifyCrossSessionsOptions()

mfxStatus Launcher::CreateSafetyBuffers()
{
    SafetySurfaceBuffer* pBuffer     = NULL;
    SafetySurfaceBuffer* pPrevBuffer = NULL;

    for (mfxU32 i = 0; i < m_InputParamsArray.size(); i++)
    {
        /* this is for 1 to N case*/
        if ((Source == m_InputParamsArray[i].eMode) &&
            (Native == m_InputParamsArray[0].eModeExt))
        {
            pBuffer = new SafetySurfaceBuffer(pPrevBuffer);
            pPrevBuffer = pBuffer;
            m_pBufferArray.push_back(pBuffer);
        }

        /* And N_to_1 case: composition should be enabled!
         * else it is logic error */
        if ( (Source != m_InputParamsArray[i].eMode) &&
             ( (VppComp     == m_InputParamsArray[0].eModeExt) ||
               (VppCompOnly == m_InputParamsArray[0].eModeExt) ) )
        {
            pBuffer = new SafetySurfaceBuffer(pPrevBuffer);
            pPrevBuffer = pBuffer;
            m_pBufferArray.push_back(pBuffer);
        }
    }
    return MFX_ERR_NONE;

} // mfxStatus Launcher::CreateSafetyBuffers

void Launcher::Close()
{
    while(m_pSessionArray.size())
    {
        delete m_pSessionArray[m_pSessionArray.size()-1];
        m_pSessionArray[m_pSessionArray.size() - 1] = NULL;
        delete m_pAllocArray[m_pSessionArray.size()-1];
        m_pAllocArray[m_pSessionArray.size() - 1] = NULL;
        m_pAllocArray.pop_back();
        m_pSessionArray.pop_back();
    }
    while(m_pBufferArray.size())
    {
        delete m_pBufferArray[m_pBufferArray.size()-1];
        m_pBufferArray[m_pBufferArray.size() - 1] = NULL;
        m_pBufferArray.pop_back();
    }

    while(m_pExtBSProcArray.size())
    {
        delete m_pExtBSProcArray[m_pExtBSProcArray.size() - 1];
        m_pExtBSProcArray[m_pExtBSProcArray.size() - 1] = NULL;
        m_pExtBSProcArray.pop_back();
    }

    while (m_HDLArray.size())
    {
        delete m_HDLArray[m_HDLArray.size()-1];
        m_HDLArray.pop_back();
    }
} // void Launcher::Close()

#if defined(_WIN32) || defined(_WIN64)
int _tmain(int argc, TCHAR *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    mfxStatus sts;
    Launcher transcode;
    sts = transcode.Init(argc, argv);
    fflush(stdout);
    fflush(stderr);
    MSDK_CHECK_PARSE_RESULT(sts, MFX_ERR_NONE, 1);

    transcode.Run();

    sts = transcode.ProcessResult();
    fflush(stdout);
    fflush(stderr);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, 1);

    return 0;
}

