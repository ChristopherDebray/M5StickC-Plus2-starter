#ifndef RTC_UTILS_M5STICK_ADAPTER_H
#define RTC_UTILS_M5STICK_ADAPTER_H

#include <M5Unified.h>
#include <Arduino.h>
#include "../ports/rtc_utils_port.h"

class RtcUtilsM5StickAdapter : public IRtcUtils {
public:
    uint32_t epochNow() override {
        m5::rtc_datetime_t dt;
        M5.Rtc.getDateTime(&dt);
        struct tm tmv = {0};
        tmv.tm_year = dt.date.year - 1900;
        tmv.tm_mon  = dt.date.month - 1;
        tmv.tm_mday = dt.date.date;
        tmv.tm_hour = dt.time.hours;
        tmv.tm_min  = dt.time.minutes;
        tmv.tm_sec  = dt.time.seconds;
        return (uint32_t)mktime(&tmv);
    }

    uint8_t getHours() override {
        m5::rtc_datetime_t dt;
        M5.Rtc.getDateTime(&dt);
        return dt.time.hours;
    }

    uint8_t getMinutes() override {
        m5::rtc_datetime_t dt;
        M5.Rtc.getDateTime(&dt);
        return dt.time.minutes;
    }

    uint8_t getSeconds() override {
        m5::rtc_datetime_t dt;
        M5.Rtc.getDateTime(&dt);
        return dt.time.seconds;
    }

    void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) override {
        m5::rtc_time_t time;
        time.hours   = hours;
        time.minutes = minutes;
        time.seconds = seconds;
        M5.Rtc.setTime(&time);
    }

    void setDateTime(int hours, int minutes, int seconds,
                     int year, int month, int day, int weekDay = 0) override {
        m5::rtc_time_t time;
        m5::rtc_date_t date;
        time.hours   = hours;
        time.minutes = minutes;
        time.seconds = seconds;
        date.year    = year;
        date.month   = month;
        date.date    = day;
        date.weekDay = weekDay;
        M5.Rtc.setTime(&time);
        M5.Rtc.setDate(&date);
    }
};

#endif
