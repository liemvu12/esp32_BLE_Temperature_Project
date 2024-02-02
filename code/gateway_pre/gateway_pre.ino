#include "NimBLEDevice.h"
#include <esp_task_wdt.h>
#include "EspMQTTClient.h"
//#include "BLEScan.h"

#define SECRET_MQTT_USERNAME "MxU8FiQZOzovPCgFAQUhCDw"
#define SECRET_MQTT_CLIENT_ID "MxU8FiQZOzovPCgFAQUhCDw"
#define SECRET_MQTT_PASSWORD "mGyeYbgyOBRo1YS7NDjhH9bZ"

#define SECRET_WIFI_NAME "VIETTEL"
#define SECRET_WIFI_PASSWORD "88888888"
#define CHANNEL_ID "2392409"

EspMQTTClient client(
  SECRET_WIFI_NAME,
  SECRET_WIFI_PASSWORD,
  "mqtt3.thingspeak.com",
  SECRET_MQTT_USERNAME,
  SECRET_MQTT_PASSWORD,
  SECRET_MQTT_CLIENT_ID
);
static int b;
void onConnectionEstablished() {
  Serial.println("MQTT Client is connected to Thingspeak!");
  b=1;
}

#define bleServerName1 "Node 1"
static BLEUUID serviceUUID1("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID    temperatureCharacteristicUUID1("beb5483e-36e1-4688-b7f5-ea07361b26a8");

#define bleServerName2 "Node 2"
static BLEUUID serviceUUID2("0000180d-0000-1000-8000-00805f9b34fb");
static BLEUUID    temperatureCharacteristicUUID2("00002a19-0000-1000-8000-00805f9b34fb");

BLEClient* pClient = nullptr;

static boolean doConnect = false;
static boolean connected = false;
static BLEAddress *pServerAddress;
static BLERemoteCharacteristic* temperatureCharacteristic;
const uint8_t notificationOn[] = {0x1, 0x0};

char* temperatureChar1;
char* temperatureChar2;
boolean newTemperature1 = false;
boolean newTemperature2 = false;

float temperatureValue1;
float temperatureValue2;

static int abc = 1;
// kiem tra ket 
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

void disconnectFromServer() {
    if (pClient != nullptr && pClient->isConnected()) {
        pClient->disconnect();
        Serial.println("Disconnected from the server");
    }
}



static void temperatureNotifyCallback1(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  temperatureChar1 = (char*)pData;
  temperatureValue1 = atof(temperatureChar1);
  newTemperature1 = true;
}

static void temperatureNotifyCallback2(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  temperatureChar2 = (char*)pData;
  temperatureValue2 = atof(temperatureChar2);
  newTemperature2 = true;
}




bool connectToServer(BLEAddress pAddress, BLEUUID serviceUUID, BLEUUID temperatureCharacteristicUUID) {
  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
  pClient->connect(pAddress);
  //Serial.println(" - Connected to server");
 
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    //Serial.print("Failed to find our service UUID: ");
    //Serial.println(serviceUUID.toString().c_str());
    return (false);
  }
 
  temperatureCharacteristic = pRemoteService->getCharacteristic(temperatureCharacteristicUUID);

  if (temperatureCharacteristic == nullptr) {
    //Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  //Serial.println(" - Found our characteristics");
  if(abc == 1){
  temperatureCharacteristic->registerForNotify(temperatureNotifyCallback1);
  }
  else if(abc == 2){
  //else{
  temperatureCharacteristic->registerForNotify(temperatureNotifyCallback2);
  }
  connected = true;   //*//
  return true;
}



class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice* advertisedDevice) {
    if (abc == 1 && advertisedDevice->getName() == bleServerName1) {
      advertisedDevice->getScan()->stop();
      pServerAddress = new BLEAddress(advertisedDevice->getAddress());
      doConnect = true;
      Serial.println("Device found 1. Connecting!");
    }
    else if (abc == 2 && advertisedDevice->getName() == bleServerName2) {
      advertisedDevice->getScan()->stop();
      pServerAddress = new BLEAddress(advertisedDevice->getAddress());
      doConnect = true;
      Serial.println("Device found 2. Connecting!");
    }

    
  }
};
 



void setup() {
  Serial.begin(115200);
  //client.loop();
  Serial.println("Starting Arduino BLE Client application...");

  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(2);

  // Kích hoạt WDT
    esp_task_wdt_init(30, true); // Timeout
    esp_task_wdt_add(NULL); // Thêm task hiện tại vào WDT
} 


void loop() {
client.loop();


if(b==1)
{

if(abc == 1){

  if (doConnect == true) {
    if (connectToServer(*pServerAddress, serviceUUID1, temperatureCharacteristicUUID1)) {
      //Serial.println("We are now connected to the BLE Server.");
      temperatureCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
            connected = true;
    } 
    else {
      Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
      doConnect = false;
  }
  else
  {
    abc = 2;
    disconnectFromServer();
  }


  if (newTemperature1){
    newTemperature1 = false;
  //  Serial.println(temperatureChar1);
  //  float temperatureValue1 = atof(temperatureChar1);
    Serial.println(temperatureValue1);
    //String MY_TOPIC = "channels/" + String(CHANNEL_ID) + "/publish";
    //String payload = "field1=" + String(temperatureValue1);
    //client.publish(MY_TOPIC, payload);
    disconnectFromServer();
  }
  
abc = 2;
}


else if(abc == 2){

  if (doConnect == true) {
    if (connectToServer(*pServerAddress, serviceUUID2, temperatureCharacteristicUUID2)) {
      //Serial.println("We are now connected to the BLE Server.");
      temperatureCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
            connected = true;
    } 
    else {
      Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
      doConnect = false;
  }
  else
  {
    abc = 1;
    disconnectFromServer();
  }



  if (newTemperature2){
    newTemperature2 = false;
    //Serial.println(temperatureChar2);
    //float temperatureValue2 = atof(temperatureChar2);
    Serial.println(temperatureValue2);
    //String MY_TOPIC = "channels/" + String(CHANNEL_ID) + "/publish";
    //String payload = "field2=" + String(temperatureValue2);
    //client.publish(MY_TOPIC, payload);
    disconnectFromServer();
  }

abc = 1;  
}




}

delay(1000);
//client.loop();


String MY_TOPIC = "channels/" + String(CHANNEL_ID) + "/publish";
    String payload = "field1=" + String(temperatureValue1) + "&field2=" + String(temperatureValue2);
    client.publish(MY_TOPIC, payload);



if(!connected){
    BLEDevice::getScan()->start(0);
  }


  delay(1000);  
 
  esp_task_wdt_reset();
}
