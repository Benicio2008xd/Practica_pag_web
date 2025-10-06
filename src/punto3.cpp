#include <WiFi.h>
#include <WebServer.h>
// --- Configuración de Red ---
const char* ssid = "CESJT";
const char* password = "itisjtsmg";

// --- Configuración del Servidor y Hardware ---
WiFiServer server(80); // Puerto 80 es el estándar para HTTP
const int ledPin = 2; 
int leds[3] = {0,1,2}; //verde, amarillo, rojo
String modo;
// --- Página Web ---
// Se guarda en memoria de programa (PROGMEM) para ahorrar RAM. Es más eficiente que usar un String.
const char pagina_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Control LED ESP32</title>
  <style>
    body {
    justify-content: center;
    background-color: white;
    }
    html { font-family: sans-serif; text-align: center; }
    .lento{ display: inline-block; background-color:rgb(255, 0, 0); border 3px solid black; color: white;width: 200px;
height: 200px
           padding: 16px 30px; text-decoration: none; font-size: 23px; margin: 2px; cursor: pointer; border-radius:
100px; }
    .normal { background-color:rgb(251, 255, 0); border 3px solid black; color: white;width: 200px; height: 200px
           padding: 16px 30px; text-decoration: none; font-size: 23px; margin: 2px; cursor: pointer; border-radius:
100px; }       
    .rapido{ display: inline-block; background-color: #4CAF50; border: 3px solid black; color: white; width: 200px;
height: 200px
           padding: 16px 30px; text-decoration: none; font-size: 23px; margin: 2px; cursor: pointer; border-radius:
100px; }
    .emergencia { background-color:rgb(255, 123, 0); border 3px solid black; color: white;width: 200px; height: 200px
           padding: 16px 30px; text-decoration: none; font-size: 23px; margin: 2px; cursor: pointer; border-radius:
100px; }
    .titulo {
        font-family: Impact;
        font-size: 45px;
        font-weight: bold;
        text-decoration: underline;
    }
  </style>
</head>
<body>
    <div class="titulo">
      <h1>SEMÁFORO</h1>
        <p>Seleccione el modo del semaforo:</p>
    </div>
    <div class="boton">
        <p>
    <a href="/lento"><button class="btn lento">lento</button></a>
  </p>
  <p>
    <a href="/normal"><button class="btn normal">normal</button></a>
  </p>
  <p>
    <a href="/rapido"><button class="btn rapido">rapido</button></a>
  </p>
  <p>
    <a href="/emergencia"><button class="btn emergencia">emergencia</button></a>
  </p>
    </div>
     <p><span>Modo: <b>XXX</b></span></p>
</body>
</html>
)rawliteral";


void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // --- Conexión a la red Wi-Fi ---
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int timeout = 20; // 20 intentos de 500ms = 10 segundos
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    Serial.print(".");
    timeout--;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFallo la conexion. Reiniciando...");
    delay(1000);
    ESP.restart();
  }

  // --- Iniciar Servidor ---
  Serial.println("\nConexion Wi-Fi exitosa!");
  Serial.print("IP del servidor: http://");
  Serial.println(WiFi.localIP());
  server.begin();
  
}


void loop() {
  // 1. Esperar a que un cliente se conecte
  WiFiClient client = server.available();
  if (!client) {
    return; // Si no hay cliente, no hacer nada y salir del loop
  }

  Serial.println("\n[Nuevo Cliente Conectado]");

  // 2. Leer la petición del cliente línea por línea
  String header = "";           // Para guardar la petición HTTP completa
  String currentLine = "";      // Para guardar la línea actual que estamos leyendo

  // El bucle se ejecuta mientras el cliente esté conectado
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();   // Leer un caracter a la vez
      header += c;              // Añadirlo a la petición completa

      // Si el caracter es un salto de línea, procesamos la línea
      if (c == '\n') {
        // Una línea en blanco (dos saltos de línea seguidos) indica el fin de la petición HTTP
        if (currentLine.length() == 0) {

            // 3. Interpretar la petición y actuar
            if (header.indexOf("GET /lento") >= 0) {
                modo= "Lento";
                Serial.println("modo lento activado");
                digitalWrite(leds[0],LOW);
                digitalWrite(leds[1],LOW);
                digitalWrite(leds[2],HIGH);
                delay(70000);
                digitalWrite(leds[0],LOW);
                digitalWrite(leds[1],HIGH);
                digitalWrite(leds[2],LOW);
                delay(3000);
                digitalWrite(leds[0],HIGH);
                digitalWrite(leds[1],LOW);
                digitalWrite(leds[2],LOW);
                delay(50000);
            } else if (header.indexOf("GET /normal") >= 0) {
                modo= "normal";
                Serial.println("modo normal activado");
                digitalWrite(leds[0],LOW);
                digitalWrite(leds[1],LOW);
                digitalWrite(leds[2],HIGH);
                delay(45000);
                digitalWrite(leds[0],LOW);
                digitalWrite(leds[1],HIGH);
                digitalWrite(leds[2],LOW);
                delay(2000);
                digitalWrite(leds[0],HIGH);
                digitalWrite(leds[1],LOW);
                digitalWrite(leds[2],LOW);
                delay(65000);
            } else if (header.indexOf("GET /rapido") >= 0) {
                modo= "rapido";
                Serial.println("modo rapido activado");
                digitalWrite(leds[0],LOW);
                digitalWrite(leds[1],LOW);
                digitalWrite(leds[2],HIGH);
                delay(20000);
                digitalWrite(leds[0],LOW);
                digitalWrite(leds[1],HIGH);
                digitalWrite(leds[2],LOW);
                delay(1000);
                digitalWrite(leds[0],HIGH);
                digitalWrite(leds[1],LOW);
                digitalWrite(leds[2],LOW);
                delay(40000);
            } else if (header.indexOf("GET /emergencia") >= 0) {
                modo= "emergencia";
                Serial.println("modo emergencia activado");
                digitalWrite(leds[0],LOW);
                digitalWrite(leds[1],HIGH);
                digitalWrite(leds[2],HIGH);
                delay(1500);
            }  
            String pagina = String(pagina_html);
            pagina.replace("XXX", "<b>" + modo + "</b>");
            
            // 4. Enviar la respuesta HTTP (construida manualmente)
            // Encabezado de la respuesta
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println(); // Esta línea en blanco es OBLIGATORIA para separar encabezado y cuerpo

            // Cuerpo de la respuesta (la página web)
            client.print(pagina);

            // Salimos del bucle 'while' ya que hemos respondido
            break;
        } else {
          // Si no es una línea en blanco, la reseteamos para leer la siguiente
          currentLine = "";
        }
      } else if (c != '\r') {
        // Si no es un salto de línea ni un retorno de carro, lo añadimos a la línea actual
        currentLine += c;
      }
    }
  }

  // 5. Cerrar la conexión
  client.stop();
  Serial.println("[Cliente Desconectado]");
}
