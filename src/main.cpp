
#include "vars.h"
#include "hard.h"
#include "http.h"

void setup() {
  pinMode(SD_CARD_HARD_PIN,OUTPUT); //-- ножка 53 в оутпут, что бы работали устройства SPI


  HARD.set_def();
  HARD.Reset();
  HARD.Start();
  HTTP.clearvars();
  HARD.mega_sets_load();
  HARD.Systime_prn_timer = millis();

  HARD.waitNTP = false;
  // Udp.begin(ntplocalPort);
  // HARD.sendNTPpacket(timeServer);

  
  // unsigned long seventyYears;
  // Serial.println(sizeof(seventyYears));
  HARD.systime_serial = true;
}

void loop() {
  // time block ---------------------------------------------------------------
  // запрос, например - раз в месяц.
  // запоминаем время последней синхронизации
  // если что-то не так, раз в сутки повторяем запрос. если не получается  - выводим ошибку.

  if(HARD.needSync & !HARD.waitNTP & ((millis() - HARD.NTPservicetimer) > 3600)){
      //ждем час для повторной попытки
      Udp.begin(ntplocalPort);
      HARD.sendNTPpacket(timeServer);
      HARD.waitNTP = true;
      HARD.NTPservicetimer = millis();
  }
  if(HARD.needSync & HARD.waitNTP & ((millis() - HARD.NTPservicetimer) > 2000)){
    Serial.println("Server is not request");
    HARD.waitNTP = false;
    HARD.NTPservicetimer = millis();
  }
  HARD.getNTPpacket();
  
  // time in Serial block ----------------------------------------------------
  if((HARD.systime_serial) & ((millis() - HARD.Systime_prn_timer) >= 1000)){
    Serial.print("\r");
    HARD.systime_serial_prn();
    HARD.Systime_prn_timer = millis();
  }


  // LAN block ---------------------------------------------------------------
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("linkStatus == LinkOFF");
    HARD.LAN_start();
    }


  // LAN block ---------------------------------------------------------------
  HTTP.listener();



}

