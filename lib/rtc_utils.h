#ifndef RTC_UTILS_H
#define RTC_UTILS_H

#include <M5Unified.h>
#include <Arduino.h>

// Get current epoch time from hardware RTC
uint32_t rtcEpochNow() {
  m5::rtc_datetime_t dt;
  M5.Rtc.getDateTime(&dt);
  
  struct tm tmv = {0};
  tmv.tm_year = dt.date.year - 1900;
  tmv.tm_mon  = dt.date.month - 1;
  tmv.tm_mday = dt.date.date;
  tmv.tm_hour = dt.time.hours;
  tmv.tm_min  = dt.time.minutes;
  tmv.tm_sec  = dt.time.seconds;
  
  time_t tt = mktime(&tmv);
  return (uint32_t)tt;
}

// Set RTC time
void setRTCTime(int hours, int minutes, int seconds, 
                int year, int month, int day, int weekDay = 0) {
  m5::rtc_time_t time;
  m5::rtc_date_t date;
  
  time.hours = hours;
  time.minutes = minutes;
  time.seconds = seconds;
  
  date.year = year;
  date.month = month;
  date.date = day;
  date.weekDay = weekDay;
  
  M5.Rtc.setTime(&time);
  M5.Rtc.setDate(&date);
}

#endif