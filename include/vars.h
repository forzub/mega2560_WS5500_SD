#ifndef vars_h
#define vars_h

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Ethernet.h"
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <DS1307RTC.h>
#include <TimeLib.h>


// включает в себя модуль w5500, часы DS3231, флешпамять в виде карты памяти. 
// если из этого что-то не работает - отключаем систему и сообщение на LCD
//----------------- hard ------------------------
#define SD_CARD_HARD_PIN 53
#define SD_CARD_SS_PIN 45


extern uint32_t timer1;

struct hardware_struct{
  byte mac[6] =  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  IPAddress ip;
  IPAddress myDns;
  byte  UTCoffset;

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

  uint8_t last_day_of_update;
  uint8_t last_mon_of_update;
  uint8_t last_year_of_update;

  uint8_t hour    :   5;  // время следующего срабатывания - часы - ttab
  uint8_t minute  :   6;  // время следующего срабатывания - минуты - ttab

  uint16_t con_position;  // контрольная позиция (для аварийного отключения)
  uint16_t new_position;  // позиция к которой стремимся - ttab       
  uint16_t min_position;  // значение датчика положения - sd окно откріто на 0%       //292
  uint16_t max_position;  // значение датчика положения - sd окно откріто на 100%     //106
  uint16_t delta;         // погрешность - sd                                         //3
  
  tmElements_t tm;
  const char *monthName[12]  =  {"Jan", "Feb", "Mar", "Apr", "May", "Jun","Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  

  void Reset();
  void Start();
  void mega_dev_init();
  void mega_dev_all_off();
  void mega_sets_load();
  void mega_sets_save();
  void set_def();
  void LAN_start();
  void time_sync();
  String iptostr(IPAddress nip);

  void Console(const char ss);
  void Console(String ss);
  void Consoleln(const char ss);
  void Consoleln(String ss); 
  void ConsoleTime();

  void sendNTPpacket(const char * address);
  void getNTPpacket();

  void systime_serial_prn();
  void systime_str(char *time_str);
};


extern LiquidCrystal_I2C lcd;
extern hardware_struct HARD;

//---------------------------------- time-struct ---------------------
#define NTP_PACKET_SIZE 48
const char timeServer[] = "time.nist.gov";
extern unsigned int ntplocalPort;
extern byte packetBuffer[NTP_PACKET_SIZE];

// struct time_struct{
    
    
//     // char* clock_time_data();
//     // char* filename_time_format();
//     // char* log_time_format();
//     // bool getTime(const char *str);
//     // bool getDate(const char *str);
    
// };

// extern time_struct TIME;


//---------------------------------- server-struct -------------------
#define HTTP_GET 0
#define HTTP_DELETE 1
#define HTTP_PUT 2
#define HTTP_POST 3
#define HTTP_WACHDOG_TIME 2000

struct server_struct{
  EthernetClient client;
  
  
  byte method;
  const char *method_tx[4] = {"GET","DELETE","PUT","POST"};
  struct byte_buf { byte buf[500];  int len;  };        //-- буфер для чтения
 
  byte_buf http_buf;
  char url_address[50] = "";   //-- путь к файлу на SD
  char filename[50]= "";        //-- имя файла
  char params[255]= "";         //-- строка параметров
  char boundary[70]= "";        //-- директива boundary

  boolean is_content_count  : 1;
  uint32_t content_length;      //-- Заголовок Content-Length - это число, обозначающее точную длину байта тела HTTP.
  uint32_t content_count;
                                //-- Тело HTTP начинается сразу после первой пустой строки, найденной после начальной строки и заголовков.
  unsigned long http_wachdog_timer; 
                                


  void listener();
  void reqst_run();
  void clearvars();
  void clear_str(char*, int);
  void clear_buf();
  void printvars();

  void    send(int code, const char *Content_Type, const char *message);
  boolean hasArg(char *str);
  String  arg(const char *str);
  void    returnFail(const char* msg);
  void    returnOK(); 

  boolean loadFromSdCard();
  void    handleNotFound();
  void    printDirectory();
  void    handleDelete();
  void    handleCreate();
  void    deleteRecursive(String path);

  void    get_parsing(byte , char *);

  void    bytecat(byte *, int *, byte *, int);
  void    bytecpy(byte *, int *, byte *, int);
};

extern EthernetServer server;
extern EthernetClient client;
extern server_struct HTTP;
extern EthernetUDP Udp;

//---------------------------------- описание boundary
// POST /form.html HTTP/1.1\r\n
// Host: server.com\r\n
// Referer: http://server.com/form.html\r\n
// User-Agent: Mozilla\r\n
// Content-Type: multipart/form-data; boundary=-------------573cf973d5228\r\n
// Content-Length: 288\r\n
// Connection: keep-alive\r\n
// Keep-Alive: 300\r\n
// (пустая строка)\r\n
// ---------------573cf973d5228\r\n
// Content-Disposition: form-data; name="field"\r\n
// \r\n
// text\r\n
// ---------------573cf973d5228\r\n
// Content-Disposition: form-data; name="file"; filename="sample.txt"\r\n
// Content-Type: text/plain\r\n
// \r\n
// Content file\r\n
// ---------------573cf973d5228--



//---------------------------------- win-struct -------------------

#define W_POSITION A2
#define BUT_DOWN 30
#define BUT_UP 28
#define BUT_SET 24
#define POWER_CONTROL 26
#define ERR_SIGNAL 6

#endif