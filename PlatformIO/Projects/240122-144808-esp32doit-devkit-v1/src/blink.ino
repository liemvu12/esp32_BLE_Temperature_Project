// float tempC = std::stof(rxValue);
/*
  Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
  Ported to Arduino ESP32 by Evandro Copercini
  updated by chegewara and MoThunderz
*/
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
// #include <stdio.h>
// #include <string.h>
// #include <WiFi.h>
// #include <EspMQTTClient.h>
// #include "ThingSpeak.h"

// Initialize all pointers
BLEServer* pServer = NULL;                        // Pointer to the server
BLECharacteristic* pCharacteristic_1 = NULL;      // Pointer to Characteristic 1
BLECharacteristic* pCharacteristic_2 = NULL;      // Pointer to Characteristic 2
// BLEDescriptor *pDescr_1;                          // Pointer to Descriptor of Characteristic 1
// BLE2902 *pBLE2902_1;                              // Pointer to BLE2902 of Characteristic 1
// BLE2902 *pBLE2902_2;                              // Pointer to BLE2902 of Characteristic 2

// #define SECRET_MQTT_USERNAME "JBIaPSE6CDs9EiwmAjYGNhA"
// #define SECRET_MQTT_CLIENT_ID "JBIaPSE6CDs9EiwmAjYGNhA"
// #define SECRET_MQTT_PASSWORD "B1x0Cq0CQniqo2pbxmib9ebh"

// #define SECRET_WIFI_NAME "HuyHoangBKHN"           // YOUR_WIFI_NAME
// #define SECRET_WIFI_PASSWORD "Huyhoang18112k2"   // YOUR_WIFI_PASSWORD
// #define CHANNEL_ID "2398828"                // YOUR_CHANNEL_ID

// Some variables to keep track on device connected
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Variable that will continuously be increased and written to the client

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
// UUIDs used in this example:
#define SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// #define CHARACTERISTIC_UUID_1 "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_2 "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"

// EspMQTTClient client(
//   SECRET_WIFI_NAME,
//   SECRET_WIFI_PASSWORD,
//   "mqtt3.thingspeak.com",
//   SECRET_MQTT_USERNAME,
//   SECRET_MQTT_PASSWORD,
//   SECRET_MQTT_CLIENT_ID
// );

// void onConnectionEstablished() {
//   Serial.println("MQTT Client is connected to Thingspeak!");
// }


// Callback function that is called whenever a client is connected or disconnected
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  // pCharacteristic_1 = pService->createCharacteristic(
  //                     CHARACTERISTIC_UUID_1,
  //                     BLECharacteristic::PROPERTY_NOTIFY
  //                   );                   

  pCharacteristic_2 = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_2,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |                      
                      BLECharacteristic::PROPERTY_NOTIFY
                    );  

  // Create a BLE Descriptor  
  // pDescr_1 = new BLEDescriptor((uint16_t)0x2901);
  // pDescr_1->setValue("A very interesting variable");
  // pCharacteristic_1->addDescriptor(pDescr_1);

  // // Add the BLE2902 Descriptor because we are using "PROPERTY_NOTIFY"
  // pBLE2902_1 = new BLE2902();
  // pBLE2902_1->setNotifications(true);                 
  // pCharacteristic_1->addDescriptor(pBLE2902_1);

  // pBLE2902_2 = new BLE2902();
  // pBLE2902_2->setNotifications(true);
  // pCharacteristic_2->addDescriptor(pBLE2902_2);

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

// unsigned long lastTime = 0;
// unsigned long delayTime = 5000; // set a period of sending data.

// void publishData(float tempC){
//   if (client.isConnected() && (millis() - lastTime > delayTime)){
//     // TempAndHumidity data = dhtSensor.getTempAndHumidity();
//     // float t = data.temperature;
//     // float h = data.humidity;
//     if (isnan(tempC)) {
//       Serial.println(F("Failed to read from Client!"));
//       return;
//     }
//     Serial.print(F("\nTemperature: "));
//     Serial.print(tempC);
//     Serial.print(F("°C "));
//     // Serial.print(F("Humidity: "));
//     // Serial.print(humidity);
//     // Serial.println(F("%  "));

//     Serial.println(F("\nPublising data to Thingspeak"));
    
//     String MY_TOPIC = "channels/" + String(CHANNEL_ID) + "/publish";  // build your topic: channels/<channelID>/publish
//     String payload = "field1=" + String(tempC);  // build your payload: field1=<value1>&field2=<value2>
//     client.publish(MY_TOPIC, payload);

//     Serial.println("Data published");
//     lastTime = millis();
//   }
// }

void loop() {
    // notify changed value
    if (deviceConnected) {
      // pCharacteristic_1 is an integer that is increased with every second
      // in the code below we send the value over to the client and increase the integer counter
      // pCharacteristic_1->setValue(value);
      // pCharacteristic_1->notify();
      // value++;

      // pCharacteristic_2 is a std::string (NOT a String). In the code below we read the current value
      // write this to the Serial interface and send a different value back to the Client
      // Here the current value is read using getValue() 
      std::string rxValue = pCharacteristic_2->getValue();
      Serial.print("Characteristic 2 (getValue Temperature): ");
      Serial.println(rxValue.c_str());
      

      // Here the value is written to the Client using setValue();
      // String txValue = "String with random value from Server: " + String(random(1000));
      // pCharacteristic_2->setValue(txValue.c_str());
      // Serial.println("Characteristic 2 (setValue): " + txValue);

      // In this example "delay" is used to delay with one second. This is of course a very basic 
      // implementation to keep things simple. I recommend to use millis() for any production code
      // client.loop();
      // publishData(tempC);
      delay(1000);
    }
    // The code below keeps the connection status uptodate:
    // Disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // Connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}