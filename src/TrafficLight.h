#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <thread>
#include <memory>
#include <ctime>
#include <random>
#include <iostream>
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
    MessageQueue() {
        std::deque<T> _queue(1);
        std::cout << "Max size = " << _queue.max_size() << std::endl;
        std::cout << "size = " << _queue.size() << std::endl;
    }
    void send(T &&phase);
    T receive();
    int getSize() { return _queue.size(); }

private:
    // limit queue size to only one (the most recent) using fill constructor http://www.cplusplus.com/reference/deque/deque/deque/
    std::deque<T> _queue;
    std::condition_variable _cond;
    std::mutex _mtxMQ;
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject.
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“.
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value.

enum TrafficLightPhase
{
    green,
    red,
};

class TrafficLight : public TrafficObject
{
public:
    TrafficLight()
    {
        _currentPhase = TrafficLightPhase::red;
    }

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();
    void printTLQueueSize() { 
        std::unique_lock<std::mutex> lck(_mtx);
        std::cout << "Traffic Light #" << this->getID() << " queue size = " << _messageQ.getSize() << std::endl;
        lck.unlock();
    }

private:
    // typical behaviour methods
    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;
    long _id;

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling
    // send in conjunction with move semantics.
    MessageQueue<TrafficLightPhase> _messageQ;

    // NOTE: we should not create a share pointer b/c each TrafficLight object
    // has its own MessageQueue. IE there is no master queue for all TrafficLights.
    // std::shared_ptr<MessageQueue<TrafficLightPhase>> _messageQ;

    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif