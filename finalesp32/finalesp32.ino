#include <WiFi.h>
#include <WebServer.h>
// Wi-Fi credentials
const char* ssid = "Informatics College";
const char* password = "INf0RM@TICs2025";

WiFiServer server(80); // Create a Wi-Fi server on port 80

// Variable to store the temperature value received from Arduino
float temperature = 0.0;
float humidity = 0.0;

// Variables to store data received from Arduino
int lightIntensity = 0;
int soilMoisture = 0;
int fanStatus = 0;
int lightStatus = 0;
int pumpStatus = 0;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Start the Wi-Fi server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  // Check if data is available from Arduino
  if (Serial.available() > 0) {
    // Assuming the Arduino sends data in the format: "Temperature: <value>, Humidity: <value>"
    String data = Serial.readStringUntil('\n');
    Serial.println("Data received from Arduino: " + data);

    // Parse temperature and humidity values
    if (data.indexOf("Temperature:") >= 0 && data.indexOf("Humidity:") >= 0 && data.indexOf("Light Intensity:") >= 0 && data.indexOf("Soil Moisture:") >= 0 && data.indexOf("Fan:") >= 0 && data.indexOf("Light:") >= 0 && data.indexOf("Pump:") >= 0){
      int tempIndex = data.indexOf("Temperature:") + 12; // Find the starting index of temperature value
      int humidityIndex = data.indexOf("Humidity:") + 9;  // Find the starting index of humidity value
      int intensityIndex = data.indexOf("Light Intensity:") + 16; // Find the starting index of intensity value
      int soilIndex = data.indexOf("Soil Moisture:") + 14; // Starting index of soil moisture value
      int fanIndex = data.indexOf("Fan:") + 4;
      int lightIndex = data.indexOf("Light:") + 6;
      int pumpIndex = data.indexOf("Pump:") + 5;

      temperature = data.substring(tempIndex, data.indexOf(',', tempIndex)).toFloat();

      // Extract humidity value
      humidity = data.substring(humidityIndex, data.indexOf(',', humidityIndex)).toFloat();
      //Extract light Intensity
      lightIntensity = data.substring(intensityIndex, data.indexOf(',', intensityIndex)).toInt(); // Convert to integer
      //Extract Soil Moisture value
      soilMoisture = data.substring(soilIndex, data.indexOf(',', soilIndex)).toInt(); // Convert to integer

      fanStatus = data.substring(fanIndex, data.indexOf(',', fanIndex)).toInt();
      lightStatus = data.substring(lightIndex, data.indexOf(',', lightIndex)).toInt();
      pumpStatus = data.substring(pumpIndex, data.indexOf(',', pumpIndex)).toInt();

      // Print for verification (optional, can be removed)
      Serial.println("Parsed Temperature: " + String(temperature));
      Serial.println("Parsed Humidity: " + String(humidity));
      Serial.println("Parsed Light Intensity: " + String(lightIntensity));
      Serial.println("Parsed Soil Moisture: " + String(soilMoisture));
      Serial.println("Parsed Fan Status: " + String(fanStatus));
      Serial.println("Parsed Light Status: " + String(lightStatus));
      Serial.println("Parsed Pump Status: " + String(pumpStatus));
    }
  }
  WiFiClient client = server.available(); // Check if a client is connected
  if (client) {
    Serial.println("Client connected!");
    String request = client.readStringUntil('\r'); // Read the client's request
    Serial.println(request);
    client.flush();

    // Check if the request is to fetch DHT22 data
    if (request.indexOf("GET /data") >= 0) {
      // Create a JSON response with the data received from Arduino
      String jsonResponse = "{";
      jsonResponse += "\"temperature\": " + String(temperature, 2) + ",";
      jsonResponse += "\"humidity\": " + String(humidity, 2) + ",";
      jsonResponse += "\"lightIntensity\": " + String(lightIntensity) + ",";
      jsonResponse += "\"soilMoisture\": " + String(soilMoisture) + ",";
      jsonResponse += "\"fanStatus\": " + String(fanStatus) + ",";
      jsonResponse += "\"lightStatus\": " + String(lightStatus) + ",";
      jsonResponse += "\"pumpStatus\": " + String(pumpStatus);
      jsonResponse += "}";

      // Send HTTP response
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Access-Control-Allow-Origin: *");
      client.println("Connection: keep-alive");
      client.println();
      client.println(jsonResponse);

      Serial.println("Response sent: " + jsonResponse);
    } else {
      // Handle 404 Not Found
      client.println("HTTP/1.1 404 Not Found");
      client.println("Access-Control-Allow-Origin: *");
      client.println("Connection: keep-alive");
      client.println();
    }
  }
}