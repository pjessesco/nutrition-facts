//
// This software is released under the MIT license.
//
// Copyright (c) 2021 Jino Park
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include "nutrition_facts_common.h"

#if defined(WIN32)
#include <Windows.h>
#include <Winuser.h>
#include <threadpoollegacyapiset.h>
#define IDT_TIMER_1 1001

#include <cstring>

namespace NF{

    HANDLE timer_handle_;

    // Data gathering does not consider a profiling option. It'll applied in Profiler::Show()
    // Compared to linux implementation, this callback function does not block main thread.
    inline void record_counter(PVOID, BOOLEAN){
        thread_local_profile_record.gather(callee);
    }

    void Profiler::Start(ProfileMode mode_){
        mode = mode_;
        std::cout<<"For windows, Nutrition-facts profiler does not support multi-thread profiling. It'll trace main thread only."<<std::endl;
        HANDLE timer_handle_;
        CreateTimerQueueTimer(&timer_handle_, NULL, record_counter, NULL, 0, 10, WT_EXECUTEDEFAULT);
        start = std::chrono::system_clock::now();
    }

    // TODO : It may not support pause and restart
    void Profiler::End(){
        duration = std::chrono::system_clock::now() - start;
        ChangeTimerQueueTimer(timer_handle_, 0, 0, 0);
        for(auto e:thread_local_profile_record.m_data){
            global_profile_record[e.first] += e.second;
        }
    }

}
#endif
