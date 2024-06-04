#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Registra o nome da rede wifi que será usada e sua respectiva senha em variáveis
const char* ssid = "AAPM";
const char* password = "senai117";

// Registra endereço do MQTT Broker que será utilizado na variável
char* mqtt_server = "broker.app.wnology.io";

//*****************************celula 1*****************************************
//id: 6622558f6d789fc7f7eed1f4
//Access Key:81abb950-29d4-4b73-8645-fc6cd1101c0c
//Access Secret:6bd2e86e3be8c3bd8ee93d518b2d775fc8a537b90f64ff96d1c722a8f2df17dc
//********************************************************************************

//*****************************celula 2*****************************************
//id: 6622570f752526d1f7e90651
//Access Key:e6f92758-17cc-404a-9c84-7c600b3fa751
//Access Secret:18fee3ef7a2dfc2279a58bcbafbfc40fc4ddc0a7f9379c05bada17181b08ff92
//********************************************************************************

//*****************************celula 3********************************************
//const char* deviceID="662257d36d789fc7f7eed767";
//const char* accesskey="993c989c-b140-421a-98fc-8f51a845a381";
//const char* secretkey="526665619cfc9b1bd5852916859b3ff3a507dbb6953ad86a4a799a23a14d8fa3";
//const char* state_topic="wnology/662257d36d789fc7f7eed767/state";
//const char* command_topic="wnology/662257d36d789fc7f7eed767/command";

//********************************************************************************

//*****************************celula 4********************************************
//const char* deviceID="66225897396f6f630e3c4c27";
//const char* accesskey="4f0beea8-4644-412b-b198-e6a2ab78d692";
//const char* secretkey="5dd594df72f02a71afecae4eebcbaeb53155a6c6451a3610e75b0f827a5e19fa";
//const char* state_topic="wnology/66225897396f6f630e3c4c27/state";
//const char* command_topic="wnology/66225897396f6f630e3c4c27/command";
//*********************************************************************************


//**************************celula 3 gravado no esp32***********************************************************************************************
const char* deviceID="662257d36d789fc7f7eed767";
const char* accesskey="993c989c-b140-421a-98fc-8f51a845a381";
const char* secretkey="526665619cfc9b1bd5852916859b3ff3a507dbb6953ad86a4a799a23a14d8fa3";
const char* state_topic="wnology/662257d36d789fc7f7eed767/state"; //Registra configuração para envio de informações na variável
const char* command_topic="wnology/662257d36d789fc7f7eed767/command";//Registra configuração para recebimento de dados na variável
//**********************************************************************************************************************************
const char* comando;//
bool setMotor = 0;
bool Motor = 0;

#define botaoLigado 4 
#define botaoDesliga 5
#define FeedBack 2
#define LigaMotor 27
#define saida1 12 // LED indica Motor Ligado
#define saida2 13
#define saida3 14 //Define LED indica FALHA

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Definição da saída conectada ao sensor DHT
#define DHTPIN 15  

// Uncomment whatever DHT sensor type you're using
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)   

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Variables to hold sensor readings
float temp;
float hum;

//Variável para monitorar o estado do motor
int EstadoMotor = 0;

char json[90];

// LED Pin
//const int ledPin = 12;

DynamicJsonDocument doc(1024);

void setup() {
  Serial.begin(115200);
 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(botaoLigado, INPUT);
  pinMode(botaoDesliga, INPUT);
  pinMode(FeedBack, INPUT);
  pinMode(saida1, OUTPUT);
  pinMode(saida2, OUTPUT);
  pinMode(saida3, OUTPUT);
  pinMode(LigaMotor, OUTPUT);
  digitalWrite(LigaMotor, HIGH);

  dht.begin();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

void callback(char* topic, byte* message, unsigned int length) {  //Função quwe possibilita receber dados da wegnology no formato json, e atribuir o esperado à variáveis do programa
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

deserializeJson(doc,messageTemp);//
  comando=doc["name"];
  Motor=doc["payload"]["liga"];
  Serial.print("Comando recebido:");
  Serial.println(comando);
  Serial.print("Pacote recebido:");
  Serial.println(setMotor);

  //----------------------------------------------------INICIO: LOGICA DE ACIONAMENTO REMOTO ---------------------------------------------------------------------------------------
  if (String(topic) == command_topic) {
    Serial.print("Changing output to ");
    if(Motor==1){
      Serial.println("true");
      setMotor = 1;
      digitalWrite(saida1,HIGH);
      digitalWrite(LigaMotor, LOW); //Aciona relê de ligamento da contatora (Relè aciona em Zero)
      digitalWrite(saida2,HIGH);//Liga indicação de acionamento remoto
      EstadoMotor = 1;// Informa a plataforma IoT que o motor foi ligado

    }
    else if(Motor==0){
      Serial.println("false");
      setMotor = 0;
      digitalWrite(saida1,LOW);
      digitalWrite(LigaMotor, HIGH); //Aciona relê de ligamento da contatora
      EstadoMotor = 0;// Informa a plataforma IoT que o motor foi desligado
      digitalWrite(saida2,LOW);//Desliga indicação de acionamento remoto
    }
  }
      //-----------------------------------Fim da Logica de Acionamento Remoto ----------------------------------------------------------------------------------------------
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connect(deviceID,accesskey,secretkey)) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(deviceID,accesskey,secretkey)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(command_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}
void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  client.subscribe(command_topic);

  //-------------------------------------------------------LOGICA DE ACIONAMENTO LOCAL------------------------------------------------------------------------------------------

  if(digitalRead(botaoLigado)==HIGH) //Efetua a leitura do botão Liga 
  { 
    digitalWrite(saida1,HIGH);//Liga LED de sinalização
    digitalWrite(LigaMotor, LOW);// LIga MOtor
    digitalWrite(saida2,LOW);//Desliga indicação de acionamento remoto
    EstadoMotor = 1;// Informa a plataforma IoT que o motor foi ligado
    delay(1500);//Aguarda um tempo para confirmação do feedBack

    
  }

  /*if((digitalRead(FeedBack)== LOW) && (EstadoMotor==1))//Verifica se houve falha no Feedback
      {
        digitalWrite(saida1,LOW);
        digitalWrite(LigaMotor, HIGH);//Desliga MOtor
        digitalWrite(saida3,HIGH);
        delay(200);
        digitalWrite(saida3,LOW);
        delay(200);
        EstadoMotor = 2;// Identificada Falha do Motor
      } 
  */
         
  if(digitalRead(botaoDesliga) == HIGH)//Verifica se o botão desliga foi pressionado
  {
    EstadoMotor = 0;//Motor desligado 
    digitalWrite(saida1,LOW);
    digitalWrite(LigaMotor, HIGH);
    digitalWrite(saida2,LOW);//Desliga indicação de acionamento remoto
  } 

  //---------------------------------------Fim da Logica de Acionamento Local--------------------------------------------------------------------------------------------------

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
   
    // New DHT sensor readings
    hum = dht.readHumidity();
    // Read temperature as Celsius (the default)
    temp = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //temp = dht.readTemperature(true);
    
    // Convert the value to a char array
    char tempString[8];
    dtostrf(temp, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);

      
    // Convert the value to a char array
    char humString[8];
    dtostrf(hum, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);

    char EstadoMotorString[8];
    dtostrf(EstadoMotor, 1, 2, EstadoMotorString);
    Serial.print("EstadoMotor: ");
    Serial.println(EstadoMotorString);
    Serial.println(comando);
    Serial.println(setMotor);


    
   
    String cjson = "{\"data\": {\"tEmperature\": " + String(temp) + ", \"hUmidity\": " + String(hum)+ ",\"EstadoMotor\": " + String(EstadoMotor)+ "} }";
    cjson.toCharArray(json,90);
    client.publish(state_topic,json);
  }
}
