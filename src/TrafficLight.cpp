#include <iostream>
#include <random>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include "TrafficLight.h"

#define printVariableNameAndValue(x) std::cout << "name**" << (#x) << "** value => " << x << "\n"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait()
    // to wait for and receive new messages and pull them from the queue using move semantics.
    // The received object should then be returned by the receive function.

    std::unique_lock<std::mutex> lck(_mtxMQ);
    _cond.wait(lck, [this] { return !_queue.empty(); });
    T lightPhase = std::move(_queue.back());
    // remove the last element from the _queue
    _queue.pop_back();
    return lightPhase;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex>
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lck(_mtxMQ);
    _queue.emplace_front(std::move(msg));
    // _queue should only contain one element, so push out the last element
    if (_queue.size() == 2)
        _queue.pop_back(); 
    _cond.notify_one();
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop
    // runs and repeatedly calls the receive function on the message queue.
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {
        if (_messageQ.receive() == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started
    // in a thread when the public method „simulate“ is called.
    // To do this, use the thread queue in the base class.
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

double getRandomWaitTimeInMs()
{
    /*
    Return a random number between 4000-6000.
    */
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    std::uniform_real_distribution<double> rand(4, 6);

    // obtain a seed from the timer
    myclock::duration d = myclock::now() - beginning;
    unsigned seed = d.count();

    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> distribution(4, 6);
    return distribution(generator) * 1000;
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the
    //  time between two loop cycles
    //  and toggles the current phase of the traffic light between red  and green
    //  and sends an update method to the message queue using move semantics.
    //  The cycle duration should be a random value between 4 and 6 seconds.
    //  Also, the while-loop should use std::this_thread::sleep_for to wait 1ms
    //  between two cycles.

    int cycleDuration = getRandomWaitTimeInMs();
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    lastUpdate = std::chrono::system_clock::now();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration)
        {
            cycleDuration = getRandomWaitTimeInMs(); // reset light duration
            switch(_currentPhase){
                case TrafficLightPhase::green:
                    cycleDuration = YELLOW_LIGHT_DURATION; // overwrite duration to static value ms
                    _currentPhase = TrafficLightPhase::yellow;
                    break;
                case TrafficLightPhase::yellow:
                    _currentPhase = TrafficLightPhase::red;
                    break;
                case TrafficLightPhase::red:
                    _currentPhase = TrafficLightPhase::green;
                    break;
            }

            // Toggle the current phase of the traffic light between red and green
            // _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
            _messageQ.send(std::move(_currentPhase));
            lastUpdate = std::chrono::system_clock::now();
            
        }
    }
}
