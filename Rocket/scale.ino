#include "HX711.h"
#include <SD.h>
#include <LiquidCrystal.h>
#include "RTClib.h"

//delay globals
const int standard_delay = 1000;
const int short_delay = 100;
const int error_delay = 5000;
const int LCD_delay = 100;

// HX711 globals
HX711 scale(A1, A0);                          
const float calibration_factor = 102.58;          // калибровка!
float scale_result;
 
// SD CARD globals
File myFile;

// LCD globals
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// RTC global
RTC_DS1307 RTC;
int RTC_compensation = -0.0094;
DateTime now;
DateTime check;

// button global
const int buttonPin = 6;

// output globals 
int counter;
String message;

void setup() 
{
    bool result = false;
    counter=0;
    // start debug
    Serial.begin(9600);

    initRTC();
    initButton();
    initScale();
    result = initSDCard();
    
    if (result)
    {
        stop();
    }
    else
    {
        wait_for_button();
    }
}

void loop() 
{
    while(1)
    {
        // get scale data
        scale_result=scale.get_units(1);
        // get current time
        check = RTC.now();
        // update LED not so frequent as measure to avoid blinking
        if (counter%LCD_delay==0)
        {
            lcd.clear();
            lcd.setCursor(14, 0);
            lcd.print(String((check.unixtime()-now.unixtime())));
            lcd.setCursor(0, 1);
            lcd.print(String(scale_result));
            counter=0;
            // needed to avoid scaling-RTC error measurement
            calibration_factor+=RTC_compensation;
            scale.set_scale(calibration_factor);
            Serial.println("Calibration factor updated to: " + String(calibration_factor));
        }
        else
        {
            counter++;
        }
        //generate output
        message = String(millis()) + "," + String(scale_result) + "," + String(check.unixtime()) +"," + String(check.unixtime()-now.unixtime());
        Serial.println(message);
        myFile.println(message);
        // check measurement timeout, if so, close file
        if ((check.unixtime()-now.unixtime())>60)
        {
            myFile.close();
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Measurement stop!");
            Serial.println("Measurement stop!");
            delay(error_delay);
            lcd.clear();
            return;
        }
    }
}

void stop()
{
    while(1);
}

void wait_for_button()
{
    while(1)
    {
        if(digitalRead(buttonPin)==HIGH) //если кнопка нажата   
        {
            digitalWrite(PIN_BUTTON,!digitalRead(PIN_BUTTON));
            startloop();
        }
    }
}

void startloop()
{
    myFile = SD.open("scale.txt", FILE_WRITE);
    delay(standard_delay);
    // if the file opened okay, start looping:
    if (myFile) 
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Start measure!");
        delay(error_delay);    
        myFile.println();
        myFile.println(getTimeStamp());
        loop();
    }
    else
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Can't open file!");
        Serial.println("Can't open file!");
        delay(error_delay);
        lcd.clear();
        //pinMode(backLight, OUTPUT);
        //digitalWrite(backLight, LOW); 
        stop();
        return;
    }
}

String getTimeStamp()
{
  now = RTC.now();
  return String(now.year(), DEC) + "/" + String(now.month(), DEC) + "/" + String(now.day(), DEC) + " " + String(now.hour(), DEC) + ":" + String(now.minute(), DEC);
}

void initRTC()
{
    RTC.begin();
    if (! RTC.isrunning()) 
    {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        // uncomment it & upload to set the time, date and start run the RTC!
        //RTC.adjust(DateTime(__DATE__, __TIME__));
    }    
}

void initButton()
{
    pinMode(buttonPin, INPUT); // set button pin
}

void initLCD()
{
    lcd.begin(16, 2);
    // Print a message to the LCD
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Hello!");
    Serial.println("Hello!");
    delay(standard_delay);
    Serial.println(getTimeStamp());
    lcd.setCursor(0, 1);
    lcd.print(getTimeStamp());
    delay(standard_delay);    
}

void initScale()
{
    // включаем весы
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Initializing ");
    lcd.setCursor(0, 1);
    lcd.print("scale...");
    Serial.println("Init scale...");
    delay(standard_delay);
    scale.set_scale();
    Serial.println("Resetting tare...");
    delay(short_delay);
    scale.tare(); // reset to 0
    Serial.println("Set calibration... factor " + String(calibration_factor));
    delay(short_delay);
    scale.set_scale(calibration_factor); // apply calibration
}

bool initSDCard()
{
    // включаем SD-карту
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Initializing ");
    lcd.setCursor(0, 1);
    lcd.print("SD card...");
    Serial.println("Initializing SD card...");
    delay(standard_delay);
    if (!SD.begin(10)) 
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Init SD failed!");
        Serial.println("Init SD failed!");
        delay(error_delay);
        lcd.clear();
        //pinMode(backLight, OUTPUT);
        //digitalWrite(backLight, LOW); 
        return true;
    }
    Serial.println("Opening IO file...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("Init done, ");
    lcd.setCursor(0, 1);
    lcd.println("opening IO file...");
    myFile = SD.open("scale.txt", FILE_WRITE);
    delay(standard_delay);
    // if the file opened, return false
    if (myFile)
    {
        myFile.close();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Init OK. ");
        lcd.setCursor(0, 1);
        lcd.print("Scale ready!");
        Serial.println("Init OK. Scale ready!");
        delay(standard_delay);
        return false;
    }
    else
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Can't open file!");
        Serial.println("Can't open file!");
        delay(error_delay);
        lcd.clear();
        //pinMode(backLight, OUTPUT);
        //digitalWrite(backLight, LOW); 
        return true;
    }
}
