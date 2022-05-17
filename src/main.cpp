
#include "variables.h"

int old_value = 0;

void setup() {
  Serial.begin(9600);

  pinMode(SD_CARD_HARD_PIN,OUTPUT); //-- ножка 53 в оутпут, что бы работали устройства SPI
  
  HARD.set_def();
  HARD.Reset();
  HARD.Start();

  server.begin();
  delay(500);
  
  // HTTP.clearvars();

  HARD.Systime_prn_timer = millis();
  
  uint32_t sync_time = SyncNptTime() + (3600 * HARD.timeZone);
  if(( sync_time - HARD.last_sync_time) > 2629743){
    #ifdef CONSOLE
      Serial.println(F("Clock need sync > "));
    #endif
    watch.settimeUnix(sync_time); 
    #ifdef CONSOLE
      Serial.print(F("Clock-time > ")); Serial.println(watch.gettime("d-m-Y, H:i:s, D"));
    #endif
    HARD.last_sync_time = sync_time;
    HARD.mega_sets_save();
  };
  
  mega_dev_init();

  // unsigned long seventyYears;
  // Serial.println(sizeof(seventyYears));
  HARD.systime_serial = true;
  
  TimeElements te;
  te.Year = watch.year + 30;
  te.Month = watch.month;
  te.Day = watch.day;
  te.Hour = 0;
  te.Minute = 0;
  te.Second = 0;
  
  Serial.print("Year ");Serial.println(te.Year);
  Serial.print("Month ");Serial.println(te.Month);
  Serial.print("Day ");Serial.println(te.Day);
  Serial.print("Hour ");Serial.println(te.Hour);
  Serial.print("Minute ");Serial.println(te.Minute);
  Serial.print("Second ");Serial.println(te.Second);


  time_t t =  makeTime(te);
  Serial.print("t ");Serial.println(t);
  Serial.print("watch ");Serial.println(watch.gettimeUnix());
  Serial.print("watch ");Serial.println(watch.gettimeUnix() - t);
  
  // isWeaterRequest = true; 
}

void loop() {
  
  
  // time in Serial block ----------------------------------------------------
  // if((HARD.systime_serial) & ((millis() - HARD.Systime_prn_timer) >= 1000)){
  //   Serial.print(watch.gettime("H:i:s"));
  //   Serial.print("\r");
  //   HARD.Systime_prn_timer = millis();
  // }


  // LAN block ---------------------------------------------------------------
  if (Ethernet.linkStatus() == LinkOFF) {
      HARD.LAN_start();
    }


  // WEB block ---------------------------------------------------------------
  HTTP.listener();


  // WEATHER block ---------------------------------------------------------------
  if(isWeaterRequest){ weather_send_request(weather); isWeaterRequest = false; }
  
  if(isWeather){ weather_handler(weather);  }
  if((isWeather) && ((millis() - weather_watchdog) >= WEATHER_WATCHDOG)){
    Serial.println(F("weater: time is over"));
    weather.stop();  isWeather = false;
  }

  
  // DIGITAL block ---------------------------------------------------------------
  if(!digitalRead(BUT_SET)){
    digitalWrite(ERR_SIGNAL,LOW);
  }else{
    digitalWrite(ERR_SIGNAL,HIGH);
  }


  // ANALOG block ---------------------------------------------------------------
  // if((millis() - HARD.Systime_prn_timer) > 200 ){
  //   int value = HARD.value_on_potentiometer();
  //   if( abs(old_value - value) > HARD.delta ){
  //     Serial.print("value on potentiometer: ");
  //     Serial.println(value);
  //     old_value = value;
  //   }
  //   HARD.Systime_prn_timer = millis();
  // }

}

