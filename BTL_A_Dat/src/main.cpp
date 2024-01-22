#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#include <string.h>
#include <WiFi.h>
#include <EspMQTTClient.h>
#include "ThingSpeak.h"
// put function declarations here:

#define SECRET_MQTT_USERNAME "JBIaPSE6CDs9EiwmAjYGNhA"
#define SECRET_MQTT_CLIENT_ID "JBIaPSE6CDs9EiwmAjYGNhA"
#define SECRET_MQTT_PASSWORD "B1x0Cq0CQniqo2pbxmib9ebh"

#define SECRET_WIFI_NAME "HuyHoangBKHN"           // YOUR_WIFI_NAME
#define SECRET_WIFI_PASSWORD "Huyhoang18112k2"   // YOUR_WIFI_PASSWORD
#define CHANNEL_ID "2398828"                // YOUR_CHANNEL_ID

#define ADC_VREF_mV    3300.0 // in millivolt
#define ADC_RESOLUTION 4096.0
#define PIN_LM35       36 // ESP32 pin GPIO4 (ADC0) connected to LM35

LiquidCrystal_I2C lcd(0x27,16,2);

EspMQTTClient client(
  SECRET_WIFI_NAME,
  SECRET_WIFI_PASSWORD,
  "mqtt3.thingspeak.com",
  SECRET_MQTT_USERNAME,
  SECRET_MQTT_PASSWORD,
  SECRET_MQTT_CLIENT_ID
);

void onConnectionEstablished() {
  Serial.println("MQTT Client is connected to Thingspeak!");
}

byte doC[] = {
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

void data_led_display(float temperature){

  float tempF = temperature * 9 / 5 + 32;

  lcd.setCursor(0,0);
  lcd.printf("Temp: %0.2f C",temperature);
  lcd.setCursor(11,0);
  lcd.write(0);

  lcd.setCursor(0,1);
  lcd.printf("TempF: %0.2f F",tempF);
  lcd.setCursor(12,1);
  lcd.write(0);

  if(temperature >= 10 && temperature < 20){
    digitalWrite(27,HIGH);
    digitalWrite(26,LOW);
    digitalWrite(25,LOW);
  }else{
    if(temperature >= 20 && temperature < 30){
      digitalWrite(27,HIGH);
      digitalWrite(26,HIGH);
      digitalWrite(25,LOW);
    }else {
      
        digitalWrite(27,HIGH);
        digitalWrite(26,HIGH);
        digitalWrite(25,HIGH);

    }
  }

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.init();       //Khởi động màn hình. Bắt đầu cho phép Arduino sử dụng màn hình
  lcd.backlight();   //Bật đèn nền
  lcd.createChar(0, doC);
  pinMode(25,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(27,OUTPUT);

}

unsigned long lastTime = 0;
unsigned long delayTime = 5000; // set a period of sending data.

void publishData(float tempC){
  if (client.isConnected() && (millis() - lastTime > delayTime)){
    // TempAndHumidity data = dhtSensor.getTempAndHumidity();
    // float t = data.temperature;
    // float h = data.humidity;
    if (isnan(tempC)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    Serial.print(F("\nTemperature: "));
    Serial.print(tempC);
    Serial.print(F("°C "));
    // Serial.print(F("Humidity: "));
    // Serial.print(humidity);
    // Serial.println(F("%  "));

    Serial.println(F("\nPublising data to Thingspeak"));
    
    String MY_TOPIC = "channels/" + String(CHANNEL_ID) + "/publish";  // build your topic: channels/<channelID>/publish
    String payload = "field1=" + String(tempC);  // build your payload: field1=<value1>&field2=<value2>
    client.publish(MY_TOPIC, payload);

    Serial.println("Data published");
    lastTime = millis();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();

  // read the ADC value from the temperature sensor
  int adcVal = analogRead(PIN_LM35);
  // convert the ADC value to voltage in millivolt
  float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
  // convert the voltage to the temperature in °C
  float tempC = milliVolt / 10;
  tempC = tempC + 10;
  // convert the °C to °F
  float tempF = tempC * 9 / 5 + 32;


  // print the temperature in the Serial Monitor:
  Serial.print("Temperature: ");
  Serial.print(tempC);   // print the temperature in °C
  Serial.print("°C");
  Serial.print("  ~  "); // separator between °C and °F
  Serial.print(tempF);   // print the temperature in °F
  Serial.println("°F");

  data_led_display(tempC);
  publishData(tempC);
  delay(2000);

}

// put function definitions here:
