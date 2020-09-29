
#include <WiFi.h>
#include <FirebaseESP32.h>

//digital sensors
#include "DHT.h"
#include "time.h"
#define DHTPIN 4
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);


#define FIREBASE_HOST "https://hackmit-df9ea.firebaseio.com/"
#define FIREBASE_AUTH "yourauthtoken"
#define WIFI_SSID "GUSEC"
#define WIFI_PASSWORD ""

//analog sensors
#define MQ5 34 //PIN NUMBER
#define MQ7 35 //PIN NUMBER
#define FIRE 33
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

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
 configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
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
  pinMode(INPUT,FIRE);
  
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
  boolean fire_ir = digitalRead(FIRE);
    //-------------------------------------------------------------
  //MQ5 Gas
  R0 = 2225;
   sensorValue = analogRead(35);
   sensor_volt = (sensorValue*3.3)/4096;
   RS_gas = (3.3-sensor_volt)/sensor_volt;
   ratio = RS_gas/R0; //Replace R0 with the value found using the sketch above
   x = 1538.46 * ratio;
   float ppm2 = pow(x,-1.709);


struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
    char timeMinute[3];
  strftime(timeMinute,3, "%M", &timeinfo);
    char timeSec[3];
  strftime(timeSec,3, "%S", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();

  //-------------------------------------------------------------
  //send the data to firebase
  //this may need revision
  //in example.ino not sure how firebseData gets initialized

  try{
    Firebase.setString(firebaseData, path + "/day of the week", timeWeekDay); //MONOXIDE data
  }catch(int e){
    Serial.println("dayweek ERROR: Exception no: " + e );
  }
  try{
    Firebase.setString(firebaseData, path + "/timehour", timeHour); //MONOXIDE data
  }catch(int e){
    Serial.println("hour ERROR: Exception no: " + e );
  }
   try{
    Firebase.setString(firebaseData, path + "/timeminute", timeMinute); //MONOXIDE data
  }catch(int e){
    Serial.println("hour ERROR: Exception no: " + e );
  }
   try{
    Firebase.setString(firebaseData, path + "/timesec", timeSec); //MONOXIDE data
  }catch(int e){
    Serial.println("hour ERROR: Exception no: " + e );
  }
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
    Firebase.setFloat(firebaseData, path + "/FIRE", fire_ir); //MONOXIDE data
  }catch(int e){
    Serial.println("Mfire ERROR: Exception no: " + e );
  }
  try{
    Firebase.setFloat(firebaseData, path + "/Temperature", t); //TEMPERATURE data in Celcius
  }catch(int e){
    Serial.println("TEMPERATURE ERROR: Exception no: " + e );
  }
}
