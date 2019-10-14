/* ***************************************************************************
 * This sketch contains weather station logic                                *
 *                                                                           *
 * Sketch uses Arduino Nano controller, and it contains limited amount of    *
 * RAM. Due to that - to achieve stability - at least 20% of RAM should be   *
 * free and debug serial output is commented-out in this sketch.             *
 *                                                                           *
 * Flight controller contains:                                               *
 *    - Arduino Nano v3 (CH340g), AHT10 humidity sensor, AC-DC supply unit,  *
 *      BMP180 barometer/thermometer, SD card module, DC-DC Low Voltage (4V) *
 *      WAVGAT SIM900A GSM module, 7805 stabilizer, LED, 2x18650,            *
 *                                                                           *
 * Third-party libraries:                                                    *
 *    - https://github.com/Thinary/AHT10                                     *
 *                                                                           *
 * Logic:                                                                    *
 *    1) Init all modules                                                    *
 *    2) Wait until GSM/GPRS connection established                          *
 *    3) Start looping:                                                      *
 *       a) Get data from internal (or external) sensors                     *
 *       b) Send data via GPRS as REST POST request                          *
 *       c) Wait for next data obtaning window (1-5 min)                     *
 *       d) Switch sensor type (external\internal), go to "a"                * 
 *                                                                           *
 * Sketch written by Iliya Vereshchagin 2019.                                *
 *****************************************************************************/

#include <Wire.h>
#include <GSM.h>
#include <Adafruit_BMP085.h>
#include <Thinary_AHT10.h>

// Delay globals
const long cycle_delay = 300000;

// BMP180 globals
Adafruit_BMP085 barometer_internal;
Adafruit_BMP085 barometer_external;
#define TCAADDR 0x70

// AHT10 globals
AHT10Class AHT10_internal;
AHT10Class AHT10_external;

// GSM globals
char PINNUMBER[]="";
char GPRS_APN[]="GPRS_APN"; // replace your GPRS APN
char GPRS_LOGIN[]="login";    // replace with your GPRS login
char GPRS_PASSWORD[]="password"; // replace with your GPRS password
GSMClient client;
GPRS gprs;
GSM gsmAccess;
char server[] = "my.ip.address.here";
int port = 80; 
char endpoint[] = "/YOUR/REST/POST";

// Power monitor PINNUMBER
const int POWER_PIN=10;

// Internal/External switch
bool is_external;

// multiplexor selector for BMP180
void tcaselect(uint8_t i) 
{
    if (i > 7)
    {
        return;
    }
    Wire.beginTransmission(TCAADDR);
    Wire.write(1 << i);
    Wire.endTransmission();
}


// Init functions

void initBarometer()
{
    tcaselect(0);
    barometer_internal.begin();
    tcaselect(1);
    barometer_external.begin();
}

void initHumidSensor()
{
    Wire.begin();
    if (AHT10_internal.begin(eAHT10Address_Low) && AHT10_external.begin(eAHT10Address_High))
    {
        Serial.println("Init AHT10 Sucess.");
    }
    else
    {
        Serial.println("Init AHT10 Failure.");
        while(true);
    }  
}

void initGSM()
{
    bool notConnected = true;
    while (notConnected) 
    {
        if ((gsmAccess.begin(PINNUMBER) == GSM_READY) & (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) 
        {
            notConnected = false;
        } 
        else 
        {
            Serial.println("Not connected");
            delay(1000);
        }
    }
}

void initPowerPin()
{
	pinMode(POWER_PIN, INPUT);
}


// Get functions

double getTemperature(bool external)
{
    double temperature;
    if (external)
    {
        tcaselect(1);
        temperature = (barometer_external.readTemperature() + AHT10_external.GetTemperature())/2.0;
    }
    else
    {
        tcaselect(0);
        temperature = (barometer_internal.readTemperature() + AHT10_internal.GetTemperature())/2.0;
    }
    return temperature;
}

double getPressure(bool external)
{
    double pressure;
    if (external)
    {
        tcaselect(1);
        pressure = barometer_external.readPressure()/133.322;
    }
    else
    {
        tcaselect(0);
        pressure = barometer_internal.readPressure()/133.322;
    }
    return pressure;
}

double getHumidity(bool external)
{
    double humidity;
    if (external)
    {
        humidity = AHT10_external.GetHumidity();
    }
    else
    {
        humidity = AHT10_internal.GetHumidity(); 
    }
    return humidity;
}

double getDewPoint()
{
    return AHT10_external.GetDewPoint();
}

// GSM functions

void sendDataViaGPRS(String data)  // TODO: REWORK TO API
{
    if (client.connect(server, port)) 
    {
        Serial.println("Sending data: " + data);
        // Make a HTTP request:
        client.print("GET ");
        client.print(endpoint);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(server);
        client.println("Connection: close");
        client.println();
    } 
    else 
    {
        // if you didn't get a connection to the server:
        Serial.println("connection failed");
    }
}

void sendPowerLossState()
{
		// TODO
}

void setup()
{
    Serial.begin(9600);
    delay(10);
	initPowerPin();
    initBarometer();
    initHumidSensor();
	intGSM();
}

void loop()
{   
    is_external = !is_external;
    String data_string = String(is_external) + "," + String(getTemperature(is_external)) + "," + String(getPressure(is_external)) + "," + String(getHumidity(is_external)) + "," + String(getDewPoint());
    sendDataViaGPRS(data_string);
	if (digitalRead(POWER_PIN)==LOW)
	{
		sendPowerLossState();
	}
    delay(cycle_delay);
}

