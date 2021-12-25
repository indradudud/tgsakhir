#define BLYNK_PRINT Serial
#include <SPI.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include "TRIGGER_WIFI.h"
#include "TRIGGER_GOOGLESHEETS.h"

#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht11(DHTPIN, DHTTYPE);

//Configure Blynk
char auth[] = "cZywoUP-5V8IyEER0ksieYkFAqttLVtE";
char ssid[]="Mina";
char pass[]="tikitaka";

// Configure Firebase
#define FIREBASE_HOST "interfacing-1385b-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "uOMPtk7jjYTTAmQRphyxInDjj8OPfR3Q9Jn2TO0v"

// Configure Google Sheet
char column_name_in_sheets[ ][20] = {"value1","value2"};
String Sheets_GAS_ID = "AKfycbwnlAaOgtRtVA_mGXerIkvypEGopkzFtI6niTnUQQ8JRcLxx2V0oLT-zd0xYB7BDUzTLw";
int No_of_Parameters = 2;

// Wifi
#define WIFI_SSID "Mina"
#define WIFI_PASSWORD "tikitaka"

FirebaseData firebaseData;

// Connect ke app blynk
SimpleTimer timer;
void sendSensor(){
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();

  if (isnan(h) || isnan(t)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
}

void setup() {
  Serial.begin(115200);

  //Blynk
  dht11.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while(WiFi.status()!= WL_CONNECTED){
    Serial.print(".");
    delay(400);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Blynk.begin(auth, ssid, pass);
  dht11.begin();
  timer.setInterval(1000L, sendSensor);

  //Google Sheet
  while (!Serial);
  dht11.begin();
  WIFI_Connect("Mina","tikitaka");
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
}

void loop() {
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();

  //Blynk
  Blynk.run();
  timer.run();
  delay(1000);

  //Firebase
  if (isnan(h) || isnan(t)){
    Serial.println("Failed to read from DHT sensor!");
    delay(1000);
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println();
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println();
  
  if (Firebase.setFloat(firebaseData, "/Result/Humidity", h)){
    Serial.println("Humidity Sent");
  }else{
    Serial.println("Humidity Failed to Send");
    Serial.println("Error: "+firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/Result/Temperature", t)){
    Serial.println("Temperature Sent");
  }else{
    Serial.println("Temperature Failed to Send");
    Serial.println("Error: "+firebaseData.errorReason());
  }

  //Google Sheet
  Data_to_Sheets(No_of_Parameters,  h,  t);
  Serial.println();
  delay(5000);                       
}
