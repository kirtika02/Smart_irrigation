#include <SoftwareSerial.h>
SoftwareSerial SIM900(9, 8); //RX, TX
#include <DHT.h>


#define DHTPIN A0 //DHT11 Sensor connected to A0 Pin
#define DHTTYPE DHT11 //DHT11 Sensor declared
DHT dht(DHTPIN, DHTTYPE) // Initialize DHT Sensor for Arduino
int maxTemp = 25;
float hum; //Stores humidity value
float temp; //Stores temperature value
int SensorValue;
#define SOILSENSOR A1
#define MAXDRYNESS 666

#define WATERPIN 10
#define WATERDELAY 5000
#define WATERPOSTDELAY 5000
String textMessage;
String message, motorState;

void setup()
{
  digitalWrite(WATERPIN, LOW);
  pinmode(WATERPIN, OUTPUT);
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  SIM900.begin(9600);   // Setting the baud rate of GSM Module  
  dht.begin();
  delay(10000);  // Give time to log on to network
  Serial.println("GSM SIM900A is connected to netweork.");
  SIM900.print("AT+CMGF=1\r");  // Setting SIM900 to SMS Mode
  delay(100);
  SIM900.println("AT+CNMI=2,2,0,0,0\r"); // AT command to receive a live message
  delay(100);
}


void loop()
{
  
  if(SIM900.available()>0){
   textMessage = SIM900.readString();
   Serial.print(textMessage);    
   delay(10);   
   }
   if(textMessage.indexOf("motor on")>=0){
    //lcd.clear();
   digitalWrite(WATERPIN, HIGH);
    motorState = "on";
    Serial.println("motor set to ON");  
    textMessage = "ON";
    //lcd.setCursor(0, 1 );
    //lcd.print("motor on");
    delay(1000);      
  }
  if(textMessage.indexOf("motor off")>=0){
    // Turn off relay and save current state
    digitalWrite(WATERPIN, LOW);
    motorState = "off";
    Serial.println("motor set to OFF");
    textMessage = "OFF";
  } 
    if(textMessage.indexOf("state motor")>=0){
    String message = "motor is " + motorState;
    sendSMS(message);
    Serial.println("motor state resquest");
    textMessage = "";
  } 
  
  Soil_Moisture();

  TempHum(); 
  if(textMessage.indexOf("check sensors")>=0){
    String message = "temperature is: " + temp + " humidity is: " + hum + " soil moisture level is:" + SensorValue;
    sendSMS(message);

    Serial.println("temperature is: " + temp + " humidity is: " + hum + " soil moisture level is:" + SensorValue);
    textMessage = "";
  } 
}

void Soil_Moisture()
{
  SensorValue = analogRead(SOILSENSOR); // Reading Soil Moisture Sensor
  Serial.print("Soil Moisture = ");
  Serial.println(SensorValue);

  if(SensorValue >= MAXDRYNESS)
  {
     // if the soil is too dry start watering for 3/4 a second then
     // wait for 5 seconds before monitoring again
     Serial.println("Soil dry, start watering");
     digitalWrite(WATERPIN, HIGH);
     sendSMS1();
     delay(WATERDELAY);
     sendSMS2();
  }
  else if(SensorValue < MAXDRYNESS)
  {
     Serial.println("Soil has enough moisture, stop watering.");
     digitalWrite(WATERPIN, LOW);
     sendSMS2();
     delay(WATERPOSTDELAY);
  }
   
  delay(20000);
}

void TempHum()
{
  delay(2000);
  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  if (temp>maxTemp)
  {
    digitalWrite(WATERPIN, HIGH);
  }
  // else
  // {
  //   digitalWrite(WATERPIN, LOW);
  // }
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  delay(10000); //Delay 2 sec.
}

void sendSMS1()
{
  SIM900.print("AT+CMGF=1\r"); // AT command to set SIM900 to SMS mode 
  delay(100);
  SIM900.println("AT + CMGS = \"+919920445044\"");// change to your sim900's your phone number 
  delay(100);

  SIM900.println("Soil moisture is low. WATER PUMP: ON");// Send the SMS 
  delay(100);
  SIM900.println((char)26);// End AT command with a ^Z, ASCII code 26 
  delay(100);
  SIM900.println();  // Give module time to send SMS
  delay(5000);  
}

void sendSMS2()
{
  SIM900.print("AT+CMGF=1\r"); // AT command to set SIM900 to SMS mode 
  delay(100);
  SIM900.println("AT + CMGS = \"+919920445044\"");// change to your sim900's your phone number 
  delay(100);
  SIM900.println("Soil is good");// Send the SMS 
  delay(100);
  SIM900.println((char)26);// End AT command with a ^Z, ASCII code 26 
  delay(100);
  SIM900.println();  // Give module time to send SMS
  delay(5000);  
}