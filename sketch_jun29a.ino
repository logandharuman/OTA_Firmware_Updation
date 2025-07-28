#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "vivo 1938";
const char* password = "logan@2104";

const int ledPin = 2; // GPIO pin for the LED

const char* firmwareUrl = "https://drive.usercontent.google.com/download?id=1RcWouLllIBlUguQVWMnSCk-y9mxzfIY8&export=download"; // Direct download link for firmware file              

//https://drive.google.com/file/d/1RcWouLllIBlUguQVWMnSCk-y9mxzfIY8/view?usp=drive_link

#define CURRENT_FIRMWARE_VERSION "1.0.2" // Update this line with the new firmware version

unsigned long previousMillis = 0;
const long interval = 1000; // Interval for blinking the LED
bool ledState = false;

unsigned long updateCheckMillis = 0;
const long updateInterval = 60000; // Interval for checking updates (1 hour)

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(ledPin,LOW);  //newly added
}

void loop() {
  unsigned long currentMillis = millis();

  // Blink the LED at a regular interval
//  if (currentMillis - previousMillis >= interval) {
//    previousMillis = currentMillis;
//    ledState = !ledState;
//    digitalWrite(ledPin, ledState ? HIGH : LOW);
//    Serial.print("LED is now ");
//    Serial.println(ledState ? "ON" : "OFF");
//  }

  
  digitalWrite(ledPin,HIGH); //newly added
  delay(1000);
  digitalWrite(ledPin,LOW); //newly added
  delay(1000);
  
  // Check for updates every hour
  if (currentMillis - updateCheckMillis >= updateInterval) {
    updateCheckMillis = currentMillis;
    checkForUpdates();
  }
}

void checkForUpdates() {
  HTTPClient http;

  Serial.println("Checking for firmware updates...");

  // Download the firmware binary
  http.begin(firmwareUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    bool canBegin = Update.begin(contentLength);

    if (canBegin) {
      Serial.println("Begin OTA update");
      WiFiClient* client = http.getStreamPtr();
      size_t written = Update.writeStream(*client);

      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully");

        // Compare the embedded version in the new firmware
        const char* newFirmwareVersion = "1.0.1"; // This should be read from the new firmware
        if (strcmp(CURRENT_FIRMWARE_VERSION, newFirmwareVersion) != 0) {
          if (Update.end()) {
            Serial.println("OTA update finished!");
            if (Update.isFinished()) {
              Serial.println("Update successfully completed. Rebooting.");
              ESP.restart();
            } else {
              Serial.println("Update not finished? Something went wrong.");
            }
          } else {
            Serial.println("Error Occurred. Error #: " + String(Update.getError()));
          }
        } else {
          Serial.println("Firmware version is the same, no update needed.");
          Update.abort(); // Abort the update
        }
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
      }
    } else {
      Serial.println("Not enough space to begin OTA");
    }
  } else {
    Serial.println("Firmware not found at " + String(firmwareUrl));
  }

  http.end();
}
