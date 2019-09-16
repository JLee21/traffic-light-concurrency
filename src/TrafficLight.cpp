#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */
template <typename T>
T MessageQueue<T>::receive()
{
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
    std::lock_guard<std::mutex> lck(_mtxMQ);
    _queue.emplace_front(std::move(msg));
    // _queue should only contain one element, so push out the last element
    if (_queue.size() == 2)
        _queue.pop_back();
    _cond.notify_one();
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        if (_messageQ.receive() == TrafficLightPhase::green)
            return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

int getRandomWaitTime()
{
    /*
    Return a random number between and including 4-6
    http://www.cplusplus.com/reference/random/uniform_int_distribution/
    */
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();

    // obtain a seed from the timer
    myclock::duration d = myclock::now() - beginning;
    unsigned seed = d.count();

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(4, 6);
    return distribution(generator);
}

void TrafficLight::cycleThroughPhases()
{
    int cycleDuration = getRandomWaitTime();
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    lastUpdate = std::chrono::system_clock::now();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration)
        {
            cycleDuration = getRandomWaitTime(); // reset light duration
            switch (_currentPhase)
            {
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

            _messageQ.send(std::move(_currentPhase));
            lastUpdate = std::chrono::system_clock::now();
        }
    }
}
