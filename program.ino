#include <LiquidCrystal.h>
#include <Servo.h>

#define SERVO_PIN 6
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 90

#define SENSOR_PIN 7


#define BTN_LEFT_PIN 3
#define BTN_START_PIN 2
#define BTN_RIGHT_PIN 4
#define BTN_SETTINGS_PIN 5

#define BUZZER_PIN 1

#define STRING_INITIAL_UP     "=== AFR V1.0 ==="
#define STRING_INITIAL_DWN    "= PRESS  START ="
#define STRING_FIREPREPARE_UP   "=== AFR V1.0 ==="
#define STRING_FIREPREPARE_DWN  "== GET  READY =="
#define STRING_FIREPROCESS_UP   "SHOTS COUNT: 000"
#define STRING_FIREPROCESS_DWN  "TIME: 00:00:0000"

enum Stage
{ 
  sInitial,
  sFirePrepare,
  sFireProcess,
  sFireEnd,
  sSettings,
  sNone
};

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

Servo srv;

Stage currentStage = sNone, previousStage = sNone;

long startTime, lastTime, elapsedTime;
long hitCount = 0;
bool isHitComing = false;
char timeStr[10];

void setup() {
  
  pinMode(SERVO_PIN, OUTPUT);
  
  pinMode(SENSOR_PIN, INPUT);
  
  pinMode(BTN_START_PIN, INPUT_PULLUP);
  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BTN_SETTINGS_PIN, INPUT_PULLUP);
  
  pinMode(BUZZER_PIN, OUTPUT);

  srv.attach(SERVO_PIN);
  
  requestStageChange(sInitial);
}

// we save both stages to distinguish if we've just entered stage or has already been in it last time
void requestStageChange(Stage newStage)
{
  if (currentStage != newStage)
  {
    previousStage = currentStage;
    currentStage = newStage;
  }
}

void loop() {
  
  // absolute domination of Settings button
  //if (!digitalRead(BTN_SETTINGS_PIN))
  //{
  //  if (currentStage != sSettings)
  //    currentStage = sSettings;
  //  else
  //    currentStage = sInitial;
  //}

  // stage loop
  switch(currentStage)
  {
    case sInitial:
        if (currentStage != previousStage)
        {
          lcd.begin(16, 2);
          lcd.print(STRING_INITIAL_UP);
          lcd.setCursor(0, 1);
          lcd.print(STRING_INITIAL_DWN);

          srv.write(SERVO_MIN_ANGLE);
          
          previousStage = currentStage;
        }
    
    if (!digitalRead(BTN_START_PIN)) 
          requestStageChange(sFirePrepare);
      break;
    case sFirePrepare:
      // lcd
      lcd.setCursor(0, 0);
      lcd.print(STRING_FIREPREPARE_UP);
      lcd.setCursor(0, 1);
      lcd.print(STRING_FIREPREPARE_DWN);
    
      // servo
      srv.write(SERVO_MAX_ANGLE);
      delay(1000);
      srv.write(SERVO_MIN_ANGLE);
      
      //pre-delay
      delay(3000);
      tone(BUZZER_PIN, 2000, 1000);
    
      delay(random(1000, 4000));
      tone(BUZZER_PIN, 4000, 1000);
    
      // init FireProcess stage
      startTime = millis();

      lcd.setCursor(0, 0);
      lcd.print(STRING_FIREPROCESS_UP);
      lcd.setCursor(0, 1);
      lcd.print(STRING_FIREPROCESS_DWN);
    
      requestStageChange(sFireProcess);
      break;
    case sFireProcess:
      // debug information, remove
      //float timeRange = (millis() - startTime) / 1000.0;
      //lcd.setCursor(2, 1);
      //lcd.print(timeRange, 2);
    
      // count hits
      {
      int val = digitalRead(SENSOR_PIN);
      if (digitalRead(SENSOR_PIN))
        {
          if (!isHitComing)
          {
            // new hit detected
            isHitComing = true;
            
            // display time
            elapsedTime = millis() - startTime;
            
            // commented by Creepy Gnome
            //lastTime += elapsedTime;
            //sprintf(timeStr, "%02d:%02d:%04d"); // DO FORMATTING
            
            // renew hit count
            hitCount++;
            lcd.setCursor(16 - String(hitCount).length(), 0); // 1 line less that sprintf
          lcd.print(hitCount);
            
            // added by Creepy Gnome
            lcd.setCursor(6, 1);
          lcd.print(TimeToString(elapsedTime));
          }
        }
      else
        {
          isHitComing = false;
        }
      }
      break;
    case sSettings:
        if (currentStage != previousStage)
        {
          
          previousStage = currentStage;
        }
  }
 delay(100);
}

// written by Creepy Gnome
char * TimeToString(unsigned long t)
{
 static char str[12];
 long m = t / 60000;
 t = t % 60000;
 int s = t / 1000;
 int ms = t % 1000;
 sprintf(str, "%02ld:%02d:%04d", m, s, ms);
 return str;
}
