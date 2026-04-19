#ifndef RTC_UTILS_PORT_H
#define RTC_UTILS_PORT_H

#include <stdint.h>

class IRtcUtils {
public:
    virtual ~IRtcUtils() = default;

    virtual uint32_t epochNow() = 0;
    virtual uint8_t  getHours() = 0;
    virtual uint8_t  getMinutes() = 0;
    virtual uint8_t  getSeconds() = 0;
    virtual void     setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) = 0;
    virtual void     setDateTime(int hours, int minutes, int seconds,
                                 int year, int month, int day, int weekDay = 0) = 0;
};

#endif
