#include <SerialFlow.h>

// pin const
const int ledPin = 5;
const int ignitionLedPin = 6;
const int buttonPin = 4;

// delay const
const int resend_delay = 500;
const int startup_delay = 3000;
const int ignition_delay = 7500;
const int magic_word = 4242;

// serial flow settings
SerialFlow rd(9,10);
 
void setup()
{
    //start debug output
    Serial.begin(9600);
    initPins();
    delay(startup_delay);
    initWiFi();
    delay(startup_delay);
    //start wait for signal
    turnLEDOn();
    return;
}
 
void loop()
{
    checkButton();
}
 
void turnLEDOn()
{
    Serial.println("Turn ON standby LED");
    digitalWrite(ledPin, HIGH); 
}

void turnLEDOff()
{
    Serial.println("Turn OFF standby LED");
    digitalWrite(ledPin, LOW);  
}

void turnILEDOn()
{
    Serial.println("Turn ON ignition LED");
    digitalWrite(ignitionLedPin, HIGH);  
}

void turnILEDOff()
{
    Serial.println("Turn OFF ignition LED");
    digitalWrite(ignitionLedPin, LOW);  
}

void checkButton()
{
    if (digitalRead(buttonPin)==HIGH)
    {
        Serial.println("Button pressed");
        sendMessage();
    }
}

void stop()
{
    Serial.println("Stopped");
    while(1);
}

void sendMessage()
{
    Serial.println("Sending magic keyword: " + String(magic_word));
    turnILEDOn();
    sendPacket();
    Serial.println("Ignition delay");
    delay(ignition_delay);
    turnILEDOff();
}

void sendPacket()
{
  Serial.println("Encoding packet");
  rd.setPacketValue( magic_word );
  Serial.println("Sending, try 1...");
  rd.sendPacket();
  delay(resend_delay);
  Serial.println("Sending, try 2...");
  rd.sendPacket();
  delay(resend_delay);
  Serial.println("Sending, try 3...");
  rd.sendPacket();
}

void initPins()
{
    //Setup pin modes
    Serial.println("Set pins");
    pinMode(ledPin, OUTPUT);
    pinMode(ignitionLedPin, OUTPUT);
    pinMode(buttonPin, OUTPUT);
    digitalWrite(buttonPin, LOW);
    pinMode(buttonPin, INPUT);
}

void initWiFi()
{
    //Setup wi-fi
    Serial.println("Set wi-fi");
    rd.setPacketFormat(2, 1);
    rd.begin(0xF0F0F0F0D2LL,0xF0F0F0F0E1LL);    
}

