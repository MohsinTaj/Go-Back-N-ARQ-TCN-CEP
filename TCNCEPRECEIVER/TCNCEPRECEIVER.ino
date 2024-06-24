#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LiquidCrystal.h>
const int rs = D0, en = D1, d4 = D2, d5 = D3, d6 = D4, d7 = D5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const char *ssid = "ESP32-AP";
const char *password = "12345678";
const char *serverIP = "192.168.4.1";  // IP address of the ESP32 AP
const uint16_t serverPort = 80;
WiFiClient tcpClient;
ESP8266WebServer webServer(3000);

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>HDLC & Go Back N ARQ Configuration</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f0f0f0;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
      overflow: hidden;
    }
    .container {
      background-color: #ffffff;
      padding: 20px;
      border-radius: 8px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
      text-align: center;
      z-index: 1;
      position: relative;
    }
    h1 {
      color: #333333;
    }
    input[type="text"], select {
      width: 80%;
      padding: 10px;
      margin: 10px 0;
      border: 1px solid #cccccc;
      border-radius: 4px;
    }
    input[type="submit"] {
      background-color: #4CAF50;
      color: white;
      padding: 10px 20px;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }
    input[type="submit"]:hover {
      background-color: #45a049;
    }
    .message {
      margin-top: 20px;
      font-size: 1.2em;
    }
    .background {
      position: absolute;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: linear-gradient(45deg, #2196F3, #E91E63);
      background-size: 400% 400%;
      animation: gradient 15s ease infinite;
      z-index: 0;
    }
    @keyframes gradient {
      0% { background-position: 0% 50%; }
      50% { background-position: 100% 50%; }
      100% { background-position: 0% 50%; }
    }
  </style>
</head>
<body>
  <div class="background"></div>
  <div class="container">
    <h1>HDLC & Go Back N ARQ Configuration</h1>
    <form action="/send" method="POST">
      <label for="serverIP">Select Receiver:</label>
      <select name="serverIP" id="serverIP">
        <option value="192.168.4.1">Receiver 1 (Default)</option>
        <option value="192.168.4.2">Receiver 2</option>
        <option value="192.168.4.3">Receiver 3</option>
        <!-- Add more options as needed -->
      </select>
      <br>
      <input type="text" name="numbers" placeholder="Enter data (comma separated)" required />
      <br>
      <input type="submit" value="Send Data" />
    </form>
    <div class="message" id="message"></div>
  </div>
  <script>
    document.querySelector("form").addEventListener("submit", function(e) {
      e.preventDefault();
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/send", true);
      xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
      xhr.onreadystatechange = function() {
        if (xhr.readyState == 4 && xhr.status == 200) {
          document.getElementById("message").innerText = xhr.responseText;
        }
      };
      var numbers = document.querySelector("input[name='numbers']").value;
      xhr.send("numbers=" + encodeURIComponent(numbers));
    });
  </script>
</body>
</html>

)rawliteral";

void handleRoot() {
  webServer.send(200, "text/html", htmlPage);
}

void handleSend() {
  if (webServer.hasArg("numbers")) {
    String numStr = webServer.arg("numbers");
    Serial.print("Sending numbers: ");
     lcd.clear();
  
  // Set the cursor to the top left corner
  lcd.setCursor(0, 0);
  
  // Print the message on the first line
  lcd.print("Sending numbers: ");
  
  // Set the cursor to the second line
  lcd.setCursor(0, 1);
  
  // Print the numbers
  lcd.print(numStr);
  
  // Delay to keep the message on the screen
  delay(1000);
    Serial.println(numStr);

    if (tcpClient.connect(serverIP, serverPort)) {
      tcpClient.print(numStr);
      tcpClient.print("\r\n");

      unsigned long startTime = millis();
      while (tcpClient.available() == 0 && millis() - startTime < 5000) {
        delay(10);
      }

      if (tcpClient.available()) {
        String response = tcpClient.readStringUntil('\r');
        if (response.indexOf("ACK") != -1) {
          webServer.send(200, "text/plain", "Success");
        } else {
          webServer.send(200, "text/plain", "Fail");
        }
      } else {
        webServer.send(200, "text/plain", "Fail");
      }
    } else {
      webServer.send(200, "text/plain", "Connection to server failed");
    }
  } else {
    webServer.send(200, "text/plain", "No numbers provided");
  }
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
   lcd.print("Client is ON!");
  delay(1000);
  lcd.clear();
  // Connect to the Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
   lcd.setCursor(0, 0);
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print("Wi-Fi...");
  delay(1000);
  lcd.clear();
  while (WiFi.status() != WL_CONNECTED) {
    
    delay(1000);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  lcd.setCursor(0,0);
  lcd.print("Connected to ");
  lcd.setCursor(0,1);
  lcd.print("Wi-Fi");
  lcd.clear();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0,0);
lcd.print(WiFi.localIP());
delay(2000);
  lcd.clear();
  
  // Start the web server
  webServer.on("/", handleRoot);
  webServer.on("/send", HTTP_POST, handleSend);
  webServer.begin();
  Serial.println("Web server started");
  lcd.setCursor(0,0);
  lcd.print("Web server");
  lcd.setCursor(0,1);
  lcd.print("started");
}

void loop() {
  webServer.handleClient();
}
