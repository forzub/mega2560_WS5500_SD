#include "hard.h"


//---------------------------------------------------------------------
void hardware_struct::set_def(){
    ip    = {192, 168, 0, 177};
    myDns = {192, 168, 0, 1};  
    UTCoffset = 3;
    
    min_position = 292;
    max_position = 106;
    delta = 3;

    last_day_of_update = 1;
    last_mon_of_update = 1;
    last_year_of_update = 30;
}

//---------------------------------------------------------------------
void hardware_struct::Reset(){
  onError         = false;
  Lan_adapter     = false;
  hasSD           = false;
  Lan_connection  = false;
  hasClock        = false;
  waitNTP         = false;
  needSync        = false;


}

//---------------------------------------------------------------------
void hardware_struct::Start(){
  
  Serial.begin(9600);
  lcd.init();
  
  Console(F("\nSerial init. LCD init. SD init > "));

  pinMode(SD_CARD_SS_PIN,OUTPUT);
  if (SD.begin(SD_CARD_SS_PIN))
        {
          hasSD = true; 
          Consoleln(F("SD Card initialized."));}    //  SD Card initialized.
        else{
          hasSD = false; 
          Consoleln(F("SD Card not initialized.")); //  SD Card not initialized.
          return;}          
  
  Console(F("LAN init > "));
  LAN_start();

  Console(F("Clock init > "));
  hasClock = RTC.read(tm);
    

  if(!hasSD | !hasClock | !Lan_adapter){
    Serial.println(F("HARD error:"));
    if(!hasSD){Serial.println(F("SD Card"));}
    if(!hasClock){Serial.println(F("Clock"));}
    if(!Lan_adapter){Serial.println(F("Lan Adapter"));}
    Serial.println(F("HARD stop."));
    while(true){}
  }
    char var[4];
    itoa(tm.Day,var,DEC);           Console(var); Console(F(" ")); 
    Console(monthName[tm.Month-1]); Console(F(" "));
    itoa(1970 + tm.Year,var,DEC);   Console(var); Console(F(" "));

    if(tm.Hour < 10) {Console(F("0"));}
    itoa(tm.Hour,var,DEC);   Console(var);Console(F(":"));
    
    if(tm.Minute < 10) {Console(F("0"));} 
    itoa(tm.Minute,var,DEC);   Console(var);Console(F(":"));
    
    if(tm.Second < 10) {Console(F("0"));}
    itoa(tm.Second,var,DEC);   Consoleln(var);

}
//---------------------------------------------------------------------
void hardware_struct::systime_serial_prn(){
    char prn[5];
    RTC.read(tm);
    if((tm.Day) < 10){ Serial.print(F("0")); }
    itoa(tm.Day,prn,DEC); Serial.print(prn);Serial.print(F(" "));
    Serial.print(monthName[tm.Month - 1]);Serial.print(F(" "));
    itoa(tm.Year + 1970,prn,DEC); Serial.print(prn);Serial.print(F(" "));

    if((tm.Hour) < 10){ Serial.print(F("0")); }
    itoa(tm.Hour,prn,DEC); Serial.print(prn);Serial.print(F(":"));
    if((tm.Minute) < 10){ Serial.print(F("0")); }
    itoa(tm.Minute,prn,DEC); Serial.print(prn);Serial.print(F(":"));
    if((tm.Second) < 10){ Serial.print(F("0")); }
    itoa(tm.Second,prn,DEC); Serial.print(prn);    
}
//---------------------------------------------------------------------
void hardware_struct::LAN_start(){
  //-- start w5500
  Lan_adapter = true; 
    if (Ethernet.linkStatus() == LinkOFF || !Lan_connection){
          Ethernet.begin(mac, ip, myDns);
          if (Ethernet.hardwareStatus() == EthernetNoHardware){
              Lan_adapter = false; 
              Consoleln(F("Lan adapter was not found."));
              return;
          }else{
              if (Ethernet.linkStatus() == LinkOFF) {
                Lan_connection = false; 
                Consoleln(F("E-cable is not connected.")); 
                return;
                }
              else{
              Lan_connection = true; 
              server.begin();
              delay(500);
              Consoleln(F("E-cable is connected."));
              Console(F("Set IP:"));
              Consoleln(iptostr(Ethernet.localIP()));
              return;}
          }
        }

}
//---------------------------------------------------------------------
void hardware_struct::mega_dev_all_off(){

  digitalWrite(BUT_DOWN,HIGH);
  digitalWrite(BUT_UP,HIGH);
  digitalWrite(POWER_CONTROL,HIGH);

}

//---------------------------------------------------------------------
void hardware_struct::mega_dev_init(){
    
    analogReference(INTERNAL2V56);
    pinMode(W_POSITION,INPUT);
    pinMode(BUT_SET,INPUT);

    pinMode(BUT_UP,OUTPUT);
    pinMode(BUT_DOWN,OUTPUT);
    pinMode(POWER_CONTROL,OUTPUT);
    pinMode(ERR_SIGNAL,OUTPUT);

    digitalWrite(BUT_UP,HIGH);
    digitalWrite(BUT_DOWN,HIGH);
    digitalWrite(POWER_CONTROL,HIGH);
    digitalWrite(ERR_SIGNAL,HIGH);

}
//---------------------------------------------------------------------
String hardware_struct::iptostr(IPAddress nip){
  return  String(nip[0]) + "." + 
          String(nip[1]) + "." + 
          String(nip[2]) + "." + 
          String(nip[3]);
}

//---------------------------------------------------------------------
void hardware_struct::mega_sets_save(){

    SD.remove("setup.txt");
    
    File dataFile = SD.open("setup.txt",FILE_WRITE);
    if(!dataFile){
      Serial.println(F("file setup is not created"));
      return;
    } 

    dataFile.write(ip[0]);
    dataFile.write(ip[1]);
    dataFile.write(ip[2]);
    dataFile.write(ip[3]);

    dataFile.write(myDns[0]);
    dataFile.write(myDns[1]);
    dataFile.write(myDns[2]);
    dataFile.write(myDns[3]);

    byte* point;
    point = (byte*)&min_position;
    dataFile.write(*(point));
    dataFile.write(*(point+1));

    point = (byte*)&max_position;
    dataFile.write(*(point));
    dataFile.write(*(point+1));

    point = (byte*)&delta;
    dataFile.write(*(point));
    dataFile.write(*(point+1));

    dataFile.write(last_day_of_update);
    dataFile.write(last_mon_of_update);
    dataFile.write(last_year_of_update);    

    dataFile.close();

}
//---------------------------------------------------------------------
void hardware_struct::mega_sets_load(){
  File dataFile = SD.open("setup.txt");
  if(!dataFile){
    Serial.println(F("file setup is not found"));
    dataFile.close();
    mega_sets_save(); 
    return;
  } 
  ip[0] = dataFile.read();
  ip[1] = dataFile.read();
  ip[2] = dataFile.read();
  ip[3] = dataFile.read();

  myDns[0] = dataFile.read();
  myDns[1] = dataFile.read();
  myDns[2] = dataFile.read();
  myDns[3] = dataFile.read();

  byte* point;
  
  point = (byte*)&min_position;
  *(point)    = dataFile.read();
  *(point+1)  = dataFile.read();
  
  point = (byte*)&max_position;
  *(point)    = dataFile.read();
  *(point+1)  = dataFile.read();
  
  point = (byte*)&delta;
  *(point)    = dataFile.read();
  *(point+1)  = dataFile.read(); 

  last_day_of_update = dataFile.read();
  last_mon_of_update = dataFile.read();
  last_year_of_update = dataFile.read();
  
  dataFile.close();
  char prn[5];
  Console(F("last date of sync > ")); 
  itoa(last_day_of_update,prn,DEC);
  Console(prn); Console(" ");
  Console(monthName[last_mon_of_update - 1]); Console(" ");
  itoa(last_year_of_update + 1970,prn,DEC);
  Consoleln(prn);


  if (!RTC.read(tm)) {
      Serial.println(F("Clock is not ready. System stoped."));
      while(true){}
     }else{
      int last_time = last_day_of_update + last_mon_of_update * 30 + last_year_of_update * 365;
      int now_time = tm.Day + tm.Month * 30 + tm.Year *365;
      //Consoleln(String(now_time - last_time));
      if((false) || ((now_time - last_time) > 30)){
        Consoleln(F("start time sync."));
        //включить синхронизацию времени.
        waitNTP = true;
        needSync = true;
        Udp.begin(ntplocalPort);
        sendNTPpacket(timeServer);
        NTPservicetimer = millis();
      }
   }
}



//---------------------------------------------------------------------
void hardware_struct::sendNTPpacket(const char * address){
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
                                    // Initialize values needed to form NTP request
                                    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;            // Stratum, or type of clock
    packetBuffer[2] = 6;            // Polling Interval
    packetBuffer[3] = 0xEC;         // Peer Clock Precision
                                    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;

                                    // all NTP fields have been given values, now
                                    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123);  // NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}

//---------------------------------------------------------------------
void hardware_struct::getNTPpacket(){
  if(!waitNTP) {/*Serial.println("\r exit");*/ return;}

  if (Udp.parsePacket()){
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

      unsigned long secsSince1900 = highWord << 16 | lowWord;
      // Serial.print("Seconds since Jan 1 1900 = ");
      // Serial.println(secsSince1900);
      // Serial.print("Unix time = ");
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears;
    
      epoch += UTCoffset*3600;
      // Serial.println(epoch);
      RTC.set(epoch);
      RTC.read(tm);


      last_day_of_update = tm.Day;
      last_mon_of_update = tm.Month;
      last_year_of_update = tm.Year;

      mega_sets_save();

      // Serial.print("\rThe UTC time is ");
      // Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
      // Serial.print(':');
      // if (((epoch % 3600) / 60) < 10) {
      //   Serial.print('0');
      // }
      // Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
      // Serial.print(':');
      // if ((epoch % 60) < 10) {        
      //   Serial.print('0');
      // }
      // Serial.print(epoch % 60);

     Ethernet.maintain();
      waitNTP = false;
      needSync = false;     
  }
}

//---------------------------------------------------------------------
void hardware_struct::Console(const char ss){
    Serial.print(ss);
}

//---------------------------------------------------------------------
void hardware_struct::Console(String ss){
    Serial.print(ss);
}
//---------------------------------------------------------------------
void hardware_struct::Consoleln(const char ss){
    Serial.println(ss);
}

//---------------------------------------------------------------------
void hardware_struct::Consoleln(String ss){
    Serial.println(ss);
}