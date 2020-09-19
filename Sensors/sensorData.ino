
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
#include <DHT.h>
#include <IRremote.h>


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

//Sensor instances
DHT temp_sensor(5, DHT11);

//part of IR sensor
IRrecv irrecv(7);
decode_results results;

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
  pinMode(MQ7, INPUT);

  //miscellaneous sensor setup
  
  //part of IR sensor
  irrecv.enableIRIn();
  irrecv.blink13(true);

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
  //other 3 sensor quantities
  temperature = temp_sensor.readTemperature(); //temp
  int mq5gas_value = analogRead(MQ5); //gas
  int mq7gas_value = analogRead(MQ7); //monoxide

  //part of IR sensor
  if(irrecv.decode(@results)){
        Serial.println(results.value, HEX);
        Serial.println(results.value, DEC); //infared
        irrecv.resume();
        Serial.println();
  }

  //send the data to firebase
  //this may need revision
  //in example.ino not sure how firebseData gets initialized
  try{
    Firebase.set(firebaseData, path + "/Infared"); //IR data
  }catch(int e){
    Serial.println("INFARED ERROR: Exception no: " + e + " occured.");
  }

  try{
    Firebase.set(mq5gas_value, path + "/Gas"); //GAS data
  }catch(int e){
    Serial.println("GAS ERROR: Exception no: " + e + " occured.");
  }

  try{
    Firebase.set(mq7gas_value, path + "/Monoxide"); //MONOXIDE data
  }catch(int e){
    Serial.println("MONOXIDE ERROR: Exception no: " + e + " occured.");
  }

  try{
    Firebase.set(firebaseData, path + "/Temperature"); //TEMPERATURE data
  }catch(int e){
    Serial.println("TEMPERATURE ERROR: Exception no: " + e + " occured.");
  }
}
