
/*
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2020 mobizt
 * 
 * This example is for FirebaseESP32 Arduino library v 3.7.3 and later
 *
*/

//This example shows how to read, store and update database using get, set, push and update functions.

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>


#define FIREBASE_HOST "https://hackmit-df9ea.firebaseio.com/"
#define FIREBASE_AUTH "gGcWbTzz1naLRKpR3EBPcHg3rBdzDyvMwNtGxPQ1"
#define WIFI_SSID "GUSEC"
#define WIFI_PASSWORD ""

#define MQ5 34 //CHANGE mq5pin TO CUSTOM PIN NUMBER 


//Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;

//Sensor instances
DHT temp_sensor(5, DHT11);

/*
Setup firebase 
*/
void setup()
{

  /*Connect to Wifi*/
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  /*Analog sensor initialization here*/
  pinMode(MQ5, INPUT);

  /*Connect to firebase*/
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  /*Read data here (or somewhere else if better)*/

}

void loop()
{
  temperature = temp_sensor.readTemperature();
  int mq5gas_value = analogRead(MQ5);
}
