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
    void send(T&& phase);
    T receive();

private:
    std::deque<T> _queue;
    std::condition_variable _cond;
    std::mutex _mtx;
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
    TrafficLight(){
        std::uniform_int_distribution<double> rand(0, 100);
        std::mt19937 gen;
        
        _currentPhase = TrafficLightPhase::red;
        // std::time_t result = std::time(nullptr);
        // _id = (long)result;

        _id = rand(gen);
        std::cout << "Create Traffic Light " << _id << std::endl;
    }

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;
    long _id;

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.
    std::shared_ptr<MessageQueue<TrafficLightPhase> > _messageQ;

    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif