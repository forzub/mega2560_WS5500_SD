#include "hard.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
hardware_struct HARD;
iarduino_RTC watch(RTC_DS1307);

//---------------------------------------------------------------------
void hardware_struct::set_def(){
    ip    = {192, 168, 0, 177};
    myDns = {192, 168, 0, 1};  
    
    min_position = 292;
    max_position = 106;
    delta = 3;

    last_sync_time = 0;
    timeZone = 3;
}

//---------------------------------------------------------------------
void hardware_struct::Reset(){
  onError         = false;
  hasSD           = false;
  Lan_adapter     = true;
  Lan_connection  = true;
  hasClock        = false;
  waitNTP         = false;
  needSync        = false;


}

//---------------------------------------------------------------------
void hardware_struct::Start(){
  
  
  lcd.init();
  
  Console(F("\nSerial init. LCD init. SD init > "));

  pinMode(SD_CARD_SS_PIN,OUTPUT);
  if (SD.begin(SD_CARD_SS_PIN))
        {
          hasSD = true; 
          #ifdef CONSOLE
            Serial.println(F("SD Card initialized."));}    //  SD Card initialized.
          #endif
        else{
          hasSD = false;
          #ifdef CONSOLE 
            Serial.println(F("SD Card not initialized.")); //  SD Card not initialized.
          #endif
          return;
        }

  mega_sets_load();
  // def_output();         
  
  Console(F("LAN init > "));
  LAN_start();

  Console(F("Clock init > "));
  watch.begin();
  #ifdef CONSOLE
    Serial.println(watch.gettime("d-m-Y, H:i:s, D"));
  #endif
  hasClock = true;
    

  if(!hasSD | (watch.Unix == 0)){
    #ifdef CONSOLE
      Serial.println(F("HARD error:"));
      if(!hasSD){Serial.println(F("SD Card"));}
      if(watch.Unix == 0){Serial.println(F("Clock"));}
      Serial.println(F("HARD stop."));
    #endif
    while(true){}
  }
    
}
//---------------------------------------------------------------------
void hardware_struct::systime_serial_prn(){
    // char prn[5];
    // RTC.read(tm);
    // if((tm.Day) < 10){ Serial.print(F("0")); }
    // itoa(tm.Day,prn,DEC); Serial.print(prn);Serial.print(F(" "));
    // Serial.print(monthName[tm.Month - 1]);Serial.print(F(" "));
    // itoa(tm.Year + 1970,prn,DEC); Serial.print(prn);Serial.print(F(" "));

    // if((tm.Hour) < 10){ Serial.print(F("0")); }
    // itoa(tm.Hour,prn,DEC); Serial.print(prn);Serial.print(F(":"));
    // if((tm.Minute) < 10){ Serial.print(F("0")); }
    // itoa(tm.Minute,prn,DEC); Serial.print(prn);Serial.print(F(":"));
    // if((tm.Second) < 10){ Serial.print(F("0")); }
    // itoa(tm.Second,prn,DEC); Serial.print(prn);    
}
//---------------------------------------------------------------------
void hardware_struct::systime_str(char *time_str){
  // strcpy(time_str,"");
  //   char prn[5];
  //   RTC.read(tm);
  //   if((tm.Hour) < 10){ strcat(time_str,"0");}
  //   itoa(tm.Hour,prn,DEC); strcat(time_str,prn); strcat(time_str,":");
  //   if((tm.Minute) < 10){ strcat(time_str,"0"); }
  //   itoa(tm.Minute,prn,DEC); strcat(time_str,prn); strcat(time_str,":");
  //   if((tm.Second) < 10){ strcat(time_str,"0"); }
  //   itoa(tm.Second,prn,DEC); strcat(time_str,prn); strcat(time_str," "); 
}


//---------------------------------------------------------------------
void hardware_struct::LAN_start(){
  //-- start w5500
    Ethernet.begin(mac, ip, myDns);
          
          if (Ethernet.hardwareStatus() == EthernetNoHardware){
              if(Lan_adapter){
                Serial.println(F("Lan adapter was not found."));
              }
                Lan_adapter = false; 
              return;
          }else{
              Lan_adapter = true;
              
              if (Ethernet.linkStatus() == LinkOFF) {
                if(Lan_connection){
                  Serial.println(F("E-cable is not connected.")); 
                }
                Lan_connection = false;
                return;
                }
              else{
                Lan_connection = true; 
                Consoleln(F("E-cable is connected."));
                Console(F("Set IP:"));
                Consoleln(iptostr(Ethernet.localIP()));
                return;
              }
          }
        

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
   

    dataFile.write(SETUP_FILE_HEADER);

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

    point = (byte*)&last_sync_time;
    dataFile.write(*(point));
    dataFile.write(*(point+1));
    dataFile.write(*(point+2));
    dataFile.write(*(point+3));

    dataFile.write(timeZone);

    dataFile.close();

}
//---------------------------------------------------------------------
void hardware_struct::mega_sets_load(){
  File dataFile = SD.open("setup.txt");
  
  if(!dataFile){
    #ifdef CONSOLE
      Serial.println(F("file setup is not found"));
    #endif
    dataFile.close();
    mega_sets_save(); 
    return;
  } 

  byte header = dataFile.read();

  if(header != SETUP_FILE_HEADER){
    #ifdef CONSOLE
      Serial.print("header not support ");
      Serial.println(header);
    #endif
    dataFile.close();
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

  point = (byte*)&last_sync_time;
    *(point) = dataFile.read();
    *(point+1) = dataFile.read();
    *(point+2) = dataFile.read();
    *(point+3) = dataFile.read();
  
  timeZone = dataFile.read();
  
  dataFile.close();
  

}
//---------------------------------------------------------------------
void def_output(){

  Serial.println();
  Serial.print("ip "); Serial.println( HARD.iptostr(HARD.ip) );
  Serial.print("myDns "); Serial.println( HARD.iptostr(HARD.myDns) );
  Serial.print("last_sync_time "); Serial.println( HARD.last_sync_time );
  Serial.print("timeZone "); Serial.println( HARD.timeZone );

  Serial.print("min_position "); Serial.println( HARD.min_position );
  Serial.print("max_position "); Serial.println( HARD.max_position );
  Serial.print("delta "); Serial.println( HARD.delta );
  Serial.println();

}

//---------------------------------------------------------------------
int hardware_struct::value_on_potentiometer(){

  return analogRead(W_POSITION);
}


