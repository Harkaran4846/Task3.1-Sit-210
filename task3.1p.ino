#include <WiFiNINA.h>  
#include <Wire.h>     
#include <BH1750.h>    

// WiFi network credentials
char ssid[] = "Abcdf";              // Your WiFi SSID
char password[] = "12345678";       // Your WiFi Password
WiFiClient wifiClient;             

// Create an instance of the BH1750 light sensor library 
BH1750 lightSensor;


const char* serverName = "maker.ifttt.com"; 
String webhookPath = "https://maker.ifttt.com/trigger/Light Sensor/with/key/gfOCNi_53Nrzt5E86Wsu0BN2p_JjdFQOx4tnCxiI7M5";  
String httpRequestData;  // String to store the data to be sent in the HTTP request

void setup() {
  Serial.begin(9600); 
  while (!Serial); 

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {  
    WiFi.begin(ssid, password);  
    delay(2000); 
    Serial.print(".");  
  }

  Serial.println("\nConnected to WiFi!");  

  Wire.begin();  // Initialize I2C communication for the sensor
  lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);  // Initialize the BH1750 sensor in continuous high-resolution mode
  Serial.println("BH1750 Sensor Initialized");  
}

void loop() {
  // Read light level from the BH1750 sensor in Lux
  float lux = lightSensor.readLightLevel();  
  Serial.print("Light Intensity: ");  
  Serial.print(lux);  
  Serial.println(" LUX"); 

  // Prepare the HTTP GET request with sensor data
  httpRequestData = "?value1=" + String(lux);  

  // Check if light intensity is above 300 or below 50, then trigger the webhook
  if (lux >= 300 || lux < 50) {  
    sendToIFTTT();  
  }

  delay(45000);  
}

void sendToIFTTT() {
  // Connect to the IFTTT server on port 80 (HTTP)
  if (wifiClient.connect(serverName, 80)) { 
    // HTTP GET request with the path and data
    wifiClient.println("GET " + webhookPath + httpRequestData + " HTTP/1.1");  
    // Send the host name in the HTTP header
    wifiClient.println("Host: " + String(serverName));  
    wifiClient.println("Connection: close");  
    wifiClient.println();  

    // Wait for the server to respond and print the response to the Serial Monitor
    while (wifiClient.connected()) {  
      if (wifiClient.available()) {  
        char c = wifiClient.read();  
        Serial.print(c);  
      }
    }
  }

  wifiClient.stop();  
  Serial.println();  // Print a new line for readability in Serial Monitor
}
