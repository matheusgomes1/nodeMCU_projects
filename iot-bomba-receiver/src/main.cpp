#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#define MOTOR D1
#define BUTTON D4
#define BUZZER D2
#define LED_WIFI D7
#define LED_NIVEL D8

#ifndef STASSID
#define STASSID "MARCIA"
#define STAPSK "gomes123"
#endif

const char *ssid = STASSID; // your network SSID (name)
const char *pass = STAPSK;  // your network password

WiFiServer server(17); // to listen por 17 
WiFiClient client;

String messageRcv;
boolean motor = false;

String recebe_msg();

void buzzerRing();
void reestabelecer_conexao();

void ICACHE_RAM_ATTR triggerButton();

void setup() {
  Serial.begin(9600);
  
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  IPAddress ip(10, 0, 0, 205); // where xx is the desired IP Address
  IPAddress gateway(10, 0, 0, 1); // set gateway to match your network
  IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
  IPAddress dns(8, 8, 8, 8);
  WiFi.config(ip, gateway, subnet, dns);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass); // ip 10.0.0.210
  
  pinMode(LED_WIFI, OUTPUT);
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_WIFI, HIGH);
    if (digitalRead(BUTTON) == LOW) {
      triggerButton();
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  ArduinoOTA.onStart([]() {
    Serial.println("Inicio...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("nFim!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Autenticacao Falhou");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Falha no Inicio");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Falha na Conexao");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Falha na Recepcao");
    else if (error == OTA_END_ERROR) Serial.println("Falha no Fim");
  });
  ArduinoOTA.begin();

  server.begin();
  Serial.println(F("Server started"));
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  digitalWrite(MOTOR, HIGH);
  motor = false;
  pinMode(LED_NIVEL, OUTPUT);
  digitalWrite(LED_WIFI, LOW);
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
      triggerButton();
  }

  ArduinoOTA.handle();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reestabelecendo conexão");
    reestabelecer_conexao();
  }
  // checando se cliente conseguiu se conectar
  
  client = server.available();
  if (!client) {
    Serial.println("cliente offline");
    delay(200);
    return;
  } 
  
  digitalWrite(LED_WIFI, HIGH);
  Serial.println("new client");
  client.setTimeout(1000); // default is 1000
  messageRcv = recebe_msg();
  Serial.println(messageRcv);
  
  if (messageRcv.indexOf("beep") == 0 && motor == false) { // vazio e motor desligado
    digitalWrite(LED_NIVEL, HIGH);
    Serial.println("Buzzer Ring");
    buzzerRing();
  } 

  if (messageRcv.indexOf("alert") == 0) {
    digitalWrite(LED_NIVEL, HIGH);
    Serial.println("LED ALERTA NÍVEL");
  }

  if (messageRcv.indexOf("cheio") == 0) {
    Serial.println("Desliga motor");
    motor = false;
    digitalWrite(MOTOR, HIGH);
    digitalWrite(LED_NIVEL, LOW);
  }
  delay(150);
  digitalWrite(LED_WIFI, LOW);
}

String recebe_msg() {
  String rcv = client.readString();
  return rcv;
}

void triggerButton() {
  if(motor == true) {
    Serial.println("Desliga Motor");
    motor = false; // motor desligado
    digitalWrite(MOTOR, HIGH);
  } else {
    Serial.println("Liga Motor");
    motor = true; // motor ligado
    digitalWrite(MOTOR, LOW);
    digitalWrite(LED_NIVEL, LOW);
  }
  delay(2000);
}

void reestabelecer_conexao() {
  digitalWrite(LED_WIFI, HIGH);
  while (WiFi.status() != WL_CONNECTED)
  {
    if (digitalRead(BUTTON) == LOW) {
      triggerButton();
    }
    delay(200);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_WIFI, LOW);
}

void buzzerRing() {
  int c = 0;
  while(c < 3) {
    if (digitalRead(BUTTON) == LOW) {
      triggerButton();
    }
    Serial.println("apita");
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
    c++;
  }
}