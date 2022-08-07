/*

  1. Photoresistor indicate whether or not there is a flame detected.
  2. If a fire is detected, lcd will turn on.
  3. The 1st buzzer will not turn on unless there is a smoke and flame detected.
  4. If there is a fire detected, buzzer will alarm and lcd will show that "FIRE IS DETECTED".
  5. If PIR sensor detects a person, buzzer and led will turn on.
  6. 2nd buzzer indicate if there is a person stuck in the fire.
  7. "0-9" button is for the interval of time that 2nd buzzer will turn on.
  8. Power button to turn of the circuit.
*/

#include <IRremote.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(32, 16, 2);

// IR VARIABLE
int IRpin = 3;
IRrecv IR(IRpin);

// BUZZER VARIABLE
int buzzer1 = 8;
int buzzer2 = 7;

// PIR VARIABLE
int PIR_pin = 2;
int pirVal;

// LED
int LEDpin1 = 10;
bool State = LOW;
bool pRes_State;
int LEDpin2 = 4;
bool pRes_State2;

// PHOTORESISTOR
int pRes = A3;
int pRes_Val;

// GAS Sensor
int Gas_pin = 12;
int gasVal;

// MILLIS
unsigned long currentms;
unsigned long pastms;
int i = 500;      // default interval

bool powerCond = false;
bool trigger = true;

void setup()
{
  Serial.begin(9600);
  IR.enableIRIn();

  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.setBacklight(0);

  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);
  pinMode(LEDpin1, OUTPUT);
  pinMode(LEDpin2, OUTPUT);
  pinMode(PIR_pin, INPUT);
  pinMode(Gas_pin, INPUT);
  pinMode(pRes, INPUT);
}

void loop()
{
  remote_Control();
  readValues();
  if (pRes_Val >= 600)
  {
    lcd.setBacklight(1);
    pRes_State = true;
  }
  while (gasVal == HIGH && pRes_State)
  {
    readValues();
    remote_Control();
    
    if (gasVal == HIGH && trigger)
    {
      lcd.setCursor(6, 0);
      lcd.print("FIRE");
      lcd.setCursor(4, 1);
      lcd.print("DETECTED");
    }

    digitalWrite(buzzer1, HIGH);
    digitalWrite(LEDpin1, HIGH);


    if (pirVal == 1)
    {
      trigger = false;
      currentms = millis();
      lcd.setCursor(5, 0);
      lcd.print("PERSON");
      lcd.setCursor(4, 1);
      lcd.print("DETECTED");
      remote_Control();
      Serial.println(i);
      if (currentms - pastms >= i)
      {
        pastms = currentms;
        State = !State;
      }
      digitalWrite(buzzer2, State);
      digitalWrite(LEDpin2, State);
    }
    if (pirVal == 0 && trigger == false)
    {
      lcd.clear();
      digitalWrite(buzzer2, LOW);
      digitalWrite(LEDpin2, LOW);
    }
  }
  if (powerCond)
  {
    lcd.clear();
    trigger = true;
    State = LOW;
    digitalWrite(buzzer1, LOW);
    digitalWrite(LEDpin1, LOW);
    digitalWrite(buzzer2, LOW);
    digitalWrite(LEDpin2, LOW);
    lcd.setCursor(4, 0);
    lcd.print("Powering");
    lcd.setCursor(6, 1);
    lcd.print("Off!");
    delay(1000);
    lcd.clear();
    lcd.setBacklight(0);
    while (1) {}
  }
}

void remote_Control()
{
  while (IR.decode())
  {
    //Serial.println(IR.decodedIRData.decodedRawData, HEX);

    if (IR.decodedIRData.decodedRawData == 0xFF00BF00)
    {
      lcd.setBacklight(1);
      Serial.println("Power");
      powerCond = true;
      pRes_State = false;
    }
    else if (IR.decodedIRData.decodedRawData == 0xF30CBF00)
    {
      Serial.println("0");
      i = 0;
    }
    else if (IR.decodedIRData.decodedRawData == 0xEF10BF00)
    {
      Serial.println("1");
      i = 500;
    }
    else if (IR.decodedIRData.decodedRawData == 0xEE11BF00)
    {
      Serial.println("2");
      i = 1000;
    }
    else if (IR.decodedIRData.decodedRawData == 0xED12BF00)
    {
      Serial.println("3");
      i = 1500;
    }
    else if (IR.decodedIRData.decodedRawData == 0xEB14BF00)
    {
      Serial.println("4");
      i = 2000;
    }
    else if (IR.decodedIRData.decodedRawData == 0xEA15BF00)
    {
      Serial.println("5");
      i = 2500;
    }
    else if (IR.decodedIRData.decodedRawData == 0xE916BF00)
    {
      Serial.println("6");
      i = 3000;
    }
    else if (IR.decodedIRData.decodedRawData == 0xE718BF00)
    {
      Serial.println("7");
      i = 3500;
    }
    else if (IR.decodedIRData.decodedRawData == 0xE619BF00)
    {
      Serial.println("8");
      i = 4000;
    }
    else if (IR.decodedIRData.decodedRawData == 0xE51ABF00)
    {
      Serial.println("9");
      i = 4500;
    }
    IR.resume();
  }
}

void readValues()
{
  pirVal = digitalRead(PIR_pin);
  pRes_Val = analogRead(pRes);
  gasVal = digitalRead(Gas_pin);
}
