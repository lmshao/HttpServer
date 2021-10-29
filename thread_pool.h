//
// Created by lmshao on 2021/10/27.
//

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <pthread.h>

#include <mutex>
#include <queue>

class ThreadPool {
  public:
    typedef void (*function)(void *);

    struct ThreadTask {
        function task;  // function ptr
        void *arg;      // params
        ThreadTask(function task, void *arg) : task(task), arg(arg) {}

        ThreadTask() : task(nullptr), arg(nullptr){};
    };

    explicit ThreadPool(int threadsLimit = 100);

    ~ThreadPool();

    friend void *workerThread(void *arg);

    void addTask(function task, void *arg);

    static ThreadPool *getInstance();

    static void deleteInstance();

  private:
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    pthread_attr_t _attr;

    std::queue<ThreadTask> _tasks;
    int _count;  // 当前工作线程个数
    int _idle;   // 当前空闲的线程个数
    int _threadsLimit;
    int _quit;  //

    bool _valid;

    static ThreadPool *instance_;
    static std::mutex mutex_;
};

#endif  //_THREAD_POOL_H
