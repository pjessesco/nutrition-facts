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

#include <sys/time.h>

#include <csignal>
#include <cstring>

#include <algorithm>
#include <tuple>
#include <string>
#include <unordered_map>
#include <atomic>
#include <iomanip>
#include <vector>
#include <chrono>

namespace NF{

#define NF_RECORD_FUNC(string) \
    NF::ProfileFunction NF_MARKER_PROFILED_FUNCTION(string);

    using GlobalProfileRecord = std::unordered_map<const char *, std::atomic<unsigned int>>;
    static GlobalProfileRecord global_profile_record;

    struct ProfileRecordPerThread{
        ProfileRecordPerThread() : m_data() {}
        ~ProfileRecordPerThread(){
            // Pass thread_local record to global record
            for(auto e:m_data){
                global_profile_record[e.first] += e.second;
            }
        }
        inline void gather(const char *callee){
            m_data[callee]++;
        }
        std::unordered_map<const char *, unsigned int> m_data;
    };

    constexpr char *unmarked_str = "Unmarked";
    thread_local ProfileRecordPerThread thread_local_profile_record;
    thread_local const char *callee = unmarked_str;
    struct itimerval it;
    struct sigaction sa;

    enum class ProfileMode{
        TrackAll,
        TrackMarkedOnly
    };

    static ProfileMode mode = ProfileMode::TrackMarkedOnly;

    std::chrono::system_clock::time_point start;
    std::chrono::duration<double> sec;

    struct ProfileFunction{
        ProfileFunction(const char *new_callee){
            caller = callee;
            callee = new_callee;
        }
        ~ProfileFunction(){
            callee = caller;
        }
        const char *caller;
    };

    inline void signal_handler(int _){
        if((mode == ProfileMode::TrackMarkedOnly && unmarked_str!=callee) ||
           mode == ProfileMode::TrackAll)
            thread_local_profile_record.gather(callee);
    }

    void START(ProfileMode mode_ = ProfileMode::TrackMarkedOnly){
        mode = mode_;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = &signal_handler;
        sa.sa_flags = SA_RESTART;
        sigaction(SIGPROF, &sa, NULL);

        it.it_value.tv_sec = 0;
        it.it_value.tv_usec = 10000;
        it.it_interval.tv_sec = 0;
        it.it_interval.tv_usec = 10000;

        setitimer(ITIMER_PROF, &it, NULL);

        start = std::chrono::system_clock::now();
    }

    void END(){
        sec = std::chrono::system_clock::now() - start;
        memset(&sa, 0, sizeof(sa));
        memset(&it, 0, sizeof(it));
    }

    void SHOW(){
        if(it.it_value.tv_usec != 0){
            std::cout<<"Profiling is not terminated, call NF::END() to terminate"<<std::endl;
            NF::END();
        }

        if(global_profile_record.empty()){
            std::cout<<"Nutrition Facts table is empty! You probably forgot starting profiling with NF::START(), or marking functions using NF_RECORD_FUNC()."<<std::endl;
            return;
        }

        // TODO : Decide width programmatically
        // TODO : Sort by percentage
        std::cout<<"+- Nutrition Facts ----+-----------+"<<std::endl;
        std::cout<<"|      Description     |   Ratio   |"<<std::endl;
        std::cout<<"+----------------------+-----------+"<<std::endl;
        double total = 0;
        for(std::pair<const char *, unsigned int> e:global_profile_record){
            total += e.second;
        }
        for(std::pair<const char *, unsigned int> e:global_profile_record){
            std::cout<<"| "<< std::setw(20) << e.first;
            std::cout<<" | " << std::setw(8) <<(e.second / total) * 100 <<"% |"<<std::endl;
        }
        std::cout<<"+----------------------+-----------+"<<std::endl;
        std::cout<<"|      Total sample : "<<std::setw(7)<<total<<"      |"<<std::endl;
        std::cout<<"|      Duration : "<<std::setw(7)<<sec.count() << " sec"<<"      |"<<std::endl;
        std::cout<<"+----------------------------------+"<<std::endl;
    }

}
