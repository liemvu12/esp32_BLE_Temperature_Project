#include <NimBLEDevice.h>

NimBLEScanResults results; // Khai báo biến results ở phạm vi toàn cục
NimBLEScan *pScan; // Khai báo biến pScan ở phạm vi toàn cục
NimBLERemoteCharacteristic *pCharacteristic;
NimBLERemoteService *pService;
NimBLEClient *pClient;

// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
const int potPin = 34;

// variable for storing the potentiometer value
int potValue = 0;

void setup() {
    Serial.begin(115200); // Khởi tạo UART với baud rate 115200
    delay(1000); // Đợi 1 giây cho UART hoạt động

    NimBLEDevice::init("note2");
    pScan = NimBLEDevice::getScan(); // Gán đối tượng quét vào biến pScan
    results = pScan->start(10); // Gán kết quả quét vào biến results

    NimBLEUUID serviceUuid("ABCD");

    for(int i = 0; i < results.getCount(); i++) {
        NimBLEAdvertisedDevice device = results.getDevice(i);
        
        if (device.isAdvertisingService(serviceUuid)) {
            pClient = NimBLEDevice::createClient();
            
            if (pClient->connect(&device)) {
                pService = pClient->getService(serviceUuid);
                
                if (pService != nullptr) {
                    pCharacteristic = pService->getCharacteristic("1234");
                    
                    if (pCharacteristic != nullptr) {
                        std::string value = pCharacteristic->readValue();
                        // print or do whatever you need with the value
                        Serial.println(value.c_str());
                    }
                }
            } else {
                Serial.println("Failed to connect");
            }
        }
    }
}

void loop() {
    // Reading potentiometer value
    potValue = analogRead(potPin);
    std::string strpotValue = std::to_string(potValue);
    Serial.println(potValue);
    if (pCharacteristic != nullptr) {
        pCharacteristic->writeValue(strpotValue);
    }
    delay(500);
}
