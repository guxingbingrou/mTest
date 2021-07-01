#include <iostream>
#include <memory>
#include "ThreadPool.hpp"

int main(){
    auto thread_pool = std::make_unique<ThreadPool>(-2);
    for(int i =0 ; i < 10 ; ++i){
        thread_pool->addTask(
            [i](){
                std::cout << "it is the " << i << "task" << std::endl;
            }
        );        
    }


}