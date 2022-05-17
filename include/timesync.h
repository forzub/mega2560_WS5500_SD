#ifndef timesync_h
#define timesync_h

#include <Wire.h>
#include "Ethernet.h"

#define CONSOLE




void sendNTPpacket(EthernetUDP &Udp, const byte * address, byte packetBuffer[], const int NTP_PACKET_SIZE);
unsigned long SyncNptTime();


#endif