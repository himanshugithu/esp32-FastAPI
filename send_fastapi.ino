#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // Include the ArduinoJson library


const char* ssid = "pass : FFH";                                 // Replace with your Wi-Fi SSID
const char* password = "11111111";                               // Replace with your Wi-Fi password
const char* serverName = "http://192.168.43.176:8080";  // Replace with your FastAPI server IP and endpoint

#define DHTPIN 4       // DHT11 connected to GPIO 4
#define DHTTYPE DHT11  // Define the type of DHT sensor
DHT dht(DHTPIN, DHTTYPE);
bool promptShown = false;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
   dht.begin();
}

void loop() {
  if (!promptShown) {
    Serial.println("\n\n\nChoose Operation:\n 1.GET\n 2.POST\n 3.EXIT\n");
    promptShown = true;
  }

  if (Serial.available() > 0) {
    String incomingString = Serial.readStringUntil('\n');
    incomingString.trim();

    if (incomingString == "1") {
      Serial.println("GET");
      get();
    } else if (incomingString == "2") {
      Serial.println("POST");
      // Call the POST function with the data from the GET request (if available)
      postDHT11Data();
    } else if (incomingString == "3") {
      Serial.println("EXIT");
    } else {
      Serial.println("Invalid Input");
    }

    promptShown = false;
  }
}

void get() {
  if (WiFi.status() == WL_CONNECTED) {
    String getServer = String(serverName) + "/dht11/latest";
    HTTPClient http;
    http.begin(getServer);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response from server: " + response);

      // Parse the JSON response
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, response);
      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
      }

      // Print the JSON object
      serializeJsonPretty(doc, Serial);
      Serial.println();

    } else {
      Serial.println("Error on sending GET: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void postDHT11Data() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    temperature = 0.0;
    humidity = 0.0;
    
  }

  StaticJsonDocument<200> jsonDoc;
  jsonDoc["temperature"] = temperature;
  jsonDoc["humidity"] = humidity;

  String jsonData;
  serializeJson(jsonDoc, jsonData);

  post(jsonData);  // Send the JSON data
}

void post(String jsonData) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String postServer = String(serverName) + "/dht11";  // Append /post to serverName
    http.begin(postServer);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response from server: " + response);
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}