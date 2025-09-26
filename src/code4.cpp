//4. Dar el diagrama de conexiones y codificar un servidor web síncrono con el ESP32
//que simule un monitor de tensión de un sensor de potenciómetro, y, además, el encendido/apagado de un LED cuyo brillo lo determina dicho potenciómetro.
//Para este control se posee una página web con las siguientes características:
//◦ Título de primer nivel con leyenda “Monitor Tensión”, todo con mayúsculas, con fuente ‘Courier’, negrita e itálica de 32px. Utilizar la clase “titulo”.
//◦ Color del fondo del cuerpo rojo.
//◦ Todo centrado.
//◦ Una división de clase “borde” de ancho 170px y alto 100px con borde negro de 5px sólido de radio 18px y color de fondo negro.
//◦ Dentro de esta división hay otra división de clase “pantalla” de ancho 160px y alto 90px con borde negro de 1px sólido de radio 10px y color de fondo cyan. Agregar margen arriba y abajo de 4px e izquierda y derecha de 4px.
//◦ Dentro de esta última división hay una sección (span) con la leyenda “Tensión: X.XXV” y reemplazar el X.XX por la tensión medida en cuestión, la tensión sola en negrita. Agregar dos espacios en blanco antes de la leyenda para centrar.
//◦ Abajo, agregar un botón de toggle (apaga el LED si este está encendido y viceversa) de ancho 80px, alto 45px, color de fondo amarillo, color de texto negro, borde negro de 2px sólido y radio de borde de 80px.
//Este tiene el texto “APAGAR” o “ENCENDER” según corresponda. Utilizar la clase “boton”.


#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CESJT";
const char* password = "itisjtsmg";

//Segun lo que investigamos, no podemos utilizar el analogWrite como en arduino, por lo que usaremos ledcWrite. 
//Tambien debemos configurar un canal, frecuencia y resolucion de bits para el pwm del esp32.

int led = 2;
int pot = 34;// Potenciómetro en GPIO34 (entrada analógica)

// Configuración PWM para ESP32
const int canalPWM = 0;       // Canal 0
const int frecuencia = 5000;  // Frecuencia de 5kHz
const int resolucion = 8;     // Resolución de 8 bits (0-255)

WebServer server(80);

const char pagina_template[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Monitor Tensión</title>
  <style>
    html, body { height: 100%; margin: 0; }
    body {
      background: red;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: flex-start;
      text-align: center;
      font-family: sans-serif;
      padding-top: 30px;
      box-sizing: border-box;
    }

    .titulo {
      font-family: 'Courier', monospace;
      font-size: 32px;
      font-weight: 700;
      font-style: italic;
      text-transform: uppercase;
      margin: 0 0 20px 0;
    }

    .borde {
      width: 170px;
      height: 100px;
      border: 5px solid black;
      border-radius: 18px;
      background: black;
      box-sizing: border-box;
      display: flex;
      align-items: center;
      justify-content: center;
      margin-bottom: 16px;
    }

    .pantalla {
      width: 160px;
      height: 90px;
      border: 1px solid black;
      border-radius: 10px;
      background: cyan;
      margin: 4px;
      box-sizing: border-box;
      display: flex;
      align-items: center;
      justify-content: center;
      overflow: hidden;
    }

    .pantalla span {
      font-size: 16px;
      line-height: 1;
      white-space: nowrap;
    }

    .tension-valor {
      font-weight: 700;
    }

    .boton {
      width: 80px;
      height: 45px;
      background: yellow;
      color: black;
      border: 2px solid black;
      border-radius: 80px;
      font-weight: 700;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      justify-content: center;
      margin-top: 12px;
      user-select: none;
    }
  </style>
</head>
<body>
  <h1 class="titulo">Monitor Tensión</h1>

  <div class="borde">
    <div class="pantalla">
      <span class="tension-valor" id="valor">  Tensión: <b>X.XX V</b></span>
    </div>
  </div>

  <a href="/toggle"><button class="boton">BOTONTEXTO</button></a>
</body>
</html>
)rawliteral";

bool EstLED = false;
float tension = 0.0;
String estadoLED;

void LED() {
  int valor = analogRead(pot);
  tension = (valor / 4095.0) * 3.3;
  int pote = map(valor, 0, 4095, 0, 255);// Escalar a 8 bits
  if (EstLED) {
    ledcWrite(canalPWM, pote);// Controla brillo con potenciómetro
    estadoLED = "APAGAR";
  }else{
    ledcWrite(canalPWM, 0);
    estadoLED = "ENCENDER";
  }
}

void botonLED() {
  EstLED = !EstLED;
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void setup() {
  Serial.begin(115200);

  // Configurar PWM para LED
  ledcAttachPin(led, canalPWM);
  ledcSetup(canalPWM, frecuencia, resolucion);

  // Conexión WiFi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int timeout = 20;
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

  Serial.println("\nWiFi conectado!");
  Serial.print("Dirección IP: http://");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    LED(); // Actualiza el estado del LED y la tensión
    String pagina = pagina_template;

    pagina.replace("X.XX", String(tension,2));//limito a 2 decimales
    pagina.replace("BOTONTEXTO", estadoLED);

    server.send(200, "text/html", pagina);
  });

  server.on("/toggle", botonLED);

  server.begin();
}

void loop() {
  server.handleClient();
}