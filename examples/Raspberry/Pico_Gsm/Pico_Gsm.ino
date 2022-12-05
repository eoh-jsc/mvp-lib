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

/* Select your apn */
#define APN_VIETTEL
// #define APN_VINAPHONE
// #define APN_MOBIFONE

/* Select your modem */
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM900

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>
#include <SoftwareSerial.h>

SoftwareSerial SerialGsm(17, 16);
TinyGsm modem(SerialGsm);

ERaTimer timer;

#if defined(APN_VIETTEL)
    const char apn[] = "v-internet";
    const char user[] = "";
    const char pass[] = "";
#elif defined(APN_VINAPHONE)
    const char apn[] = "m3-world";
    const char user[] = "mms";
    const char pass[] = "mms";
#elif defined(APN_MOBIFONE)
    const char apn[] = "m-wap";
    const char user[] = "mms";
    const char pass[] = "mms";
#else
    const char apn[] = "internet";
    const char user[] = "";
    const char pass[] = "";
#endif

const int pwrPin = 7;

/* This function print uptime every second */
void timerEvent() {
    ERA_LOG("Timer", "Uptime: %d\n", ERaMillis() / 1000L);
}

void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    /* Set GSM module baud rate */
    SerialGsm.begin(115200);

    ERa.begin(modem, apn, user, pass, pwrPin);

    /* Setup timer called function every second */
    timer.setInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
    timer.run();
}
