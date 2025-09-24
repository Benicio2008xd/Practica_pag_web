#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CESJT";
const char* password = "itisjtsmg";

// Pines
const int ledPin = 2;         // LED (PWM)
const int potPin = 34;        // Potenciómetro (Entrada analógica)

WebServer server(80);

bool ledEncendido = true;     // Estado de encendido del LED

// Plantilla HTML
const char pagina_template[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset='utf-8'>
  <title>Monitor Tensión</title>
  <style>
    body {
      background-color: red;
      text-align: center;
    }
    .titulo {
      font-family: 'Courier', monospace;
      font-weight: bold;
      font-style: italic;
      font-size: 32px;
      color: white;
    }
    .borde {
      width: 170px;
      height: 100px;
      border: 5px solid black;
      border-radius: 18px;
      background-color: black;
      margin: 0 auto;
    }
    .pantalla {
      width: 160px;
      height: 90px;
      border: 1px solid black;
      border-radius: 10px;
      background-color: cyan;
      margin: 4px;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 20px;
    }
    .pantalla span {
      font-weight: bold;
    }
    .boton {
      width: 80px;
      height: 45px;
      background-color: yellow;
      color: black;
      border: 2px solid black;
      border-radius: 80px;
      font-size: 14px;
      font-weight: bold;
      cursor: pointer;
    }
  </style>
</head>
<body>
  <h1 class='titulo'>MONITOR TENSIÓN</h1>
  <div class='borde'>
    <div class='pantalla'>  Tensión: <span>__TENSION__</span>V</div>
  </div>
  <p><a href='/toggle'><button class='boton'>__BOTON_TEXTO__</button></a></p>
</body>
</html>
)rawliteral";

// Función para alternar encendido/apagado
void toggle() {
  ledEncendido = !ledEncendido;
  server.sendHeader("Location", "/"); // dice que la página se vaya a /
  server.send(302, "text/plain", ""); // 302 dice que el /toggle no está y que vaya a la locación de antes
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(potPin, INPUT);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  int timeout = 20;
  while (WiFi.status() != WL_CONNECTED && timeout-- > 0) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nError de conexión, reiniciando...");
    delay(1000);
    ESP.restart();
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Página principal
  server.on("/", []() {
    int valorADC = analogRead(potPin);
    float voltaje = (valorADC / 4095.0) * 3.3;
    String strVolt = String(voltaje, 2);

    // Ajusta brillo del LED si está encendido
    if (ledEncendido) {
      int brilloPWM = map(valorADC, 0, 4095, 0, 255);
      ledcWrite(0, brilloPWM); // Canal 0 PWM
    } else {
      ledcWrite(0, 0);
    }

    // Prepara la página HTML
    String pagina = pagina_template;
    //pagina.replace busca el texto y lo reemplaza con lo que pongas.
    pagina.replace("__TENSION__", strVolt);
    pagina.replace("__BOTON_TEXTO__", ledEncendido ? "APAGAR" : "ENCENDER");
    server.send(200, "text/html", pagina);
  });

  server.on("/toggle", toggle);

  // Configurar PWM para el LED
  ledcSetup(0, 5000, 8);      // Canal 0, frecuencia 5kHz, resolución 8 bits
  ledcAttachPin(ledPin, 0);   // Conecta pin al canal PWM

  server.begin();
}

void loop() {
  server.handleClient();
}