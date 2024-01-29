#include <WiFiNINA.h>
#include <BH1750.h>
#include <Wire.h>

const char* ssid = "Hakeem";
const char* password = "Hussain@123";

WiFiClient client;                  // Create a Wi-Fi client object
BH1750 lightMeter;                  // Create a BH1750 light sensor object

const char* hostName = "maker.ifttt.com"; // IFTTT server hostname
const String pathName = "/trigger/Light/with/key/f-vMYFm_51vx1FAZjdgIgonnFOOxaVBjvLyK2rSDobq"; // IFTTT webhook path
String queryString = "?value1=57&value2=25"; // Initial query parameters for the webhook

void setup() {
  Serial.begin(9600);               // Start serial communication for debugging
  while (!Serial);

  WiFi.begin(ssid, password);       // Connect to Wi-Fi network
  Wire.begin();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to Wi-Fi...");
  }

  if (client.connect(hostName, 80)) { // Attempt to establish a connection with the IFTTT server on port 80 (HTTP)
    Serial.println("Connection established successfully with the server on port 80....");
  } else {
    Serial.println("Connection not established");
  }

  lightMeter.begin();               // Initialize the BH1750 light sensor
  Serial.println("Connected to Wi-Fi...");
}

void loop() {
  Serial.print("Reading light sensor data.......");
  float lux = lightMeter.readLightLevel(); // Read light level from the sensor

  queryString = "?value1=" + String(lux); // Prepare the query string
  Serial.println(queryString);

  if (lux > 500) { // Check if the light level exceeds the threshold
    // Make an HTTP GET request to the IFTTT webhook
    client.print("GET " + pathName + queryString + " HTTP/1.1\r\n");
    client.print("Host: " + String(hostName) + "\r\n");
    client.print("Connection: close\r\n\r\n");
    client.flush();

    // Wait for a response from the server and print it to the serial monitor
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }

    client.stop(); // Disconnect from the server
    Serial.println();
    Serial.println("Disconnected from the server....");
  }

  queryString = ""; // Reset the query string
  delay(3000); // Wait for 3 seconds before looping
}