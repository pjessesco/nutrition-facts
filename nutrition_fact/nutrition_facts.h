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
#include <sstream>

namespace NF{

#define NF_RECORD_FUNC(string) \
    NF::ProfileFunction NF_MARKER_PROFILED_FUNCTION(string);

    using GlobalProfileRecord = std::unordered_map<const char *, std::atomic<unsigned int>>;
    static GlobalProfileRecord global_profile_record;

    using ProfileRecordType = std::pair<const char *, unsigned int>;

    bool compare_ratio(const ProfileRecordType &a, const ProfileRecordType &b){
        return a.second > b.second;
    }

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

    const std::string title = " Nutrition Facts ";
    const int title_len = title.length();
    const std::string col1_title = "Description";
    const std::string col2_title = "Ratio";

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

    std::string one_column_writer(int width, const std::string &str,
                                  const std::string &blank=" ", const std::string &border="|"){
        std::string substr(width, blank[0]);

        std::copy(str.begin(), str.end(), substr.begin());
        return border + blank + substr + blank + border;
    }

    std::string two_column_writer(int width1, int width2,
                                  const std::string &str1, const std::string &str2,
                                  const std::string &blank=" ", const std::string &border="|"){
        std::string substr1(width1, blank[0]);
        std::string substr2(width2, blank[0]);

        std::copy(str1.begin(), str1.end(), substr1.begin());
        std::copy(str2.begin(), str2.end(), substr2.begin());
        return border + blank + substr1 + blank + border + blank +substr2 + blank + border;
    }


    // TODO : This function can be simplified using <format>, which is not implemented yet.
    void SHOW(){
        if(it.it_value.tv_usec != 0){
            std::cout<<"Profiling is not terminated, call NF::END() to terminate"<<std::endl;
            NF::END();
        }

        if(global_profile_record.empty()){
            std::cout<<"Nutrition Facts table is empty! You probably forgot starting profiling with NF::START(), or marking functions using NF_RECORD_FUNC()."<<std::endl;
            return;
        }

        // Sort record by sample
        std::vector<ProfileRecordType> sorted_global_record(global_profile_record.begin(), global_profile_record.end());
        std::sort(sorted_global_record.begin(), sorted_global_record.end(), compare_ratio);

        int longest_description_len = -1;
        int total_sample = 0;
        for(ProfileRecordType e : sorted_global_record){
            longest_description_len = std::max<int>(strlen(e.first), longest_description_len);
            total_sample += e.second;
        }

        int width1 = std::max<int>(longest_description_len, strlen("Description"));
        int width2 = 6;

        std::cout<<"+"<<title;
        if(width1 < title_len){
            std::cout<< std::string(width1 + width2 + 5 - title_len, '-')<<"+"<<std::endl;
        }
        else{
            std::cout<<std::string(width1 + 2 - title_len, '-')<<"+"<<std::string(width2 + 2, '-')<<"+"<<std::endl;
        }

        std::cout<<two_column_writer(width1, width2, "Description", "Ratio")<<std::endl;
        std::cout<<two_column_writer(width1, width2, "", "", "-", "+")<<std::endl;
        for(ProfileRecordType e : sorted_global_record){
            std::stringstream ss;
            ss << std::setprecision(4)<<(e.second / (float)total_sample * 100);

            std::cout<<two_column_writer(width1, width2, e.first, ss.str()+"%")<<std::endl;
        }
        std::cout<<two_column_writer(width1, width2, "", "", "-", "+")<<std::endl;
        std::cout<<one_column_writer(width1 + width2 + 3, "Total sample : "+std::to_string(total_sample))<<std::endl;

        std::cout<<one_column_writer(width1 + width2 + 3, "Duration : "+std::to_string(sec.count())+"s")<<std::endl;
        std::cout<<one_column_writer(width1 + width2 + 3, "", "-", "+")<<std::endl;
    }
}
