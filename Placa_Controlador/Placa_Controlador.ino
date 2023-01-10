//PROJETO MQTT
//TIPO:PLACA CONTROLADORA
//PLACA:ESP-8266
//DESENVOLVEDOR: JOABE FERNANDES
//DATA: 01/12/2022
//===========Informaçoes led Placa =========
//Led aceso Continuo ==>> Problema de conexão broker ou wifi
//Led apagado Continuo ==>> conexão broker e wifi ok

//=====BIBLIOTÉCAS NECESSARIAS========

#include <ESP8266WiFi.h> // BIBLIOTÉCA REFERENTE A CONEXÃO ESP => WIFI
#include <PubSubClient.h>// BIBLIOTÉCA REFERENTE AO PROTOCOLO MQTT
#include <iostream>
#include <cstring>
#include <stdlib.h>

#define pinLED 2  //GP2 ESP-8266
#define Tam_Armaz_Dados_Temp 20 //Representa a quantidade de dados que serão gravados temporariamente para fins do Histórico da Temperatura
#define Tam_Armaz_Dados_Umi 20 //Representa a quantidade de dados que serão gravados temporariamente para fins do Histórico da Umidade

//==========WiFi====================

//const char* SSID = "SHALLON 2.4G";    // SSID  nome da rede WiFi que deseja se conectar
//const char* PASSWORD = "ADONAIHE";   // Senha da rede WiFi que deseja se conectar

const char* SSID = "Galaxy M31A4A7"; // SSID  nome da rede WiFi que deseja se conectar
const char* PASSWORD = "naosei123"; // Senha da rede WiFi que deseja se conectar
WiFiClient wifiClient; // Declarando variável Client


//==========MQTT SERVER===================

const char* BROKER_MQTT = "test.mosquitto.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                        // Porta do Broker MQTT

#define ID_MQTT  "BCI02"                  //ID da placa 
#define TOPIC_SUBSCRIBE_A "BCIBotao1"     //Assina tópicos 
#define TOPIC_SUBSCRIBE_B "mediaTemp"     //Assina tópicos 
#define TOPIC_SUBSCRIBE_C "mediaUmi"      //Assina tópicos 
#define TOPIC_SUBSCRIBE_D "Historico"     //Assina tópicos 
#define TOPIC_SUBSCRIBE_E "SetPoint_Temp" //Assina tópicos 
#define TOPIC_SUBSCRIBE_F "SetPoint_Umi"  //Assina tópicos 
#define TOPIC_SUBSCRIBE_G "Send_SetPoint_Temp" //Assina tópicos 
#define TOPIC_SUBSCRIBE_H "Send_SetPoint_Umi"  //Assina tópicos 
#define TOPIC_SUBSCRIBE_I "solicitaSetpoints"  //Assina tópicos 

#define TOPIC_PUBLISA "Historico"      //Assina tópicos 
#define TOPIC_PUBLISB "RecebeSetpoints"      //Assina tópicos 

PubSubClient MQTT(wifiClient);         // Instancia o Cliente MQTT passando o objeto espClient

//Declaração das Funções

void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void Alerta();


// Variavel Global


char Data_Valores_Sensores [Tam_Armaz_Dados_Temp][30]; // Variavel memoria volatil Temperatura
char Data_Valores_SensoresB [Tam_Armaz_Dados_Umi][30]; // Variavel memoria volatil Temperatura
char Data_Valores_SPT [Tam_Armaz_Dados_Umi][30]; // Variavel memoria volatil Temperatura
char Data_Valores_SPU [Tam_Armaz_Dados_Umi][30]; // Variavel memoria volatil Temperatura

int Aux_Temp = 0;
int Aux_Umi = 0;
char setpoint_Temp[30]="70" ;
char setpoint_Umi[30]="50" ;
int Acomparador1;
int Acomparador2;
int Bcomparador1;
int Bcomparador2;

void setup() {
  
  pinMode(pinLED, OUTPUT);
  Serial.begin(115200);
  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(controle);
}

void loop() {
  mantemConexoes();
  MQTT.loop();
}

void mantemConexoes() {
  if (!MQTT.connected()) {
    conectaMQTT();
  }

  conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
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


      MQTT.subscribe(TOPIC_SUBSCRIBE_A);
      MQTT.subscribe(TOPIC_SUBSCRIBE_B);
      MQTT.subscribe(TOPIC_SUBSCRIBE_C);
      MQTT.subscribe(TOPIC_SUBSCRIBE_D);
      MQTT.subscribe(TOPIC_SUBSCRIBE_E);
      MQTT.subscribe(TOPIC_SUBSCRIBE_F);
      MQTT.subscribe(TOPIC_SUBSCRIBE_G);
      MQTT.subscribe(TOPIC_SUBSCRIBE_H);
      MQTT.subscribe(TOPIC_SUBSCRIBE_I);
      
      digitalWrite(2, HIGH); // led apaga quando conectado
    }
    else {
      
      digitalWrite(2, LOW);// led acende quando não conectado
      Serial.println("Noo foi possivel se conectar ao broker.");
      Serial.println("Nova tentatica de conexao em 10s");
      delay(10000);
    }
  }
}







void controle(char* topic, byte* payload, unsigned int length)
{
  String msg ;


  Serial.println("Entrei no controle");

  Serial.println(topic);  //AVISO NA PORTA SERIAL PARA EFEITO DE DEBUGAÇÃO DO CÓDIGO 

  if (!strcmp(topic, "BCIBotao1"))
  {
  
    for (int i = 0; i < length; i++)   //obtem a string do payload recebido
    {
      char c = (char)payload[i];
      msg += c;

    }

    if (msg ==  "1") {
      digitalWrite(pinLED, LOW);
      delay(500);
      digitalWrite(pinLED, HIGH);
    }

    
  }


  else if (!strcmp(topic, "mediaTemp"))
  {
     Serial.println("Entrei no na media temp"); //AVISO NA PORTA SERIAL PARA EFEITO DE DEBUGAÇÃO DO CÓDIGO  

    for (int i = 0; i < length; i++)
    {
      char c = (char)payload[i];
      msg += c;
    }
    
   // Serial.println(msg);                       // MOSTRA O CODIGO LIDO (PARA EFEITO DE DEBUGAÇÃO)
     



    if (Aux_Temp < Tam_Armaz_Dados_Temp)         // VERIFICA SE A MATRIZ DE STRING ESTA CHEIA, CASO ESTEJA VAZIA SERÁ TRUE 
    {     
      strcpy(Data_Valores_Sensores[Aux_Temp],  msg.c_str());
      strcpy( Data_Valores_SPT[Aux_Temp],  setpoint_Temp);

         Serial.println("ESSE É O SPT:" ); 
         Serial.println( Data_Valores_SPT[Aux_Temp] ); 
         Acomparador1=atoi(Data_Valores_Sensores[Aux_Temp]);
         Acomparador2=atoi(Data_Valores_SPT[Aux_Temp]);
      Aux_Temp = Aux_Temp + 1 ;
    }
    else {                                        // CASO A MATRIZ DE STRING ESTA CHEIA, EXECUTE. 

      for (int i = 1; i < Tam_Armaz_Dados_Temp; i++)
      {
        strcpy(Data_Valores_Sensores[i - 1], Data_Valores_Sensores[i]); // TODOS OS DADOS SERÃO MOVIDO PARA UMA CASA ANTERIOR E O 1° DADO PERDIDO 
        strcpy( Data_Valores_SPT[i - 1],  Data_Valores_SPT[i]);
        
      }

      Aux_Temp = Aux_Temp - 1; // POSSIBILITA GRAVAR MAIS 1 DADO DA POSIÇÃO VAZIA DO VETOR
    }
    
 Serial.print( Acomparador1 ); 
  Serial.print( Acomparador2 ); 

    if(Acomparador1 > Acomparador2)
    {     int t =400;
      Alerta(t );
   }

      
  }

  else if (!strcmp(topic, "mediaUmi")) // FUNÇÃO COM AS MESMA FUNCIONALIDADES DO IF ACIMA, PORÉM AGORA PARA UMIDADE.
  {
    Serial.println("Entrei no na media Umi");

    for (int i = 0; i < length; i++)
    {
      char c = (char)payload[i];
      msg += c;

    }
   
    if (Aux_Umi < Tam_Armaz_Dados_Umi)
    {
      strcpy(Data_Valores_SensoresB[Aux_Umi],  msg.c_str());
        strcpy( Data_Valores_SPU[Aux_Umi],  setpoint_Umi);
         Bcomparador1=atoi(Data_Valores_SensoresB[Aux_Umi]);
         Bcomparador2=atoi(Data_Valores_SPU[Aux_Umi]);
      Aux_Umi = Aux_Umi + 1 ;

    }
    else {
      for (int i = 1; i < Tam_Armaz_Dados_Umi; i++)
      {
        strcpy(Data_Valores_SensoresB[i - 1], Data_Valores_SensoresB[i]);
        strcpy( Data_Valores_SPU[i - 1],  Data_Valores_SPU[i]);
        


        
      }

      Aux_Umi = Aux_Umi - 1;
      
    }
 Serial.print( Bcomparador1 ); 
  Serial.print( Bcomparador2 ); 

    if(Bcomparador1 > Bcomparador2)
    {  int t =100;   
      Alerta( t);
   }


    
  }




  else if (!strcmp(topic, "Historico"))// RESPOSNSAVEL POR TRANSMITIR OS DADOS SALVOS QUANDO SOLICITADO PELO SUPERVISORIO
  {
    char Pacote_Dados_Historico[60]; // MENSAGEM QUE SERA ENVIADA
    for (int i = 0; i < length; i++)
    {
      char c = (char)payload[i];
      msg += c;
    }

   // Serial.println("Estou enviando historico"); // PARA CONTROLE DEBUGAÇÃO


    if (msg == "H") {
      
      Serial.println("Essa e a matriz historico");
      
      for (int i = 0; i < Tam_Armaz_Dados_Temp; i++)
      {
       
        strcpy(Pacote_Dados_Historico,  "");
        strcat(Pacote_Dados_Historico, Data_Valores_Sensores[i]);
        strcat(Pacote_Dados_Historico, "J");
        strcat(Pacote_Dados_Historico, Data_Valores_SensoresB[i]);
        strcat(Pacote_Dados_Historico, "J");
        strcat(Pacote_Dados_Historico, Data_Valores_SPT[i]);
        strcat(Pacote_Dados_Historico, "J");
        strcat(Pacote_Dados_Historico,  Data_Valores_SPU[i]);
     
       //estrutura de Pacote_Dados_Historico[30]: valor temperatura + J + valor umidade
        
        MQTT.publish(TOPIC_PUBLISA, Pacote_Dados_Historico); // Envia Mensagem pro Broker
        Serial.println(Pacote_Dados_Historico);
        delay(100);

        msg = "F";

      }
    }
  }


   else if (!strcmp(topic, "Send_SetPoint_Temp"))
   {
     Serial.println("Mudando setpoint Temp");
     
    strcpy(setpoint_Temp, "");
  for (int i = 0; i < length; i++)
    {
      char c = (char)payload[i];
       msg += c;

    }
   strcpy(setpoint_Temp, msg.c_str());
    Serial.println("Novo Setpoint temp e");
     Serial.println(setpoint_Temp);
    strcpy(topic, "");

   
  }

    else if (!strcmp(topic, "Send_SetPoint_Umi"))
   {

     strcpy(setpoint_Umi, "");
     
  Serial.println("Mudando setpoint Umi");
  
  
  for (int i = 0; i < length; i++)
    {
      char c = (char)payload[i];
      msg += c;

    }

   strcpy(setpoint_Umi, msg.c_str());
    Serial.println("Novo Setpoint UMi e");
     Serial.println(setpoint_Umi);
     strcpy(topic, "");
  }
  
else if (!strcmp(topic, "solicitaSetpoints"))
   {
char pacote_setpoint[30];

        strcpy(pacote_setpoint,"");
        strcat(pacote_setpoint, setpoint_Temp);
        strcat(pacote_setpoint, "J");
        strcat(pacote_setpoint, setpoint_Umi);
        
MQTT.publish(TOPIC_PUBLISB, pacote_setpoint);
Serial.println(pacote_setpoint);
 strcpy(topic, "");



        
  }






}




void Alerta(int t)
{

  
      digitalWrite(pinLED, LOW);
      delay(t);
      digitalWrite(pinLED, HIGH);
      delay(t);
  digitalWrite(pinLED, LOW);
      delay(t);
      digitalWrite(pinLED, HIGH);
  
   
  
  }
