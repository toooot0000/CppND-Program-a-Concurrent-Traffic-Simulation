#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

/* 
template <typename T>
T MessageQueue<T>::receive()
{
    FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    to wait for and receive new messages and pull them from the queue using move semantics. 
    The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
}
*/

/* Implementation of class "TrafficLight" */

/* 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    runs and repeatedly calls the receive function on the message queue. 
    Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
}

*/

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this]()
               { return !_queue.empty(); });
    T ret = _queue.front();
    _queue.pop_front();
    return ret;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::scoped_lock<std::mutex> uLock(_mutex);
    _queue.emplace_back(std::move(msg));
    _cond.notify_one();
}

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::Red;
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    std::scoped_lock<std::mutex> sLock(_mutex);
    return _currentPhase;
}

void TrafficLight::toggleTrafficLight()
{
    std::scoped_lock<std::mutex> sLock(_mutex);
    if (_currentPhase == TrafficLightPhase::Green)
        _currentPhase = TrafficLightPhase::Red;
    else
        _currentPhase = TrafficLightPhase::Green;
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles
    // and toggles the current phase of the traffic light between red and green and sends an update method
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds.
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
    using namespace std::chrono;
    time_point<system_clock> lastUpdate = system_clock().now();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(4000, 6000);

    milliseconds dur(dis(gen));
    while (true)
    {
        std::this_thread::sleep_for(microseconds(1));
        time_point<system_clock> now = system_clock().now();
        if (lastUpdate + dur <= now)
        {
            lastUpdate = now;
            dur = milliseconds(dis(gen));
            toggleTrafficLight();
            _queue.send(std::move(_currentPhase));
        }
    }
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class.
    threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop
    // runs and repeatedly calls the receive function on the message queue.
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        TrafficLightPhase phase = _queue.receive();
        if (phase == TrafficLightPhase::Green)
            return;
    }
}