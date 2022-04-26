#include <mutex>
#include <condition_variable>

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

class Semaphore {
    private:
        std::mutex mtx;
        std::condition_variable cv;
        int count;
    public:
        Semaphore (int count_ = 0) : count(count_) {}
        inline void notify()
        {
            std::unique_lock<std::mutex> lock(mtx);
            count++;
            cv.notify_one();
        }

        inline void wait()
        {
            std::unique_lock<std::mutex> lock(mtx);
            while(count == 0){
                cv.wait(lock);
            }
            count--;
        }
};

#endif