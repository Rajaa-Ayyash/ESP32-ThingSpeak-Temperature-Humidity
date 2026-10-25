
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* apiKey = "INSERT_THINGSPEAK_WRITE_API_KEY";
const char* server = "http://api.thingspeak.com/update";


const char* universityID = "12220345";

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("ESP32 ThingSpeak Project");
  Serial.print("University ID: ");
  Serial.println(universityID);

  WiFi.begin(ssid, password, 6);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read DHT22!");
    delay(20000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(server) +
                 "?api_key=" + String(apiKey) +
                 "&field1=" + String(temperature, 2) +
                 "&field2=" + String(humidity, 2);

    http.begin(url);

    int httpCode = http.GET();

    if (httpCode == 200) {
      String response = http.getString();

      if (response.toInt() > 0) {
        Serial.println("Data sent successfully!");
        Serial.println("Entry ID: " + response);
      } else {
        Serial.println("ThingSpeak did not accept the update.");
      }
    } else {
      Serial.print("HTTP Error: ");
      Serial.println(httpCode);
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected!");
  }

  Serial.println("----------------------");
  delay(20000);
}
