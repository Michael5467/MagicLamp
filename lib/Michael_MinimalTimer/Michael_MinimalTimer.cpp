#include "Michael_MinimalTimer.h"

M_MinimalTimer::M_MinimalTimer(uint32_t interval) {
    _interval = interval;
    _timer = millis();
}

void M_MinimalTimer::setInterval(uint32_t interval) {
    _interval = interval;
}

uint32_t M_MinimalTimer::getInterval() {
    return _interval;
}

uint32_t M_MinimalTimer::getMillis() {
    return _millis;
}

// boolean M_MinimalTimer::isReady() {
//   if ((long)millis() - _timer >= _interval) {
//     _timer = millis();
//     return true;
//   } 
//   return false;
// }

// boolean M_MinimalTimer::isReady() {
//   uint32_t thisMls = millis();
//   if (thisMls - _timer >= _interval) {
//     do {
//       _timer += _interval;
//       if (_timer < _interval) 
//         break;
//     } while (_timer < thisMls - _interval);
//     return true;
//   }
//   return false;
// }

boolean M_MinimalTimer::isReady() {
	_millis = millis();
    uint32_t delta = 0;
    if (_millis < _timer) {
        delta = 0xFFFFFFFF - _timer + _millis + 1;
    }
    else {
        delta = _millis - _timer;
    }
    if (delta >= _interval) {
        _timer += _interval;
        return true;
    }
    return false;
}

void M_MinimalTimer::reset() {
    _timer = millis();
}