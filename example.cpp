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

inline float COMPLEX_CODE_EXAMPLE(float x){
    float t = x;
    for(int i=0;i<10;i++){
        t = sin(t);
    }
    return t;
}

float execute1(float x){
    // Use NF_RECORD_FUNC("describe") to profile
    NF_RECORD_FUNC("execute1 execute 1")
    return COMPLEX_CODE_EXAMPLE(x);
}

float execute2(float x){
    // Use NF_RECORD_FUNC("describe") to profile
    NF_RECORD_FUNC("execute2()")
    return COMPLEX_CODE_EXAMPLE(x);
}

float execute3(float x){
    // Use NF_RECORD_FUNC("describe") to profile
    NF_RECORD_FUNC("Function 3, target ratio : 30%")
    return COMPLEX_CODE_EXAMPLE(x);
}

float execute4(float x){
    // Use NF_RECORD_FUNC("describe") to profile
    NF_RECORD_FUNC("f4()")
    return COMPLEX_CODE_EXAMPLE(x);
}

// This function will call `executeN()` functions with given probability
void base(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1,10);

    for(int i=0;i<20000000;i++){
        int rand_flt = dis(gen);
        switch(rand_flt){
            case 1:
                execute1((float)i/rand_flt);
                break;
            case 2: case 3:
                execute2((float)i/rand_flt);
                break;
            case 4: case 5: case 6:
                execute3((float)i/rand_flt);
                break;
            case 7: case 8: case 9: case 10:
                execute4((float)i/rand_flt);
                break;
        }
    }

}

int main(){
    /*
     * NF::START() starts recording.
     * `TrackAll` mode traces every function as `Unmarked` function.
     * `TrackMarkedOnly` mode traces function marked using `NF_RECORD_FUNC()`.
     * Default mode is `TrackMarkedOnly`.
     *
     * NF::START(NF::ProfileMode::TrackAll);
     * NF::START(NF::ProfileMode::TrackMarkedOnly);
     */

    NF::START();

    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    std::thread th1(base);
    std::thread th2(base);
    std::thread th3(base);
    std::thread th4(base);
    std::thread th5(base);
    std::thread th6(base);
    std::thread th7(base);
    std::thread th8(base);
    std::thread th9(base);
    std::thread th10(base);
    std::thread th11(base);
    std::thread th12(base);
    std::thread th13(base);
    std::thread th14(base);
    std::thread th15(base);

    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
    th6.join();
    th7.join();
    th8.join();
    th9.join();
    th10.join();
    th11.join();
    th12.join();
    th13.join();
    th14.join();
    th15.join();

    // NF::END() stops recording
    NF::END();

    // NF::SHOW() shows profile result
    NF::SHOW();
    return 0;
}