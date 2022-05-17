#ifndef http_h
#define http_h

#define DEBUG

#include <Arduino.h>
#include <SD.h>
#include <Wire.h>
#include "Ethernet.h"
#include "console.h"
#include "mega.h"
#include "hard.h"
#include "weatherWebServer.h"

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

  const char table_filename [9] = "ttab.txt";


  boolean is_content_count  : 1;
  uint32_t content_length;      //-- Заголовок Content-Length - это число, обозначающее точную длину байта тела HTTP.
  uint32_t content_count;
                                //-- Тело HTTP начинается сразу после первой пустой строки, найденной после начальной строки и заголовков.
  unsigned long http_wachdog_timer; 

  const char textplain[11] = "text/plain";                             


  void listener();
  void listener1();
  void reqst_run();
  void clearvars();
  void clear_str(char*, int);
  void clear_buf();
  void printvars();

  void    send(int code, const char *Content_Type, const char *message);
  boolean hasArg(const char *str);
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

  void get_table_from_SD();
  void get_setup_from_SD();
  void set_table_on_SD();
  void set_setup_on_SD();
  
};

void str_point_to_ip(IPAddress &target, byte len, char* text);

void open_handle();




extern server_struct HTTP;
extern EthernetServer server;

#define TABLE_FILE_HEADER 15

#endif

