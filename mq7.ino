
float RS_gas = 0;
float ratio = 0;
float sensorValue = 0;
float sensor_volt = 0;
float R0 = 2450.0;//this would be 2225 for thw lpg mq sensor , also change the input pin numbers and variables
 
void setup() {
 Serial.begin(9600);
}
 
void loop() {
   sensorValue = analogRead(34);
   sensor_volt = (sensorValue*3.3)/4096;
   RS_gas = (3.3-sensor_volt)/sensor_volt;
   ratio = RS_gas/R0; //Replace R0 with the value found using the sketch above
   float x = 1538.46 * ratio;
   float ppm = pow(x,-1.709);
   Serial.print("PPM: ");
   Serial.println(ppm);
   delay(1000);
}
