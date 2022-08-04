#include <Arduino.h>
#include <Adafruit_GFX.h>    // Adafruit Grafik-Bibliothek
#include <Adafruit_SSD1306.h>// Adafruit ST7735-Bibliothek
#include "SD.h"
#include "FS.h"
#include <SPI.h>
#include <Wire.h>
#include <LoRa.h> 
#include <HardwareSerial.h>      

#define OLED_RESET     4
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C

#define ss 5
#define rst 14
#define dio0 2

//Parámetros de calidad de la transmision
int val_rssi, val_rssi_mean=0;
float val_snr=0.0;

//Variable para coroborar datos
uint8_t Conta_Ramp=1;
uint32_t Conta_datos=1;
uint32_t data1=0;
uint32_t data2=0;

//Manejo de la SD card:
File SD_card_file;
int UltimaPocicion = 0;
int Valor = 0;
char Archivo[50] = "";
String cadena = "";
char Doc[50] = "";

//Creación de objetos para manejar periféricos
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup()
{
    //Inicializacion de los puertos seriales
    Serial.begin(9600);
    //Inicializando el display Oled
    oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.clearDisplay();
    oled.setCursor(0,0);

    //Iniciando LoRa
    oled.println("LoRa Sender...");
    oled.display();
    LoRa.setPins(ss, rst, dio0);
    if (!LoRa.begin(433E6))
    { // or 915E6
        oled.println("Starting LoRa failed!");
        oled.display();
        while (1);//De no poder iniciarse, se quedara indefinidamente aqui
    }
    //Estas configuraciones deben ser las mismas en el receptor y transmisor. 
    LoRa.setTxPower(20);
    LoRa.setSignalBandwidth(250E3);
    LoRa.setSpreadingFactor(12); //LoRa.setSpreadingFactor(9);
    LoRa.setPreambleLength(6);  //LoRa.setPreambleLength(8);
    LoRa.setCodingRate4(7);     //LoRa.setCodingRate4(8)
    LoRa.enableCrc();
    
    delay(2000);
    oled.println("OK!,Parameters:");
    oled.display();
    delay(2000);
    oled.println("TXpower:20dBm");
    oled.println("Bandwidth:250kHz");
    oled.println("SpreadingFactor:12");
    oled.println("Preamble:6");
    oled.println("CodingRate:4/7");
    oled.display();
    delay(3000);
    oled.clearDisplay();
    //Iniciando la comunicación con la memoria SD
    oled.setCursor(0,0);
    oled.println("Iniciando SD ...");
    oled.display();
    if (!SD.begin(13))
    {
        //Serial.println("No se pudo inicializar");
        oled.println("No se pudo inicializar");
        oled.display();
    }
    else
    {

        //Serial.println("inicializacion exitosa");
        oled.println("SD conectada...");
        oled.display();
    }

    SD_card_file = SD.open("/Tracker_data/Num_doc.txt"); //abrimos  el archivo
    int totalBytes = SD_card_file.size();
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
        //Serial.print("Cadena Leida:");
        //Serial.println(cadena);
        Valor = cadena.toInt();
        sprintf(Doc, "Doc#-->Data%d_TX.txt", Valor);
        sprintf(Archivo, "/Tracker_data/Data%d.txt", Valor);
        SD_card_file = SD.open(Archivo, FILE_WRITE);
        SD_card_file.println("Contador_TX,Numero_dato");
        SD_card_file.close();
        Valor = Valor + 1;
        SD_card_file = SD.open("/Tracker_data/Num_doc.txt", FILE_WRITE);
        SD_card_file.println(Valor);
        SD_card_file.close();
        oled.println("Datalogger ok!");
        oled.println(Doc);
        oled.display();
        delay(3000);
    }
    else
    {
        //Serial.println("Error al abrir el archivo");
        oled.println("Error al abrir el archivo");
        oled.display();
        delay(3000);
    }
	
}

void loop()
{
    oled.clearDisplay();
    oled.setCursor(0,0);
	oled.println("Enviando datos");
    oled.print("-->Contador:");
    oled.println(Conta_Ramp);
    oled.print("-->Contador Nro:");
    oled.println(Conta_datos);
    oled.display();
    data1=Conta_Ramp+100000;
    data2=Conta_datos+100000;
    Serial.println(Conta_datos);
    LoRa.beginPacket();
    LoRa.print("A");
    LoRa.print(data1);
    LoRa.print("B");
    LoRa.print(data2);
    LoRa.print("C");
    LoRa.endPacket();
    delay(50);
    Guardar_SD();
    Conta_datos=Conta_datos+1;
    Conta_Ramp=Conta_Ramp+1;
    if (Conta_Ramp>5)
    {
        Conta_Ramp=1;
    }
    delay(1500);
    
}

void Guardar_SD()
{
  
    SD_card_file = SD.open(Archivo, FILE_APPEND);
    if (!SD_card_file)
    {
            //Serial.println("  No SD =(");
        SD_card_file.close();
    }
    else
    {

        SD_card_file.print(Conta_Ramp);
        SD_card_file.print(",");
        SD_card_file.println(Conta_datos);
        SD_card_file.close();
    }

}