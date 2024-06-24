#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>
const int rs = D0, en = D1, d4 = D2, d5 = D3, d6 = D4, d7 = D5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const char *ssid = "ESP32-AP";
const char *password = "12345678";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.print("Server is ON!");
  delay(1000);
  lcd.clear();
  // Set up Wi-Fi access point
  WiFi.softAP(ssid, password);
  lcd.setCursor(0, 0); 
  lcd.print(ssid);
  lcd.setCursor(0, 1);
  lcd.print(password);
  delay(2000);
  lcd.clear();
  Serial.println("Access Point Started");
  lcd.setCursor(0, 0);
  lcd.print("Access Point");
  lcd.setCursor(0, 1);
  lcd.print("Started");
  delay(2000);
  lcd.clear();
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  lcd.setCursor(0, 0);
  lcd.print("IP Address: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.softAPIP());
  delay(2000);
  
  // Start the server
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Connected.");
    while (client.connected()) {
      if (client.available()) {
        // Read the incoming data
        String request = client.readStringUntil('\r');
        Serial.print("Received: ");
        Serial.println(request);
        lcd.setCursor(0, 0);
        lcd.print("Received: ");
        lcd.setCursor(0, 1);
        lcd.print(request);
        delay(2000);
        lcd.clear();
        // Acknowledge receipt
        client.print("ACK\r\n");
        lcd.setCursor(0, 0);
        lcd.print("Sent ");
        lcd.setCursor(0, 1);
        lcd.print("acknowledgement.");
        delay(2000);
        Serial.println("Sent acknowledgment.");
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
