#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include "TrafficLight.h"

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
    threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        TrafficLightPhase phase = _queue.receive();
        if (phase == TrafficLightPhase::Green)
            return;
    }
}