#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"

#define DHTTYPE DHT11 // Type of the temperature sensor
const int DHTPin = 2; // Pin used for the DHT11 sensor is Pin D1 = GPIO5
DHT dht(DHTPin, DHTTYPE); // Initialize DHT sensor

const char* ssid = "Kibet"; // Your WiFi name or SSID
const char* password = "123456789"; // Your WiFi password

//----------------------------------------Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------

WiFiClientSecure client; // Create a WiFiClientSecure object.

String GAS_ID = "AKfycbz5oNY9rr4OrJSpCiLKOQHF77g9LnTeG-V9fOqs_NSIioo9X2AEzmjEVaa-i3FxeDbWFw"; // Spreadsheet script ID

void setup() {
    Serial.begin(115200);
    delay(500);

    dht.begin();  // Start reading DHT11 sensor
    delay(500);

    WiFi.begin(ssid, password); // Connect to WiFi
    Serial.println("");

    //----------------------------------------Wait for connection
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    //----------------------------------------

    Serial.println("");
    Serial.print("Successfully connected to: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    //----------------------------------------

    client.setInsecure();
}

void loop() {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    int h = dht.readHumidity();
    float t = dht.readTemperature(); // Read temperature in Celsius

    // Check if any reads failed and exit early (to try again)
    if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        delay(500);
        return;
    }
    String Temp = "Temperature: " + String(t) + " °C";
    String Humi = "Humidity: " + String(h) + " %";
    Serial.println(Temp);
    Serial.println(Humi);

    sendData(t, h); // Calls the sendData subroutine
}

// Subroutine for sending data to Google Sheets
void sendData(float tem, int hum) {
    Serial.println("==========");
    Serial.print("Connecting to ");
    Serial.println(host);

    //----------------------------------------Connect to Google host
    if (!client.connect(host, httpsPort)) {
        Serial.println("Connection failed");
        return;
    }
    //----------------------------------------

    //----------------------------------------Processing and sending data
    String string_temperature = String(tem);
    String string_humidity = String(hum, DEC);
    String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature + "&humidity=" + string_humidity;
    Serial.print("Requesting URL: ");
    Serial.println(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: BuildFailureDetectorESP8266\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("Request sent");
    //----------------------------------------

    //----------------------------------------Checking whether the data was sent successfully or not
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            Serial.println("Headers received");
            break;
        }
    }
    String line = client.readStringUntil('\n');
    if (line.startsWith("{\"state\":\"success\"")) {
        Serial.println("ESP8266/Arduino CI successful!");
    } else {
        Serial.println("ESP8266/Arduino CI has failed");
    }
    Serial.print("Reply was: ");
    Serial.println(line);
    Serial.println("Closing connection");
    Serial.println("==========");
    Serial.println();
    //----------------------------------------
}
