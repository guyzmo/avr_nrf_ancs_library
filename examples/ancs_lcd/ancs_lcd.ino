//
//  ancs_lcd.ino
//  
//
//  Created by Luke Berndt on 8/24/14.
//
//
#include <lib_aci.h>
#include <SPI.h>
#include <EEPROM.h>

#include <notif.h>

#include <LiquidCrystal.h>
#include <LCDKeypad.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

Notif notif(12,11);


// Pins in use
#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
#define LCD_BACKLIGHT_PIN         10  // D10 controls LCD backlight
// ADC readings expected for the 5 buttons on the ADC input
#define RIGHT_10BIT_ADC           0  // right
#define UP_10BIT_ADC            145  // up
#define DOWN_10BIT_ADC          329  // down
#define LEFT_10BIT_ADC          505  // left
#define SELECT_10BIT_ADC        741  // right
#define BUTTONHYSTERESIS         10  // hysteresis for valid button sensing window
//return values for ReadButtons()
#define BUTTON_NONE               0  //
#define BUTTON_RIGHT              1  //
#define BUTTON_UP                 2  //
#define BUTTON_DOWN               3  //
#define BUTTON_LEFT               4  //
#define BUTTON_SELECT             5  //
//some example macros with friendly labels for LCD backlight/pin control, tested and can be swapped into the example code as you like
#define LCD_BACKLIGHT_OFF()     digitalWrite( LCD_BACKLIGHT_PIN, LOW )
#define LCD_BACKLIGHT_ON()      digitalWrite( LCD_BACKLIGHT_PIN, HIGH )
#define LCD_BACKLIGHT(state)    { if( state ){digitalWrite( LCD_BACKLIGHT_PIN, HIGH );}else{digitalWrite( LCD_BACKLIGHT_PIN, LOW );} }

byte antenna_char[8] = {
    B00000,
    B00000,
    B00000,
    B00100,
    B00100,
    B00100,
    B01110,
};



byte connected_char[8] = {
    B00000,
    B01110,
    B10001,
    B00100,
    B00100,
    B00100,
    B01110,
};


byte disconnected_char[8] = {
    B10001,
    B01010,
    B00100,
    B01010,
    B10101,
    B00100,
    B01110,
};

byte text_char[8] = {
    B00000,
    B01110,
    B01110,
    B01010,
    B01010,
    B01110,
    B00000,
};

byte email_char[8] = {
    B00000,
    B11111,
    B11011,
    B10101,
    B10001,
    B11111,
    B00000,
    
};

char wait = ' ';
unsigned long last_screen_update = 0;
unsigned long backlight_started = 0;
boolean backlight = false;
boolean connected = false;

void ancs_connected() {
    connected = true;
    }
void ancs_disconnected() {
    connected = false;
    }

void ancs_notifications(ancs_notification_t* notif) {

    char line1[16];
    char line2[16];
    
    strncpy(line1, notif->title, 16);
    strncpy(line2, notif->message, 16);
    line1[15] = '\0';
    line2[15] = '\0';
    lcd.clear();

    Serial.print (F("["));
    if ((notif->flags & ANCS_EVT_FLAG_SILENT) == ANCS_EVT_FLAG_SILENT)
        Serial.print(F("-"));
    else if ((notif->flags & ANCS_EVT_FLAG_IMPORTANT) == ANCS_EVT_FLAG_IMPORTANT)
        Serial.print(F("!"));
    else
        Serial.print(" ");
    Serial.print (F("] "));
    Serial.print(F("Notif #")); Serial.print( notif->uid); Serial.print( F(" ; from: '")); Serial.print( notif->app); Serial.println( F("'"));
    Serial.print(F("   category: "));
    switch (notif->category) {
        case ANCS_CATEGORY_INCOMING_CALL:
            Serial.println(F("incoming call"));
            break;
        case ANCS_CATEGORY_MISSED_CALL:
            Serial.println(F("missed call"));
            break;
        case ANCS_CATEGORY_VOICEMAIL:
            Serial.println(F("voicemail call"));
            break;
        case ANCS_CATEGORY_SOCIAL:
            Serial.println(F("social msg"));
            break;
        case ANCS_CATEGORY_OTHER:
            Serial.println(F("other"));
            break;
        case ANCS_CATEGORY_SCHEDULE:
            Serial.println(F("schedule"));
            break;
        case ANCS_CATEGORY_EMAIL:
            Serial.println(F("email"));
            break;
        case ANCS_CATEGORY_NEWS :
            Serial.println(F("news"));
            break;
        case ANCS_CATEGORY_HEALTH_FITNESS:
            Serial.println(F("health & fitness"));
            break;
        case ANCS_CATEGORY_BUSINESS_FINANCE:
            Serial.println(F("business & finance"));
            break;
        case ANCS_CATEGORY_LOCATION:
            Serial.println(F("location"));
            break;
        case ANCS_CATEGORY_ENTERTAINMENT:
            Serial.println(F("entertainment"));
            break;
        default:
            Serial.print(F("unknown: "));
            Serial.println(notif->category, DEC);
            break;
            //return;
    }
    Serial.print(F("   title:    '")); Serial.print( notif->title    ); Serial.println("'");
    Serial.print(F("   subtitle: '")); Serial.print( notif->subtitle ); Serial.println("'");
    Serial.print(F("   message:  '")); Serial.print( notif->message  ); Serial.println("'");
   lcd.setCursor(0,0);
   lcd.print((char)3);
    lcd.setCursor(2,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
    backlight = 1;
    LCD_BACKLIGHT_ON();
    backlight_started = millis();

}


void setup(void)
{
    Serial.begin(115200);
    //Wait until the serial port is available (useful only for the Leonardo)
    //As the Leonardo board is not reseted every time you open the Serial Monitor
#if defined (__AVR_ATmega32U4__)
    while(!Serial)
    {}
#endif
    Serial.println(F("Arduino setup"));
    notif.setup();
    notif.set_notification_callback_handle(ancs_notifications);
    notif.set_connect_callback_handle(ancs_connected);
    notif.set_disconnect_callback_handle(ancs_disconnected);
    lcd.begin(16, 2);
    digitalWrite( LCD_BACKLIGHT_PIN, HIGH );  //backlight control pin D3 is high (on)
    pinMode( LCD_BACKLIGHT_PIN, OUTPUT );     //D3 is an output
    lcd.createChar(0, antenna_char);
    lcd.createChar(1, connected_char);
    lcd.createChar(2, disconnected_char);
    lcd.createChar(3, text_char);
    lcd.createChar(4, email_char);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    Apple iOS   ");
    lcd.setCursor(0,1);
    lcd.print(" Notifications  ");

}

void loop()
{
    if((millis() - last_screen_update) > 1000) {
      lcd.setCursor(15,0);
      lcd.print(wait);
      if (wait == 0)
      {
          if (connected){
              wait = 1;
          }else {
              wait = 2;
          }
      } else {
        wait=0;
      }
      last_screen_update = millis();
    }
    if(backlight && ((millis() - backlight_started) > 60000)) {
        backlight = 0;
        LCD_BACKLIGHT_OFF();
    }

    notif.ReadNotifications();
}
