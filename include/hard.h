#ifndef hard_h
#define hard_h

#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <iarduino_RTC.h>

#include "http.h"
#include "timesync.h"
#include "console.h"
#include "mega.h"
// #include "variables.h"



#define SETUP_FILE_HEADER 15

// включает в себя модуль w5500, часы DS3231, флешпамять в виде карты памяти. 
// если из этого что-то не работает - отключаем систему и сообщение на LCD
//----------------- hard ------------------------
#define SD_CARD_HARD_PIN 53
#define SD_CARD_SS_PIN 45


struct hardware_struct{
  byte mac[6] =  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  IPAddress ip;
  IPAddress myDns;


  uint32_t NTPservicetimer;
  uint32_t Systime_prn_timer;

  boolean Lan_connection  :1;
  boolean Lan_adapter     :1;
  boolean onError         :1;
  boolean hasSD           :1;
  boolean hasClock        :1;
  boolean waitNTP         :1;
  boolean needSync        :1;
  boolean systime_serial  :1;

        
  uint32_t last_sync_time;
  uint8_t timeZone;       // TimeZone
   

  uint8_t hour    :   5;  // время следующего срабатывания - часы - ttab
  uint8_t minute  :   6;  // время следующего срабатывания - минуты - ttab

  uint16_t con_position;  // контрольная позиция (для аварийного отключения)
  uint16_t new_position;  // позиция к которой стремимся - ttab       
  uint16_t min_position;  // значение датчика положения - sd окно откріто на 0%       //292
  uint16_t max_position;  // значение датчика положения - sd окно откріто на 100%     //106
  uint16_t delta;         // погрешность - sd                                         //3
  
//   const char *monthName[12]  =  {"Jan", "Feb", "Mar", "Apr", "May", "Jun","Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  

    void Reset();
    void Start();
    void mega_sets_load();
    void mega_sets_save();
    void set_def();
    void LAN_start();
    String iptostr(IPAddress nip);


    void systime_serial_prn();
    void systime_str(char *time_str);

    int value_on_potentiometer();
 
};

void def_output();
extern LiquidCrystal_I2C lcd;
extern hardware_struct HARD;
extern iarduino_RTC watch;


#endif