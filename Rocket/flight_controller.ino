#include <SD.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "TinyGPS++.h"
#include "SoftwareSerial.h"

//Serial globals
const int SerialBaudRate = 9600;

// delay globals
const int standard_delay = 1000;
const int big_delay = 120000;
const int counting_delay = 500;
const int flight_delay = 100;
const int tone_delay = 500;
const int rescue_delay = 5000;

// GSM globals
const int GSM_RX = 10, GSM_TX = 11;
const int GSMBaudRate = 9600;
SoftwareSerial gsm_connection(GSM_RX, GSM_TX);
String phone_number = "+71234567"; // obfuscate, change here before use!

// GPS globals
const int GPS_RX = 0, GPS_TX = 1;
const int GPSBaudRate = 37600;
SoftwareSerial gps_connection(GPS_RX, GPS_TX);
TinyGPS gps;


// Gyro globals
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

// Barometer globals
Adafruit_BMP085 bmp;
const long normal_pressure = 99592; // Moscow - 747 mm Hg
long pressure;
long temperature;
long altitude;

// SD globals
String filename = "flight_data.txt";
File myFile;

// button globals
const int buttonPin = 7;
const int buzzerPin = 4;

void setup()
{
    Serial.begin(SerialBaudRate);
    initButton();
    initBuzzer();
    initBarometer();
    initGPS();
    initGyro();
    initSDCard();
    delay(big_delay); // we need to assure, that connection to GPS established
    writeGPSTS();
    waitForButton();
}

/* Init functions */

void initGSM()
{
    gsm_connection.begin(GSMBaudRate);
    Serial.println("GSM Start");
    delay(standard_delay);
}

void initGPS()
{
    gps_connection.begin(GPSBaudRate);
    Serial.println("GPS Start");
    delay(standard_delay);
}

void initGyro()
{
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
    Serial.println("Gyro start");
}

bool initSDCard()
{
    Serial.println("Initializing SD card...");
    delay(standard_delay);
    if (!SD.begin(10)) 
    {
        Serial.println("Init SD failed!");
        stop();
    }
    Serial.println("Opening IO file...");
    myFile = SD.open(filename, FILE_WRITE);
    delay(standard_delay);
    // if the file opened, return false
    if (myFile)
    {
        Serial.println("SD ready");
        delay(standard_delay);
    }
    else
    {
        Serial.println("Can't open file!");
        stop();
    }
}

void initButton()
{
    pinMode(buttonPin, INPUT); 
    Serial.println("Button pin set");
}

void initBarometer()
{
    if (!bmp.begin()) 
    {
        Serial.println("No pressure sensor found");
        stop();
    }
    Serial.println("Barometer set");
}

void initBuzzer()
{
    pinMode(buzzerPin, INPUT);
    Serial.println("Buzzer pin set");
}

/* Execution functions */

void sendSMS()
{
    initGSM();
    Serial.println("Sending SMS...");
    delay(standard_delay);
    gsm_connection.print("\r");
    delay(standard_delay);
    gsm_connection.print("AT+CMGF=1\r");
    delay(standard_delay);
    gsm_connection.print("AT+CMGS=\"+" + phone_number + "\"\r");
    delay(standard_delay);
    gsm_connection.print("I'm landed. Please recover me in ");
    gsm_connection.print("www.google.com/maps/place/");
    gsm_connection.print(gps.location.lat(), 6);
    gsm_connection.print(",");
    gsm_connection.print(gps.location.lng(), 6);
    gsm_connection.print("\r");
    delay(standard_delay);
    gsm_connection.print((char)26);
    delay(standard_delay);
    gsm_connection.print(0x1A);
    gsm_connection.print(0x0D);
    gsm_connection.print(0x0A);
    delay(standard_delay);
    Serial.println("SMS sent");
}

void writeFlightData()
{
    String string_to_store = String(millis()) + "," + getGPSTimeStamp() + "," + getGPSData() + "," + getGyroData() + "," + getBarometerData();
    myFile.println(string_to_store);
    Serial.println(string_to_store);
}

void writeGPSTS()
{
    myfile.println("\nSTART SEQUENCE BEGIN\n");
    myFile.println(getGPSTimeStamp);
}

String getGPSTimeStamp()
{
    String retVal = String(gps.date.year()) + "-" + String(gps.date.month()) + "-" + String(gps.date.day()) + "," + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()) + ":" + String(gps.time.centisecond());
    return retVal;
}

String getGPSData()
{
    String retVal = String(gps.location.lat()) + "," + String(gps.location.lng()) + "," + String(gps.altitude.meters()) + "," + String(gps.speed.kmph()) + "," + String(gps.course.deg()) + "," + String(gps.hdop.hdop());
    return retVal;
}

String getGyroData()
{
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
    AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
    AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    String retVal = String(AcX) + "," + String(AcY) + "," + String(AcZ) + "," + String(Tmp/340.00+36.53) + "," + String(GyX) + "," + String(GyY) + "," + String(GyZ);
    return retVal;
}

String getBarometerData()
{
    String retVal = String(bmp.readPressure()) + "," + String(bmp.readAltitude(normal_pressure)) + "," + String(bmp.readTemperature());
}

/* Loop functions */
void waitForButton()
{
    while(1)
    {
        if(digitalRead(buttonPin)==HIGH) 
        {
            digitalWrite(PIN_BUTTON,!digitalRead(PIN_BUTTON));
            Serial.println("Button pressed, start flight sequence");
            delay(big_delay);
            loop();
        }
    }
}

void loop()
{
    int counter = 0;
    bool flag = false;
    while(1)
    {
        writeFlightData();
        // we need to assure that we're landed, re-check about hundred of ticks
        if (flag == true)
        {
            if (AcX == 0 && AcY == 0 && AcZ == 0)
            {
                counter++;
            }
            if (counter > counting_delay)
            {
                stopFlight();
            }
        }
        // we need to assure that we're in flight, re-check about hundred of ticks
        else
        {
            if (counter < -flight_delay)
            {
                flag = true;
                counter = 0;
            }
        }
    }
}

void stopFlight()
{
    Serial.print("Landed, saving data and call for recovery");
    myFile.close();
    sendSMS();
    delay(big_delay);
    sendSMS();
    delay(big_delay);
    sendSMS();
    delay(big_delay);
     rescueBeep();
}

void rescueBeep()
{
    while(1)
    {
        beep(5000);
        beep(5000);
        beep(5000);
        delay(rescue_delay);
    }
}

void stop()
{
    while(1);
}

/* buzzer functions */
void beep(int ghz)
{
    tone(buzzerPin,ghz,tone_delay);
    delay(tone_delay);
    pinMode(buzzerPin, INPUT);
    delay(tone_delay);
}
