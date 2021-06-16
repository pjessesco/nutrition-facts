# Nutrition Facts for C++

Tiny header-only sampling profiler for C++

    + Nutrition Facts ------------------------------+--------+
    | Description                                   | Ratio  |
    +-----------------------------------------------+--------+
    | 40% execute4()                                | 41.3%  |
    | execute3() : 30%                              | 29.31% |
    | This function is called with 20% probability. | 18.77% |
    | execute1() 10% ->                             | 10.62% |
    +-----------------------------------------------+--------+
    | Total sample : 2269                                    |
    | Duration : 12.279807s                                  |
    +--------------------------------------------------------+

### Features

- Header-only inline sampling profiler
- Supports Windows, macOS and Linux(tested on Ubuntu)
- Supports multi-thread profiling for Linux and macOS
- Provides an option to profile a non-marked function.

### Integration
    
`nutrition-facts` is header-only, just copy `nutrition_facts` directory to your include path.

To import using CMake, add
    
    include_directories(ext/nutrition-facts)

in your `CMakeLists.txt`. You will be able to include as

    #include <nutrition_facts/nutrition_facts.h>


### Usage

    void function(){
        // NF_MARK_FUNC() marks function to be profiled
        NF_MARK_FUNC("This is function()")
        ...
        ...
    }

    int main(){
        // NF::Profiler::Start(); starts recording
        NF::Profiler::Start();
    
        function();

        // NF::Profiler::End() shows profile result
        NF::Profiler::End();
    
        // NF::Profiler::Show() shows profile result
        NF::Profiler::Show();

        return 0;
    }

Refer `example.cpp` for actual executable example.



