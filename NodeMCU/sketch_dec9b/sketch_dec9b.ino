#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid = "Sol_2.4G";
const char* password = "235961749";

// Firebase database URL (Realtime DB)
String FIREBASE_HOST = "https://scoring-system-embeded-default-rtdb.asia-southeast1.firebasedatabase.app/";
String FIREBASE_API_KEY = "AIzaSyC8AqtEShzw2Oic6bFVAsMCXx0Vl_C95Kc";  // Database Secret

// --------------------------------
// UART from STM32 (GPIO pins)
// --------------------------------
#define RXD1 18   // ESP32-S3 RX
#define TXD1 17   // ESP32-S3 TX (not used, but required)
HardwareSerial stm32(1);  // UART1

void setup() {
  Serial.begin(115200);
  stm32.begin(115200, SERIAL_8N1, RXD1, TXD1);
  delay(500);

  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  handleIncomingData("idle,0");
}

void loop() {
  if (stm32.available()) {
    String msg = stm32.readStringUntil('\n');
    msg.trim();

    if (msg.length() > 0) {
      Serial.print("Received from STM32: ");
      Serial.println(msg);

      // Parse and send
      handleIncomingData(msg);
    }
  }
}

void handleIncomingData(String raw) {
  int commaIndex = raw.indexOf(',');
  if (commaIndex == -1) {
    Serial.println("⚠️ Invalid data format (missing comma)");
    return;
  }

  // Split status and score
  String status = raw.substring(0, commaIndex);
  String scoreStr = raw.substring(commaIndex + 1);

  status.trim();
  scoreStr.trim();

  int score = scoreStr.toInt();

  Serial.print("Status: "); Serial.println(status);
  Serial.print("Score: "); Serial.println(score);

  sendToFirebase(status, score);
}

void sendToFirebase(String status, int score) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi disconnected!");
    return;
  }

  HTTPClient http;
  String url = FIREBASE_HOST + "/current_game.json?auth=" + FIREBASE_API_KEY;

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  // Construct clean JSON
  String json = "{\"status\": \"" + status + "\", \"score\": " + String(score) + "}";

  int code = http.PUT(json);
  Serial.print("Firebase HTTP code: ");
  Serial.println(code);

  if (code > 0) {
    Serial.println("Firebase response:");
    Serial.println(http.getString());
  }

  http.end();
}