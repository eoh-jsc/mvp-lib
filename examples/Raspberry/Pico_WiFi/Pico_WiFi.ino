/*************************************************************
  Download latest ERa library here:
    https://github.com/eoh-jsc/era-lib/releases/latest
    https://www.arduino.cc/reference/en/libraries/era
    https://registry.platformio.org/libraries/eoh-ltd/ERa/installation

    ERa website:                https://e-ra.io
    ERa blog:                   https://iotasia.org
    ERa forum:                  https://forum.eoh.io
    Follow us:                  https://www.fb.com/EoHPlatform
 *************************************************************/

/* Note: Preferences Arduino IDE
    https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
*/

// Enable debug console
// #define ERA_DEBUG
// #define ERA_SERIAL Serial

// You should get Auth Token in the ERa App or ERa Dashboard
#define TINY_GSM_MODEM_ESP8266
#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>
#include <SoftwareSerial.h>

SoftwareSerial SerialWiFi(17, 16);
TinyGsm modem(SerialWiFi);

ERaTimer timer;

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

const int rstPin = 7;

/* This function print uptime every second */
void timerEvent() {
    ERA_LOG("Timer", "Uptime: %d\n", ERaMillis() / 1000L);
}

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    /* Set GSM module baud rate */
    SerialWiFi.begin(115200);

    ERa.begin(modem, ssid, pass, rstPin);

    /* Setup timer called function every second */
    timer.setInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
    timer.run();
}
