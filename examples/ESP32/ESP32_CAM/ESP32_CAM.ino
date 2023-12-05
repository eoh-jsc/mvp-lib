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

// Enable debug console
// Set CORE_DEBUG_LEVEL = 3 first
// #define ERA_DEBUG
// #define ERA_SERIAL Serial

/* Select ERa host location (VN: Viet Nam, SG: Singapore) */
#define ERA_LOCATION_VN
// #define ERA_LOCATION_SG

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "ERA2706"

#include <Arduino.h>
#include <ERaSimpleEsp32.hpp>
#include <SimStreamer.h>
#include <OV2640Streamer.h>
#include <CRtspSession.h>

/*
* Download the Micro-RTSP library at:
* https://github.com/geeksville/Micro-RTSP.git#fc43f56
*/

const char ssid[] = "YOUR_SSID";
const char pass[] = "YOUR_PASSWORD";

OV2640 cam;
WiFiServer rtspServer(554);
CStreamer* streamer = nullptr;
static bool connected = false;

// URI: e.g. rtsp://<ip>:<port>/mjpeg/1
void handleStream() {
    if (!connected) {
        return;
    }
    if (streamer == nullptr) {
        return;
    }
    const uint32_t millisPerFrame = 100;
    static uint32_t lastFrame = ERaMillis();
    streamer->handleRequests(0);
    uint32_t currentMillis = ERaMillis();
    if (streamer->anySessions()) {
        if ((currentMillis - lastFrame) > millisPerFrame) {
            streamer->streamImage(currentMillis);
            lastFrame = currentMillis;
            currentMillis = ERaMillis();
            if ((currentMillis - lastFrame) > millisPerFrame) {
                ERA_LOG("RTSP", "Warning exceeding max frame rate of %d ms", currentMillis - lastFrame);
            }
        }
    }
    WiFiClient rtspClient = rtspServer.accept();
    if (!rtspClient) {
        return;
    }
    ERA_LOG("RTSP", "Client: %s", rtspClient.remoteIP().toString().c_str());
    streamer->addSession(rtspClient);
}

ERA_CONNECTED() {
    connected = true;
}

ERA_DISCONNECTED() {
    connected = false;
}

/* This function print uptime every second */
void timerEvent() {
    ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}

void setup() {
    /* Setup debug console */
#if defined(ERA_DEBUG)
    Serial.begin(115200);
#endif
    while (cam.init(esp32cam_aithinker_config) != ESP_OK) {}

    /* Set board id */
    // ERa.setBoardID("Board_1");
    /* Initializing the ERa library. */
    ERa.begin(ssid, pass);

    /* Start RTSP */
    rtspServer.begin();
    streamer = new OV2640Streamer(cam);

    /* Setup timer called function every second */
    ERa.addInterval(1000L, timerEvent);
}

void loop() {
    ERa.run();
    handleStream();
}