#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Declarando variáveis de tempo
// LEMBRANDO QUE "60000ms", EQUIVALE A 1 MINUTO
int tempoNivelBaixo = 210000; // Nível baixo da água = 3,5 minutos
int tempoNivelMedio = 380000; // Nível médio
int tempoNivelAlto = 500000; // Nível alto
int contador = 0;
int num_molho = 0;
int nivelDeAgua = 0;

// Declarando a variável de estado da máquina
String state;
String estadoGeral;
String estadoDaMaquina;
String modoSelecionado;
String modo;
bool inicio = false;
// Declarando variáveis dos pinos
int ledInicio = 12; // liga o led que se refere ao inicio do ciclo
int entradaSabao = 14;
int entradaAmaciante = 15;
int mH = 5; // Rotação do motor, no sentido horário
int mAH = 4; // Rotação do motor, no sentido anti-horário
int bombaEAtuador = 13;
// Dados de acesso à rede WiFi
const char* ssid = "Sampaio";
const char* pass = "sampaio032021";

ESP8266WebServer server(80);

void setup() {
  // Defindindo os pinos de saída
  pinMode(ledInicio, OUTPUT);
  pinMode(entradaSabao, OUTPUT);
  pinMode(entradaAmaciante, OUTPUT);
  pinMode(mH, OUTPUT);
  pinMode(mAH, OUTPUT);
  pinMode(bombaEAtuador, OUTPUT);
  
  // Iniciando todos os pinos como desligados
  digitalWrite(ledInicio, LOW);
  digitalWrite(entradaSabao, HIGH);
  digitalWrite(entradaAmaciante, LOW);
  digitalWrite(mH, HIGH);
  digitalWrite(mAH, HIGH);
  digitalWrite(bombaEAtuador, HIGH);
  
  // Iniciando monitor serial
  Serial.begin(115200);
  delay(250);
  Serial.print("Conectando");
  delay(500);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(250);
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());
  // Iniciando servidor
  server.on("/", HTTP_GET, handleRoot);
  server.begin();
  Serial.println("HTTP server iniciado");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  state = (server.hasArg("state"));
  if(state == "1") {
    estadoGeral = server.arg("state");
    if(estadoGeral == "on") {
      digitalWrite(ledInicio, HIGH);
    } 
    server.send(200, "application/json", "{\"Estado da máquina\":\"" + estadoGeral + "\"}");
  } else if(state == "0") {
    modoSelecionado = server.arg("modo");
    if(modoSelecionado == "baixo") {
      nivelDeAgua = 1;
      //server.send(200, "application/json", "{\"Níve de água selecionado\":\"" + modoSelecionado + "\"}");
    } else if(modoSelecionado == "medio") {
      nivelDeAgua = 2;
      //server.send(200, "application/json", "{\"Níve de água selecionado\":\"" + modoSelecionado + "\"}");
    } else if(modoSelecionado == "alto") {
      nivelDeAgua = 3;
      //server.send(200, "application/json", "{\"Níve de água selecionado\":\"" + modoSelecionado + "\"}");
    } else if(modoSelecionado == "teste_lavagem") {
      if(estadoGeral == "on") {
        delay(5000);
        testeLavagem();
      }
    } else if(modoSelecionado == "teste_centrifugar") {
      if(estadoGeral == "on") {
        delay(5000);
        testeCentrifugar();
      }
    } else if(modoSelecionado == "auto") {
      modo = "auto";
    } else if(modoSelecionado == "lavar") {
      lavar(); 
      //server.send(200, "application/json", "{\"Modo Selecionado\":\"" + modoSelecionado + "\"}");
    } else if(modoSelecionado == "molho") {
      molho();
      //server.send(200, "application/json", "{\"Modo Selecionado\":\"" + modoSelecionado + "\"}");
    } else if(modoSelecionado == "enxaguar") {
      enxaguar();
      //server.send(200, "application/json", "{\"Modo Selecionado\":\"" + modoSelecionado + "\"}");
    } else if(modoSelecionado == "centrifugar") {
      centrifugar();
      //server.send(200, "application/json", "{\"Modo Selecionado\":\"" + modoSelecionado + "\"}");
    } else if(modoSelecionado == "iniciar") {
      if(modo == "auto") {
        switch(nivelDeAgua) {
          case 1:
            nivelBaixo();
            break;
          case 2:
            nivelMedio();
            break;
          case 3:
            nivelAlto();
            break;
        }
      }
    }
    server.send(200, "application/json", "{\"Modo selecionado\":\"" + modoSelecionado + "\"}");
  }
}

// Nível baixo
void nivelBaixo() { 
  digitalWrite(entradaSabao, LOW);
  delay(tempoNivelBaixo);
  digitalWrite(entradaSabao, HIGH);
  delay(5000);
  bater_inicial(); // no bater_inicial, vai bater antes do molho.
}
// Nível médio
void nivelMedio() { 
  digitalWrite(entradaSabao, LOW);
  delay(tempoNivelMedio);
  digitalWrite(entradaSabao, HIGH);
  delay(5000);
  bater_inicial(); // no bater_inicial, vai bater antes do molho.
}
// Nível alto
void nivelAlto() { 
  digitalWrite(entradaSabao, LOW);
  delay(tempoNivelAlto);
  digitalWrite(entradaSabao, HIGH);
  delay(5000);
  bater_inicial(); // no bater_inicial, vai bater antes do molho.
}
// Bater inicial = 30 segundos
void bater_inicial() {
  for(contador = 0; contador < 15; contador++) {
    digitalWrite(mAH, HIGH);
    delay(500);
    digitalWrite(mH, LOW);
    delay(500);

    digitalWrite(mH, HIGH);
    delay(500);
    digitalWrite(mAH, LOW);
    delay(500); 
  }
  digitalWrite(mAH, HIGH);
  digitalWrite(mH, HIGH);
  molho();
}
void bater_molho() {
  for(contador = 0; contador < 7; contador++) {
    digitalWrite(mAH, HIGH);
    delay(500);
    digitalWrite(mH, LOW);
    delay(500);

    digitalWrite(mH, HIGH);
    delay(500);
    digitalWrite(mAH, LOW);
    delay(500); 
  }
  digitalWrite(mAH, HIGH);
  digitalWrite(mH, HIGH);
  molho();
}
// Molho = à cada 10 minutos chama a função de bater 1
void molho() {
  if(num_molho < 4) {
    delay(600000);
    num_molho = num_molho + 1;
    bater_molho();
  } else {
    delay(5000);
    lavar();
  }
}
// Lavar
void lavar() {
  for(contador = 0; contador < 75; contador++){
    digitalWrite(mAH, HIGH);
    delay(500);
    digitalWrite(mH, LOW);
    delay(500);

    digitalWrite(mH, HIGH);
    delay(500);
    digitalWrite(mAH, LOW);
    delay(500); 
  }
  digitalWrite(mAH, HIGH);
  digitalWrite(mH, HIGH);
  delay(5000);
  enxaguar();
}

void enxaguar() {
  if(nivelDeAgua == 1) {
    digitalWrite(bombaEAtuador, LOW);
    delay(70000);
  } else if(nivelDeAgua == 2) {
    digitalWrite(bombaEAtuador, LOW);
    delay(120000);
  } else if(nivelDeAgua == 3) {
    digitalWrite(bombaEAtuador, LOW);
    delay(220000);    
  }
  delay(6000);
  for(contador = 0; contador < 4; contador++) {
    digitalWrite(mH, LOW);
    delay(2000);
    digitalWrite(mH, HIGH);
    delay(2000);  
  }
  digitalWrite(mH, LOW);
  delay(120000);
  digitalWrite(mH, HIGH);
  digitalWrite(bombaEAtuador, HIGH);
  delay(5000);
  // após centrifugar por 2m, liberar a entrada de água no reservatório de amaciante
  if(nivelDeAgua == 1) {
    digitalWrite(entradaAmaciante, HIGH);
    delay(tempoNivelBaixo);
    digitalWrite(entradaAmaciante, LOW);
    delay(5000);
  } else if(nivelDeAgua == 2) {
    digitalWrite(entradaAmaciante, HIGH);
    delay(tempoNivelMedio);
    digitalWrite(entradaAmaciante, LOW);
    delay(5000);
  } else if(nivelDeAgua == 3) {
    digitalWrite(entradaAmaciante, HIGH);
    delay(tempoNivelAlto);
    digitalWrite(entradaAmaciante, LOW);
    delay(5000);
  }
  // após a entrada de água com amaciante, vamos bater, colocar 5 minutos de molho e depois bater novamente
  for(contador = 0; contador < 45; contador++){
    digitalWrite(mAH, HIGH);
    delay(500);
    digitalWrite(mH, LOW);
    delay(500);

    digitalWrite(mH, HIGH);
    delay(500);
    digitalWrite(mAH, LOW);
    delay(500); 
  }
  digitalWrite(mAH, HIGH);
  digitalWrite(mH, HIGH);
  // molho enxague, por 5 minutos
  delay(300000);
  for(contador = 0; contador < 45; contador++){
    digitalWrite(mAH, HIGH);
    delay(500);
    digitalWrite(mH, LOW);
    delay(500);

    digitalWrite(mH, HIGH);
    delay(500);
    digitalWrite(mAH, LOW);
    delay(500); 
  }
  digitalWrite(mAH, HIGH);
  digitalWrite(mH, HIGH);
  delay(5000);
  centrifugar();
}

void centrifugar() {
  if(nivelDeAgua == 1) {
    digitalWrite(bombaEAtuador, LOW);
    delay(70000);
  } else if(nivelDeAgua == 2) {
    digitalWrite(bombaEAtuador, LOW);
    delay(120000);
  } else if(nivelDeAgua == 3) {
    digitalWrite(bombaEAtuador, LOW);
    delay(220000);    
  }
  delay(6000);
  for(contador = 0; contador < 4; contador++) {
    digitalWrite(mH, LOW);
    delay(2000);
    digitalWrite(mH, HIGH);
    delay(2000);
  }
  digitalWrite(mH, LOW);
  delay(240000);
  digitalWrite(mH, HIGH);
  digitalWrite(bombaEAtuador, HIGH);
  delay(5000);
  // Finalizar
  finalizarCiclos();
}

void testeLavagem() {
  for(contador = 0; contador < 7; contador++){
    digitalWrite(mAH, HIGH);
    delay(500);
    digitalWrite(mH, LOW);
    delay(500);

    digitalWrite(mH, HIGH);
    delay(500);
    digitalWrite(mAH, LOW);
    delay(500); 
  }
  digitalWrite(mAH, HIGH);
  digitalWrite(mH, HIGH);
  delay(5000);
}

void testeCentrifugar() {
  digitalWrite(bombaEAtuador, LOW);
  delay(6000);
  digitalWrite(mH, LOW);
  delay(15000);
  digitalWrite(mH, HIGH);
  digitalWrite(bombaEAtuador, HIGH);
  delay(5000);
  digitalWrite(ledInicio, LOW);
}
void finalizarCiclos() {
  digitalWrite(ledInicio, LOW);
  digitalWrite(entradaSabao, HIGH);
  digitalWrite(entradaAmaciante, LOW);
  digitalWrite(mH, HIGH);
  digitalWrite(mAH, HIGH);
  digitalWrite(bombaEAtuador, HIGH);
  // Zerando variáveis
  contador = 0;
  num_molho = 0;
  nivelDeAgua = 0;
}
