#ifndef _THREAD_POOL_H__
#define _THREAD_POOL_H__
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <stdexcept>
using TASK = std::function<void()>;
class ThreadPool
{
private:
    /* data */
    std::queue<TASK> m_tasks;
    std::vector<std::thread> m_threads;
    std::mutex m_mutex;
    std::condition_variable m_condition;

    bool m_is_stop = false;


public:

    explicit ThreadPool(int size);
    ~ThreadPool();

    void addTask(const TASK &task);
};

ThreadPool::ThreadPool(int size)
{
    if(size <= 0){
        m_is_stop = true;
        throw std::runtime_error("thread size is less than zero");
    }
    for(int i=0 ; i<size; ++i){
        m_threads.emplace_back([this](){
            while(true){
                TASK task;
                {
                    std::unique_lock<std::mutex> lck(this->m_mutex);
                    this->m_condition.wait(lck, 
                        [this]{
                            return this->m_is_stop || !this->m_tasks.empty();
                        }
                    );

                    if(this->m_is_stop && this->m_tasks.empty()){
                        return;
                    }

                    // auto t = this->m_tasks.front();
                    task = std::move(this->m_tasks.front());
                    this->m_tasks.pop();                    
                }

                task();

            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lck(m_mutex);
        m_is_stop = true;
    }

    m_condition.notify_all();
    for(auto &t : m_threads){
        if(t.joinable())
            t.join();
    }

}

void ThreadPool::addTask(const TASK& task){
    {
        std::unique_lock<std::mutex> lck(m_mutex);
        m_tasks.push(std::move(task));
    }

    m_condition.notify_one();
    
}


#endif