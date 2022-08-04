#include <Arduino.h>
#include <Adafruit_GFX.h>    // Adafruit Grafik-Bibliothek
#include <Adafruit_SSD1306.h>// Adafruit ST7735-Bibliothek
#include "SD.h"
#include "FS.h"
#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>  
#include <TinyGPS.h>     
#include <HardwareSerial.h>    

#define OLED_RESET     4
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C

//LoRa
#define ss 5
#define rst 14
#define dio0 2

//Puerto serial
#define Rx_ss 16
#define Tx_ss 17

//Pines para interactuar
#define TX_done 25

//Manejo de datos GPS
TinyGPS gps;
float latitud_rx, longitud_rx, altura_rx;
unsigned long age;
int year;
byte month, day, hour, minute, second, hundredths;
char fecha__hora_UTC[32];//Para mostrar la fecha como string


unsigned short sentences = 0, failed = 0;
int distance=0;
float azimuth=0.0;
long int_lat=0, int_long=0, int_alt=0;

//Parámetros de calidad de la transmision
int val_rssi, val_rssi_mean=0;
float val_snr=0.0;

//Punto de referencia Inicial
double Latitud_ref =-0.233387;//
double Longitud_ref =-78.447908;//

//Banderas para control de flujo
bool F1=false;
bool F2=false;
bool F3=false;
bool F4=false;
bool F5=false;
bool F6=false;
bool F7=false;
bool F8=false;
bool F9=false;
bool F10=false;
bool newData=false;

bool flag_A=false;
int flag_B=1;
bool flag_C=false;
bool flag_D=false;

//Manejo de la SD card:
File SD_card_file;
int UltimaPocicion=0;
int Valor=0;
char Archivo[50]="";
String cadena = "";
char Doc[50]="";

//Variable para coroborar datos
uint32_t Muestra_data=0;
uint32_t Contador=0;

//Creación de objetos para manejar periféricos
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
HardwareSerial mySerial_port(2);

void setup()
{
    //Inicializacion de los puertos seriales
    Serial.begin(9600);
    mySerial_port.begin(9600, SERIAL_8N1, Rx_ss, Tx_ss);
    
    //Iniciando pines para pulsantes
    pinMode(TX_done,OUTPUT);
    digitalWrite(TX_done,LOW);

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
    LoRa.setSpreadingFactor(12);//LoRa.setSpreadingFactor(9);
    LoRa.setPreambleLength(6);//LoRa.setPreambleLength(8);
    LoRa.setCodingRate4(7); //LoRa.setCodingRate4(8)
    LoRa.enableCrc();
    delay(2000);

    //Iniciando la comunicación con la memoria SD
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
        SD_card_file.println("Latitud,Longitud,Item_TX");
        SD_card_file.close();
        Valor = Valor + 1;
        SD_card_file = SD.open("/Tracker_data/Num_doc.txt", FILE_WRITE);
        SD_card_file.println(Valor);
        SD_card_file.close();
        oled.println("Datalogger ok!");
        oled.println(Doc);
        oled.display();
    }
    else
    {
        //Serial.println("Error al abrir el archivo");
        oled.println("Error al abrir el archivo");
        oled.display();
    }
    delay(2500);
    oled.clearDisplay();
    oled.display();
	
}

void loop()
{
    datos_GPS_RX();
    if (F7 == true)
        {
            F9=false;
            F8=Borrar_escena(F8);
            oled.clearDisplay();
            oled.setTextSize(1);
            oled.setCursor(0, 0);
            oled.println("LoRa Sender...");
            oled.println("-----Posc.Actual-----");
            
            oled.setCursor(0, 14);
            oled.print("lat:");
            print_item_GPS(latitud_rx, 35, 16, 6);
            oled.setCursor(0, 24);
            oled.print("long:");
            print_item_GPS(longitud_rx, 35, 24, 6);

            oled.display();
            LoRa_send_packet();
            Guardar_SD();
        }
        else
        {
            F8=false;
            F9=Borrar_escena(F9);
            oled.setTextSize(1);
            oled.setCursor(0, 0);
            oled.println("LoRa Sender...");
            oled.println("Esperando GPS Tx...");
            oled.display();
        }
	
}

void datos_GPS_RX()
{
    for (unsigned long start = millis(); millis() - start < 2000;)//Recibir datos GPS por 1seg.
    {
        while (mySerial_port.available())
        {
            char tramaGPS = mySerial_port.read();
            if (gps.encode(tramaGPS)) // Nueva secuencia recibida
            {
                newData = true;
            }
        }
    }

    if (newData == true)
    {
        gps.f_get_position(&latitud_rx, &longitud_rx, &age);
        altura_rx = gps.f_altitude();
        gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
        //Verificar que los datos decodificados sean correctos:
        F3 = gps_rx_validos(latitud_rx, TinyGPS::GPS_INVALID_F_ANGLE);
        F4 = gps_rx_validos(longitud_rx, TinyGPS::GPS_INVALID_F_ANGLE);
        F5 = gps_rx_validos(altura_rx, TinyGPS::GPS_INVALID_F_ALTITUDE);
        F6 = gps_rx_validos(age, TinyGPS::GPS_INVALID_AGE);

        if (F3 == true && F4 == true && F5 == true) //Para controlae escena de espera del GPS_RX
        {
            F7 = true;
        }
        else
        {
            F7 = false;
        }

        newData = false;
    
    }
}

bool gps_rx_validos(float val, float invalid)
{
    bool GPS_data_ok = false;
    if (val == invalid)
    {
        GPS_data_ok = false;
    }
    else
    {
        GPS_data_ok = true;
    }

    return GPS_data_ok;
}

bool Borrar_escena(bool Bandera_estado)
{
    if (Bandera_estado == false)
    {
        oled.clearDisplay();
        oled.display();
        Bandera_estado = true;
    }
    return Bandera_estado;
}

void print_item_GPS(float item,int x, int y, int precs_dig)
{
    oled.setCursor(x, y);
    oled.println(item, precs_dig);
}

void LoRa_send_packet()
{
    digitalWrite(TX_done,HIGH);
    if (Contador==5) Contador=0;
    Contador=Contador+1;
    int_lat = latitud_rx * 1000000.0;
    int_long = longitud_rx * 1000000.0;
    Muestra_data=Contador+100;
    LoRa.beginPacket();
    LoRa.print("A");
    LoRa.print(int_lat);
    LoRa.print("B");
    LoRa.print(int_long);
    LoRa.print("C");
    LoRa.print(Muestra_data);
    LoRa.print("D");
    LoRa.endPacket();
    delay(50);
    digitalWrite(TX_done,LOW);

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
        SD_card_file.print(latitud_rx, 6);
        SD_card_file.print(",");
        SD_card_file.print(longitud_rx, 6);
        SD_card_file.print(",");
        SD_card_file.println(Muestra_data);
        SD_card_file.close();
    }
}
