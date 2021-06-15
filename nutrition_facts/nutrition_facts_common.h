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

#include <algorithm>
#include <tuple>
#include <string>
#include <unordered_map>
#include <atomic>
#include <iomanip>
#include <vector>
#include <chrono>
#include <sstream>
#include <iostream>

#include <cstring>

namespace NF{
    // `GlobalProfileRecord` is accumulated data from every thread.
    // It is updated when thread is terminated, or Profiler::End() is called.
    using GlobalProfileRecord = std::unordered_map<const char *, std::atomic<unsigned int>>;
    static GlobalProfileRecord global_profile_record;

    // Pair of function description and its sample count
    using ThreadProfileRecordType = std::pair<const char *, unsigned int>;

    // `TrackAll` : Trace every function, unmarked functions will be printed as "Unmarked".
    // `TrackMarkedOnly` : Trace only marked function.
    enum class ProfileMode{
        TrackAll,
        TrackMarkedOnly
    };

    // By default, profiling mode is set to `TrackMarkedOnly`.
    static ProfileMode mode = ProfileMode::TrackMarkedOnly;

    // Struct used to store function samples.
    // It is intended to be used per thread (Note that the count type is NOT atomic compared to `GlobalProfileRecord`).
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


    // Strings
    constexpr char *unmarked_str = "Unmarked";
    // Function is treated as "Unmarked" without NF_MARK_FUNC().

#if defined(WIN32)
    const char *callee = unmarked_str;
    ProfileRecordPerThread thread_local_profile_record;
#else
    thread_local const char *callee = unmarked_str;
    thread_local ProfileRecordPerThread thread_local_profile_record;
#endif

    const std::string title(" Nutrition Facts ");
    const int title_len = title.length();
    const std::string col1_title("Description");
    const std::string col2_title("Ratio");

    // Timer for estimating duration between `Start()` and `End()`
    static std::chrono::system_clock::time_point start;
    static std::chrono::duration<double> duration;

    // `ProfileScope` represents function executed at the time.
    struct ProfileScope{
        ProfileScope(const char *new_callee){
            m_caller = callee;
            callee = new_callee;
        }
        ~ProfileScope(){
            callee = m_caller;
        }
        const char *m_caller;
    };

    // Macro function used to mark function for profiling.
    #define NF_MARK_FUNC(string) \
        NF::ProfileScope NF_MARKER_PROFILED_FUNCTION(string);

    class Profiler{
    public:
        // This function starts to call `record_counter()` per period.
        // This function must :
        //    1. Activate calling `Profiler::record_counter()` per period.
        //    2. Initialize `NF::start` variable (i.e start = std::chrono::system_clock::now();)
        // Its implementation is OS-dependent.
        static void Start(ProfileMode mode_ = ProfileMode::TrackMarkedOnly);

        // This function ends timer initialized in `Start()`.
        // This function must :
        //     1. Accumulate main thread's `NF::thread_local_profile_record` data to `NF::global_profile_record`.
        //     2. Store duration between `NF::start` and  until now`NF::duration`
        //     3. Stops calling `Profiler::record_counter()`
        // Its implementation is OS-dependent.
        static void End();

        // This function prints profiling data, considering length of descriptions.
        // TODO : This function can be simplified using <format>, which is not implemented yet.
        static void Show(){
            if(global_profile_record.empty()){
                std::cout<<"Nutrition Facts table is empty! You probably forgot starting profiling with NF::START(), or marking functions using NF_RECORD_FUNC()."<<std::endl;
                return;
            }

            // Sort record by sample
            std::vector<ThreadProfileRecordType> sorted_global_record(global_profile_record.begin(), global_profile_record.end());
            std::sort(sorted_global_record.begin(), sorted_global_record.end(), compare_ratio);

            int longest_description_len = -1;
            int total_sample = 0;
            for(ThreadProfileRecordType e : sorted_global_record){
                longest_description_len = std::max<int>(strlen(e.first), longest_description_len);
                total_sample += e.second;
            }

            int width1 = std::max<int>(longest_description_len, col1_title.length());
            int width2 = 6;

            std::cout<<"+"<<title;
            if(width1 < title_len){
                std::cout<<std::string(width1 + width2 + 5 - title_len, '-')<<"+"<<std::endl;
            }
            else{
                std::cout<<std::string(width1 + 2 - title_len, '-')<<"+"<<std::string(width2 + 2, '-')<<"+"<<std::endl;
            }

            std::cout<<two_column_writer(width1, width2, col1_title, col2_title)<<std::endl;
            std::cout<<two_column_writer(width1, width2, "", "", "-", "+")<<std::endl;
            for(ThreadProfileRecordType e : sorted_global_record){
                std::stringstream ss;
                ss << std::setprecision(4)<<(e.second / (float)total_sample * 100);

                std::cout<<two_column_writer(width1, width2, e.first, ss.str()+"%")<<std::endl;
            }
            std::cout<<two_column_writer(width1, width2, "", "", "-", "+")<<std::endl;
            std::cout<<one_column_writer(width1 + width2 + 3, "Total sample : "+std::to_string(total_sample))<<std::endl;

            std::cout << one_column_writer(width1 + width2 + 3, "Duration : " + std::to_string(duration.count()) + "s") << std::endl;
            std::cout<<one_column_writer(width1 + width2 + 3, "", "-", "+")<<std::endl;
        }
    private:

        // This function defined in each OS-impl header files.
        // It accumulates a sample to thread local data.
        // Intended to be called per period by `Start()`

        // void record_counter()

        // Helper function for printing profiling result
        static std::string one_column_writer(int width, const std::string &str,
                                             const std::string &blank=" ",
                                             const std::string &border="|"){
            std::string substr(width, blank[0]);

            std::copy(str.begin(), str.end(), substr.begin());
            return border + blank + substr + blank + border;
        }

        // Helper function for printing profiling result
        static std::string two_column_writer(int width1, int width2,
                                             const std::string &str1,
                                             const std::string &str2,
                                             const std::string &blank=" ",
                                             const std::string &border="|"){
            std::string substr1(width1, blank[0]);
            std::string substr2(width2, blank[0]);

            std::copy(str1.begin(), str1.end(), substr1.begin());
            std::copy(str2.begin(), str2.end(), substr2.begin());
            return border + blank + substr1 + blank + border + blank +substr2 + blank + border;
        }

        // Used to sort profiling results
        static bool compare_ratio(const ThreadProfileRecordType &a, const ThreadProfileRecordType &b){
            return a.second > b.second;
        }
    };
}
