#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

const int sensorPin = A0;       // Soil moisture sensor pin
const int redLEDPin = 9;        // Red LED pin
const int yellowLEDPin = 10;    // Yellow LED pin
const int greenLEDPin = 11;     // Green LED pin

WiFiClient wifi;
const char* ssid = "Flybox-3B2493";
const char* password = "yH3eXsp2CGEh";
const char* token = "e5N0jMV2SBCuMaZSQ5Xkdp:APA91bHjGj8AbuARFycW8Vp2iPfArDa_XaaHhKDzOKsMsPtvL-VWsiRkMcy982DA6l26OtIpWv6J8jJCpK95_KETBGQoRQHPiDeh9e6AxZzVU6ayfSsrXlJEzhUpl2B9M7OcwpT2Zz-f";

HttpClient http(wifi, "34.71.80.157", 8887);

void setup() {
  pinMode(redLEDPin, OUTPUT);
  pinMode(yellowLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  Serial.begin(9600);  // Initialize serial communication for debugging

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(greenLEDPin, LOW);
  }
  digitalWrite(greenLEDPin, HIGH);
  Serial.println("Connected to WiFi");
  
} 

void loop() {
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

  delay(3000);  // Wait for 3 seconds before the next reading
}

void sendData(const String& jsonData) {
  Serial.print("Connected to: "); Serial.println(WiFi.SSID());
  Serial.print("Your IP: "); Serial.println(WiFi.localIP());
  
  String url = "/api/fcm/notif/one/" + String(token);

  http.beginRequest();
  http.post(url);
  http.sendHeader("Content-Type", "application/json");
  http.sendHeader("Content-Length", jsonData.length());
  http.beginBody();
  http.print(jsonData);
  http.endRequest();

  int httpCode = http.responseStatusCode();
  String response = http.responseBody();
  
  if (httpCode > 0) {
    Serial.print("POST code: "); Serial.println(httpCode);
    Serial.println(response);
  } else {
    Serial.print("POST failed with status: "); Serial.println(httpCode);
  }
}
