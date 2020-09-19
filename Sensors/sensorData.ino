
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

//digital sensors
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);


#define FIREBASE_HOST "https://hackmit-df9ea.firebaseio.com/"
#define FIREBASE_AUTH "gGcWbTzz1naLRKpR3EBPcHg3rBdzDyvMwNtGxPQ1"
#define WIFI_SSID "GUSEC"
#define WIFI_PASSWORD ""

//analog sensors
#define MQ5 34 //PIN NUMBER
#define MQ7 35 //PIN NUMBER


//Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;

//Firebase URI Path
String path = "SensorData";

//MQ7
float RS_gas = 0;
float ratio = 0;
float sensorValue = 0;
float sensor_volt = 0;
float R0 = 2450.0;

//setup firebase
void setup()
{
  /*Connect to Wifi*/
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  //miscellaneous sensor setup
  dht.begin();

  /*Connect to firebase*/
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "small");
}

void loop()
{
  //--------------------------------------------------------------
  //DHT Temperature Sensor
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  //-------------------------------------------------------------
  //MQ7 Carbon Monoxide
  R0 = 2450;
   sensorValue = analogRead(34);
   sensor_volt = (sensorValue*3.3)/4096;
   RS_gas = (3.3-sensor_volt)/sensor_volt;
   ratio = RS_gas/R0; //Replace R0 with the value found using the sketch above
   float x = 1538.46 * ratio;
   float ppm = pow(x,-1.709);

  //-------------------------------------------------------------
    //-------------------------------------------------------------
  //MQ5 Gas
  R0 = 2225;
   sensorValue = analogRead(35);
   sensor_volt = (sensorValue*3.3)/4096;
   RS_gas = (3.3-sensor_volt)/sensor_volt;
   ratio = RS_gas/R0; //Replace R0 with the value found using the sketch above
   x = 1538.46 * ratio;
   float ppm2 = pow(x,-1.709);

  //-------------------------------------------------------------
  //send the data to firebase
  //this may need revision
  //in example.ino not sure how firebseData gets initialized

  try{
    Firebase.setFloat(firebaseData, path + "/Monoxide", ppm); //MONOXIDE data
  }catch(int e){
    Serial.println("MONOXIDE ERROR: Exception no: " + e );
  }
  
    try{
    Firebase.setFloat(firebaseData, path + "/LPG", ppm2); //MONOXIDE data
  }catch(int e){
    Serial.println("LPG ERROR: Exception no: " + e );
  }

  try{
    Firebase.setFloat(firebaseData, path + "/Temperature", t); //TEMPERATURE data in Celcius
  }catch(int e){
    Serial.println("TEMPERATURE ERROR: Exception no: " + e );
  }
}
