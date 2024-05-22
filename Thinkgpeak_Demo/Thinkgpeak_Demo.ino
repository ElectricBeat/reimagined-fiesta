#include <Time.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

char* ssid = "Vasantha-wifi";                               // your network SSID (name) 
char* password = "Vasantha@123";                            // your network password

WiFiClient  client;
const unsigned long channelID = 2529489;                    // Your ThingSpeak Channel ID
const char * myWriteAPIKey = "BZTMC86ZV7RZ6S7A";
#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 4;                                   // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus);                                // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);                        // Pass our oneWire reference to Dallas Temperature sensor 

const int updateInterval = 60000;                           // Update interval in milliseconds (1 minute)
unsigned long  Interval;
unsigned long lastUpdateTime = 0;                           // Stores the last update time
const int sensorPin = 14;
volatile int pulseCount = 0;
volatile unsigned long previousTime = 0;
volatile unsigned long timeDifference = 1;
volatile float Ans;
int RPM,SPM;
unsigned long currentTime;

void setup() 
{
  Serial.begin(115200);                                       // Start the Serial Monitor
  sensors.begin();                                            // Start the DS18B20 sensor

  ThingSpeak.begin(client);                                   // Initialize ThingSpeak

  WiFi.begin(ssid, password);                                 // Connect to Wi-Fi
  
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED)                       // Wait for connection
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");                                          // Connected to Wi-Fi
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(sensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, RISING);  
}

void loop() 
{
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  ThingSpeak.setField(1, temperatureC);                         // Field 1: Temperature

  Serial.print("timeDifference: ");
  Serial.print(timeDifference);
  Serial.print("\t");
  RPM = (((60*1000)/timeDifference));
  if(RPM <= 300)
  {
    SPM = RPM;
  }
  Serial.print("RPM: ");
  Serial.print(SPM);
  Serial.print("\t");
  ThingSpeak.setField(2, SPM);                                  // Field 2: RPM
  
  if(RPM >= 500)
  {
//    RPM = 000;
//    Serial.print("SPM: ");
//    Serial.print(SPM);
//    Serial.print("\t");
    Serial.print("RPM: ");
    Serial.print(RPM);
    Serial.print("\t");
    ThingSpeak.setField(2, SPM);                                  // Field 2: RPM
    RPM = 0;
  }
  
//  RPM = 1/Ans;
//  Serial.print("RPM: ");
//  Serial.print(RPM);
//  Serial.print("\t");

//  Serial.print("Interval: ");
//  Serial.print(Interval);
//  Serial.print("\t");
//  Serial.print("previousTime: ");
//  Serial.print(previousTime);
//  Serial.println("\t");

  unsigned long NowcurrentTime = millis();
  Interval = NowcurrentTime - previousTime;
  if(Interval >= updateInterval)
  {
    Interval = 0;
    previousTime = NowcurrentTime;

    Serial.print("Time out: RPM is restart : 000");
    timeDifference = 1;
    Ans = 0;
    RPM = 0;
//    Serial.print("RPM : ");
//    Serial.print(RPM);
//    Serial.print("\t");
  }

//  ThingSpeak.setField(1, temperatureC);                         // Field 1: Temperature
//  ThingSpeak.setField(2, RPM);                                  // Field 2: RPM

  Serial.println(" ");

  int statusCode = ThingSpeak.writeFields(channelID, myWriteAPIKey);
  if (statusCode == 200) 
  {
    Serial.println("Data sent to ThingSpeak successfully!");
  } 
  else 
  {
    Serial.print("Error sending data to ThingSpeak: ");
    Serial.println(statusCode);
  }
delay(15000);
//previousTime = currentTime;                                       // Update previousTime for the next pulse
}

ICACHE_RAM_ATTR void countPulse() 
{
  currentTime = millis();
  if (currentTime > previousTime) 
  {
    timeDifference = currentTime - previousTime;
  }
  else 
  {
    timeDifference = (unsigned long)(millis() + (65535 - previousTime));   // Account for timer overflow
  }
  previousTime = currentTime;                                       // Update previousTime for the next pulse
}
