#include "vars.h"

unsigned int ntplocalPort = 8888;
byte packetBuffer[48];

uint32_t timer1;

LiquidCrystal_I2C lcd(0x27, 16, 2);
hardware_struct HARD;

// time_struct TIME;

EthernetServer server(80);
EthernetClient client;
server_struct HTTP;
EthernetUDP Udp;