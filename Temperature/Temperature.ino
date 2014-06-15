#include "OneButton.h"
#include "EEPROM.h"
#include "Timer.h"

 
#define RELAY1 A1           // Connect humidifier to Relay1
#define RELAY2 A2           // Connect heater to Relay2
#define TEMP_SET 25         // Starting Temperature
#define MAX_TEMP 30         // Max Temperature
#define HUM_SET 40          // Threshold humidity
#define HUM_SET2 60
#define SWITCH_PIN A5       // Connect the switch between pin A5 and ground
#define CYCLE 10           //Time in minutes for each cycle

#include "LiquidCrystal.h"
#define LCD_RS 11           // * LCD RS pin to digital pin 12 - Green
#define LCD_EN 12           // * LCD Enable pin to digital pin 11 - Yellow
#define LCD_D4 5            // * LCD D4 pin to digital pin 5 - Blue
#define LCD_D5 6            // * LCD D5 pin to digital pin 6 - Blue
#define LCD_D6 7            // * LCD D6 pin to digital pin 7 - Blue
#define LCD_D7 8            // * LCD D7 pin to digital pin 8 - Blue

#include "dht11.h"
#define DHT11PIN 13

dht11 DHT11;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
int temp_set;
Timer t;
OneButton button1(SWITCH_PIN, true);

void setup() {
  pinMode(DHT11PIN, INPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_EN, OUTPUT);
  pinMode(LCD_D4, OUTPUT);
  pinMode(LCD_D5, OUTPUT);
  pinMode(LCD_D6, OUTPUT);
  pinMode(LCD_D7, OUTPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  lcd.begin(16, 2);
  Serial.begin(9600);
  if(EEPROM.read(0) >= MAX_TEMP+1){
    EEPROM.write(0,TEMP_SET);
    temp_set=TEMP_SET;
  }
  else {temp_set=EEPROM.read(0);}
  lcd.print("Setpoint: ");
  lcd.print(temp_set);            // Display the setpoint temperature for 2 sec
  delay(2000);
  checkTemp(0);
  t.every(60000*CYCLE,checkTemp,(void*)0);
  button1.attachClick(tempPlus);
  button1.attachPress(tempMinus);
}

void loop() {
  // put your main code here, to run repeatedly: 
  button1.tick();
  t.update();
}

void tempPlus(){
      Serial.println("Plus");
      temp_set++;
      if (temp_set > MAX_TEMP) temp_set = TEMP_SET; // Cycle between TEMP_SET and MAX_TEMP
      lcd.clear();
      lcd.print("Setpoint: ");
      lcd.print(temp_set);
      EEPROM.write(0,temp_set);
      delay(1000);
      checkTemp(0);
}

void tempMinus(){
      Serial.println("Minus");
      temp_set--;
      if (temp_set < TEMP_SET) temp_set = MAX_TEMP; // Cycle between TEMP_SET and MAX_TEMP
      lcd.clear();
      lcd.print("Setpoint: ");
      lcd.print(temp_set);
      EEPROM.write(0,temp_set);
      delay(2000);
      checkTemp(0);  
}

void checkTemp(void* context)
{
  int chk = DHT11.read(DHT11PIN);
  // Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
               // Serial.println("OK"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
               // Serial.println("Checksum error"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
               // Serial.println("Time out error"); 
                break;
    default: 
                lcd.println("Unknown error"); 
                break;
  }
  
  lcd.setCursor(0, 0);
  Serial.println(DHT11.humidity); 
  lcd.print("Humidity: ");
  lcd.print((float)DHT11.humidity, 2);
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print((float)DHT11.temperature, 2);
  Serial.println(DHT11.temperature); 
  if(DHT11.humidity < HUM_SET2){ 
    digitalWrite(RELAY1, HIGH);
    Serial.println("H1");
  }
  if (DHT11.humidity > HUM_SET) {
    digitalWrite(RELAY1,LOW);
    Serial.println("H0");
  }
  if(DHT11.temperature < temp_set){ 
    digitalWrite(RELAY2, HIGH);
    Serial.println("T1");
  }
  if (DHT11.temperature > temp_set){
    digitalWrite(RELAY2,LOW);
    Serial.println("T0");
  }
}

