#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CESJT";
const char* password = "itisjtsmg";

const int ledPin = 2; 

WebServer server(80);

bool ledEncendido = true;

// pagina
const char pagina_template[] PROGMEM = R"rawliteral(
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
      font-weight: bold;
      cursor: pointer;
      text-decoration: none;
    }
    .normal { 
      background-color: yellow; 
      width: 200px; height: 200px;
      color: #000;
      font-size: 23px;
      border: 3px solid #000;
      border-radius: 100px;
      margin: 15px auto;
      font-weight: bold;
      cursor: pointer;
      text-decoration: none;
    }
    .rapido { 
      background-color: green; 
      width: 200px; height: 200px;
      color: #000;
      font-size: 23px;
      border: 3px solid #000;
      border-radius: 100px;
      margin: 15px auto;
      font-weight: bold;
      cursor: pointer;
      text-decoration: none;
    }
    .emergencia { 
      background-color: orange; 
      width: 200px; height: 200px;
      color: #000;
      font-size: 23px;
      border: 3px solid #000;
      border-radius: 100px;
      margin: 15px auto;
      font-weight: bold;
      cursor: pointer;
      text-decoration: none;
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

  <span>Modo: <b>XXX</b></span>
</body>
</html>
)rawliteral";


int leds[3] = {1,2,3};
String ModoTexto;

void ModoLento() {
    ModoTexto = "Lento";
    // Rojo 70s, Amarillo 3s, Verde 50s
      digitalWrite(leds[0], HIGH); 
      delay(70000);
      digitalWrite(leds[0], LOW); 
      digitalWrite(leds[1], HIGH); 
      delay(3000);
      digitalWrite(leds[1], LOW); 
      digitalWrite(leds[2], HIGH); 
      delay(50000);
      digitalWrite(leds[2], LOW);
      server.sendHeader("Location", "/"); // dice que la página se vaya a /
      server.send(302, "text/plain", ""); // 302 dice que el /toggle no está y que vaya a la locación de antes
}

void ModoNormal() {
    ModoTexto = "Normal";
  // Rojo 40s, Amarillo 3s, Verde 30s
      digitalWrite(leds[0], HIGH); 
      delay(40000);
      digitalWrite(leds[0], LOW); 
      digitalWrite(leds[1], HIGH); 
      delay(3000);
      digitalWrite(leds[1], LOW); 
      digitalWrite(leds[2], HIGH); 
      delay(30000);
      digitalWrite(leds[2], LOW);
      server.sendHeader("Location", "/"); // dice que la página se vaya a /
      server.send(302, "text/plain", ""); // 302 dice que el /toggle no está y que vaya a la locación de antes
}

void ModoRapido() {
    ModoTexto = "Rapido";
  // Rojo 20s, Amarillo 1s, Verde 40s
      digitalWrite(leds[0], HIGH); 
      delay(20000);
      digitalWrite(leds[0], LOW); 
      digitalWrite(leds[1], HIGH); 
      delay(1000);
      digitalWrite(leds[1], LOW); 
      digitalWrite(leds[2], HIGH); 
      delay(40000);
      digitalWrite(leds[2], LOW);
      server.sendHeader("Location", "/"); // dice que la página se vaya a /
      server.send(302, "text/plain", ""); // 302 dice que el /toggle no está y que vaya a la locación de antes
} 

void ModoEmergencia() {
    ModoTexto = "Emergencia";
  // Rojo y amarillo parpadean cada 1.5s
      for (int i = 0; i < 5; i++) {
        digitalWrite(leds[0], HIGH); 
        digitalWrite(leds[1], HIGH); 
        delay(1500);
        digitalWrite(leds[0], LOW); 
        digitalWrite(leds[1], LOW); 
        delay(1500);
      }
    server.sendHeader("Location", "/"); // dice que la página se vaya a /
    server.send(302, "text/plain", ""); // 302 dice que el /toggle no está y que vaya a la locación de antes
}
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // conexion al wifi como antes
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
  Serial.println("\nWiFi conectado!");
  Serial.print("Dirección IP: http://");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    
    String pagina = pagina_template; //hago una variable para no modificar a la original
    //parte para cambiar lo que dice el boton una vez que se prende o apaga
    
    //pagina.replace busca el texto y lo reemplaza con lo que pongas. 
    pagina.replace("XXX", ModoTexto);

    server.send(200, "text/html", pagina);
  });

  server.on("/lento", ModoLento);
  server.on("/normal", ModoNormal);
  server.on("/rapido", ModoRapido);
  server.on("/emergencia", ModoEmergencia);

  server.begin();
}

void loop() {
  server.handleClient();
}