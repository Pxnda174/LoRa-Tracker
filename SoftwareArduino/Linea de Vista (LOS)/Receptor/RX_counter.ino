
#include <Arduino.h>
#include <Adafruit_GFX.h>  
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>         
#include <TinyGPS.h>    
#include <HardwareSerial.h> 
#include <HMC5883L.h>
#include <SD.h>
#include <FS.h>

//Asignacion de pines para el modulo LoRa:
#define ss 5
#define rst 14
#define dio0 2
/* https://how2electronics.com/esp32-lora-sx1278-76-transmitter-receiver/
ESP32 Pins	            SX1278 Pins
GND 	                  GND
3.3V	                  VCC
D5(CS)	                  NSS
D23 	                  MOSI
D19 	                  MISO
D18 	                  SCK
D14 	                  RST
D2 	                      DIO0

*/

//Asignacion de pines para el display tft:
#define TFT_CS 12
#define TFT_RST 27
#define TFT_DC 13

//Creación de clases para uso de las librerias
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
File SD_card_file;

//Variables para recepcion de datos LoRa
String inString = ""; // string to hold input
int val = 0;
String Datos_in = "";
int val_rssi, val_rssi_mean = 0;
float val_snr = 0.0;

//Variables para manipular datos de la SD
int UltimaPocicion=0;
int Valor=0;
char Archivo[50]="";
char Doc[50]="";
String cadena = "";
int totalBytes;

uint32_t Contador=0;
uint32_t Dato_num=0;


void setup()
{
    Serial.begin(9600);//Inicialización puerto serie (Comunicación con el PC)
//Inicialización del display tft
    tft.initR(INITR_BLACKTAB);
    tft.invertDisplay(false);
    //tft.setRotation(3);
    tft.fillScreen(ST7735_WHITE);                   //Background NEGRO de la TFT
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE); // set color de texto magenta con fondo negro (revisar seccion de Notas)
    tft.setTextSize(1);                             
    tft.setCursor(30,40); 
//Inicializacion modulo LoRa
    tft.println(F("LoRa Receiver"));
    LoRa.setPins(ss, rst, dio0);
    if (!LoRa.begin(433E6)) 
    { // or 915E6
       tft.setTextColor(ST7735_YELLOW, ST7735_WHITE); 
       tft.setCursor(0,48);
       tft.println(F("Starting LoRa failed!"));
       while (1);//De no iniciar correctamente el módulo LoRa se encierra en el lazo infinito y no se ejecuta lo demás
    }
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.setCursor(0,48);
    tft.println(F("Configuration completed successfully!...."));
    //Configuraciones extra del modulo LoRa (Revisar la seccion de Notas )
      LoRa.setSignalBandwidth(250E3);
      LoRa.setSpreadingFactor(12);
      LoRa.setPreambleLength(6);
      LoRa.setCodingRate4(7);
      LoRa.setGain(0);
      LoRa.enableCrc();
    //
    if (!SD.begin(33))
    { //Con esta definicion de 33 se cambia de pin CS...si no, SD.begin() coloca por defecto el GPIO5
        //Serial.println("Card Mount Failed");
        //Serial.println("No SD =(");
        tft.setCursor(0, 68);                         //Tabala de colores que en realidad estan siendo mostrados
        tft.setTextColor(ST7735_GREEN, ST7735_WHITE); //COLOR MAGENTA
        tft.println("SD failed, error!");
    }
    else
    {
        //Serial.println("SD =)");
        tft.setCursor(0, 68);                         //Tabala de colores que en realidad estan siendo mostrados
        tft.setTextColor(ST7735_GREEN, ST7735_WHITE); //COLOR MAGENTA
        tft.println("SD started, ok!");
    }

    SD_card_file = SD.open("/Tracker_data/Num_doc.txt"); //abrimos  el archivo
    totalBytes = SD_card_file.size();
    if (SD_card_file)
    {
        if (UltimaPocicion >= totalBytes)
            UltimaPocicion = 0;
        SD_card_file.seek(UltimaPocicion);

        //--Leemos una línea de la hoja de texto--------------
        while (SD_card_file.available())
        {

            char caracter = SD_card_file.read();
            cadena = cadena + caracter;
            UltimaPocicion = SD_card_file.position();
            if (caracter == 10) //ASCII de nueva de línea
            {
                break;
            }
        }
        //---------------------------------------------------
        SD_card_file.close(); //cerramos el archivo

        //Rutina para evitar la sobreescritura de la base de datos
        Valor = cadena.toInt();
        sprintf(Doc, "Doc#-->Data%d_RX.txt", Valor);
        sprintf(Archivo, "/Tracker_data/Data%d_RX.txt", Valor);
        SD_card_file = SD.open(Archivo, FILE_WRITE);
        SD_card_file.println("Contador_TX,Numero_dato,RSSI,SNR");
        SD_card_file.close();
        Valor = Valor + 1;
        SD_card_file = SD.open("/Tracker_data/Num_doc.txt", FILE_WRITE);
        SD_card_file.println(Valor);
        SD_card_file.close();
        tft.setCursor(0, 78);      
        tft.println("Datalogger ok!");
        tft.println(Doc);
    }
    else
    {
        //Serial.println("Error al abrir el archivo");
        tft.setCursor(0, 78);  
        tft.println("Error al abrir el archivo");
    }

    delay(5000);
    tft.fillScreen(ST7735_WHITE);//Borrar toda la escena mostrada (Display clear)
	
}

void loop()
{
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setTextSize(1);
    tft.setCursor(20,10);      
    tft.print(F("Datos recibidos"));
    datos_TX_in();
    tft.setCursor(10,30);      
    tft.print(F("Contador:"));
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.print(Contador);
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setCursor(10,40);      
    tft.print(F("Dato numero:"));
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.print(Dato_num);

    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setCursor(10,50);      
    tft.print(F("RSSI:"));
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.print(val_rssi_mean);
    tft.print("dB");

    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setCursor(10,60);      
    tft.print(F("SNR:"));
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.print((int)val_snr);
    tft.print("dB");

	delay(10);
    //tft.fillScreen(ST7735_WHITE);
}

void datos_TX_in()
{
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        // read packet
        while (LoRa.available())
        {
            int inChar = LoRa.read();
            inString += (char)inChar;
            Datos_in = inString;
        }
        Serial.println(Datos_in);
        separar_datos(Datos_in);
        val_snr = LoRa.packetSnr();
        val_rssi_mean=LoRa.packetRssi();
        Guardar_SD();
        inString = "";
    }

}

void separar_datos(String data_tx)
{
    long temp = 0;
    int index1 = 0;
    int index2 = 0;
    String dato_string = "";

    index1 = data_tx.indexOf('A');
    index2 = data_tx.indexOf('B');
    dato_string = data_tx.substring(index1 + 1, index2);
    Serial.println(dato_string);
    temp = dato_string.toInt();
    Contador=temp;

    index1 = data_tx.indexOf('B');
    index2 = data_tx.indexOf('C');
    dato_string = data_tx.substring(index1 + 1, index2); 
    Serial.println(dato_string);
    temp = dato_string.toInt();
    Dato_num=temp;
}

void Guardar_SD()
{
  
    SD_card_file = SD.open(Archivo, FILE_APPEND);
    if (!SD_card_file)
    {
        SD_card_file.close();
    }
    else
    {

        SD_card_file = SD.open(Archivo, FILE_APPEND);
        SD_card_file.print(Contador);
        SD_card_file.print(",");
        SD_card_file.print(Dato_num);
        SD_card_file.print(",");
        SD_card_file.print(val_rssi_mean);
        SD_card_file.print(",");
        SD_card_file.println(val_snr, 2);
        SD_card_file.close();
    }

}