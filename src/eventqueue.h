#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H


#include "event.h"
#include <mutex>
#include <queue>
//singleton wrapper for fifo std::queue
class event_queue{

    public:
        std::queue<std::shared_ptr<event>> events;
        std::mutex queue_mutex;

        static event_queue& getInstance();


        void enqueue(std::shared_ptr<event> const & e);

        std::shared_ptr<event> dequeue();

        event_queue(event_queue const &) = delete;//prevents these functions from being called
        void operator = (event_queue const &) = delete; 
        event_queue(event_queue &&) = delete;
        void operator = (event_queue &&) = delete;

    private:
        event_queue(){ }
};


#endif
