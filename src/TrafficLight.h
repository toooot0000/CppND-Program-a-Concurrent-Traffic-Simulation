#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    MessageQueue() {}
    void send(T &&item);
    T receive();

private:
    std::deque<T> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject.
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“.
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value.

enum TrafficLightPhase
{
    Green = 0,
    Red = 1
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight(){};

    // getters / setters

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods

    void toggleTrafficLight();

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling
    // send in conjunction with move semantics.
    MessageQueue<TrafficLightPhase> _queue;

    std::condition_variable _condition;
    std::mutex _mutex;

    TrafficLightPhase _currentPhase;

    void cycleThroughPhases();
};

#endif