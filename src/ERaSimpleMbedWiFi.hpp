#ifndef INC_ERA_SIMPLE_MBED_WIFI_HPP_
#define INC_ERA_SIMPLE_MBED_WIFI_HPP_

#include <ERa/ERaDetect.hpp>
#if !defined(TINY_GSM_RX_BUFFER)
    #define TINY_GSM_RX_BUFFER ERA_MQTT_BUFFER_SIZE
#endif

#include <rtos.h>
#include <ERa/ERaApiMbedDef.hpp>
#include <Adapters/ERaWiFiClient.hpp>
#include <ERa/ERaApiMbed.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Utility/ERaFlashMbed.hpp>
#include <Task/ERaTaskMbed.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
    ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaWiFi< ERaMqtt<TinyGsmClient, MQTTClient> > ERa;
#endif

#endif /* INC_ERA_SIMPLE_MBED_WIFI_HPP_ */
