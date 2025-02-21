#define BLYNK_TEMPLATE_ID "TMPL2xtV0bUPh"
#define BLYNK_TEMPLATE_NAME "SMART SILO"
#define BLYNK_AUTH_TOKEN "obNRbgYWYE2EQ_B90hW8yYI1Bm8L5mUo"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Kibet";  // Type your WiFi name
char pass[] = "123456789";  // Type your WiFi password

#define DHTPIN 2          // Digital pin connected to DHT sensor
#define DHTTYPE DHT11     // Using DHT11 sensor
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Use dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Error: Failed to read from DHT sensor!");
    return;
  }

  // Send data to Blynk
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, t);

  // Display temperature and humidity in Serial Monitor
  Serial.println("-------------------------------");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" °C");
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %RH");
  Serial.println("-------------------------------");

  // Alerts for temperature and humidity (No Email, Only Blynk Notifications)
  if (t > 30) {
    Blynk.logEvent("temp_high_alert", "Warning! Temperature above 40°C!");
    Serial.println("ALERT: Temperature above 40°C! Notification sent.");
  }
  
  if (t < 10) {
    Blynk.logEvent("temp_low_alert", "Warning! Temperature below 10°C!");
    Serial.println("ALERT: Temperature below 10°C! Notification sent.");
  }
  
  if (h > 70) {
    Blynk.logEvent("humidity_high_alert", "Warning! Humidity above 70%!");
    Serial.println("ALERT: Humidity above 70%! Notification sent.");
  }
  
  if (h < 20) {
    Blynk.logEvent("humidity_low_alert", "Warning! Humidity below 20%!");
    Serial.println("ALERT: Humidity below 20%! Notification sent.");
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(2500L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
