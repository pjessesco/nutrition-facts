# Nutrition Facts for C++

Tiny sampling profiler for C++



### Usage

    void function(){
        // NF_RECORD_FUNC() marks function to be profiled
        NF_RECORD_FUNC("This is function()")
        ...
        ...
    }

    int main(){
        // NF::START() starts recording
        NF::START();
    
        function();

        // NF::END() stops recording
        NF::END();
    
        // NF::SHOW() shows profile result
        NF::SHOW();

        return 0;
    }

Refer `example.cpp` for actual executable example.

### Example output

    +- Nutrition Facts ----+-----------+
    |      Description     |   Ratio   |
    +----------------------+-----------+
    |     execute1() : 10% |  9.62801% |
    |     execute2() : 20% |  20.9336% |
    |     execute4() : 40% |  40.7732% |
    |     execute3() : 30% |  28.6652% |
    +----------------------+-----------+
    |      Total sample :    1371      |
    |      Duration : 7.07569 sec      |
    +----------------------------------+

