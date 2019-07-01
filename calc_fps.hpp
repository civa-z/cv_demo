#include <iostream>
#include <list>

#include "time_now.hpp"

class CalcFPS {
public:
    CalcFPS(int max_size = 10):max_size(max_size + 1){}
    
    float getfps(){
        if (timestamp_list.size() == max_size){
            timestamp_list.pop_front();
        }

        if (timestamp_list.size() < 2){
            timestamp_list.push_back(TimeNow::get_mil_time_now());
            return 0;
        }
        else {
            timestamp_list.push_back(TimeNow::get_mil_time_now());
        }
        float average = 1.0 * (timestamp_list.back() - timestamp_list.front()) / (timestamp_list.size() -1);
        return 1000 / average;
    }

private:
    const int max_size;
    std::list<long long> timestamp_list;
};


