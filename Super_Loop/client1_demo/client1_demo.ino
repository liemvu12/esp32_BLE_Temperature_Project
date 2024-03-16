#include <NimBLEDevice.h>

NimBLEScanResults results; // Khai báo biến results ở phạm vi toàn cục
NimBLEScan *pScan; // Khai báo biến pScan ở phạm vi toàn cục
NimBLERemoteCharacteristic *pCharacteristic;
NimBLERemoteService *pService;
NimBLEClient *pClient;

const int switchPin = 14;  // Chân GPIO kết nối với công tắc

void setup() {
    Serial.begin(115200); // Khởi tạo UART với baud rate 115200
    delay(1000); // Đợi 1 giây cho UART hoạt động
    pinMode(switchPin, INPUT);

    NimBLEDevice::init("note1");
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
    int switchState = digitalRead(switchPin);
    std::string strpotValue;
    if (switchState == HIGH) {
        strpotValue = "500";
        Serial.println("500");
        // Trả về giá trị 500
    } else {
        strpotValue = "2000";
        Serial.println("2000");
        // Trả về giá trị 2000
    }
    if (pCharacteristic != nullptr) {
        pCharacteristic->writeValue(strpotValue);
    }
    delay(500);
}
