#include <WiFi.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

const int sensorPin = A0;  // Soil moisture sensor pin
const int redLEDPin = 9;   // Red LED pin
const int yellowLEDPin = 10; // Yellow LED pin
const int greenLEDPin = 11;  // Green LED pin

WiFiClient wifi;
const char* ssid     = "Amagaraï's iPhone";
const char* password = "King Ama10";
const char* token = "e5N0jMV2SBCuMaZSQ5Xkdp:APA91bHjGj8AbuARFycW8Vp2iPfArDa_XaaHhKDzOKsMsPtvL-VWsiRkMcy982DA6l26OtIpWv6J8jJCpK95_KETBGQoRQHPiDeh9e6AxZzVU6ayfSsrXlJEzhUpl2B9M7OcwpT2Zz-f";
// HttpClient http(wifi, "http://34.71.80.157", 8887);
void setup() {
  pinMode(redLEDPin, OUTPUT);
  pinMode(yellowLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  Serial.begin(9600);  // Initialize serial communication for debugging

  if(WiFi.status() != WL_CONNECTED ){
    Serial.println("Wifi not connected...");
    WiFi.begin(ssid, password); 
  } 
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("...");
  } 
}


void loop() {
  // Create an HttpClient object with the WiFiClient instance
    HttpClient http(wifi, "34.71.80.157", 8887);
  int sensorValue = analogRead(sensorPin);  // Read the soil moisture sensor value
  Serial.print("Soil moisture level: ");
  Serial.println(sensorValue);


  if (sensorValue <= 400) {  // Adjust the threshold values based on your sensor
    // Soil is fully watered (100%-60% moisture)
    digitalWrite(greenLEDPin, HIGH);
    digitalWrite(yellowLEDPin, LOW);
    digitalWrite(redLEDPin, LOW);
  } else if (sensorValue <= 450) {
    // Soil is drying out (30%-60% moisture)
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(yellowLEDPin, HIGH);
    digitalWrite(redLEDPin, LOW);
  } else {
    // Soil is very dry (below 30% moisture)
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(yellowLEDPin, LOW);
    digitalWrite(redLEDPin, HIGH);

    StaticJsonDocument<200> jsonDoc;
    jsonDoc["titre"] = "ForoKow | Urgent";
    jsonDoc["contenu"] = "Votre champs a besoin d'eau";

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    sendData(jsonString);
  }


  delay(3000);  // Wait for 1 second before the next reading
}


void sendData(const String& jsonData) {
  

  Serial.print("Connected to: "); Serial.println(WiFi.SSID());
  Serial.print("Your IP: "); Serial.println(WiFi.localIP());
  
  
  String url = "/api/fcm/notif/one/"+String(token);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(jsonData);
 
  
 if (httpCode > 0) {
    Serial.printf("POST code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      Serial.println(response);
    }
  } else {
    Serial.printf("POST failed: error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}