//PROJETO MQTT
//TIPO:PLACA SENSOR DE CAMPO
//PLACA:ESP-8266
//DESENVOLVEDOR: JOABE FERNANDES
//DATA: 01/12/2022

//===========Informaçoes led Placa =========
//Led aceso Continuo ==>> Problema de conexão broker ou wifi
//Led apagado Continuo ==>> conexão broker e wifi ok

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define pinBotao1 12  //D6
#define pinLED 2  //GP2 ESP-8266

//WiFi
const char* SSID = "Galaxy M31A4A7";                // SSID / nome da rede WiFi que deseja se conectar
const char* PASSWORD = "naosei123";                 // Senha da rede WiFi que deseja se conectar



WiFiClient wifiClient;

//MQTT Server
const char* BROKER_MQTT = "test.mosquitto.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                      // Porta do Broker MQTT

//Definindo a ID da placa. Cada placa tem ser ID único no Broker.
#define ID_MQTT  "BCI01"  

//Aqui definimos no Broker onde cada mensagem específica será tratada, ou seja,
// estamos assinando tópicos diferentes no broker.  
#define TOPIC_PUBLISH "BCIBotao1"    
#define TOPIC_PUBLISb "Sensor/TEMPERATURA"
#define TOPIC_PUBLISc "mediaTemp"
#define TOPIC_PUBLISd "Sensor/UMIDADE"
#define TOPIC_PUBLISe "mediaUmi"
#define TOPIC_PUBLISf "DataSensor"

PubSubClient MQTT(wifiClient);        // Instância o Cliente MQTT passando o objeto espClient

//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void enviaPacote();     //Envia Mensagem para o Broker

void setup() {
  
  pinMode(pinLED, OUTPUT);
  pinMode(pinBotao1, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);

  Serial.begin(115200);

  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void loop() {
  mantemConexoes();
  enviaValores();
  MQTT.loop();
}

void mantemConexoes() {
  if (!MQTT.connected()) {
    conectaMQTT();
  }

  conectaWiFi(); //Se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP());
}

void conectaMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao Broker com sucesso!");
       digitalWrite(2, HIGH); // led apaga quando conectado
    }
    else {
       digitalWrite(2, LOW);// led acende quando não conectado
      Serial.println("Nao foi possivel se conectar ao broker.");
      Serial.println("Nova tentatica de conexao em 10s");
      delay(10000);
    }
  }
}

void enviaValores() {
  static bool estadoBotao1 = HIGH;
  static bool estadoBotao1Ant = HIGH;
  static unsigned long debounceBotao1;

  int valorTemp;
  int valorUmi;
  char sensoranalogicoTemp[30];
  char mediaTemp[30];

  char sensoranalogicoUmi[30];
  char mediaUmi[30];


  estadoBotao1 = digitalRead(pinBotao1);
  if (  (millis() - debounceBotao1) > 30 ) {  //Elimina efeito Bouncing
    if (!estadoBotao1 && estadoBotao1Ant) {

      //Botao Apertado
      MQTT.publish(TOPIC_PUBLISH, "1");
      Serial.println("Botao1 APERTADO. Payload enviado.");

      debounceBotao1 = millis();
    } else if (estadoBotao1 && !estadoBotao1Ant) {

      //Botao Solto
      MQTT.publish(TOPIC_PUBLISH, "0");
      Serial.println("Botao1 SOLTO. Payload enviado.");

      debounceBotao1 = millis();
    }

  }

//======================================================

  
  //LEITURA DO SENSORE TEMPERATURA PARA LEITOR GAUS
  valorTemp = map(analogRead(A0), 0, 1023, 0,70);// Transforma a leitura do sensor em parâmentros de temperatura.
                                                 // Cada sensor tem suas especificações
  itoa(valorTemp, sensoranalogicoTemp, 10);      // Transforman um nuémo em String para ser enviado
  MQTT.publish(TOPIC_PUBLISb, sensoranalogicoTemp);// Envia para o broker no tópico defino a mensagem lida pelo sensor 
  delay(100);
  
  //LEITURA DO SENSORE UMIDADE PARA LEITOR GAUS
  valorUmi = map(analogRead(A0), 0, 1023, 0, 99);
  itoa(valorUmi, sensoranalogicoUmi, 10);
  MQTT.publish(TOPIC_PUBLISd, sensoranalogicoUmi);
  
  //ENVIA A LEITURA DOS SENSORES PARA A MONTAGEM DO GRÁFICO
  if (  (millis() - debounceBotao1) > 2000 ) {
    valorTemp = map(analogRead(A0), 0, 1023, 0, 70);
    itoa(valorTemp, mediaTemp, 10);
    MQTT.publish(TOPIC_PUBLISc, mediaTemp);
    //============================================
    valorUmi = map(analogRead(A0), 0, 1023, 0, 99);
      itoa(valorUmi, mediaUmi, 10.0);    
    MQTT.publish(TOPIC_PUBLISe, mediaUmi);
    debounceBotao1 = millis();

  }

  estadoBotao1Ant = estadoBotao1;
}
