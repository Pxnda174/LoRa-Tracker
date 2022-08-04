/*NOTAS:
Versión: 2.2.1
Display TFT usado: KMR-1.8 SPI
Tarjeta de desarrollo: ESP32 DEV kit C
Modelo de GPS: ublox neo-6m
Módulo de radiofrecuencia: xl1278-smt
Sensor magnetómetro: HMC5883L

Nota1: El controlador que emplea el display TFT es un ST7753, al tener problema con las diferentes inicializaciones del display
       junto con la libreria (Pixeles sin control, erroneo tamaño del display, etc.) se uso la configuración que se encuentra en
       el void setup. Con esta configuración se tiene el error de que los colores que se quieren mostrar no corresponden al
       configurado, por lo cual se procedio a realizar un sketch de prueba de colores para saber cual color se mostrará(revisar
       el stetch TFT_colores_esp32.ino).
       https://blog.simtronyx.de/en/a-1-8-inch-tft-color-display-hy-1-8-spi-and-an-arduino/

Nota2: Para m{as informacion sobre el uso de la libreria LoRa.h, revisar el siguiente repositorio:
        https://github.com/sandeepmistry/arduino-LoRa    en la sección de API.md

Nota3: Esta versión incluye el uso de una brujula digital usando el sensor QMC5883L, tener en cuenta que si se va usar el sensor HCM5883L,
       se debe usar una libreria diferenet ya que el address deñ sensor y registros son diferentes, a pesar que ambos modelos de sensores
       hacen exactamente lo mismo. Ver la referencia para mas informacion.
       https://surtrtech.com/2018/02/01/interfacing-hmc8553l-qmc5883-digital-compass-with-arduino/

Nota4: Las fórmulas utilizdas para el cálculo de la distancia y azimuth empleando por las librerias, son las siguientes:
       Para el cálculo de distancia se usa la formula de Vincenty, para mas información revisar el siguiente enlace:
       https://en.wikipedia.org/wiki/Great-circle_distance#Formulas
       Para el cálculo del azimuth se emplea la fórmula conocida como forward azimuth, para mas información revisar el enlace:
       http://www.movable-type.co.uk/scripts/latlong.html
       https://structural-analyser.com/domains/Surveying/chapter06/

Nota5: Como referencia de las diferentes funciones matemáticas que se pueden usar, revisar el siguiente enlace:
       http://www.nongnu.org/avr-libc/user-manual/group__avr__math.html#ga4ea549372745dda4018ab4b5a94483a6


*/

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
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Asignacion de pines para el modulo LoRa:
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

// Asignacion de pines para el display tft:
#define TFT_CS 12
#define TFT_RST 27
#define TFT_DC 13

// Asignacion de pines UART para módulo GPS
#define Rx_ss 16
#define Tx_ss 17

// GPIO a ser usado como selector:
#define Modo 26

// LED de notificacion
#define Rx_failed 25

// Creación de Objetos para uso de las librerias
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
HardwareSerial mySerial_port(2);
TinyGPS gps;
HMC5883L compass;
File SD_card_file;
BluetoothSerial BTSerial;

// Variables para recepcion de datos LoRa
String
    inString = "", // string to hold input
    Datos_in = "";
int
    val = 0,
    val_rssi = 0,
    val_rssi_mean = 0;
float
    val_snr = 0.0;

// Variables para la conversion de datos String to Float
float
    latitud_tx = 0.0,
    longitud_tx = 0.0,
    altura_tx = 0.0,
    ante_latitud_tx = 0.0,
    ante_longitud_tx = 0.0,
    ante_altura_tx = 0.0;
int
    ante_distancia = 0,
    ante_direccion = 0,
    Item_conta = 0;

// Variables bool para control de eventos
bool
    F1 = false,
    F2 = false,
    F3 = false,
    F4 = false,
    F5 = false,
    F6 = false,
    F7 = false,
    F8 = false,
    F9 = false,
    F10 = false,
    F11 = false,
    newData = false;

// Variables para recepccion de datos GPS
float
    latitud_rx = 0.0,
    longitud_rx = 0.0,
    altura_rx = 0.0;
int year;
byte month, day, hour, minute, second, hundredths;
unsigned long age;
char fecha__hora_UTC[32]; // Para mostrar la fecha como string
// unsigned long age, date, chars = 0;

// Variables para ubicar al drone en función de latitud y longitud recibidas
int distance = 0;
float azimuth = 0.0;
char Data_string[10];

// Variables para manejo de la brujula digital
const int
    centreX = 128 / 2,
    centreY = 160 / 2,
    diameter = 50;
char *cuadrantes[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
double angle = 0.0;
int last_dx, last_dy, dx, dy;
Vector norm;
float heading, declinationAngle;

// Variables para manipular datos de la SD
int
    UltimaPocicion = 0,
    Valor = 0;
char Archivo[50] = "", Doc[50] = "";
String cadena = "";
int totalBytes;

// Para cronometrar el tiempo de ejecución de instrucciones del Micro
volatile uint32_t StartTimer = 0;
volatile uint32_t EndTimer = 0;
//
volatile uint32_t contador = 0;
volatile uint32_t ActualTimer = 0;
volatile uint32_t BaseTime = 1;

// Variable para coroborar datos
uint32_t Muestra_data = 0;

void setup()
{
    Serial.begin(9600);                                  // Inicialización puerto serie (Comunicación con el PC)
    mySerial_port.begin(9600, SERIAL_8N1, Rx_ss, Tx_ss); // Inicialización puerto serie (Comunicación con el GPS)
    BTSerial.begin("ESP32_tracker");
    // GPIO del selector como entrada:
    pinMode(Modo, INPUT_PULLUP);
    pinMode(Rx_failed, OUTPUT);
    // Inicializacion del display tft:
    tft.initR(INITR_BLACKTAB);
    tft.invertDisplay(false);
    // tft.setRotation(3);
    tft.fillScreen(ST7735_WHITE);                 // Background NEGRO de la TFT
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE); // set color de texto magenta con fondo negro (revisar seccion de Notas)
    tft.setTextSize(1);
    tft.setCursor(30, 40);
    // Inicializacion del compass digital:
    compass.begin(); // Si no se puede iniciar el compass, no se continua con el resto del programa...se queda la pantalla en negro
    compass.setRange(HMC5883L_RANGE_1_3GA);
    compass.setMeasurementMode(HMC5883L_CONTINOUS);
    compass.setDataRate(HMC5883L_DATARATE_30HZ);
    compass.setSamples(HMC5883L_SAMPLES_8);
    // Set calibration offset. See HMC5883L_calibration.ino
    compass.setOffset(30, -245); // 22,-102
    last_dx = centreX;
    last_dy = centreY;
    // Inicializacion modulo LoRa:
    tft.println(F("LoRa Receiver"));
    LoRa.setPins(ss, rst, dio0);
    if (!LoRa.begin(433E6))
    { // or 915E6
        tft.setTextColor(ST7735_YELLOW, ST7735_WHITE);
        tft.setCursor(0, 48);
        tft.println(F("Starting LoRa failed!"));
        while (1)
            ; // De no iniciar correctamente el módulo LoRa, se encierra en el lazo infinito y no se ejecuta lo demás
    }
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.setCursor(0, 48);
    tft.println(F("Configuration completed successfully!...."));
    // Configuraciones extra del modulo LoRa (Revisar la seccion de Notas o el escrito del proyecto)
    LoRa.setSignalBandwidth(250E3);
    LoRa.setSpreadingFactor(12);
    LoRa.setPreambleLength(6);
    LoRa.setCodingRate4(7);
    LoRa.setGain(0);
    LoRa.enableCrc();

    // Inicialización de la tarjeta SD
    Iniciar_SDcard();
    SD_card_file = SD.open("/Tracker_data/Num_doc.txt"); // abrimos  el archivo
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
            if (caracter == 10) // ASCII de nueva de línea
            {
                break;
            }
        }
        //---------------------------------------------------
        SD_card_file.close(); // cerramos el archivo

        // Rutina para evitar la sobreescritura de la base de datos
        Valor = cadena.toInt();
        sprintf(Doc, "Doc#-->Data%d.txt", Valor);
        sprintf(Archivo, "/Tracker_data/Data%d_RX.txt", Valor);
        SD_card_file = SD.open(Archivo, FILE_WRITE);
        SD_card_file.println("Latitud_RX,Longitud_RX,Latitud_TX,Longitud_TX,Item_conta,RSSI_RX,SNR_RX,Distancia_to_TX,Azimuth_to_TX,Fecha(UTC)");
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
        // De no encontrar el archivo en la ruta...no se guardaran los datos en la SD
        tft.setCursor(0, 78);
        tft.println("Error al abrir el archivo");
    }
    delay(3000);
    tft.fillScreen(ST7735_WHITE); // Borrar toda la escena mostrada (Display clear)
}

void loop()
{
    if (digitalRead(Modo) == HIGH) // Modo visualizador
    {
        // StartTimer=millis();
        contador = contador + 1;
        if (contador <= 20)
        {
            F2 = false;
            F1 = Borrar_escena(F1);
            datos_drone_tx();
            /*if (millis() - ActualTimer >= BaseTime)//Contador de milisegundos transcuridos
              {
                  ActualTimer = millis();
                  contador = contador + 1;
              }*/
            separar_datos(Datos_in);
            datos_GPS_RX();
            hallar_cursoRX_to_TX(latitud_rx, longitud_rx, latitud_tx, longitud_tx);
            datos_RXTX_to_tft();
            datos_Rumbo_seguir_to_tft();
            calidad_rf();
            Guardar_SD();

            ante_latitud_tx = latitud_tx;
            ante_longitud_tx = longitud_tx;
            ante_altura_tx = altura_tx;
            ante_distancia = (int)distance;
            ante_direccion = (int)azimuth;
        }
        else
        {
            F1 = false;
            F2 = false;
            F9 = Borrar_escena(F9);
            last_pos_error();

            for (unsigned long start = millis(); millis() - start < 20000;) // Intentar reconectar duran 20seg
            {
                if (digitalRead(Modo) == LOW)
                {
                    F9 = false;
                    F2 = Borrar_escena(F2);
                    print_compass_tft();
                }
                else
                {
                    F9 = Borrar_escena(F9);
                    F2 = false;
                    last_pos_error();
                    datos_drone_tx();
                    datos_GPS_RX();
                    hallar_cursoRX_to_TX(latitud_rx, longitud_rx, ante_latitud_tx, ante_longitud_tx);

                    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
                    tft.setCursor(0, 80);
                    tft.println(F(" "));
                    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
                    tft.println(F("     Objetivo a:"));
                    tft.println(F(" "));
                    tft.print("distancia:");
                    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
                    sprintf(Data_string, "%04u", distance);
                    tft.print(Data_string);
                    tft.println("m");

                    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
                    tft.print("Direccion:");
                    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
                    sprintf(Data_string, "%03u", (int)azimuth);
                    tft.print(Data_string);
                    tft.println("deg");
                    tft.println(F(" "));
                    tft.setTextColor(ST7735_ORANGE, ST7735_WHITE);
                    tft.print("Reconected...");

                    if (contador == 0)
                    {
                        if (F10 == false)
                        {
                            tft.setTextColor(ST7735_ORANGE, ST7735_WHITE);
                            tft.println("OK!");
                            F10 = true;
                            F9 = false;
                        }
                    }
                }
            }

            if (contador != 0)
            {
                tft.setTextColor(ST7735_ORANGE, ST7735_WHITE);
                tft.println("Failed!");
                tft.setTextColor(ST7735_RED, ST7735_WHITE);
                tft.println("Fatal error!, se debe");
                tft.println("reiniciar el modulo");
                tft.println("transmisor...");
                delay(5000);
                F9 = false;
            }
            F10 = false;
        }
        // EndTimer=millis();
        // Serial.println(EndTimer-StartTimer);
    }
    else // Modo Brujula digital
    {
        F1 = false;
        F2 = Borrar_escena(F2);
        print_compass_tft();
    }
}

void Iniciar_SDcard()
{
    for (unsigned long start = millis(); millis() - start < 5000;) // Tratar de conectarse durante 2 seg.
    {
        if (!SD.begin(33))
        { // Con esta definicion de 33 se cambia de pin CS...si no, SD.begin() coloca por defecto el GPIO5
            // Serial.println("Card Mount Failed");
            // Serial.println("No SD =(");
            F11 = true;
            tft.setCursor(0, 68);                         // Tabala de colores que en realidad estan siendo mostrados
            tft.setTextColor(ST7735_GREEN, ST7735_WHITE); // COLOR MAGENTA
            tft.println("SD failed, error!");
        }
        else
        {
            // Serial.println("SD =)");
            tft.setCursor(0, 68);                         // Tabala de colores que en realidad estan siendo mostrados
            tft.setTextColor(ST7735_GREEN, ST7735_WHITE); // COLOR MAGENTA
            tft.println("SD started, ok!");
        }
    }
}

bool Borrar_escena(bool Bandera_estado)
{
    if (Bandera_estado == false)
    {
        tft.fillScreen(ST7735_WHITE);
        Bandera_estado = true;
    }
    return Bandera_estado;
}

void datos_drone_tx()
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

        inString = "";
        contador = 0;
        // Envio de datos por el puerto Serial
        /*
        Serial.print("Latitud: ");
        Serial.print(latitud_tx,6);
        Serial.print("  Longitud: ");
        Serial.print(longitud_tx,6);
        Serial.print("  Muestra_num: ");
        Serial.println(Muestra_data);
        */
    }
}

void separar_datos(String data_tx_drone)
{
    long temp = 0;
    int index1 = 0;
    int index2 = 0;
    String dato_string = "";

    // LATITUD:
    index1 = data_tx_drone.indexOf('A');
    index2 = data_tx_drone.indexOf('B');
    dato_string = data_tx_drone.substring(index1 + 1, index2); // Substring_extraido
    temp = dato_string.toInt();
    latitud_tx = temp / 1000000.0;
    // LONGITUD:
    index1 = data_tx_drone.indexOf('B');
    index2 = data_tx_drone.indexOf('C');
    dato_string = data_tx_drone.substring(index1 + 1, index2); // Substring_extraido
    temp = dato_string.toInt();
    longitud_tx = temp / 1000000.0;
    // CONTADOR DE LATENCIA
    index1 = data_tx_drone.indexOf('C');
    index2 = data_tx_drone.indexOf('D');
    dato_string = data_tx_drone.substring(index1 + 1, index2); // Substring_extraido
    temp = dato_string.toInt();
    Item_conta = temp - 100;
    // Envio de datos por Bluetooth
    BTSerial.print(latitud_tx, 6);
    BTSerial.print("|");
    BTSerial.print(longitud_tx, 6);
}

void datos_GPS_RX()
{
    /*
    print_date(gps);*/

    // Intentar recibir secuencia durante un segundo
    for (unsigned long start = millis(); millis() - start < 1000;)
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
        // Verificar que los datos decodificados sean correctos:
        F3 = gps_rx_validos(latitud_rx, TinyGPS::GPS_INVALID_F_ANGLE);
        F4 = gps_rx_validos(longitud_rx, TinyGPS::GPS_INVALID_F_ANGLE);
        F5 = gps_rx_validos(altura_rx, TinyGPS::GPS_INVALID_F_ALTITUDE);
        F6 = gps_rx_validos(age, TinyGPS::GPS_INVALID_AGE);

        if (F3 == true && F4 == true && F5 == true) // Para controlae escena de espera del GPS_RX
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

void hallar_cursoRX_to_TX(float flat_rx, float flong_rx, float flat_tx, float flong_tx)
{
    if (F3 == true && F4 == true)
    {
        if (flat_tx == 1000.0 || flong_tx == 1000.0 || flat_tx == 0.0 || flong_tx == 0.0)
        {
            /*
               Si flat_tx==1000.0 o flong_tx==1000.0 significa que recibieron datos erroreos.
               Si flat_tx==0.0 o flong_tx==0.0 significa que aun no se han recibido datos.
            */
            distance = -1.0;
            azimuth = -1.0;
        }
        else
        {
            distance = TinyGPS::distance_between(flat_rx, flong_rx, flat_tx, flong_tx); // distancia en metros
            azimuth = TinyGPS::course_to(flat_rx, flong_rx, flat_tx, flong_tx);
        }
    }
}

void datos_RXTX_to_tft()
{
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setTextSize(1);
    tft.setCursor(20, 10);
    tft.print(F("Ubicacion drone"));
    tft.println(F(" "));
    tft.setCursor(0, 24);
    tft.print(F("Latitud:"));
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.println(latitud_tx, 6);
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.print(F("Longitud:"));
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.println(longitud_tx, 6);

    if (F7 == false)
    {
        tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
        tft.setCursor(10, 56);
        tft.println(F("Ubicacion Receptor"));
        tft.println(F(" "));
        tft.setCursor(0, 72);
        tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
        tft.print(F("Latitud:"));
        tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
        tft.println("NAN");
        tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
        tft.print(F("Longitud:"));
        tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
        tft.println("NAN");
        tft.println("Esperando GPS_RX...");
    }
    else
    {
        F8 = Borrar_escena(F8);
        tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
        tft.setCursor(10, 56);
        tft.println(F("Ubicacion Receptor"));
        tft.println(F(" "));
        tft.setCursor(0, 72);
        tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
        tft.print(F("Latitud:"));
        tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
        tft.println(latitud_rx, 6);
        tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
        tft.print(F("Longitud:"));
        tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
        tft.println(longitud_tx, 6);
    }
}

void datos_Rumbo_seguir_to_tft()
{
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setTextSize(1);
    tft.setCursor(25, 105);
    tft.println(F("Curso a seguir"));
    tft.println(F(""));
    tft.setCursor(0, 121);
    tft.print(F("Distancia:"));
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    // sprintf(Data_string, "%04u", (int)distance);
    // tft.print(Data_string);
    tft.print(F("          "));
    tft.setCursor(60, 121);
    tft.print((int)distance);
    tft.println("m");

    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setCursor(0, 129);
    tft.print(F("Azimuth:"));
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    // sprintf(Data_string, "%03u", (int)azimuth);
    // tft.print(Data_string);
    tft.print(F("          "));
    tft.setCursor(50, 129);
    tft.print((int)azimuth);
    tft.println("deg");
}

void calidad_rf()
{
    tft.fillRect(0, 144, 128, 2, ST7735_CYAN);
    val_snr = LoRa.packetSnr();
    val_rssi_mean = LoRa.packetRssi();
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setCursor(0, 150);
    tft.print(F("snr:"));
    tft.setCursor(25, 150);
    tft.print(F("      "));
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.setCursor(25, 150);
    tft.print((int)val_snr);
    tft.print(F("dB"));

    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setCursor(55, 150);
    tft.print(F("rssi:"));
    tft.setCursor(87, 150);
    tft.print(F("      "));
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.setCursor(87, 150);
    tft.print(val_rssi_mean);
    tft.print("dB");
}

void print_compass_tft()
{
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.setTextSize(1);
    tft.setCursor(22, 0);
    tft.print("Digital Compass");

    norm = compass.readNormalize();
    heading = atan2(norm.YAxis, norm.XAxis);
    declinationAngle = (-8.0 + (0.0 / 60.0)) / (180 / M_PI);
    heading += declinationAngle;

    if (heading < 0)
    {
        heading += 2 * PI;
    }

    if (heading > 2 * PI)
    {
        heading -= 2 * PI;
    }

    // Convert to degrees
    angle = heading * 180 / M_PI;

    Draw_Compass_Rose(128 / 2, 160 / 2, 50); // Display TFT 128x64 pixeles
    // angle= atan2((double)y + y_offset,(double)x + x_offset)* (180 / 3.141592654) + 180;
    dx = (diameter * cos((angle - 90) * 3.14 / 180)) + centreX;            // calculate X position
    dy = (diameter * sin((angle - 90) * 3.14 / 180)) + centreY;            // calculate Y position
    marker_line(last_dx, last_dy, centreX, centreY, 20, 20, ST7735_WHITE); // Erase last arrow
    marker_line(dx, dy, centreX, centreY, 5, 5, ST7735_MAGENTA);           // Draw arrow in new position
    last_dx = dx;
    last_dy = dy;
    sprintf(Data_string, "%03u", (int)angle);
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 20);
    tft.print("Ang:");
    tft.print(Data_string);
    tft.drawCircle(45, 18, 1, ST7735_MAGENTA);           // simbolo de grados deg
    tft.fillCircle(centreX, centreY, 4, ST7735_MAGENTA); // simbolo de grados deg

    tft.setTextColor(ST7735_ORANGE, ST7735_WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 147);
    tft.print("Target to=>");
    // tft.fillRect()
    sprintf(Data_string, "%04u", (int)distance);
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setTextSize(1);
    tft.setCursor(64, 143);
    tft.print("Dist:");
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.print(Data_string);
    tft.print(F("m"));

    sprintf(Data_string, "%03u", (int)azimuth);
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setTextSize(1);
    tft.setCursor(64, 152);
    tft.print("Dir:");
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.print(Data_string);
    tft.print(F("deg"));
    tft.setFont(); // resetear la fuente a por defecto
}

void marker_line(int x2, int y2, int x1, int y1, int alength, int awidth, int colour)
{
    tft.drawLine(x1, y1, x2, y2, colour);
}

void Draw_Compass_Rose(int centreX, int centreY, int diameter)
{
    int dxo, dyo, dxi, dyi;
    tft.drawCircle(centreX, centreY, diameter, ST7735_CYAN); // Draw compass circle
    // tft.drawCircle(centreX,centreY,diameter-1,ST7735_CYAN);  // Draw compass circle
    for (float i = 0; i < 360; i = i + 22.5)
    {
        dxo = diameter * cos((i - 90) * 3.14 / 180);
        dyo = diameter * sin((i - 90) * 3.14 / 180);
        dxi = dxo * 0.93;
        dyi = dyo * 0.93;
        tft.drawLine(dxi + centreX, dyi + centreY, dxo + centreX, dyo + centreY, ST7735_ORANGE);
    }
    display_item(centreX - 2, (centreY - 60), "N", ST7735_RED, 1);
    display_item(centreX - 2, (centreY + 55), "S", ST7735_RED, 1);
    display_item((centreX + 55), centreY - 3, "E", ST7735_RED, 1);
    display_item((centreX - 57), centreY - 3, "W", ST7735_RED, 1);
}

void display_item(int x, int y, String token, int txt_colour, int txt_size)
{
    tft.setCursor(x, y);
    tft.setTextColor(txt_colour);
    tft.setTextSize(txt_size);
    tft.print(token);
}

void last_pos_error()
{

    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.setCursor(0, 10);
    tft.println(F("Recepcion fallida"));
    tft.println(F("LoRa Timeout error..."));
    tft.println(F(" "));
    tft.println(F("La ultima posicion "));
    tft.println(F("recibida fue:"));

    tft.println(F(" "));
    tft.print("latitud:");
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.println(ante_latitud_tx, 6);
    tft.setTextColor(ST7735_GREEN, ST7735_WHITE);
    tft.print("longitud:");
    tft.setTextColor(ST7735_MAGENTA, ST7735_WHITE);
    tft.println(ante_longitud_tx, 6);
}

void Guardar_SD()
{
    if (F11 == false)
    {
        SD_card_file = SD.open(Archivo, FILE_APPEND);
        if (!SD_card_file)
        {
            // Serial.println("  No SD =(");
            SD_card_file.close();
        }
        else
        {
            SD_card_file.print(latitud_rx, 6);
            SD_card_file.print(",");
            SD_card_file.print(longitud_rx, 6);
            SD_card_file.print(",");

            SD_card_file.print(latitud_tx, 6);
            SD_card_file.print(",");
            SD_card_file.print(longitud_tx, 6);
            SD_card_file.print(",");

            SD_card_file.print(Item_conta);
            SD_card_file.print(",");

            SD_card_file.print(val_rssi_mean);
            SD_card_file.print(",");
            SD_card_file.print(val_snr, 2);
            SD_card_file.print(",");

            SD_card_file.print(distance);
            SD_card_file.print(",");
            SD_card_file.print(azimuth, 2);
            SD_card_file.print(",");
            SD_card_file.println(fecha__hora_UTC);
            SD_card_file.close();
        }
    }
}
