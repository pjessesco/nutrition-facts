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

#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include "nutrition_fact/nutrition_facts.h"

inline void COMPLEX_CODE_EXAMPLE(float x){
    float t = x;
    for(int i=0;i<100000000;i++){
        t = sin(t);
    }
}

void execute(float x){
    // Use NF_RECORD_FUNC("describe") to profile
    NF_RECORD_FUNC("execute")
    COMPLEX_CODE_EXAMPLE(x);
}


void base(){
    // Use NF_RECORD_FUNC("describe") to profile
    NF_RECORD_FUNC("base")

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0,1);

    float rand_flt = dis(gen);

    execute(rand_flt);

    COMPLEX_CODE_EXAMPLE(rand_flt);
}

int main(){
    // NF::START() starts recording
    NF::START();
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    std::thread t1(base);
    std::thread t2(base);
    std::thread t3(base);
    std::thread t4(base);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
    std::cout << "duration : " << sec.count() << " seconds" << std::endl;

    // NF::END() stops recording
    NF::END();
    // NF::END() shows profile result
    NF::SHOW();
    return 0;
}