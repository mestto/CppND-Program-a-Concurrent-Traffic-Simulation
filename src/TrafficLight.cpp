#include "TrafficLight.h"
#include <iostream>
#include <random>
#include <stdlib.h>

/* Implementation of class "MessageQueue" */
template <typename T> T MessageQueue<T>::receive() {

  std::unique_lock<std::mutex> lck(_mtx);
  _cond.wait(lck);

  T msg = std::move(_queue.back());
  _queue.pop_back();

  return msg;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  std::lock_guard<std::mutex> lck(_mtx); // lock
  _queue.emplace_back(msg);              // send msg
  _cond.notify_one(); // notify thread (equals sending the notification)
}

/* Implementation of class "TrafficLight" */
TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::RED; }

void TrafficLight::waitForGreen() {

  while (true) {
    _messageQueue.receive();
    if (_currentPhase == TrafficLightPhase::GREEN)
      return;
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  TrafficObject::threads.emplace_back(
      std::move(std::thread(&TrafficLight::cycleThroughPhases, this)));
}

int TrafficLight::randomGenerator() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> d(4000, 6000);

  return d(gen);
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {

  auto initTime = std::chrono::steady_clock::now();
  int duration = randomGenerator();
  while (true) {

    auto now = std::chrono::steady_clock::now();
    int elapsedTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - initTime)
            .count();

    if (elapsedTime >= duration) {
      if (_currentPhase == TrafficLightPhase::RED) {
        _currentPhase = TrafficLightPhase::GREEN;
        _messageQueue.send(std::move(_currentPhase));
      } else {
        _currentPhase = TrafficLightPhase::RED;
        _messageQueue.send(std::move(_currentPhase));
      }

      initTime = std::chrono::steady_clock::now();
      duration = randomGenerator();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
