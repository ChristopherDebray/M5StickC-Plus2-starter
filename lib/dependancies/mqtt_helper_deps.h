#ifndef MQTT_HELPER_DEPS_H
#define MQTT_HELPER_DEPS_H

#include "../ports/mqtt_helper_port.h"
#include "../adapters/mqtt_helper_m5stick_adapter.h"

inline IMQTTHelper* getM5StickMQTTHelper() {
    return new MQTTHelperM5StickAdapter();
}

#endif
