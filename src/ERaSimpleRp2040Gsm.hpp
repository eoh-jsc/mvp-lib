#ifndef INC_ERA_SIMPLE_RP2040_GSM_HPP_
#define INC_ERA_SIMPLE_RP2040_GSM_HPP_

#include <ERa/ERaDetect.hpp>
#if !defined(TINY_GSM_RX_BUFFER)
    #define TINY_GSM_RX_BUFFER ERA_MQTT_BUFFER_SIZE
#endif

#include <ERa/ERaApiArduinoDef.hpp>
#include <Adapters/ERaGsmClient.hpp>
#include <ERa/ERaApiArduino.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Zigbee/ERaZigbeeArduino.hpp>
#include <Utility/ERaFlashLittleFS.hpp>
#include <Task/ERaTaskRp2040.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<TinyGsmClient, MQTTClient> mqtt;
    ERaGsm< ERaMqtt<TinyGsmClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaGsm< ERaMqtt<TinyGsmClient, MQTTClient> > ERa;
#endif

#endif /* INC_ERA_SIMPLE_RP2040_GSM_HPP_ */
