  #include <dht.h>

dht DHT;
#define lightthreshold 3 // duration in minutes for which the led bulb glows before checking ambient light
#define presthresh 80 // threshold value for photo resistor below which led bulb starts glowing
#define pumpthresh 800 // soil humidity threshold, above which water pump kicks in

#define DHT11_PIN A5
#define pumpPin 7
#define soilPin A2
#define ledPin 3
#define pres A1

#define lightthresh lightthreshold*60

int state = 0;
int value;
float avgtemp=0;
float avgpres=0;
float avghum=0;
float avgsoil = 0; // lower value --> high conductivity. ~1000 in air. ~4 with short. ~400 muddy soil
int lighttimer = 0;
int lightflag = 0;
void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  digitalWrite(pumpPin, HIGH);
  Serial.begin(9600); // Default communication rate of the Bluetooth module
  //  Serial.println("starting now.");
}
int i = 0;
void loop() {
  avgtemp = 0;
  avgpres=0;
  avghum=0;
  avgsoil = 0;
  int chk = DHT.read11(DHT11_PIN);
  
  //checking if light timer expired
  if(lightflag==1 && lighttimer >= lightthresh)
  {
     digitalWrite(ledPin, HIGH);
     lighttimer = 0;
     lightflag = 0;
  }
  
  //calculating all the sensor data
  for(i = 1; i <= 3; i++)
  {
    avgtemp = avgtemp*(i-1)/i + DHT.temperature/i;
    avghum = avghum*(i-1)/i + DHT.humidity / i;
    avgpres = avgpres *(i-1)/ i + analogRead(pres)/i;
    avgsoil = avgsoil*(i-1)/ i + analogRead(soilPin)/i;
    if(lightflag==1) lighttimer++;
    delay(1000);
  }


//checking if sufficient ambient light
if(lightflag == 0 && avgpres<presthresh)
{
  digitalWrite(ledPin, LOW);
  lightflag = 1;
  lighttimer++;
}

//checking if enough soil humidity
if(avgsoil >= pumpthresh)
{
 //  Serial.println("Soil moist is low");
  pinMode(pumpPin, LOW);
  delay(2000);
  pinMode(pumpPin, HIGH);
}
if(avgsoil <= pumpthresh){ //Serial.println("Off now should be");
  pinMode(pumpPin, LOW);} 

//printing the values '/' is the delimiter for the android app
  Serial.print(avgpres);
  Serial.print("/");
  Serial.print(avgtemp);
  Serial.print("/");
  Serial.print(avghum);
 Serial.print("/");
 Serial.print(avgsoil);
  Serial.print("/0/0/0/0 ");
  if(Serial.available() > 0){// Checks whether data is comming from the serial port

    state = Serial.read(); // Read the data from the serial port
 }
 if (state == '0') {
  digitalWrite(ledPin, LOW); // Turn LED OFF
  state = 0;
 }
 else if (state == '1') {
  digitalWrite(ledPin, HIGH);
  state = 0;
 } 
}
