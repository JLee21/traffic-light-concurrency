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

#define YELLOW_LIGHT_DURATION 2

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    void send(T &&phase);
    T receive();
    int getSize() { return _queue.size(); }

private:
    std::deque<T> _queue;
    std::condition_variable _cond;
    std::mutex _mtxMQ;
};

enum TrafficLightPhase
{
    green,
    yellow,
    red,
};

class TrafficLight : public TrafficObject
{
public:
    TrafficLight()
    {
        _type = ObjectType::objectTrafficLight;
        _currentPhase = TrafficLightPhase::red;
    }

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;
    // NOTE: we should not create a share pointer b/c each TrafficLight object
    // has its own MessageQueue. IE there is no master queue for all TrafficLights.
    MessageQueue<TrafficLightPhase> _messageQ;

    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif