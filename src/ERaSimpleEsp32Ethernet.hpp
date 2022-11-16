#ifndef INC_ERA_SIMPLE_ESP32_ETHERNET_HPP_
#define INC_ERA_SIMPLE_ESP32_ETHERNET_HPP_

#include <Adapters/ERaEthernetEsp32Client.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
        xTaskCreatePinnedToCore(this->modbusTask, "modbusTask", 1024 * 5, this, 17, &this->_modbusTask, 1);
        xTaskCreatePinnedToCore(this->writeModbusTask, "writeModbusTask", 1024 * 5, this, 17, &this->_writeModbusTask, 1);
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(true);
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
        ERaModbus* modbus = (ERaModbus*)args;
        modbus->run(false);
    }
#endif

#if defined(ERA_ZIGBEE)
    template <class Api>
    void ERaZigbee<Api>::initZigbeeTask() {

    }

    template <class Api>
    void ERaZigbee<Api>::zigbeeTask(void* args) {

    }

    template <class Api>
    void ERaZigbee<Api>::controlZigbeeTask(void* args) {
        
    }

    template <class Api>
    void ERaZigbee<Api>::responseZigbeeTask(void* args) {
        
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::ERaModbus::begin();
#endif
#if defined(ERA_ZIGBEE)
	Base::ERaZigbee::begin();
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::runERaTask() {
}

static WiFiClient eraWiFiClient;
static ERaMqtt<WiFiClient, MQTTClient> mqtt(eraWiFiClient);
static ERaFlash flash;
ERaEthernet ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_ESP32_ETHERNET_HPP_ */