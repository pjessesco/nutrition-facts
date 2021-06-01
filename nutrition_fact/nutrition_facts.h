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

namespace NF{
    using ProfileRecord = std::unordered_map<const char *, std::atomic<unsigned int>>;
    static ProfileRecord profile_record;
    thread_local const char *target = "etc";
    struct itimerval it;
    struct sigaction sa;

    struct ProfileFunction{
        ProfileFunction(const char *str){
            tmp = target;
            target = str;
        }
        ~ProfileFunction(){
            target = tmp;
        }
        const char *tmp;
    };

    void signal_handler(int _){
        profile_record[target]++;
    }

    void START(){
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = &signal_handler;
        sa.sa_flags = SA_RESTART;
        sigaction(SIGPROF, &sa, NULL);

        it.it_value.tv_sec = 0;
        it.it_value.tv_usec = 10000;
        it.it_interval.tv_sec = 0;
        it.it_interval.tv_usec = 10000;

        setitimer(ITIMER_PROF, &it, NULL);
    }

    void END(){
        memset(&sa, 0, sizeof(sa));
        memset(&it, 0, sizeof(it));
    }

    void SHOW(){
        if(it.it_value.tv_usec != 0){
            std::cout<<"Profiling is not terminated, call NF::END() to terminate"<<std::endl;
            NF::END();
        }

        std::cout<<"+- Nutrition Facts ---+"<<std::endl;
        double total = 0;
        for(std::pair<const char *, unsigned int> e:profile_record){
            total += e.second;
        }
        for(std::pair<const char *, unsigned int> e:profile_record){
            std::cout<<"| "<< std::setw(8) << e.first;
            std::cout<<" | " << std::setw(6) <<(e.second / total) * 100 <<"% |"<<std::endl;
        }
        std::cout<<"+---------------------+"<<std::endl;
        std::cout<<"| Total sample : "<<std::setw(4)<<total<<" | "<<std::endl;
        std::cout<<"+---------------------+"<<std::endl;
    }

#define NF_RECORD_FUNC(string) \
    NF::ProfileFunction Profile(string);

}

