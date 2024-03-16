#include <NimBLEDevice.h>

NimBLEScanResults results; // Khai báo biến results ở phạm vi toàn cục
NimBLEScan *pScan; // Khai báo biến pScan ở phạm vi toàn cục
NimBLERemoteCharacteristic *pCharacteristic;
NimBLERemoteService *pService;
NimBLEClient *pClient;

#define ADC_VREF_mV    3300.0 // in millivolt
#define ADC_RESOLUTION 4096.0
#define PIN_LM35       34 // ESP32 pin GPIO36 (ADC0) connected to LM35

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
  int adcVal = analogRead(PIN_LM35);
  // convert the ADC value to voltage in millivolt
  float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
  // convert the voltage to the temperature in °C
  float tempC = milliVolt / 10;    
  std::string strpotValue = std::to_string(tempC);
  Serial.println(tempC);
  if (pCharacteristic != nullptr) {
      pCharacteristic->writeValue(strpotValue);
  }
  delay(500);
}
