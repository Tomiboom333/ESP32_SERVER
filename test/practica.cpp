#include <WiFi.h>

// --- Configuración de Red ---
const char* ssid = "CESJT";
const char* password = "itisjtsmg";

// --- Configuración del Servidor y Hardware ---
WiFiServer server(80); // Puerto 80 es el estándar para HTTP
const int ledPin = 2;  // Pin del LED integrado en la mayoría de placas ESP32

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
    .btn { display: inline-block; background-color: #4CAF50; border: none; color: white;
           padding: 16px 30px; text-decoration: none; font-size: 24px; margin: 2px; cursor: pointer; border-radius: 8px; }
    .btn-off { background-color: #f44336; }
  </style>
</head>
<body>
  <h1>Servidor Web</h1>
  <p>Control del LED interno</p>
  <p>
    <a href="/on"><button class="btn">ENCENDER</button></a>
  </p>
  <p>
    <a href="/off"><button class="btn btn-off">APAGAR</button></a>
  </p>
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
          if (header.indexOf("GET /on") >= 0) {
            Serial.println("Request para encender el LED");
            digitalWrite(ledPin, HIGH);
          } else if (header.indexOf("GET /off") >= 0) {
            Serial.println("Request para apagar el LED");
            digitalWrite(ledPin, LOW);
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