#include <SerialFlow.h>

//pin consts
const int buzzerPin = 4;
const int ledPin = 5;
const int powerPin = 6;
const int buttonPin = 2;

//delay const
const int tone_delay = 500;
const int sequence_delay = 0;
const int ignition_delay = 3000;
const int magic_word = 3735928559; // 0xDEADBEEF

//serial flow settings
SerialFlow rd(9,10);
 
int counter = 0;
 
void setup()
{
    //start debug output
    Serial.begin(9600);
    initPins();
    initWiFi();
    //start wait for signal
    turnLEDOn();
    return;
}
 
void loop()
{
    checkButton();
    checkPacket();
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

void finalCountdown()
{
    Serial.println("Countdown...");
    Serial.println("5...");
    beep(1000);
    Serial.println("4...");
    beep(2000);
    Serial.println("3...");
    beep(3000);
    Serial.println("2...");
    beep(4000);
    Serial.println("1...");
    beep(5000);
    Serial.println("0...");
}

void checkButton()
{
    Serial.println("Button pressed");
    if (digitalRead(buttonPin)==HIGH)
    {
        launch();
    }
}

void ignite()
{
    Serial.println("Ignition!");
    digitalWrite(powerPin, HIGH);
    delay(ignition_delay);
    digitalWrite(powerPin, LOW);
    Serial.println("Done");
}

void stop()
{
    Serial.println("Stopped");
    while(1);
}

void checkPacket()
{
    unsigned int data;
    if( rd.receivePacket() )
    {
        Serial.println("Recieved packet... decoding...");
        data = rd.getPacketValue(0);
        if (data == magic_word)
        {
            Serial.println("Launch sequence obtained, starting...");
            launch();
        }
    }
}

void launch()
{
    turnLEDOff();
    delay(sequence_delay);
    finalCountdown();
    ignite();
    turnLEDOn();
}

void beep(int ghz)
{
    tone(buzzerPin,ghz,tone_delay);
    delay(tone_delay);
    pinMode(buzzerPin, INPUT);
    delay(tone_delay);
}

void initPins()
{
    //Setup pin modes
    Serial.println("Set pins");
    noTone(buzzerPin);
    pinMode(buzzerPin, INPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(powerPin, OUTPUT);
    pinMode(buttonPin, OUTPUT);
    digitalWrite(buttonPin, LOW);
    digitalWrite(powerPin, LOW);   
    pinMode(buttonPin, INPUT);
}

void initWiFi()
{
    //Setup wi-fi
    Serial.println("Set wi-fi");
    rd.setPacketFormat(2, 1);
    rd.begin(0xF0F0F0F0E1LL,0xF0F0F0F0D2LL);    
}
