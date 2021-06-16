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

#include <sys/time.h>
#include <csignal>
#include <cstring>

#if !defined(WIN32)
namespace NF{

    struct itimerval it;
    struct sigaction sa;

        // Data gathering does not consider a profiling option. It'll applied in Profiler::Show()
        // Compared to Windows implementation, this callback function blocks threads.
        inline void record_counter(int _){
            thread_local_profile_record.gather(callee);
        }

        void Profiler::Start(ProfileMode mode_){
            mode = mode_;
            memset(&sa, 0, sizeof(sa));
            sa.sa_handler = &record_counter;
            sa.sa_flags = SA_RESTART;
            sigaction(SIGPROF, &sa, NULL);

            it.it_value.tv_sec = 0;
            it.it_value.tv_usec = 10000;
            it.it_interval.tv_sec = 0;
            it.it_interval.tv_usec = 10000;

            setitimer(ITIMER_PROF, &it, NULL);

            start = std::chrono::system_clock::now();
        }

        void Profiler::End(){
            for(auto e:thread_local_profile_record.m_data){
                global_profile_record[GlobalProfileKey(thread_id, e.first)] += e.second;
            }
            duration = std::chrono::system_clock::now() - start;
            memset(&sa, 0, sizeof(sa));
            memset(&it, 0, sizeof(it));
        }
}
#endif
