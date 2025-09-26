#include <WiFi.h>
#include <Arduino.h>
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
    <a href="/seg1"><button class="btn">segmento 1</button></a>
  </p>
  <p>
    <a href="/seg2"><button class="btn">segmento 2</button></a>
  </p>
  <p>
    <a href="/0"><button class="btn">numero 0</button></a>
  </p>
  <p>
    <a href="/1"><button class="btn">numero 1</button></a>
  </p>
  <p>
    <a href="/2"><button class="btn">numero 2</button></a>
  </p>
  <p>
    <a href="/3"><button class="btn">numero 3</button></a>
  </p>
  <p>
    <a href="/4"><button class="btn">numero 4</button></a>
  </p>
  <p>
    <a href="/5"><button class="btn">numero 5</button></a>
  </p>
  <p>
    <a href="/6"><button class="btn">numero 6</button></a>
  </p>
  <p>
    <a href="/7"><button class="btn">numero 7</button></a>
  </p>
  <p>
    <a href="/8"><button class="btn">numero 8</button></a>
  </p>
  <p>
    <a href="/9"><button class="btn">numero 9</button></a>
  </p>
</body>
</html>
)rawliteral";
int num = 10;
int seg1[7]={22,23,24,25,26,27,28};
int seg2[7]={11,12,13,14,15,16,17};
int nums[10][7]={
  {1,1,1,1,1,1,0},//0
  {0,1,1,0,0,0,0},//1
  {1,1,0,1,1,0,1},//2
  {1,1,1,1,0,0,1},//3
  {0,1,1,0,0,1,1},//4
  {1,0,1,1,0,1,1},//5
  {1,0,1,1,1,1,1},//6
  {1,1,1,0,0,0,0},//7
  {1,1,1,1,1,1,1},//8
  {1,1,1,0,0,1,1}//9
};
int segmento = 0;
void setup() {
  Serial.begin(115200);
 for(int i= 0;i<7;i++){
        pinMode(seg1[i], OUTPUT);
        digitalWrite(seg1[i], LOW);
        pinMode(seg2[i], OUTPUT);
        digitalWrite(seg2[i], LOW);
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
          
          if (header.indexOf("GET /seg1") >= 0) {
            segmento = 1 ;
          } else if (header.indexOf("GET /seg2") >= 0){
            segmento = 2;
          }
          if (header.indexOf("GET /0") >= 0) {
            num=0;
          } else if (header.indexOf("GET /1") >= 0) {
            num=1;
          } else if (header.indexOf("GET /2") >= 0) {
            num=2;
          } else if (header.indexOf("GET /3") >= 0) {
            num=3;
          } else if (header.indexOf("GET /4") >= 0) {
            num=4;
          } else if (header.indexOf("GET /5") >= 0) {
            num=5;
          } else if (header.indexOf("GET /6") >= 0) {
            num=6;
          } else if (header.indexOf("GET /7") >= 0) {
            num=7;
          } else if (header.indexOf("GET /8") >= 0) {
            num=8;
          } else if (header.indexOf("GET /9") >= 0) {
            num=9;
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
  switch(num){
    case 0:
            Serial.println("Request para numero 0");
            if(segmento = 1){
                for(int i=0;i<7;i++){
                    digitalWrite(seg1[i], nums[0][i]);
                }
            }
            if(segmento = 2){
                for(int i=0;i<7;i++){
                    digitalWrite(seg2[i], nums[0][i]);
                }
            }
            break;
    case 1:
            Serial.println("Request para numero 1");
            if(segmento = 1){
                for(int i=0;i<7;i++){
                    digitalWrite(seg1[i], nums[1][i]);
                }
            }
            if(segmento = 2){
                for(int i=0;i<7;i++){
                    digitalWrite(seg2[i], nums[1][i]);
                }
            }
            break;
    case 2:
            Serial.println("Request para numero 2");
            if(segmento = 1){
                for(int i=0;i<7;i++){
                    digitalWrite(seg1[i], nums[2][i]);
                }
            }
            if(segmento = 2){
                for(int i=0;i<7;i++){
                    digitalWrite(seg2[i], nums[2][i]);
                }
            }
            break;
    case 3:
            Serial.println("Request para numero 3");
            if(segmento = 1){
                for(int i=0;i<7;i++){
                    digitalWrite(seg1[i], nums[3][i]);
                }
            }
            if(segmento = 2){
                for(int i=0;i<7;i++){
                    digitalWrite(seg2[i], nums[3][i]);
                }
            }
            break;
    case 4:
            Serial.println("Request para numero 4");
            if(segmento = 1){
                for(int i=0;i<7;i++){
                    digitalWrite(seg1[i], nums[4][i]);
                }
            }
            if(segmento = 2){
                for(int i=0;i<7;i++){
                    digitalWrite(seg2[i], nums[4][i]);
                }
            }
            break;
    case 5:
            Serial.println("Request para numero 5");
            if(segmento = 1){
                for(int i=0;i<7;i++){
                    digitalWrite(seg1[i], nums[5][i]);
                }
            }
            if(segmento = 2){
                for(int i=0;i<7;i++){
                    digitalWrite(seg2[i], nums[5][i]);
                }
            }
            break;
    case 6:
            Serial.println("Request para numero 6");
            if(segmento = 1){
                for(int i=0;i<7;i++){
                    digitalWrite(seg1[i], nums[6][i]);
                }
            }
            if(segmento = 2){
                for(int i=0;i<7;i++){
                    digitalWrite(seg2[i], nums[6][i]);
                }
            }
            break;
    case 7:
            Serial.println("Request para numero 7");
            if(segmento = 1){
                for(int i=0;i<7;i++){
                    digitalWrite(seg1[i], nums[7][i]);
                }
            }
            if(segmento = 2){
                for(int i=0;i<7;i++){
                    digitalWrite(seg2[i], nums[7][i]);
                }
            }
            break;
    case 8:
            Serial.println("Request para numero 8");
            if(segmento = 1){
                for(int i=0;i<7;i++){
                    digitalWrite(seg1[i], nums[8][i]);
                }
            }
            if(segmento = 2){
                for(int i=0;i<7;i++){
                    digitalWrite(seg2[i], nums[8][i]);
                }
            }
            break;
    case 9:
            Serial.println("Request para numero 9");
            if(segmento = 1){
                for(int i=0;i<7;i++){
                    digitalWrite(seg1[i], nums[9][i]);
                }
            }
            if(segmento = 2){
                for(int i=0;i<7;i++){
                    digitalWrite(seg2[i], nums[9][i]);
                }
            }
            break;
    default: 
        break;
  }
  // 5. Cerrar la conexión
  client.stop();
  Serial.println("[Cliente Desconectado]");
}