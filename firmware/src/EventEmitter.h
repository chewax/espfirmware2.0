/*
  EventEmitter.h - Static Library for handling global events
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/

#ifndef __EventEmitter_h
#define __EventEmitter_h

#include <Arduino.h>
#include <string>
#include <map>
#include <vector>

typedef std::function<void()> EventListener;

class EventEmitter
{
  public:
    EventEmitter();
    static void on(const std::string& event, const EventListener& listener);
    static void emit(const std::string& event);
    static std::map<std::string, std::vector<EventListener>> suscribers;
};

std::map<std::string, std::vector<EventListener>> EventEmitter::suscribers = {};

void EventEmitter::on(const std::string& event, const EventListener& listener)
{ 
  EventEmitter::suscribers[event].push_back(listener);
}

//Tiggers all listner for an event
void EventEmitter::emit(const std::string& event)
{
  Debug::printf("[EventEmitter] %s\n",event.c_str());

  auto s_it = EventEmitter::suscribers.find(event);

  if (s_it != EventEmitter::suscribers.end()) {
    // std::vector<EventListener> handlers = s_it->second;
    for (const auto &h_it:s_it->second) h_it();
  }
}

#endif