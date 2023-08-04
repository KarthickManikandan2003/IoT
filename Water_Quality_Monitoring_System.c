#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>
#include <DHT.h>
#define DHTPIN 0
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE,15);


#include<ESP8266Wifi.h>
const int potPin=A0;
float ph;
float Value=0;
int buf[10],temp;
int  avgValue;
char ssid[] = "OPPO A9 2020";
char password[] = "b7702cf4ba05";
const char* host="api.thingspeak.com";
const char* writeAPIKey = "UQ18LSFWU5QF3HTV";



void setup() {
  Serial.begin(9600);
  dht.begin();

  delay(1000);
  Serial.println("Connecting to");
        Serial.println(ssid);
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
}

void loop() {
   float temperature = dht.readTemperature();
    if(isnan(temperature)){
    return ;
  }

  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(potPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  int avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
   float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=(3.5*phValue)-2;                      //convert the millivolt into pH value

 

  WiFiClient client;
  const int httpPort=80;
  if(!client.connect(host,httpPort)){
    return;
  }

  String url = "/update?key=";
  url+=writeAPIKey;
  url+="&field1=";
  url+=String(phValue);
  url+="&field2=";
  url+=String(temperature);


  client.print(String("GET ")+url+"HTTP/1.1\r\n" +
              "Host:" + host +"\r\n" +
              "Connection: close\r\n\r\n");

  Serial.print("PH value");
  Serial.print(phValue);
  Serial.print("Temperature");
  Serial.print(temperature);


  client.stop();
  Serial.println("Wait for 15 sec to update next datapack in thingsspeak");
  delay(1000);


}
