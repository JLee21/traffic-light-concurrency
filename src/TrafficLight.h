#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


// FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members.
// ** Can we just use std::queue? Maybe
// ** In the basic queue message app, an rvalue reference is not returned?

template <class T>
class MessageQueue
{
public:
    void send(TrafficLightPhase&& phase){}
    TrafficLightPhase&& phase receive(){}

private:
    static std::dequeue<TrafficLightPhase> _queue;
    std::condition_variable _cond;
    std::mutex _mtx;
    
};

enum TrafficLightPhase
{
    green,
    red,
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight(){
        _currentPhase = TrafficLightPhase::red;
    }
    ~TrafficLight();

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.

    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif