
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h> 
#include <SPIFFS.h> 
#include <ESP32Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>


#define FORMAT_SPIFFS_IF_FAILED true
#define OLED_SDA 22
#define OLED_SCL 21           
#define ServoPin 19
#define Led1Pin 18
#define BtnPin 12
#define TempPin 33
#define LightPin 32
#define Led2Pin 23

Adafruit_SH1106 display(22, 21);

OneWire oneWire(TempPin);
DallasTemperature sensor(&oneWire);
Servo servo;
String IP;
const char* ssid = "iPhone Даниил";
const char* password = "loxotron";
char tempData[3] = {0,0,0};
char brightData[3] = {0,0,0};
float butData=0;
AsyncWebServer server(80);
AsyncWebSocket ws("/test");
int lenght = 0;


void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
 
  if(type == WS_EVT_CONNECT){
    //Serial.println("Websocket client connection received");
  } else if(type == WS_EVT_DISCONNECT){
    //Serial.println("Client disconnected");
  } else if(type == WS_EVT_DATA){
    lenght = len;

    switch((char)data[0]){
      case 'T':
    for(int i = 1; i < 4;i++){
      tempData[i-1] = data[i];
    }
    break;
  case 'I':
    for(int i = 1; i < 4;i++){
      brightData[i - 1] = data[i];
    }
    break;
  case 'B':
    butData = 1;
    break;
  case 'V':
    butData = 0;
    break;
    }
  }
}

void pageRoot(AsyncWebServerRequest * request)
{
  request->send(SPIFFS, "/index.html", "text/html");
}


void setup(){
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        return;
  }
  Serial.begin(9600);
 
  WiFi.begin(ssid, password);
 
  
 
 
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);


  server.on("/", HTTP_GET, pageRoot);
  server.begin();
  Serial.begin(9600);
  sensor.begin();
  sensor.setResolution(12);
  
  servo.attach(ServoPin);
  pinMode(Led1Pin,OUTPUT);
  pinMode(BtnPin,INPUT);
  pinMode(LightPin,INPUT);
  pinMode(Led2Pin, OUTPUT);
  analogReadResolution(11); 
  display.begin(SH1106_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();
}
int digits2int(int *list, int n) {
  int res = 0, i, Pow;
  for (i = n-1, Pow = 1; i>=0; --i, Pow*=10) {
    res += list[i] * Pow;
  }
  return res;
}
int LedWebData = 0;
int TempWebData = 0;
int DataBright[3] ;
int DataTemp[3];
int PrevLed = 0;
int TekLed = 0;
int PrevTemp = 0;
int TekTemp = 0;
int LedData;
int tekButton = LOW;
int prevButton = LOW;
int counter = 0;

void loop() {
  int Light = analogRead(32);
  float temperature;  
sensor.requestTemperatures();
temperature = sensor.getTempCByIndex(0);
int brightness = 0;
tekButton=digitalRead(BtnPin);
if (tekButton == HIGH && prevButton == LOW)
{
 counter++;
}
prevButton=tekButton;
if(counter % 2 == 0){
  
  for(int i = 0; i < 3; i++){
   DataBright[i] = (int)tempData[i] - 48; 
  }

    for(int i = 0; i < 3; i++){
   DataTemp[i] = (int)brightData[i] - 48 ;
  }

  LedWebData = digits2int(DataBright, 3) - 100;
  TempWebData = digits2int(DataTemp, 3) - 100;
  
analogWrite(Led1Pin, LedWebData);
analogWrite(Led2Pin, TempWebData);
  if(butData==0){
    servo.write(48);
  }
  else if(butData == 1)
  {
    servo.write(118);
  }
 // Serial.println(analogRead(LightPin));

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(" Temperature: ");
  display.print(temperature);
  display.println();
  display.print(" Light:  ");
  display.print(Light);
  display.println();
  display.print(" IP: 172.20.10.12 ");
  display.display();
  display.clearDisplay();
}
else{
  digitalWrite(Led1Pin, LOW);
switch(analogRead(32)){
  case 700 ... 3000:
 brightness = 0; 
  break;
  
  case 600 ... 699:
 brightness = 75;
  break;
   
  case 500 ... 599:
 brightness = 100;
  break;

  case 400 ... 499:
 brightness = 150;
  break;

  case 300 ... 399:
 brightness = 175;
  break;

  case 200 ... 299:
  brightness = 200;
  break;
  
  case -1 ... 199:
 brightness =  255;
  break;
}

analogWrite(Led1Pin, brightness);
  

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(" Temperature: ");
  display.print(temperature);
  display.println();
  display.print(" Light: ");
  display.print(Light);
  display.display();
  display.clearDisplay();
}
Serial.println(Light);
}
