#include <SD.h>
#include <Wire.h>
#include <SFE_BMP180.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//Serial globals
const int BaudRate = 9600;

// delay globals
const int standard_delay = 1000;
const int big_delay = 30000;
const int tone_delay = 500;
const int rescue_delay = 3000;
const unsigned long flight_delay = 120000;

// GSM globals
const int GSM_RX = 10, GSM_TX = 11;
SoftwareSerial gsm_connection(GSM_RX, GSM_TX);
const String phone_number = "+71234567"; // obfuscate, change here before use!

// GPS globals
const int GPS_RX = 0, GPS_TX = 1;
SoftwareSerial gps_connection(GPS_RX, GPS_TX);
TinyGPSPlus gps;

// Gyro globals
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

// Barometer globals
SFE_BMP180 barometer_data;
double normal_pressure = 0;

// SD globals
const String filename = "flight_data.txt";
File myFile;

// button globals
const int PIN_BUTTON = 7;
const int PIN_BUZZER = 6;

// buzzer globals;
const int frequency = 5000;

// landing globals
unsigned long start_time;

void setup()
{
    Serial.begin(BaudRate);
    initButton();
    initBuzzer();
    initBarometer();
    initGPS();
    initGyro();
    initSDCard();
    delay(big_delay); // we need to assure, that connection to GPS established
    beep(frequency); // make beep, mark init completed
    waitForButton();
    start_time = millis();
    beep(frequency); // make beep, we're ready to flight
}


/* Init functions */

void initGSM()
{
    gsm_connection.begin(BaudRate);
    Serial.println("GSM Start");
    delay(standard_delay);
}


void initGPS()
{
    gps_connection.begin(BaudRate);
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
    pinMode(PIN_BUTTON, INPUT); 
    Serial.println("Button pin set");
}


void initBarometer()
{
    barometer_data.begin();
    normal_pressure = getRawBarometerData(0);
    Serial.println("Barometer set");
}


void initBuzzer()
{
    pinMode(PIN_BUZZER, INPUT);
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
    Serial.print(millis());
    Serial.print(",");
    myFile.print(millis());
    myFile.print(",");
    Serial.print(getGPSTimeStamp());
    Serial.print(",");
    myFile.print(getGPSTimeStamp());
    myFile.print(",");
    Serial.print(getGPSData());
    Serial.print(",");
    myFile.print(getGPSData());
    myFile.print(",");
    Serial.print(getGyroData());
    Serial.print(",");
    myFile.print(getGyroData());
    myFile.print(",");
    Serial.print(getBarometerData());
    Serial.print("\n");
    myFile.print(getBarometerData());
    myFile.print("\n");
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
    String retVal = String(getRawBarometerData(0)) + "," + String(getRawBarometerData(1)) + "," + String(getRawBarometerData(2));
    return retVal;
}

double getRawBarometerData(int type)
{
    char status;
    double T, P;

    status = barometer_data.startTemperature();
    if (status != 0)
    {
        delay(status);
        status = barometer_data.getTemperature(T);
        if (type == 1)
        {
            return T;
        }
        if (status != 0)
        {
            status = barometer_data.startPressure(3);
            if (status != 0)
            {
                delay(status);
                status = barometer_data.getPressure(P, T);
                if (status != 0)
                {
                    if (type == 0)
                    {
                        return(P);
                    }
                    else
                    {
                        return barometer_data.altitude(P, normal_pressure);
                    }
                }
            }
        }
    } 
}


/* Loop functions */
void waitForButton()
{
    while(1)
    {
        if(digitalRead(PIN_BUTTON)==HIGH) 
        {
            digitalWrite(PIN_BUTTON,!digitalRead(PIN_BUTTON));
            Serial.println("Button pressed, start flight sequence");
            delay(big_delay);
            break;
        }
    }
}


void loop()
{
    while(gps_connection.available() > 0) {
        char temp = gps_connection.read();
        //Serial.write(temp); // debug
        gps.encode(temp);
    }
    
    writeFlightData();

    if (millis() - start_time > flight_delay)
    {
        stopFlight();
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

void stop()
{
    while(1)
    {
        beep(frequency);
        delay(tone_delay);
    }
}

/* buzzer functions */
void rescueBeep()
{
    while(1)
    {
        beep(frequency);
        delay(rescue_delay);
    }
}

void beep(int ghz)
{
    tone(PIN_BUZZER, ghz, tone_delay);
    delay(tone_delay);
    pinMode(PIN_BUZZER, INPUT);
    delay(tone_delay);
}
