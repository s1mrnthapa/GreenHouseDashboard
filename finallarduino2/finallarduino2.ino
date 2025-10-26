#include "DHT.h" // Include the DHT library
#include <Servo.h>
// DHT sensor setup
#define DHTPIN 2          // GPIO pin connected to the DHT22 data pin
#define DHTTYPE DHT22     // Specify the sensor type (DHT22)
DHT dht(DHTPIN, DHTTYPE);

// Ultrasonic sensor and servo motor setup
#define TRIG_PIN 9             // GPIO pin connected to the trigger pin of the ultrasonic sensor
#define ECHO_PIN 10            // GPIO pin connected to the echo pin of the ultrasonic sensor
Servo myServo;                // Create servo object
#define SERVO_PIN 13           // GPIO pin connected to the servo motor

// LDR and light relay setup
#define LDR_PIN A0             // Analog pin connected to LDR
#define LIGHT_RELAY_PIN 6      // Digital pin connected to relay module for light
int low_lightThreshold = 50; // Threshold for low light detection (adjust as needed)

// Fan relay setup
#define FAN_RELAY_PIN 5        // Digital pin connected to relay module for fan

// Soil moisture sensor setup
#define SOIL_SENSOR_PIN A1     // Analog pin connected to soil moisture sensor
#define SOIL_RELAY_PIN 8       // Digital pin connected to relay module for soil moisture control
int soilMoistureThreshold = 50; // Threshold for soil moisture (adjust as needed)

// Status variables
int fanStatus = 0;
int lightStatus = 0;
int pumpStatus = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();
  // Set relay pins as output
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(SOIL_RELAY_PIN, OUTPUT);

  // Turn relays OFF initially (assuming active HIGH relays)
  digitalWrite(LIGHT_RELAY_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, HIGH);
  digitalWrite(SOIL_RELAY_PIN, HIGH);

    // Set ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize servo motor
  myServo.attach(SERVO_PIN);
  myServo.write(90); // Move servo to initial position
}


void loop() {
  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();


  // Check if any reading failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  } 
  // Read light intensity
  int lightIntensity = analogRead(LDR_PIN);
  lightIntensity = map(lightIntensity, 1024, 10, 0, 100);
  // Control fan based on temperature
  if (temperature < 29) {
    Serial.println("Temperature above 24°C: Turning fan ON");
    digitalWrite(FAN_RELAY_PIN, LOW); // Turn fan ON
    fanStatus = 1;
  } else {
    Serial.println("Temperature below 24°C: Turning fan OFF");
    digitalWrite(FAN_RELAY_PIN, HIGH); // Turn fan OFF
    fanStatus = 0;
  }
  
  // Control light relay based on light intensity
  if (lightIntensity < low_lightThreshold) {
    Serial.println("Low light detected: Turning light ON");
    digitalWrite(LIGHT_RELAY_PIN, LOW); // Turn light ON
    lightStatus = 1;
  } else {
    Serial.println("Sufficient light: Turning light OFF");
    digitalWrite(LIGHT_RELAY_PIN, HIGH); // Turn light OFF
    lightStatus = 0;
  }
  if (temperature < 16){
    digitalWrite(LIGHT_RELAY_PIN, LOW); // Turn light ON
    lightStatus = 1;
  }

  int soilMoisture = analogRead(SOIL_SENSOR_PIN);
  soilMoisture = map(soilMoisture, 1024, 10, 0, 100);

  // Control soil moisture relay based on soil moisture level
  if (soilMoisture < soilMoistureThreshold) {
    Serial.println("Soil moisture low: Turning water pump ON");
    digitalWrite(SOIL_RELAY_PIN, LOW); // Turn water pump ON (assuming active LOW relay)
    pumpStatus = 1;
  } else {
    Serial.println("Soil moisture sufficient: Turning water pump OFF");
    digitalWrite(SOIL_RELAY_PIN, HIGH); // Turn water pump OFF
    pumpStatus = 0;
  }

   // Ultrasonic sensor distance measurement
  long duration;
  int distance;

  // Send a 10-microsecond pulse to the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo pin and calculate the distance
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  // Control servo motor based on distance
  if (distance > 0 && distance < 30) {
    // Serial.println("Object detected within 30 cm: Opening servo to 90째");
    myServo.write(90); // Move servo to 90째
  } else {
    // Serial.println("Object not detected: Closing servo to 0째");
    myServo.write(1); // Move servo to 0째
  }

  // Print all sensor data in one line for easier parsing
  Serial.print("Temperature: ");
  Serial.print(temperature, 2); // Two decimal points
  Serial.print(", Humidity: ");
  Serial.print(humidity, 2);    // Two decimal points
  Serial.print(", Light Intensity: ");
  Serial.print(lightIntensity);
  Serial.print(", Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.print(", Fan: ");
  Serial.print(fanStatus);
  Serial.print(", Light: ");
  Serial.print(lightStatus);
  Serial.print(", Pump: ");
  Serial.println(pumpStatus);

  delay(1000); // Wait before the next loop iteration
}
