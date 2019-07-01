#pragma once

#include <iostream>
#include <list>

#include "time_now.hpp"

class StatisticTime {
public:
    StatisticTime(int max_size = 0xffff):max_size(max_size), start_time(0), sum(0){}
    
    void start(){
        start_time = TimeNow::get_mil_time_now();
        is_stop = false;
    }

    void stop(){
        long long end_time = TimeNow::get_mil_time_now();
        long long elapse = end_time - start_time;
        
        if (timestamp_list.size() == max_size){
            sum -= timestamp_list.front();
            timestamp_list.pop_front();
        }
        sum += elapse;
        timestamp_list.push_back(elapse);
        is_stop = true;
    }

    float average(){
        if (!is_stop)
            return -1;

        float average = 1.0 * sum / timestamp_list.size();
        return average;
    }

private:
    const int max_size;
    bool is_stop;
    long long start_time;
    long long sum;
    std::list<long long> timestamp_list;    
};


