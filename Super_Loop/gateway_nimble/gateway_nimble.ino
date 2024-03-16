#include <NimBLEDevice.h>
#include <string.h>
#include <HTTPClient.h>
#include <WiFi.h>

const char* ssid = "iPhone của Liêm";
const char* password = "25122001";

String urlThingspeak = "https://api.thingspeak.com/update?api_key=J9KX9YJMWBO7T32X" ;
WiFiClient wifiClient;

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

String httpGETRequest(const char* Url)
{
  HTTPClient http;
  http.begin(Url);
  int responseCode = http.GET();
  String responseBody = "{}";
  if(responseCode > 0)
  {
    Serial.print("responseCode:");
    Serial.println(responseCode);
    responseBody = http.getString();
  }
  else
  {
    Serial.print("Error Code: ");
    Serial.println(responseCode);
  }
  http.end();
  return responseBody;
}



void setup() {
  Serial.begin(115200);
  delay(1000);

  // Khởi tạo bảng
  for (int i = 0; i < 10; i++) {
    dataTemperatures[i] = DataTemperature(NimBLEAddress(), "");
  }

  //setup wifi
  WiFi.begin(ssid,password);
  Serial.println("conecting");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

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
  String DataSend ="";
  for (int i = 0; i < 10; i++) {
    if (dataTemperatures[i].FlagDataSend == true) {
      dataTemperatures[i].FlagDataSend = false;

      DataSend = DataSend + "&field" + String(i) + dataTemperatures[i].data.c_str();

      // In ra địa chỉ MAC và dữ liệu nhiệt độ
      Serial.print(dataTemperatures[i].clientAddress.toString().c_str());
      Serial.print(" dataTemperatures: ");
      Serial.println(dataTemperatures[i].data.c_str());
    }
  }
  String Url = urlThingspeak + DataSend;
  httpGETRequest(Url.c_str());
  delay(1000);
}

