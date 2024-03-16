#include <NimBLEDevice.h>
#include <string.h>

NimBLEServer *pServer;
NimBLEService *pService;
NimBLECharacteristic *pCharacteristic;
NimBLEAdvertising *pAdvertising;

class DataTemperature {
public:
  NimBLEAddress clientAddress;
  std::string data;
  bool FlagDataSend;

  DataTemperature() : FlagDataSend(false) {}
  DataTemperature(NimBLEAddress clientAddress, std::string data) : FlagDataSend(false) {
    this->clientAddress = clientAddress;
    this->data = data;
  }
};


DataTemperature dataTemperatures[10];

class MyServerCallbacks : public NimBLEServerCallbacks {
public:
  void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
    NimBLEAddress clientAddress = NimBLEAddress(desc->peer_ota_addr);
    std::string Address = clientAddress.toString();
    Serial.print("Client connected: ");
    Serial.println(Address.c_str());
    // Tìm một slot trống trong dataTemperatures
    for (int i = 0; i < 10; i++) {
      if (dataTemperatures[i].clientAddress == NimBLEAddress()) {
        // Gán địa chỉ mới cho slot trống này
        dataTemperatures[i].clientAddress = clientAddress;
        break;
      }
    }
    NimBLEDevice::startAdvertising();
  }

  void onDisconnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
    NimBLEAddress clientAddress = NimBLEAddress(desc->peer_ota_addr);
    std::string Address = clientAddress.toString();
    Serial.print("Client disconnected: ");
    Serial.println(Address.c_str());
    for (int i = 0; i < 10; i++) {
      if (dataTemperatures[i].clientAddress == clientAddress) {
        // Xóa địa chỉ khỏi slot này
        dataTemperatures[i].clientAddress = NimBLEAddress();
        break;
      }
    }
  }
};

class MyCallbacks: public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc* desc) {
    NimBLEAddress clientAddress = NimBLEAddress(desc->peer_ota_addr);
    for (int i = 0; i < 10; i++) {
      if (dataTemperatures[i].clientAddress == clientAddress) {
          dataTemperatures[i].FlagDataSend = true; 
          dataTemperatures[i].data = pCharacteristic->getValue();
          break; 
      }
    }
  }
};



void setup() {
  Serial.begin(115200);
  delay(1000);

  // Khởi tạo bảng
  for (int i = 0; i < 10; i++) {
    dataTemperatures[i] = DataTemperature(NimBLEAddress(), "");
  }

  NimBLEDevice::init("NimBLE");

  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  pService = pServer->createService("ABCD");
  pCharacteristic = pService->createCharacteristic("1234");

  pService->start();

  pCharacteristic->setCallbacks(new MyCallbacks());

  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID("ABCD");
  pAdvertising->start();

}

void loop() {
  for (int i = 0; i < 10; i++) {
    if (dataTemperatures[i].FlagDataSend == true) {
      dataTemperatures[i].FlagDataSend = false;
      // In ra địa chỉ MAC và dữ liệu nhiệt độ
      Serial.print(dataTemperatures[i].clientAddress.toString().c_str());
      Serial.print(" dataTemperatures: ");
      Serial.println(dataTemperatures[i].data.c_str());
    }
  }
}

