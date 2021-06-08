# Nutrition Facts for C++

Tiny header-only sampling profiler for C++

    + Nutrition Facts ----------------------------------------+--------+
    | Description                                             | Ratio  |
    +---------------------------------------------------------+--------+
    | 40% execute4()                                          | 39.59% |
    | execute2() : 30%                                        | 30.35% |
    | This function(execute2()) is called in 20% probability. | 19.24% |
    | execute1() 10% ->                                       | 10.82% |
    +---------------------------------------------------------+--------+
    | Total sample : 4039                                              |
    | Duration : 10.068790s                                            |
    +------------------------------------------------------------------+

### Integration
    
`nutrition-facts` is header-only, just copy and paste `nutrition_facts.h`. Note that it requires at least C++11 and it does not supports Windows currently. 
    

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



