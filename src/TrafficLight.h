#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>
#include <unistd.h>

// forward declarations to avoid include cycle
class Vehicle;
enum class TrafficLightPhase;

template <class T> class MessageQueue {
public:
  MessageQueue() {}
  void send(T &&msg);
  T receive();

private:
  std::condition_variable _cond;
  std::mutex _mtx;
  std::deque<T> _queue;
};

enum class TrafficLightPhase { RED, GREEN };

class TrafficLight : public TrafficObject {
public:
  // constructor / desctructor
  TrafficLight();
  ~TrafficLight() {}

  // getters / setters
  void waitForGreen();
  void simulate();

  // typical behaviour methods
  TrafficLightPhase getCurrentPhase();
  int randomGenerator();

private:
  // typical behaviour methods
  void cycleThroughPhases();

  std::condition_variable _condition;
  std::mutex _mutex;
  TrafficLightPhase _currentPhase;

  MessageQueue<TrafficLightPhase> _messageQueue;
};

#endif