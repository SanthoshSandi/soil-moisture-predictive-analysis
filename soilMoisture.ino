#include<SPI.h>
#include<Ethernet.h>

//LOCAL NETWORK SETTINGS
byte mac[] ={0xD4,0x28,0xB2,0xFF,0xA0,0xA1};

//Thinkspeak Settings
char thingSpeakAddress[]="api.thinkspeak.com";
String writeAPIKey="";
const int updateThingSpeakInterval = 86400*1000;
long lastConnectionTime=0;
boolean lastConnected=false;
int failedCounter=0;

//Initialise Arduino Ethernet Client
EthernetClient client;

#include "DHT.h"
#define DHTPIN 2     // DHT Output Pin is connected to the digital Pin 2 of Arduino
#define DHTTYPE DHT11   // Defining the type of Humidity Sensor
DHT dht(DHTPIN, DHTTYPE);

void setup()
  {
   Serial.begin(9600);
   analogReference(INTERNAL);
   //starting Ethernet on Arduino
   startEthernet();
   pinMode(A0,INPUT);
   //Serial.print("AT+CMGF=1");
   //Serial.print("AT+CMGS=\"+919677381443\"");
   dht.begin();
  }

void loop()
  {
   int h = dht.readHumidity();
   int t = dht.readTemperature();
    if (isnan(t) || isnan(h))
      {
        Serial.println("Failed to read from DHT");
      } 
   else
      {
        Serial.print("Humidity: "); 
        Serial.print(h);
        Serial.print(" %\t");
        Serial.println();
        Serial.print("Temperature: "); 
        Serial.print(t);
        Serial.println(" *C");
      }
      
//----------------------------------------------------END OF DHT SENSOR--------------------------------------------------------------//

//-------------------------------------------BEGINNING OF SOIL MOISTURE SENSOR-------------------------------------------------------//
  int soil_moisture=digitalRead(4);  // read from analog pin A3
  int soil_moisture1=analogRead(A0);
 // Serial.println(soil_moisture);
  Serial.println(soil_moisture1);

  Serial.print("Soil Condition: ");

  if(soil_moisture1<300)
    {
      Serial.println("Dry soil.. INSUFFICIENT WATER");
      //Serial.write(0x1A);
      //while(1)
        {
        }
    }

  if((soil_moisture1>300)&&(soil_moisture1<700)) 
    {
      Serial.println("Humid soil... Proper Water");
      //while(1)
        {
        }
    }

  if((soil_moisture1>700)&&(soil_moisture1<950))
    {
      Serial.println("No soil.. Only Water... Excess Water");
      //Serial.write(0x1A);
      //while(1)
        {
        }
    }

if(client.available())
  {
   char c=client.read();
   Serial.print(c);   
  }
 
//disconnecting ThingSpeak
if(!client.connected()&&lastConnected)
  {
    Serial.print("ThingSpeak Disconnected");
    Serial.println();
    client.stop();
  }
  
//Update ThingSpeak
if(!client.connected()&&(millis()-lastConnectionTime > updateThingSpeakInterval))
  {
   updateThingSpeak("field1="+analogValue0); 
  }
  
if(failedCounter?3)
  {
   StartEthernet(); 
  }
  
lastConnected=client.connected();  
}

void updateThingSpeak(String tsData)
{
  if(client.connect(thingSpeakAddress,80))
    {
      client.print("POST /update HTTP/1.1 \n");
      client.print("Host: api.thingspeak.com \n");
      client.print("Connection: close \n");
      client.print("X-THINGSPEAKAPIKEY:"+writeAPIKey+"\n");
      client.print("Content-Type:application/x-www-form-urlencoded \n");
      client.print("Content-Length:");
      client.print(tsData.Length());
      client.print("\n \n");
      
      client.print(tsData);
      lastConnectionTime=millis();
      
      if(client.connected())
        {
          Serial.println("Connecting To ThingSpeak...");
          Serial.println();
          
          failedCounter=0;
        }\
      
      else
        {
         failedCounter++;
         Serial.println("Connection to ThingSpeak failed("+String(failedCounter,DEC)+")");
         Serial.println(); 
        }  
   else
    {
    failedCounter++;
    Serial.println("Connection to ThingSpeak failed("+String(failedCounter,DEC)+")");
    Serial.println();
    lastConnectedTime=millis();
    }
  }
 
 void startEthernet()
  {
   client.stop();
   Serial.println("Connecting Arduino to Network...");
   Serial.println();
  
   delay(1000);
  
  //Connecting to Internet and obtaining an IP address using DHCP
  
  if(Ethernet.begin(mac)==0)
     {
      Serial.println("DHCP failed, reset Arduino to try again");
      Serial.println();
     }
  else
    {
     Serial.println("Arduino Connnected to network using DHCP");
     Serial.println();
    }
    delay(1000);
  }  