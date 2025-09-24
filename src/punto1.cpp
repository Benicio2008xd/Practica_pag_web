#include <WiFi.h>

// --- Configuración de Red ---
const char* ssid = "CESJT";
const char* password = "itisjtsmg";

// --- Configuración del Servidor y Hardware ---
WiFiServer server(80); // Puerto 80 es el estándar para HTTP
int leds [5] = {14,27,26,25,33};  // Pin del LED integrado en la mayoría de placas ESP32
int animacion1 [5][5] = {
  {1,1,0,0,0},
  {0,1,1,0,0},
  {0,0,1,1,0},
  {0,0,0,1,1},
  {1,0,0,0,1}
}; 
int animacion2 [5][5] = {
  {0,0,0,1,1},
  {0,0,1,1,0},
  {0,1,1,0,0},
  {1,1,0,0,0},
  {1,0,0,0,1}
}; 
int apagar_leds [5] = {0,0,0,0,0}; 
int i=0;
int ta=0, td=1000, tp=0;
int j=0;
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
    html { font-family: sans-serif; text-align: center; }
    .btn { display: inline-block; background-color:rgba(255, 0, 0, 1); border: none; color: white;
           padding: 16px 30px; text-decoration: none; font-size: 24px; margin: 2px; cursor: pointer; border-radius: 8px; }
    .btn-a { background-color:rgba(25, 0, 255, 1); }
    .btn-b { background-color: #ff00ddff; }
  </style>
</head>
<body>
  <h1>Servidor Web</h1>
  <p>Control del LED interno</p>
  <p>
    <a href="/animacion1"><button class="btn">animacion1</button></a>
  </p>
  <p>
    <a href="/animacion2"><button class="btn btn-a">animacion2</button></a>
  </p>
  <p>
    <a href="/apagar"><button class="btn btn-b">apagar</button></a>
  </p>
</body>
</html>
)rawliteral";

void setup() {

  Serial.begin(115200);
  for(i=0;i<5;i++){
    pinMode (leds[i],OUTPUT);
    digitalWrite(leds[i],LOW);
  }

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
          if (header.indexOf("GET /animacion1") >= 0) {
            Serial.println("Request para secuencia 1");
            for(i=0;i<5;i++){
              for(j=0;j<5;j++){
                digitalWrite(leds[j], animacion1[i][j]);
              }
              delay(500);
            }

          }
          else if(header.indexOf("GET /animacion2")>=0){
            Serial.println("request para secuencia 2");
            for(i=0;i<5;i++){
                for(j=0;j<5;j++){
                  digitalWrite(leds[j],animacion2[i][j]);
                }
            delay (500);  
            }
         }
        
          else if (header.indexOf("GET /apagar") >= 0) {
            Serial.println("Request para apagar el LED");
            for(i=0;i<5;i++){
              digitalWrite(leds[i], apagar_leds[i]);
            }
          }

          // 4. Enviar la respuesta HTTP (construida manualmente)
          // Encabezado de la respuesta
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println(); // Esta línea en blanco es OBLIGATORIA para separar encabezado y cuerpo

          // Cuerpo de la respuesta (la página web)
          client.print(pagina_html);

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