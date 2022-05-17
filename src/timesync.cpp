#include "timesync.h"


//---------------------------------------------------------------------
unsigned long SyncNptTime(){
    EthernetUDP Udp;
    // const char timeServer[] = "time.nist.gov";  // IP-адрес NTP сервера  
    const byte timeServer[] = {193,204,114,232};  // IP-адрес NTP сервера
    unsigned int localPort = 8888;
    const int NTP_PACKET_SIZE = 48;
    byte packetBuffer[NTP_PACKET_SIZE];


    
    Udp.begin(localPort);
    sendNTPpacket(Udp, timeServer, packetBuffer, NTP_PACKET_SIZE);

    delay(1000);

    if (Udp.parsePacket()) {
        
    Udp.read(packetBuffer, NTP_PACKET_SIZE); 

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;

    #ifdef CONSOLE
        // Serial.print(F("Seconds since Jan 1 1900 = "));
        // Serial.println(secsSince1900);
        // Serial.print(F("Unix time = "));
        // Serial.println(epoch);
        // Serial.print(F("The UTC time is "));       
        // Serial.print((epoch  % 86400L) / 3600); 
        // Serial.print(':');
        // if (((epoch % 3600) / 60) < 10) {
        //   Serial.print('0');
        // }
        // Serial.print((epoch  % 3600) / 60); 
        // Serial.print(':');
        // if ((epoch % 60) < 10) {
        //   Serial.print('0');
        // }
        // Serial.println(epoch % 60);
    #endif
    
    return  epoch;   
  }
  return 0;
}

//---------------------------------------------------------------------
void sendNTPpacket(EthernetUDP &Udp, const byte * address, byte packetBuffer[], const int NTP_PACKET_SIZE) {

  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision

  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  Udp.beginPacket(address, 123); // NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  
  Udp.endPacket();
}