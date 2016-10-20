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

#pragma once

#include "mfxstructures.h"
#include "vm/time_defs.h"
#include "vm/strings_defs.h"
#include "math.h"

#pragma warning(disable:4100)

class CTimeStatistics
{
public:
    CTimeStatistics()
    {
#ifdef TIME_STATS
        ResetStatistics();
        start=0;
#endif
    }

    static msdk_tick GetFrequency()
    {
        if (!frequency)
        {
            frequency = msdk_time_get_frequency();
        }
        return frequency;
    }

    static mfxF64 ConvertToSeconds(msdk_tick elapsed)
    {
#ifdef TIME_STATS
        return MSDK_GET_TIME(elapsed, 0, GetFrequency());
#else
        return 0;
#endif
    }

    inline void StartTimeMeasurement()
    {
#ifdef TIME_STATS
        start = msdk_time_get_tick();
#endif
    }

    inline void StopTimeMeasurement()
    {
#ifdef TIME_STATS
        mfxF64 delta=GetDeltaTime();
        totalTime+=delta;
        totalTimeSquares+=delta*delta;

        if(delta<minTime)
        {
            minTime=delta;
        }

        if(delta>maxTime)
        {
            maxTime=delta;
        }
        numMeasurements++;
#endif
    }

    inline void StopTimeMeasurementWithCheck()
    {
#ifdef TIME_STATS
        if(start)
        {
            StopTimeMeasurement();
        }
#endif
    }

    inline mfxF64 GetDeltaTime()
    {
#ifdef TIME_STATS
        return MSDK_GET_TIME(msdk_time_get_tick(), start, GetFrequency());
#else
        return 0;
#endif
    }

    inline void PrintStatistics(const msdk_char* prefix)
    {
#ifdef TIME_STATS
        msdk_printf(MSDK_STRING("%s Total:%.3lf(%lld smpls),Avg %.3lf,StdDev:%.3lf,Min:%.3lf,Max:%.3lf\n"),prefix,totalTime*1000,numMeasurements,GetAvgTime()*1000,GetTimeStdDev()*1000,minTime*1000,maxTime*1000);
#endif
    }

    inline mfxU64 GetNumMeasurements()
    {
#ifdef TIME_STATS
        return numMeasurements;
#else
        return 0;
#endif
    }

    inline mfxF64 GetAvgTime()
    {
#ifdef TIME_STATS
        return numMeasurements ? totalTime/numMeasurements : 0;
#else
        return 0;
#endif
    }

    inline mfxF64 GetTimeStdDev()
    {
#ifdef TIME_STATS
        mfxF64 avg = GetAvgTime();
        return numMeasurements ? sqrt(totalTimeSquares/numMeasurements-avg*avg) : 0;
#else
        return 0;
#endif
    }

inline mfxF64 GetMinTime()
    {
#ifdef TIME_STATS
        return minTime;
#else
        return 0;
#endif
    }

inline mfxF64 GetMaxTime()
    {
#ifdef TIME_STATS
        return maxTime;
#else
        return 0;
#endif
    }

    inline mfxF64 GetTotalTime()
    {
#ifdef TIME_STATS
        return  totalTime;
#else
        return 0;
#endif
    }

    inline void ResetStatistics()
    {
#ifdef TIME_STATS
        totalTime=0;
        totalTimeSquares=0;
        minTime=1E100;
        maxTime=-1;
        numMeasurements=0;
#endif
    }

protected:
    static msdk_tick frequency;
#ifdef TIME_STATS
    msdk_tick start;
    mfxF64 totalTime;
    mfxF64 totalTimeSquares;
    mfxF64 minTime;
    mfxF64 maxTime;
    mfxU64 numMeasurements;
#endif
};
