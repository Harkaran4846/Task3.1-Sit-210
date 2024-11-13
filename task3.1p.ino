#include <WiFiNINA.h>         // Include library for WiFi support on Arduino
#include <Wire.h>             // Include library for I2C communication
#include <BH1750.h>           // Include library for the BH1750 light sensor

// WiFi network credentials
char ssid[] = "Abcdf";              // Your WiFi network SSID
char password[] = "12345678";       // Your WiFi network password
WiFiClient wifiClient;              // Create a client object for WiFi connection

// Create an instance of the BH1750 light sensor library
BH1750 lightSensor;

const char* serverName = "maker.ifttt.com"; // Server name for IFTTT
String webhookPath = "https://maker.ifttt.com/trigger/Light Sensor/with/key/gfOCNi_53Nrzt5E86Wsu0BN2p_JjdFQOx4tnCxiI7M5";  
// Webhook path for the IFTTT trigger
String httpRequestData;  // String to store data to be sent in the HTTP request

void setup() {
  Serial.begin(9600);           // Start serial communication at 9600 baud
  while (!Serial);              // Wait for Serial to initialize

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {   // Attempt to connect to WiFi
    WiFi.begin(ssid, password);             // Start connection with credentials
    delay(2000);                            // Wait 2 seconds between retries
    Serial.print(".");                      // Print dot for each connection attempt
  }

  Serial.println("\nConnected to WiFi!");   // Confirm successful WiFi connection

  Wire.begin();  // Initialize I2C communication for the sensor
  lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);  // Start the BH1750 sensor in continuous high-resolution mode
  Serial.println("BH1750 Sensor Initialized");          // Confirm sensor initialization
}

void loop() {
  // Read light level from the BH1750 sensor in Lux
  float lux = lightSensor.readLightLevel();  
  Serial.print("Light Intensity: ");  
  Serial.print(lux);  
  Serial.println(" LUX"); 

  // Prepare the HTTP GET request with sensor data
  httpRequestData = "?value1=" + String(lux);  

  // Check if light intensity is above 300 or below 50 to trigger the webhook
  if (lux >= 300 || lux < 50) {  
    sendToIFTTT();   // Trigger the webhook if conditions are met
  }

  delay(45000);  // Wait 45 seconds before the next check
}

void sendToIFTTT() {
  // Connect to the IFTTT server on port 80 (HTTP)
  if (wifiClient.connect(serverName, 80)) { 
    // Send an HTTP GET request with the path and data
    wifiClient.println("GET " + webhookPath + httpRequestData + " HTTP/1.1");  
    wifiClient.println("Host: " + String(serverName));  // Add the host header
    wifiClient.println("Connection: close");            // Close connection after response
    wifiClient.println();  

    // Wait for and print the server's response to the Serial Monitor
    while (wifiClient.connected()) {  
      if (wifiClient.available()) {  
        char c = wifiClient.read();  
        Serial.print(c);  
      }
    }
  }

  wifiClient.stop();  // Disconnect from the server
  Serial.println();   // Print a new line for readability in Serial Monitor
}
