//3. Codificar un servidor web síncrono que simule un controlador de un semáforo con 4 modos: lento, normal, rápido y emergencia. 
//El modo lento consiste de un rojo de 70 segundos, amarillo de 3 segundos y un verde de 50 segundos. 
//El modo normal consiste de un rojo de 45 segundos, amarillo de 2 segundos y verde de 65 segundos. 
//El modo rápido consiste de un rojo de 20 segundos, amarillo de 1 segundo y verde de 40 segundos. 
//El modo emergencia consiste de rojo y amarillo parpadeante cada 1,5 segundos.
//Para este control se posee una página web con las siguientes características:
//◦ Título de primer nivel con leyenda “Semáforo”, todo con mayúsculas, con fuente ‘Impact’, negrita y subrayada de 45px. Utilizar la clase “titulo”.
//◦ Color del fondo del cuerpo blanco.
//◦ Todo centrado.
//◦ 4 botones, uno debajo del otro, de ancho 200px y alto 200px, de color de fondo rojo (lento), amarillo (normal), verde (rápido) y naranja (emergencia); color de texto negro con tamaño de fuente de 23px, borde negro de 3px sólido de radio 100px. Utilizar clase genérica “boton” y clases individuales “lento”, “normal”, “rápido” y “emergencia”.
//◦ Abajo de todo agregar una sección (span) con la leyenda “Modo: XXX” y reemplazar el XXX por el modo seleccionado en cuestión, el modo solo en negrita.


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
  <title>Semáforo</title>
  <style>
    body { background: #fff; text-align: center; }
    .titulo {
      font-family: Impact, sans-serif;
      font-size: 45px;
      font-weight: bold;
      text-decoration: underline;
      text-transform: uppercase;
      margin-bottom: 30px;
    }
    .lento { 
      background-color: red;
      width: 200px; height: 200px;
      color: #000;
      font-size: 23px;
      border: 3px solid #000;
      border-radius: 100px;
      margin: 15px auto;
      display: block;
      font-weight: bold;
      cursor: pointer;
    }
    .normal { 
      background-color: yellow; 
      width: 200px; height: 200px;
      color: #000;
      font-size: 23px;
      border: 3px solid #000;
      border-radius: 100px;
      margin: 15px auto;
      display: block;
      font-weight: bold;
      cursor: pointer;
    }
    .rapido { 
      background-color: green; 
      width: 200px; height: 200px;
      color: #000;
      font-size: 23px;
      border: 3px solid #000;
      border-radius: 100px;
      margin: 15px auto;
      display: block;
      font-weight: bold;
      cursor: pointer;
    }
    .emergencia { 
      backround-color: orange; 
      width: 200px; height: 200px;
      color: #000;
      font-size: 23px;
      border: 3px solid #000;
      border-radius: 100px;
      margin: 15px auto;
      display: block;
      font-weight: bold;
      cursor: pointer;
    }
  </style>
</head>
<body>
  <h1 class="titulo">SEMÁFORO</h1>

  <p>
    <a href="/lento"><button class="lento">Lento</button></a>
  </p>
  <p>
    <a href="/normal"><button class="normal">Normal</button></a>
  </p>
  <p>
    <a href="/rapido"><button class="rapido">Rapido</button></a>
  </p>
    <a href="/emergencia"><button class="emergencia">Emergencia</button></a>
  </p>
</body>
</html>
)rawliteral";

int leds[3] = {1,2,3};
void setup() {
  Serial.begin(115200);
  for(int i= 0;i<3;i++){
        pinMode(leds[i], OUTPUT);
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

int modo = 0;
void loop() {
int tActual=millis();
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
            modo = 1;
          } else if (header.indexOf("GET /normal") >= 0) {
            modo = 2;
          } else if (header.indexOf("GET /rapido") >= 0) {
            modo = 3;
          } else if (header.indexOf("GET /emergencia") >= 0) {
            modo = 4;
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
  switch(modo){
    case 1:
      Serial.println("Request para modo lento");
      // Rojo 70s, Amarillo 3s, Verde 50s
      digitalWrite(leds[0], HIGH); delay(70000);
      digitalWrite(leds[0], LOW); digitalWrite(leds[1], HIGH); 
      delay(3000);
      digitalWrite(leds[1], LOW); digitalWrite(leds[2], HIGH); 
      delay(50000);
      digitalWrite(leds[2], LOW);
      break;
    case 2:
      Serial.println("Request para modo normal");
      // Rojo 45s, Amarillo 2s, Verde 65s
      digitalWrite(leds[0], HIGH); delay(45000);
      digitalWrite(leds[0], LOW); digitalWrite(leds[1], HIGH); 
      delay(2000);
      digitalWrite(leds[1], LOW); digitalWrite(leds[2], HIGH); 
      delay(65000);
      digitalWrite(leds[2], LOW);
      break;
    case 3: 
      // Rojo 20s, Amarillo 1s, Verde 40s
      digitalWrite(leds[0], HIGH); delay(20000);
      digitalWrite(leds[0], LOW); digitalWrite(leds[1], HIGH); delay(1000);
      digitalWrite(leds[1], LOW); digitalWrite(leds[2], HIGH); delay(40000);
      digitalWrite(leds[2], LOW);
      break;
    case 4:
      // Rojo y amarillo parpadean cada 1.5s
      for (int i = 0; i < 5; i++) {
        digitalWrite(leds[0], HIGH); digitalWrite(leds[1], HIGH); 
        delay(1500);
        digitalWrite(leds[0], LOW); digitalWrite(leds[1], LOW); 
        delay(1500);
      }
    break;
    default:
      modo = 0;
    break;
  }

  // 5. Cerrar la conexión
  client.stop();
  Serial.println("[Cliente Desconectado]");
}