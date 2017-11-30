
#include "eventqueue.h"


event_queue& event_queue::getInstance(){ //thread safe?
  static event_queue instance;//initialized once and guaranteed to be destroyed
  return instance;
        }

void event_queue::enqueue(std::shared_ptr<event> const & e){
  std::lock_guard<std::mutex> lg(queue_mutex);
  events.push(e);
        }

std::shared_ptr<event> event_queue::dequeue(){
  std::lock_guard<std::mutex> lg(queue_mutex);
  if(!events.empty()){
    std::shared_ptr<event> e = events.front();
    events.pop();
    return e;
  }else{
    return nullptr;
  }
}


