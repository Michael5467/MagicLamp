#include "Michael_MinimalTimer.h"

M_MinimalTimer::M_MinimalTimer(uint32_t interval)
{
  _interval = interval;
  _timer = millis();
}

void M_MinimalTimer::setInterval(uint32_t interval)
{
  _interval = interval;
}

uint32_t M_MinimalTimer::getInterval()
{
  return _interval;
}

// boolean M_MinimalTimer::isReady()
// {
//   if ((long)millis() - _timer >= _interval)
//   {
//     _timer = millis();
//     return true;
//   } 
//   return false;
// }

boolean M_MinimalTimer::isReady() {
  uint32_t thisMls = millis();
  if (thisMls - _timer >= _interval) {
    do {
      _timer += _interval;
      if (_timer < _interval) 
        break;
    } while (_timer < thisMls - _interval);
    return true;
  }
  else {
    return false;
  }
}

void M_MinimalTimer::reset()
{
  _timer = millis();
}
