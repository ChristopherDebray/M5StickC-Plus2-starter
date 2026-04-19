#ifndef RTC_UTILS_DEPS_H
#define RTC_UTILS_DEPS_H

#include "../ports/rtc_utils_port.h"
#include "../adapters/rtc_utils_m5stick_adapter.h"

inline IRtcUtils* getM5StickRtcUtils() {
    return new RtcUtilsM5StickAdapter();
}

#endif
