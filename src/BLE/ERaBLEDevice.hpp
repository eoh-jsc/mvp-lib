#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <MD5Builder.h>
#include <LinkedList.h>

const int ledPin = 2;

#define CODE "1d91d980-bbe4-4db4-8790-829a2fc62450"
#define DEVICE_NAME "E-Ra - Trung Thu"

#define SERVICE_UUID "c459d382-3ecd-41a2-8769-c502dbb88648"
#define PASSWORD "f01f1e6b-e404-4534-bdfd-58af0e1fbb59"
#define CHARACTERISTIC_CONFIRM_CODE_UUID "6ee26cfb-b42f-43ae-9ecf-f70d84bbd64f"
#define CHARACTERISTIC_CONTROL_UUID "f0e7aa38-e466-42af-b4c7-5e6ec5c9a614"


char** verifiedCodes = new char*[5];  // max
u_int8_t verifiedCount = 0;
LinkedList<uint16_t> verifiedConnectionId = LinkedList<uint16_t>();
BLEServer *pServer = nullptr;


class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
    Serial.print("Connected BLE");
    Serial.println(param->connect.conn_id);
    pServer->startAdvertising();
  };
  void onDisconnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
    Serial.print("Disconnected BLE");
    Serial.println(param->connect.conn_id);
    pServer->startAdvertising();
    verifiedConnectionId.remove(param->connect.conn_id);
  }
};


class ControlCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t* param) {
      int size = verifiedConnectionId.size();
      bool hasPermission = false;
      for (int i = 0; i < size; i++) {
        if (verifiedConnectionId.get(i) == param->connect.conn_id) {
          hasPermission = true;
          break;
        }
      }
      if (!hasPermission) {
        Serial.println("NO PERMISSION");
        return;
      }
      std::string message = pCharacteristic->getValue();
      Serial.println(message.c_str());
      u_int8_t index = message.find(';'); // <pin>;<value> reduce message size because bluetooth is slow
      const char* pinStr = message.substr(0, index).c_str();
      const char* value = message.substr(index + 1).c_str();

      // handle case virtual pin later
      u_int8_t pin = atoi(pinStr);
      pinMode(pin, OUTPUT);  // should put in setup or setup characteristic
      digitalWrite(pin, atoi(value));
    }
};


class ConfirmCallbacks: public BLECharacteristicCallbacks {
  char currentVerificationCode[9];
  void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t* param) {
    if (verifiedCount >= 5) {  // reach max
      pCharacteristic->setValue("Max connections.");
      pCharacteristic->notify();
      return;
    }



    
    std::string value = pCharacteristic->getValue();

    Serial.println(value.c_str());

    if (value == "start") {
      // random and write back
      currentVerificationCode[0] = (char) (esp_random() % 26 + '0');
      currentVerificationCode[1] = (char) (esp_random() % 26 + '0');
      currentVerificationCode[2] = (char) (esp_random() % 26 + '0');
      currentVerificationCode[3] = (char) (esp_random() % 26 + '0');
      currentVerificationCode[4] = (char) (esp_random() % 26 + '0');
      currentVerificationCode[5] = (char) (esp_random() % 26 + '0');
      currentVerificationCode[6] = (char) (esp_random() % 26 + '0');
      currentVerificationCode[7] = (char) (esp_random() % 26 + '0');
      currentVerificationCode[8] = 0;

      Serial.println(currentVerificationCode);
      pCharacteristic->setValue(currentVerificationCode);
      pCharacteristic->notify();
    }

    if (value.substr(0, 7) == "confirm") {
      char code[36 + 8 + 1]; // uuid4 + random + endstring
      for(int i = 0; i < 36; i++) {
        code[i] = PASSWORD[i];
      }
      for (int i = 0; i < 8; i++) {
        code[36 + i] = currentVerificationCode[i];
      }
      code[44] = 0;

      MD5Builder md5;
      md5.begin();
      md5.add(code);
      md5.calculate();

      char* output = new char[33];
      md5.getChars(output);

      Serial.println("verified code");
      Serial.println(output);

      if (value.substr(7, 32) == output) {
        digitalWrite(ledPin, HIGH);
        char* confirmText = new char[11];
        confirmText[0] ='c';
        confirmText[1] ='o';
        confirmText[2] ='n';
        confirmText[3] ='f';
        confirmText[4] ='i';
        confirmText[5] ='r';
        confirmText[6] ='m';
        confirmText[7] ='e';
        confirmText[8] ='d';
        confirmText[9] = verifiedCount + '0';
        confirmText[10] = 0;
        pCharacteristic->setValue(confirmText);  // confirmed<id>
        verifiedConnectionId.add(param->connect.conn_id);
      } else {
        digitalWrite(ledPin, LOW);
        
        Serial.print("Disconnecting BLE");
        Serial.println(param->disconnect.conn_id);
        pServer->disconnect(param->disconnect.conn_id);
        delete output;
      }
    }
  }
};

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  BLEDevice::init(DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *confirmCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_CONFIRM_CODE_UUID,
    BLECharacteristic::PROPERTY_READ | 
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  confirmCharacteristic->setCallbacks(new ConfirmCallbacks());
  

  BLECharacteristic *controlCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_CONTROL_UUID,
    BLECharacteristic::PROPERTY_READ | 
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  controlCharacteristic->setCallbacks(new ControlCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
}

void loop() {
}