// C++ code
//

// IMPORTS

#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

// LEDs

#define ledVerde 2
#define ledAmarelo 3
#define ledVermelho 4

// BOTOES

#define botao 5
#define btn digitalRead(botao)

// PIEZOS

#define buz 6

// LCDs

LiquidCrystal_I2C lcd(0x27, 16, 2);

// RFIDs

#define rfid_rst 9
#define rfid_sda 10

MFRC522 mfrc522(rfid_sda, rfid_rst);

// VARIAVEIS

unsigned long tempoAmarelo = 3000;
unsigned long tempoVerde = 15000;
unsigned long tempoVermelho = 10000;
unsigned long tempoCartao = 15000;

unsigned long currentMillis = 0;
unsigned long startMillis = 0;
unsigned long contadorMillis=0;
unsigned long sec = 1000;

bool temp=1;
bool esp=0;
bool verde=1;
bool limite=0;
bool vermelho=0;

// FUNCOES

void setup(){
  Serial.begin(9600);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
  pinMode(buz, OUTPUT);
  SPI.begin();  // Inicializacao da comunicacao em spi * NECESSARIO PARA A COMUNICACAO DO RFID E ARDUINO
  mfrc522.PCD_Init();  // Inicializacao do RFID * NECESSARIO PARA INICAR O RFID
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("O SINAL ESTA : ");

  delay(5);  // Pequeno delay para deixar o RFID estabilizar especialmente com fios longos * OPCIONAL IMPORTANTE
  mfrc522.PCD_DumpVersionToSerial();
}

void usoCartao(){
  esp=1;
  tone(buz,400,600);
  lcd.setCursor(1, 0);
  Serial.println("Cartao Utilizado");
  lcd.print("CARTAO UTILIZADO");
  delay(500);
  noTone(buz);
  limite=1;
  mfrc522.PICC_HaltA(); 
  return;
}

void contador(int tempo){
  if(esp && tempo>9) tone(buz,400,600);
  if(esp && tempo<10) tone(buz,200,600);
  lcd.setCursor(0,1);
  if (tempo == 9) lcd.print("  ABERTO - 9    ");
  lcd.print("  ABERTO - "+String(tempo));
  delay(500);
  noTone(buz);
}

void atraso(unsigned long tempo, bool fase){
  int i = tempo/1000+1;
  do{
    currentMillis = millis();
    if(currentMillis - startMillis >= tempo && i<=0){
      startMillis = currentMillis;
      delay(500);
      return;
    }
    if(currentMillis - contadorMillis >= sec){
      i--;
      if(!fase)contador(i);
      contadorMillis = currentMillis;
    }
    
    if((!btn && !limite && !vermelho) || (mfrc522.PICC_IsNewCardPresent() && !limite && !vermelho))usoCartao();
    //Serial.println(btn);
  }while(tempo>0);
}

void sinalVerde(){
  verde=!verde;
  lcd.setCursor(0,1);
  lcd.print("  FECHADO            ");
  sinalAmarelo(ledVerde);
  atraso(tempoVerde, 1);
  verde=!verde;
}

void sinalAmarelo(int led){
  digitalWrite(ledVermelho,0);
  digitalWrite(ledVerde,0);
  if(verde){
    digitalWrite(ledAmarelo,1);
    atraso(tempoAmarelo, 1);
  }
  digitalWrite(ledAmarelo,0);
  digitalWrite(led,1);
}

void sinalVermelho(bool cartao){
  vermelho=!vermelho;
  sinalAmarelo(ledVermelho);
  if(esp)atraso(tempoCartao,0);
  else if(!esp)atraso(tempoVermelho,0);
  esp=0;
  Serial.println("cabo o sinal vermelho");
  vermelho=!vermelho;
  limite=0;
  Serial.println(vermelho);
  Serial.println(limite);
}

void loop(){
  lcd.setCursor(1, 0);
  lcd.print("O SINAL ESTA : ");
  limite=0;
  sinalVerde();
  sinalVermelho(esp);
}
