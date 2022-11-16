#ifndef INC_ERA_ETHERNET_ESP32_CLIENT_HPP_
#define INC_ERA_ETHERNET_ESP32_CLIENT_HPP_

#include <ETH.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ERa/ERaProtocol.hpp>
#include <ERa/ERaApiEsp32.hpp>
#include <MQTT/ERaMqtt.hpp>
#include <Modbus/ERaModbusEsp32.hpp>
#include <Zigbee/ERaZigbeeEsp32.hpp>
#include <Utility/ERaFlashEsp32.hpp>

#define ERA_MODEL_TYPE                "ERa"
#define ERA_BOARD_TYPE                "ESP32"

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

class ERaEthernet
    : public ERaProto< ERaMqtt<WiFiClient, MQTTClient>, ERaFlash >
{
    const char* TAG = "Ethernet";
    friend class ERaProto< ERaMqtt<WiFiClient, MQTTClient>, ERaFlash >;
    typedef ERaProto< ERaMqtt<WiFiClient, MQTTClient>, ERaFlash > Base;

public:
    ERaEthernet(ERaMqtt<WiFiClient, MQTTClient>& _transp, ERaFlash& _flash)
        : Base(_transp, _flash)
        , authToken(nullptr)
    {}
    ~ERaEthernet()
    {}

    bool connectNetwork(uint8_t phyAddr = ETH_PHY_ADDR,
                        int power = ETH_PHY_POWER,
                        int mdc = ETH_PHY_MDC,
                        int mdio = ETH_PHY_MDIO,
                        eth_phy_type_t type = ETH_PHY_LAN8720,
                        eth_clock_mode_t clkMode = ETH_CLOCK_GPIO0_IN) {
        ERA_LOG(TAG, "Connecting network...");
        if (!ETH.begin(phyAddr, power,
                        mdc, mdio, type, clkMode)){
            ERA_LOG(TAG, "Connect failed");
            return false;
        }

        ERaDelay(1000);
        IPAddress localIP = ETH.localIP();
        ERA_FORCE_UNUSED(localIP);
        ERA_LOG(TAG, "IP: %s", localIP.toString().c_str());
        return true;
    }

    void config(const char* auth,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::begin(auth);
        this->transp.config(host, port, username, password);
    }

    void begin(const char* auth,
                uint8_t phyAddr = ETH_PHY_ADDR,
                int power = ETH_PHY_POWER,
                int mdc = ETH_PHY_MDC,
                int mdio = ETH_PHY_MDIO,
                eth_phy_type_t type = ETH_PHY_LAN8720,
                eth_clock_mode_t clkMode = ETH_CLOCK_GPIO0_IN,
                const char* host = ERA_MQTT_HOST,
                uint16_t port = ERA_MQTT_PORT,
                const char* username = ERA_MQTT_USERNAME,
                const char* password = ERA_MQTT_PASSWORD) {
        Base::init();
        this->config(auth, host, port, username, password);
        this->connectNetwork(phyAddr, power, mdc, mdio, type, clkMode);
        Base::connect();
    }

    void begin(uint8_t phyAddr = ETH_PHY_ADDR,
                int power = ETH_PHY_POWER,
                int mdc = ETH_PHY_MDC,
                int mdio = ETH_PHY_MDIO,
                eth_phy_type_t type = ETH_PHY_LAN8720,
                eth_clock_mode_t clkMode = ETH_CLOCK_GPIO0_IN) {
        this->begin(ERA_MQTT_CLIENT_ID,
                    phyAddr, power,
                    mdc, mdio,
                    type, clkMode,
                    ERA_MQTT_HOST, ERA_MQTT_PORT,
                    ERA_MQTT_USERNAME, ERA_MQTT_PASSWORD);
    }

protected:
private:
    const char* authToken;

};

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, static_cast<Proto*>(this)->ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, "Ethernet");
    cJSON_AddStringToObject(root, INFO_BSSID, "Ethernet");
    cJSON_AddNumberToObject(root, INFO_RSSI, ETH.linkSpeed());
    cJSON_AddStringToObject(root, INFO_MAC, ETH.macAddress().c_str());
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ETH.localIP().toString().c_str());
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, static_cast<uint16_t>((temprature_sens_read() - 32) * 100.0f / 1.8f));
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, ETH.linkSpeed());
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, "Ethernet");
}

#endif /* INC_ERA_ETHERNET_ESP32_CLIENT_HPP_ */
