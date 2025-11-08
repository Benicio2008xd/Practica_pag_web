#include <WiFi.h>

// --- Configuración de Red ---
const char* ssid = "CESJT";
const char* password = "itisjtsmg";

// --- Configuración del Servidor y Hardware ---
WiFiServer server(80); // Puerto 80 es el estándar para HTTP
int displaySeleccionado = 1; // 1 o 2
int seg7_1 [7] = {0,1,2,3,4,5,6};  
int seg7_2 [7] = {7,8,9,10,11,12,13};  
int numeros [10][7] = {
    {1,1,1,1,1,1,0}, //0
    {1,1,0,0,0,0,0}, //1
    {1,1,0,1,1,0,1}, //2
    {1,1,1,1,0,0,1}, //3
    {0,1,1,0,0,1,1}, //4
    {1,0,1,1,0,1,1}, //5
    {1,0,1,1,1,1,1}, //6
    {1,1,1,0,0,0,1}, //7
    {1,1,1,1,1,1,1}, //8
    {1,1,1,0,0,1,1} //9
};

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
    .btn { display: flex; background-color:rgba(255, 0, 0, 1); border: none; color: white;
           padding: 16px 30px; text-decoration: none; font-size: 24px; margin: 2px; cursor: pointer; border-radius: 8px; }
    .btn-a { display: flex; background-color:rgba(25, 0, 255, 1); }
    .btn-b { background-color: #ff00ddff; }
    .btn-c { background-color: #ff00ddff; }
    .btn-d { background-color: #ff00ddff; }
    .btn-e { background-color: #ff00ddff; }
    .btn-f { background-color: #ff00ddff; }
    .btn-g { background-color: #ff00ddff; }
    .btn-h { background-color: #ff00ddff; }
    .btn-i { background-color: #ff00ddff; }
    .btn-j { background-color: #ff00ddff; }
    .btn-k { background-color: #ff00ddff; }
  </style>
</head>
<body>
  <h1>Servidor Web</h1>
  <p>
    <a href="/display1"><button class="btn">display1</button></a>
  </p>
  <p>
    <a href="/display2"><button class="btn btn-a">display2</button></a>
  </p>
  <p>
    <a href="/0"><button class="btn btn-b">0</button></a>
  </p>
  <p>
    <a href="/1"><button class="btn btn-c">1</button></a>
  </p>
  <p>
    <a href="/2"><button class="btn btn-d">2</button></a>
  </p>
  <p>
    <a href="/3"><button class="btn btn-e">3</button></a>
  </p>
  <p>
    <a href="/4"><button class="btn btn-f">4</button></a>
  </p>
  <p>
    <a href="/5"><button class="btn btn-g">5</button></a>
  </p>
  <p>
    <a href="/6"><button class="btn btn-h">6</button></a>
  </p>
  <p>
    <a href="/7"><button class="btn btn-i">7</button></a>
  </p>
  <p>
    <a href="/8"><button class="btn btn-j">8</button></a>
  </p>
  <p>
    <a href="/9"><button class="btn btn-k">9</button></a>
  </p>
</body>
</html>
)rawliteral";

int i=0;
int ta=0, td=1000, tp=0;
int j=0;
void setup(){

  Serial.begin(115200);
  for(i=0;i<7;i++){
    pinMode (seg7_1[i],OUTPUT);
    digitalWrite(seg7_1[i],LOW);
    pinMode (seg7_2[i],OUTPUT);
    digitalWrite(seg7_2[i],LOW);
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
          if (header.indexOf("GET /display1") >= 0) {
            displaySeleccionado = 1;
        }
        else if (header.indexOf("GET /display2") >= 0) {
            displaySeleccionado = 2;
        }
        else if (header.indexOf("GET /1") >= 0) {
            for (int i = 0; i < 7; i++) {
                if (displaySeleccionado == 1) {
                    digitalWrite(seg7_1[i], numeros[1][i]);
                } else {
                    digitalWrite(seg7_2[i], numeros[1][i]);
                }
            }
        }
        else if (header.indexOf("GET /2") >= 0) {
            for (int i = 0; i < 7; i++) {
                if (displaySeleccionado == 1) {
                    digitalWrite(seg7_1[i], numeros[2][i]);
                } else {
                    digitalWrite(seg7_2[i], numeros[2][i]);
                }
            }
        }
        else if (header.indexOf("GET /3") >= 0) {
            for (int i = 0; i < 7; i++) {
                if (displaySeleccionado == 1) {
                    digitalWrite(seg7_1[i], numeros[3][i]);
                } else {
                    digitalWrite(seg7_2[i], numeros[3][i]);
                }
            }
        }
        else if (header.indexOf("GET /4") >= 0) {
            for (int i = 0; i < 7; i++) {
                if (displaySeleccionado == 1) {
                    digitalWrite(seg7_1[i], numeros[4][i]);
                } else {
                    digitalWrite(seg7_2[i], numeros[4][i]);
                }
            }
        }
        else if (header.indexOf("GET /5") >= 0) {
            for (int i = 0; i < 7; i++) {
                if (displaySeleccionado == 1) {
                    digitalWrite(seg7_1[i], numeros[5][i]);
                } else {
                    digitalWrite(seg7_2[i], numeros[5][i]);
                }
            }
        }
        else if (header.indexOf("GET /6") >= 0) {
            for (int i = 0; i < 7; i++) {
                if (displaySeleccionado == 1) {
                    digitalWrite(seg7_1[i], numeros[6][i]);
                } else {
                    digitalWrite(seg7_2[i], numeros[6][i]);
                }
            }
        }
        else if (header.indexOf("GET /7") >= 0) {
            for (int i = 0; i < 7; i++) {
                if (displaySeleccionado == 1) {
                    digitalWrite(seg7_1[i], numeros[7][i]);
                } else {
                    digitalWrite(seg7_2[i], numeros[7][i]);
                }
            }
        }
        else if (header.indexOf("GET /8") >= 0) {
            for (int i = 0; i < 7; i++) {
                if (displaySeleccionado == 1) {
                    digitalWrite(seg7_1[i], numeros[8][i]);
                } else {
                    digitalWrite(seg7_2[i], numeros[8][i]);
                }
            }
        }
        else if (header.indexOf("GET /9") >= 0) {
            for (int i = 0; i < 7; i++) {
                if (displaySeleccionado == 1) {
                    digitalWrite(seg7_1[i], numeros[9][i]);
                } else {
                    digitalWrite(seg7_2[i], numeros[9][i]);
                }
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
