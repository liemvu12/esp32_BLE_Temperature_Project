#include <NimBLEDevice.h>
#include <strings.h>
std:: string Data ;
NimBLEServer *pServer;
NimBLEService *pService ;
NimBLECharacteristic *pCharacteristic ;
NimBLEAdvertising *pAdvertising;
void setup() {
    Serial.begin(115200); // Khởi tạo UART với baud rate 115200
    delay(1000); // Đợi 1 giây cho UART hoạt động

    NimBLEDevice::init("NimBLE");
    
    pServer = NimBLEDevice::createServer();
    pService = pServer->createService("ABCD");
    pCharacteristic = pService->createCharacteristic("1234");
    
    pService->start();
    pCharacteristic->setValue("Hello Client");
    
    pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("ABCD"); 
    pAdvertising->start(); 
}

void loop() {
  Data = pCharacteristic->getValue ();
  Serial.println(Data.c_str());
  delay(500);
}
