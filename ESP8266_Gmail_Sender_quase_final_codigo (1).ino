#include <ESP8266WiFi.h>
#include "Gsender.h"
#include <IRremoteESP8266.h>
#include <IRsend.h> //Necessário inserir essa linha para atualizar o código
#include <IRrecv.h>; //Necessário inserir essa linha para atualizar o código

IRsend irsend(14); //Função responsável pela clonagem do sinal do Ar-Condicionado - LED ligado no Pino D6
int temp_pin = A0;

uint16_t tamanho = 200; // Tamanho da Linha RAW em blocos (São os números correspondentes aos botões do Ar-Condicionado) - Trocar de unsigned int para uint16_t por conta da nova versão da Biblioteca
uint16_t frequencia = 38; //32kHz Frequência do Sinal IR - Trocar de unsigned int para uint16_t por conta da nova versão da Biblioteca

float media;

int contador = 0;
bool ligado = false;
bool Ultimo_Estado = false;
int tempo;

#pragma region Globals
const char* ssid = "BS_MONITOR";                           // WIFI network name
//const char* ssid = "AndroidAP";                           // WIFI network name
const char* password = "@@@bs###";                       // WIFI network password
//const char* password = "wMahh9262";                       // WIFI network password
uint8_t connection_state = 0;                    // Connected to WIFI or not
uint16_t reconnect_interval = 10000;             // If not connected wait time to try again
#pragma endregion Globals

uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);  
        Serial.println(nSSID);
    } else {
        WiFi.begin(ssid, password);
        Serial.println(ssid);
    }

    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 50)
    {
        delay(200);
        Serial.print(".");
    }
    ++attempt;
    Serial.println("");
    if(i == 51) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Check if access point available or SSID and Password\r\n");
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}

void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state)
    {
        delay(50);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}

void setup(){
  Serial.begin(115200);
  irsend.begin(); //INICIALIZA A FUNÇÃO
  connection_state = WiFiConnect();
  if(!connection_state)  // if not connected to WIFI
//    Awaits();          // constantly trying to connect
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}

void loop(){
  bool fora_faixa = true;
//  ligado = true;
  while (fora_faixa = true){
    medir_temp();
//    Serial.println(media);
    if(/*media > 24.0 &&*/ media < 22.5){
      Serial.println("1");
      if (ligado == false){
        delay(50);
        digitalWrite(3, LOW);
        delay(10);
        digitalWrite(5, HIGH);
//        fora_faixa = false;
        Serial.println("2");
//        ligar();

        tempo = millis();
        if(tempo > (3600/2)*1000){
          ESP.reset();
        }
        break;
        Serial.println("break");
      }
    }
    if ((/*media < 24.0 ||*/ media > 22.5)){
      Serial.println("3");
      if (ligado == false){
        delay(50);
        digitalWrite(5, LOW);
        delay(10);
        digitalWrite(3, HIGH);
        fora_faixa = true;
        Serial.println("4");
        ligar();
      }
    }
    if(/*media > 24.0 && */media < 22.20){
      Serial.println("5");
      if(ligado == true){
        delay(50);
        digitalWrite(3, LOW);
        delay(10);
        digitalWrite(5, HIGH);
        fora_faixa = true;
        Serial.println("6");
        delay(10);
        for (int i = 0; i<6; i++){
          desligar();
          delay(100);
          Serial.print("i=");
          Serial.println(i);
        }
//        ligar();
        delay(10);
        Serial.println("7");        
        ligado = false;
      }
    }
  }
  delay(1000);  

}


float medir_temp(){
  float temp_vector[25];
  float soma = 0;
  for (int i = 0; i<25; i++){
    int analogValue = analogRead(temp_pin);
    float miliVolts = (analogValue/1024.0)*3300; //Converte o sinal do LM35 em Tensão
    temp_vector[i] = miliVolts/10 - 2.0;
    Serial.println(temp_vector[i]); //debuggar
    soma = soma + temp_vector[i];
    delay(1000);
  }
  media = soma/25.0;
  Serial.print("MÃ©dia - ");
  Serial.println(media);
  return media;  
}

bool ligar(){
  uint16_t ON[200] = {4452,4332,600,1588,592,528,592,1568,596,1588,620,500,568,528,568,1592,592,528,592,508,588,1556,632,504,592,500,596,1564,616,1564,596,528,568,1580,632,1572,592,528,588,1572,616,1568,592,1592,620,1560,620,1564,620,1568,616,504,596,1564,592,528,588,508,588,508,588,504,592,504,568,528,592,508,588,1552,632,1552,628,504,592,504,592,504,568,528,588,508,568,1576,632,500,592,504,592,1552,632,1552,632,1548,632,1552,632,1568,532,5236,4472,4320,592,1588,596,528,564,1596,592,1592,592,528,588,508,588,1552,632,504,564,532,592,1568,620,504,568,524,592,1568,612,1568,616,504,592,1572,616,1556,632,508,588,1556,632,1548,632,1564,620,1564,616,1552,632,1568,616,504,592,1564,620,504,592,504,588,508,588,504,592,500,596,500,596,504,592,1548,632,1552,632,500,596,500,568,528,568,528,588,508,568,1576,632,500,596,500,592,1552,632,1564,616,1596,588,1568,592,1592,580};
//  delay(100);
//  unsigned int ON[200] = {4452,4332,576,1612,604,532,564,1584,600,1584,600,532,564,532,564,1580,604,536,536,560,560,1588,600,532,564,532,564,1584,600,1580,600,540,556,1604,588,532,564,532,564,532,564,1580,604,1580,600,1600,564,1616,592,1580,580,1612,600,1584,580,1620,588,532,564,532,560,536,560,536,560,532,564,536,564,1580,604,1580,608,532,556,1588,604,532,560,536,560,536,564,1584,600,532,564,532,564,1580,600,536,560,1588,580,1616,564,1608,620,5176,4464,4344,564,1604,580,556,536,1612,604,1580,576,560,560,536,560,1584,600,532,564,536,540,1604,576,564,532,564,560,1580,576,1624,560,560,540,1612,600,536,536,560,536,560,536,1604,576,1608,576,1608,604,1580,600,1588,576,1628,560,1620,592,1580,600,536,560,532,540,556,540,556,564,532,540,556,564,1588,576,1604,576,560,564,1584,576,564,560,532,564,536,536,1608,576,560,536,560,560,1580,576,564,532,1616,600,1584,576,1608,624};  
//unsigned int ON[200] = {4452,4328,620,1564,616,476,596,1588,620,1560,624,472,616,480,616,1560,620,476,620,476,616,1564,620,476,596,496,620,1560,620,1556,624,472,620,1568,620,476,596,500,616,476,596,1584,620,1560,592,1592,616,1560,624,1560,624,1564,624,1556,600,1584,620,476,620,472,616,480,616,472,620,476,620,476,620,472,620,476,592,500,616,1560,620,476,620,472,620,476,616,1564,620,1560,596,1584,620,1564,620,476,616,1568,596,1584,620,1564,536,5236,4448,4332,624,1560,596,496,620,1564,624,1560,620,472,620,476,620,1560,620,480,616,480,616,1564,596,500,616,480,592,1584,620,1560,620,476,616,1568,620,480,616,476,596,496,620,1560,620,1560,596,1584,620,1560,624,1564,620,1568,616,1560,600,1584,620,476,620,476,616,476,620,472,616,480,616,476,620,476,596,496,620,476,616,1560,596,496,620,476,616,480,616,1564,620,1556,600,1584,620,1564,616,476,620,1564,620,1560,620,1564,540};  
  for (int i = 0; i<6; i++){
  irsend.sendRaw(ON, tamanho, frequencia);
  Serial.println("Vermelho!");
    delay(100);
    Serial.print("i=");
    Serial.println(i);
  }
//  irsend.sendRaw(ON, tamanho, frequencia);
//  Serial.println("Vermelho!");
  ligado = true;
  delay(50);
  enviar_email();
  delay(50);
  return ligado;
}
bool desligar(){
//  unsigned int OFF[200] = {9200, 4500,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 1650,650, 1650,650, 1650,650, 1650,650, 1650,650, 1650,650, 1650,650, 1650,650, 550,650, 1650,650, 550,650, 1700,600, 550,650, 550,650, 550,650, 550,650, 1650,650, 550,650, 1650,650, 550,650, 1650,650, 1650,650, 1650,650, 1650,650};
//  delay(100);
  uint16_t OFF[200] = {4476,4320,596,1588,592,528,568,1596,592,1588,596,528,592,504,564,1592,612,508,592,508,588,1556,632,500,596,500,568,1592,592,1588,596,528,588,1572,620,504,592,1548,632,1568,616,1568,616,1568,588,532,592,1568,616,1568,620,1556,632,508,588,504,592,500,596,500,592,1564,620,504,568,524,596,1568,620,1564,616,1564,596,528,588,504,592,500,596,500,596,500,596,500,596,500,588,508,588,1568,596,1588,588,1592,616,1564,620,1568,532,5260,4452,4336,600,1588,592,528,568,1588,620,1568,616,504,592,480,616,1560,620,508,588,480,596,1588,620,480,616,500,596,1560,596,1584,620,480,616,1572,616,480,620,1564,616,1564,620,1564,592,1592,616,480,592,1592,596,1588,620,1572,592,528,588,480,616,508,588,476,596,1584,620,480,612,484,612,1568,600,1584,620,1564,596,528,588,484,612,508,588,476,596,500,620,476,620,476,616,476,620,1564,596,1588,596,1584,596,1588,596,1588,540};
//  unsigned int OFF[200] = {4396,4396,604,1584,596,536,560,1584,604,1580,600,540,556,540,556,1584,600,536,560,536,560,1608,580,536,560,536,532,1608,596,1588,596,536,560,1588,604,536,560,536,556,540,556,1584,600,1584,600,1580,604,1580,600,1588,596,1588,604,1584,600,1584,596,536,560,536,560,536,560,536,560,536,560,536,560,536,560,536,560,536,556,1584,596,536,560,536,560,536,560,1584,600,1604,580,1584,600,1584,596,536,560,1588,600,1584,596,1588,548,5228,4428,4376,584,1584,600,536,556,1592,596,1584,604,532,560,536,560,1584,596,544,556,540,556,1584,604,536,560,532,532,1612,596,1588,596,536,532,1616,604,536,560,536,532,560,536,1604,576,1608,600,1584,600,1600,556,1612,576,1616,572,1608,552,1632,576,560,536,560,536,560,532,564,532,560,540,556,540,556,540,556,536,560,536,1604,576,564,532,556,540,560,536,1604,576,1608,576,1624,560,1608,572,564,532,1612,576,1608,576,1612,596};
  irsend.sendRaw(OFF, tamanho, frequencia);
  Serial.println("Verde!");
  ligado = false;
  delay(50);
  return ligado;
}

void enviar_email(){ // Parte do código que programa o envio do e-mail 
//  medir_temp();
  Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
  String subject = "Temperatura Agência Transfusional";
  String string_media;
  string_media = int(media);
  String corpo_email;
  corpo_email = "A temperatura na Agência Transfusional é de " + string_media + ". O ar condicionado já foi ligado.";
//  if(gsender->Subject(subject)->Send("barbararibeirocrepaldi@gmail.com", corpo_email)) {
  if(gsender->Subject(subject)->Send("hemocentro1@physisjr.com", corpo_email)) {  
    Serial.println("Message send.");
  } else {
    Serial.print("Error sending message: ");
    Serial.println(gsender->getError());
  }
}
