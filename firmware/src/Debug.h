#ifndef __Debug_H
#define __Debug_H

#include <string>

class Debug 
{
  public:
  Debug();

  template<typename... Args>
  static void printf(const std::string &msg, Args... args);
};

Debug::Debug(){}

template<typename... Args>
void Debug::printf(const std::string &msg, Args... args)
{
  char buffer [2 * msg.length()];
  sprintf(buffer, msg.c_str(), args...);
  Serial.print(buffer);
}

#endif