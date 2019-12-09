#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <UIPEthernet.h>
#include <Wire.h> //Built-in
#include <RtcDS3231.h> //Library Manager

#define radioID 0   //Informar "0" para um dispositivo e "1" para o outro dispositivo
uint32_t timer = 0;
RtcDS3231<TwoWire> Rtc(Wire);

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
EthernetServer server(80); //PORTA EM QUE A CONEXÃO SERÁ FEITA
RF24 radio(9, 10); // CE, CSN
const byte address[][6] = {"00001", "00002" };

boolean button_state = 0;
boolean x;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //ATRIBUIÇÃO DE ENDEREÇO MAC AO ENC28J60
byte ip[] = { 192, 168, 0, 112 };

//-------------Inicio------------------Pack
struct pack {

  
  unsigned short int ldrValor = 0;
  unsigned short int umidadeValor =  0;
  double temp = 0;
  double umid = 0;
  boolean reset;
  unsigned short int contagem_M1;
  unsigned short int contagem_M2;
  char silo_1[8] = "\0";
  char silo_2[8] = "\0";

};

pack pack_envio;
//--------------Fim-------------------Pack
//Pinos

const int reset_Pin = 3;
char check_S1[16];
char check_S2[16];
int check_M1;
int check_M2;
int check;

void setup() //----------------------------------------------Setup
{
  lcd.begin (16, 2);
  Rtc.Begin();
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.begin(9600);
  //radio
  radio.begin();

#if radioID == 0
  radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);
#else
  radio.openWritingPipe(address[1]);
  radio.openReadingPipe(1, address[0]);
#endif


  displayInicio();
  iniciarRelogio();
  radio.setPALevel(RF24_PA_MIN);


  //botoes
  pinMode(reset_Pin, OUTPUT);
  digitalWrite(reset_Pin, HIGH);

}

void loop()
{
   
  if (millis() - timer >= 2000)
  {
    Serial.print("Umidade: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(pack_envio.umid); //IMPRIME NA SERIAL O VALOR DE UMIDADE MEDIDO
  Serial.print("%"); //ESCREVE O TEXTO EM SEGUIDA
  Serial.print(" / Temperatura: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(pack_envio.temp, 0); //IMPRIME NA SERIAL O VALOR DE UMIDADE MEDIDO E REMOVE A PARTE DECIMAL
  Serial.println("*C"); //IMPRIME O TEXTO NA SERIAL

    
   Serial.print("Umidade Solo: ");
   Serial.println(pack_envio.umidadeValor);
   Serial.print("Luminosidade: ");
   Serial.println(pack_envio.ldrValor);
    timer = millis(); // Atualiza a referência
    Serial.println("");
  }
  //Relogio();
  servidor();
  print_Silos();
  Receber();
  Maquina_1();
  Maquina_2();
  resetar();
}



void Receber() {
  radio.startListening();              // ler informaçoes do outro pipe
  while (radio.available()) { //verifica se a conexão esta habilitada

    Serial.println("conexao ativa");
    radio.read(&pack_envio, sizeof(pack_envio));
    radio.flush_rx();
  }

  delay(10);

}


String checagem(int check)
{

  Serial.println("");
  Serial.print("Verificação de envio: ");
  if (check == 1) {
    Serial.println("Enviado.");
  } else {
    Serial.println("Negado.");
    radio.begin();

  }

  //return 0;
  Serial.println("");

}
