#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define ADC_VREF_mV    3300.0
#define ADC_RESOLUTION 4096.0
#define PIN_LM35       36

const int buttonPin = 33;
const int ledPin1 = 25;
const int ledPin2 = 26;
const int ledPin3 = 27;

int sttButton = 0;
int count;
 float ng1 = 10, ng2 = 20, ng3 = 30;

int adcVal;
float milliVolt;
float tempC;

unsigned long lastTime = 0;
unsigned long timerDelay = 1000;


bool deviceConnected = false;
bool oldDeviceConnected = false;



#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

BLECharacteristic TemperatureCharacteristics("beb5483e-36e1-4688-b7f5-ea07361b26a8", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor TemperatureDescriptor(BLEUUID((uint16_t)0x2902));

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      pServer->startAdvertising(); // Bắt đầu quảng cáo lại
    }
};


void setup() {

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  Serial.begin(115200);
  BLEDevice::init("Node 1");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  pService->addCharacteristic(&TemperatureCharacteristics);
  //
  TemperatureDescriptor.setValue("abc");
  //
  TemperatureCharacteristics.addDescriptor(&TemperatureDescriptor);

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);

pServer->getAdvertising()->start();
Serial.println("Waiting a client connection to notify...");

  pAdvertising->setScanResponse(true);
  //
//  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
//  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
//  Serial.println("Characteristic defined! Now you can read it in your phone!");
 }

void loop() {


sttButton = digitalRead(buttonPin);
  if(sttButton == LOW)
  {
    delay(50);
    if(sttButton == LOW)
      {
          count++;
          if (count > 1) count = 0;
          while(sttButton == LOW);
      }
  }





  if (deviceConnected) {

    if ((millis() - lastTime) > timerDelay) {
      adcVal = analogRead(PIN_LM35);
      milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
      tempC = milliVolt / 10;

      static char temperatureCTemp[6];
      dtostrf(tempC, 6, 2, temperatureCTemp);

        TemperatureCharacteristics.setValue(temperatureCTemp);
        TemperatureCharacteristics.notify();
        Serial.println(tempC);
  
    lastTime = millis();
    }

  }


//////

if(tempC < ng1)
{
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  digitalWrite(ledPin3, HIGH);
}

else if(tempC >= ng1 && tempC <ng2)
{
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, HIGH);
  digitalWrite(ledPin3, HIGH);
}

else if(tempC >= ng2 && tempC <ng3)
{
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, HIGH);
}

else if(tempC >= ng3)
{
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  digitalWrite(ledPin3, LOW);
}

//////


}
