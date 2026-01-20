#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

// ===== HTML =====
const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>ESP32 Control</title>
<style>
body { font-family: monospace; background:#111; color:#0f0; }
input, button { margin:5px; padding:5px; }
</style>
</head>
<body>

<h2>ESP32 Control</h2>

<input id="v1" placeholder="Pos X (0-180)">
<input id="v2" placeholder="Pos Y (0-180)">
<button onclick="send()">Send</button>

<script>
function send(){
  const v1 = document.getElementById("v1").value;
  const v2 = document.getElementById("v2").value;
  fetch(`/send?v1=${encodeURIComponent(v1)}&v2=${encodeURIComponent(v2)}`);
}
</script>

</body>
</html>
)rawliteral";

// Основной код
Servo x;
Servo y;

int posX = 0;
int posY = 0;
bool test = false;

void handleRoot() {
  server.send(200, "text/html", PAGE);
}

void handleSend() {
  if (server.hasArg("v1")) posX = server.arg("v1").toInt();
  if (server.hasArg("v2")) posY = server.arg("v2").toInt();

  test = true;

  server.send(204);
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  x.attach(12);
  y.attach(13);
  x.write(0);
  y.write(0);
  WiFi.softAP("Esp32_control", "TestESP32");
  Serial.println("AP started");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/send", handleSend);
  server.begin();
  Serial.println("HTTP server started");
}

// ===== LOOP =====
void loop() {
  server.handleClient();
  if(test){
    if (posX >= 0 and posX <= 180) {
      x.write(posX);
      Serial.println("Write posX: " + String(posX));
    } else {
      Serial.println("PosX out of range: " + String(posX));
    }
    if(posY >= 0 and posY <= 180){
      y.write(posY);
      Serial.println("Write posY: " + String(posY));
    } else {
      Serial.println("PosY out of range: " + String(posY));
    }
    test = false;
  }
}
