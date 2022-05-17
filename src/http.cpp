#include "http.h"

server_struct HTTP;
EthernetServer server(80);



//-----------------------------------------------------------
//--------------------- слушает порт ------------------------
void server_struct::listener(){
  
   client = server.available();
   if(client){
      Console(F("\n"));
      Console(F("New Client.\n"));
        boolean read_title = true;
        http_wachdog_timer = millis();
        char tmp_file_name[50] = "";
        boolean isFinish = false;
        boolean isbody = false;
        File dataFile;
      
      while (client.connected()){
        if (client.available()){
          //-------- чтение данных ----------------------- 
          int len = client.available(); 
          byte buff[80];
 
          if (len > 80){len = 80;} 
          if(is_content_count){content_count += len;}
          client.read(buff, len);
          
          bytecat(http_buf.buf, &http_buf.len, buff, len);

          
          //-------- чтение тєгов заголовка ------------------------
            if(read_title)
            {
                    
                char *point = strstr((char*)http_buf.buf,"\r\n");

                while(point != NULL){
                    char str_for_search[255]="";
                    strncpy( str_for_search, (char*)http_buf.buf, point - (char*)http_buf.buf );

                    //Serial.print("prn> "); Serial.print(str_for_search);Serial.println("<");

                    get_parsing(HTTP_GET, (char *)str_for_search);
                    get_parsing(HTTP_POST, (char *)str_for_search);
                    get_parsing(HTTP_PUT, (char *)str_for_search);
                    get_parsing(HTTP_DELETE, (char *)str_for_search);

                    char * st_point;
                    char content_length_str[] PROGMEM = "Content-Length:";
                    char boundary_str[] PROGMEM = "boundary=";
                    st_point = strstr(str_for_search, content_length_str);
                    if(st_point != NULL){
                      strcpy(str_for_search,st_point + strlen(content_length_str) + strlen(" "));
                      content_length = atol(str_for_search);  
                    }
                    st_point = strstr(str_for_search, boundary_str);
                    if(st_point != NULL){strcat(boundary,st_point + strlen(boundary_str));}     

                    bytecpy(http_buf.buf, &http_buf.len, (byte *)point + strlen("\r\n"), http_buf.len - (point - (char*)http_buf.buf + strlen("\r\n") ) );
                    if(strlen(str_for_search) < 2){read_title = false;}
                    point = strstr((char*)http_buf.buf,"\r\n"); 
                }    
            }       
        }   
            //------------------- HTTP_WACHDOG ------------------------------
            if( (millis() - http_wachdog_timer) > HTTP_WACHDOG_TIME ){
              Consoleln(F("http_wachdog_timer")); 
              dataFile.close(); 
              break;
              }
            
            //-------------- запрос PUT DELETE ------------------------------

            if((!read_title) & (content_length > 0) & (method != HTTP_POST)){
                if(!is_content_count){
                  content_count = http_buf.len;
                  is_content_count = true;
                }

                if((content_count) >= content_length){
                  
                  Consoleln(F("r-body:"));
                  Consoleln((char*)http_buf.buf);
                  
                  char *point = strstr((char *)http_buf.buf,"\r\n");
                  while(point != NULL){
                    char str_for_search[255] = "";
                    //Serial.print("str_for_search>");Serial.println(str_for_search);
                    //clear_str((char *)str_for_search, (int)sizeof(str_for_search));

                    strncpy(str_for_search, (char *)http_buf.buf, point - (char *)http_buf.buf);
                    bytecpy(http_buf.buf, &http_buf.len, (byte *)point + strlen("\r\n"), http_buf.len - (point - (char*)http_buf.buf + strlen("\r\n") ) );
                    point = strstr((char *)http_buf.buf,"\r\n");
                    if(strlen(str_for_search) < 2){
                      isFinish = true;
                      break;
                      }
                  }
                  if(isFinish){
                      strncat(filename,(char *)http_buf.buf,point - (char *)http_buf.buf);
                      break;
                  }
                  
                }
            }

            //-------------- запрос POST -----------------------------
            if((!read_title) & (content_length > 0) & (method == HTTP_POST)){
              if(!is_content_count){
                  content_count = http_buf.len;
                  is_content_count = true;  
                }
              
              if(!isbody){   
                  char *point = strstr((char *)http_buf.buf,"\r\n");
                  while(point != NULL){
                    char str_for_search[255] = "";
                    strncpy(str_for_search, (char *)http_buf.buf, point - (char *)http_buf.buf);
                    bytecpy(http_buf.buf, &http_buf.len, (byte *)point + strlen("\r\n"), http_buf.len - (point - (char*)http_buf.buf + strlen("\r\n") ) );
                    
                        char v_filename[] PROGMEM = "filename=\"";
                        char *st_point = strstr(str_for_search, v_filename);
                        if(st_point != NULL){
                          point = strstr(st_point + strlen(v_filename), "\"");
                          strncpy(filename,st_point + strlen(v_filename),point - (st_point + strlen(v_filename)) );  
                        }

                    point = strstr((char *)http_buf.buf,"\r\n");
                    
                    if(strlen(str_for_search) < 2){
                        isbody = true;
                        strcat(tmp_file_name, filename);
                        char *point = strstr(tmp_file_name,"."); 
                        strcpy(point + 1,"tmp");
                        SD.remove(tmp_file_name);
                      Serial.println(F(" bytes"));
                      break;
                      }
                  }// while
                }

              if(isbody){
                    int boundary_len = strlen((char *)F("--    --")) + strlen(boundary);
                    if(http_buf.len > boundary_len){
                      dataFile = SD.open(tmp_file_name, FILE_WRITE); 
                      if(!dataFile){ Serial.println(F("SD card not ready")); dataFile.close(); break;}
                      //Serial.write(http_buf.buf, http_buf.len - boundary_len);
                      dataFile.write(http_buf.buf, http_buf.len - boundary_len); 
                      dataFile.close();
                      bytecpy(http_buf.buf, &http_buf.len, http_buf.buf + http_buf.len - boundary_len, boundary_len);
                      http_wachdog_timer = millis();
                      Serial.print("\r"); Serial.print(content_count); 
                    }
                      
                      if((content_count) >= content_length) { 
                        returnOK();
                        Serial.println();
                        break;
                      }
      
                }
            }

            //-------------- запрос GET ------------------------------
            if((!read_title) & (content_length == 0)){
              break;
            }

      } //while
      
      //returnOK();


      reqst_run(); 
      delay(1);
      client.stop();
      Consoleln(F("client disconnected"));
                        //-- если размері совпадают, перезаписіваем файлы
                        if((method == HTTP_POST) & (content_count >= content_length)){
                          SD.remove(filename);
                          File sourseFile = SD.open(tmp_file_name);
                          File targetFile = SD.open(filename, FILE_WRITE);
                          int16_t len = 0;
                          uint32_t filesize = 0;
                          byte buf[80];

                          while(sourseFile.available()){
                            if((filesize + 80) < sourseFile.size()){
                              len = 80; filesize += 80;
                            }else{ 
                              len = sourseFile.size() - filesize;
                            }
                            Serial.print("\r");Serial.print(filesize);
                            sourseFile.read(buf, len);
                            targetFile.write(buf, len);
                          }
                          sourseFile.close();
                          targetFile.close();
                          SD.remove(tmp_file_name); 
                          Serial.println();
                        }

      //HARD.Consoleln((char*)http_buf.buf);
      
      printvars();
      clearvars();
      
   }
     
}
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------

//--------------------- печать переменных -------------------
void server_struct::printvars(){
  Console(F("\n"));
  Console(F("method> "));         Consoleln(method_tx[method]);
  Console(F("url_address> "));    Consoleln(url_address);
  Console(F("params> "));         Consoleln(params);
  //HARD.Console(F("boundary> "));HARD.Consoleln(boundary);
  Console(F("filename> "));       Consoleln(filename);
  Console(F("content count> "));  Consoleln(String(content_count));
  Console(F("content length> ")); Consoleln(String(content_length));
  Console(F("\n"));
}

//--------------------- очистка переменных ------------------
void server_struct::clearvars(){

// for(int i = 0; i < (int)sizeof(url_address); i++){url_address[i] = (char)NULL;}
// for(int i = 0; i < (int)sizeof(filename); i++){filename[i] = (char)NULL;}
// for(int i = 0; i < (int)sizeof(params); i++){params[i] = (char)NULL;}
// for(int i = 0; i < (int)sizeof(boundary); i++){boundary[i] = (char)NULL;}

  clear_str(url_address, (int)sizeof(url_address));
  clear_str(filename, (int)sizeof(filename));
  clear_str(params, (int)sizeof(params));
  clear_str(boundary, (int)sizeof(boundary));

  clear_buf();
  content_length = 0;
  content_count = 0;
  is_content_count = false;
  method = 0;
}

//------------------- парсинг запроса и параметров ----------
void server_struct::get_parsing(byte gts, char *sourse){
  
  char *st_point = strstr(sourse, method_tx[gts]);
  if(st_point != NULL){
    method = gts; 
    int len = strlen(sourse) - strlen(method_tx[gts]) - strlen(" ") - strlen(" HTTP/1.1");
    strncpy(params, st_point + strlen(method_tx[gts]) + strlen(" "), len );    
    st_point = strstr(params,"?");
    if(st_point != NULL){
        strncpy(url_address, params, st_point - params);
        strcpy(params,st_point);
        params[0] = '&';
      } 
      else{
        strcpy(url_address, params);
        strcpy(params,"");
      }
  }
}
                    


//----------------- как strcpy только для byte ---------------
void server_struct::bytecpy(byte * target, int * size, byte * sourse, int len){

  for(int i = 0; i < len; i++){
    target[i] = sourse[i];
  }
  *size = len;
  target[*size] = '\0';
}


//----------------- как strcat только для byte ---------------
void server_struct::bytecat(byte * target, int * size, byte * sourse, int len){
  for(int i=0; i < len; i++){target[*size + i] = sourse[i];}
  *size += len;
  target[*size] = '\0';
}


//--------------------- очистка буера -----------------------
void server_struct::clear_buf(){
  for(int i = 0; i < (int)sizeof(http_buf.buf); i++){http_buf.buf[i] = (char)NULL;}
  http_buf.len = 0;
}
//--------------------- очистка строк ------------------
void server_struct::clear_str(char * mystr, int len){
for(int i = 0; i < len; i++){mystr[i] = (char)NULL;}
}



//-----------------------------------------------------------
//--------------------- получить аргумент -------------------
String server_struct::arg(const char *str){
    char text[255] = "";
    char tex2[50] = "";
    char* point1;
    char* point2;
        
    strcat(text,"&");
    strcat(text,str); 
    strcat(text,"=");
    point1 = strstr(params,text);
    if(point1 != NULL){
        point2 = strchr(point1 + 1,'&');
        if(point2 !=NULL){
            strncpy(tex2,point1 + strlen(text),point2 - point1 - strlen(text));
            tex2[point2 - point1 - strlen(text) +1] = (char)NULL;
            //Serial.print(F("\npoint1>"));Serial.print(tex2);Serial.print("<\n");
        }else{
            strcpy(tex2,point1 + strlen(text));
        } 
        return String(tex2);
        }
    return "";
}

//-----------------------------------------------------------
//--------------------- проверить аргумент ------------------

bool server_struct::hasArg(const char *str){
    char text[255] = "";
    
    strcat(text,"&");
    strcat(text,str);
    strcat(text,"=");

    // Serial.print("hasArg->");Serial.println(str); 
    // Serial.print("hasArg->");Serial.println(text);
    // Serial.print("hasArg->");Serial.println(params);
    
    if(strstr(params,text) != NULL){return true;}
    return false;  
}

//-----------------------------------------------------------
//--------------------- отправка ответа ---------------------
void server_struct::send(int code, const char *Content_Type, const char *message){

  client.print(F("HTTP/1.1 ")); 
  client.print(code);
  client.println(F(" OK"));
  client.print(F("Content-Type: ")); 
  client.println(Content_Type);
  client.println(F("Connection: close"));  
  // the connection will be closed after completion of the response
  //aclient.println(F("Refresh: 5"));  // refresh the page automatically every 5 sec
  client.println();
  if(strlen(message) > 0){client.println(message);}  
}

//-----------------------------------------------------------
//--------------------- файл найден или не найден -----------
void server_struct::handleNotFound(){
    if(loadFromSdCard()) return;
  
    String message = "SDCARD Not Detected\n\n";
    message += "\nMethod: ";
    message += (method == HTTP_GET)?"GET":"POST";
    message += "\npath: ";
    message += url_address;
    message += "\nArguments: ";
    message += params;
    send(404, textplain, message.c_str() );
}
//-----------------------------------------------------------
//--------------------- отправка файла с SD карты -----------
bool server_struct::loadFromSdCard(){
  String path = String(url_address);
  String dataType = F("text/plain");
  if(path.endsWith(F("/"))){ path += F("index.htm");}

  if(path.endsWith(F(".src"))) path = path.substring(0, path.lastIndexOf(F(".")));
  else if(path.endsWith(F(".htm"))) dataType = F("text/html");
  else if(path.endsWith(F(".css"))) dataType = F("text/css");
  else if(path.endsWith(F(".js"))) dataType = F("application/javascript");
  else if(path.endsWith(F(".png"))) dataType = F("image/png");
  else if(path.endsWith(F(".gif"))) dataType = F("image/gif");
  else if(path.endsWith(F(".jpg"))) dataType = F("image/jpeg");
  else if(path.endsWith(F(".ico"))) dataType = F("image/x-icon");
  else if(path.endsWith(F(".xml"))) dataType = F("text/xml");
  else if(path.endsWith(F(".pdf"))) dataType = F("application/pdf");
  else if(path.endsWith(F(".zip"))) dataType = F("application/zip");
  else if(path.endsWith(F(".log"))) {}
  else if(path.endsWith(F(".tmp"))) {}

  File dataFile = SD.open(path.c_str());
  if(dataFile.isDirectory()){
    path += F("/index.htm");
    dataType = F("text/html");
    dataFile = SD.open(path.c_str());
  }
  if (!dataFile){    
      Serial.print("file > "); Serial.print(path); Serial.print(params); Serial.print(F(" 404 - FNF"));
      dataFile.close();
      return false;
  }
  char v_dawnload[9] PROGMEM = "download";
  if (hasArg(v_dawnload)){dataType = F("application/octet-stream"); Serial.println("is download");} 
  send(200, dataType.c_str(), "");
  
  unsigned long readsize = 0;
  while (dataFile.available()){
    byte rf[80];
    readsize += 80;
    if(readsize > dataFile.size()){
      dataFile.read(rf,80);
      client.write(rf, dataFile.size() - readsize + 80);
    }else{
      dataFile.read(rf,80);
      client.write(rf,80);
    } 
  }
  
  dataFile.close();
  return true;
}
//-----------------------------------------------------------
//--------------------- отправка в браузер сообщ. об ошибке -
void server_struct::returnFail(const char* msg) {
  char message[100] = "";
  strcat(message,msg);
  strcat(message, "\r\n");
  send(500, textplain, message);
}

//-----------------------------------------------------------
//--------------------- отправка в браузер списка файлов ----
void server_struct::printDirectory() {
  if(!hasArg("dir")){return returnFail("BAD ARGS");}  
  String path = arg("dir");
  if(path != "/" && !SD.exists((char *)path.c_str())) return returnFail("BAD PATH");
  File dir = SD.open((char *)path.c_str());
  path = String();
  if(!dir.isDirectory()){
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();
  send(200, "text/json", "");
  client.print("[");
  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry)
    break;
    String output;
    if (cnt > 0){output = ',';}

      output += "{\"type\":\"";
      output += (entry.isDirectory()) ? "dir" : "file";
      output += "\",\"name\":\"";
      output += entry.name();
      output += "\"";
      output += "}";
      client.print(output);
      entry.close();
    }

 client.print("]");
 dir.close();
}
//-----------------------------------------------------------
//--------------------- Создать файл ------------------------
void server_struct::handleCreate(){
 
  if(strlen(filename) == 0) return returnFail("BAD ARGS");
  String path = filename;
  if(path == "/" || SD.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;  
  }

  if(path.indexOf('.') > 0){
    File file = SD.open((char *)path.c_str(), FILE_WRITE);
    if(file){
      file.write((const char *)0);
      file.close();
    }
  } else {
    SD.mkdir((char *)path.c_str());
  }
  
  returnOK();
}

//-----------------------------------------------------------
//--------------------- Удалить файл ------------------------
void server_struct::deleteRecursive(String path){
  File file = SD.open((char *)path.c_str());
  if(!file.isDirectory()){
    file.close();
    SD.remove((char *)path.c_str());
    return;
  }

  file.rewindDirectory();
  while(true) {
    File entry = file.openNextFile();
    if (!entry) break;
    String entryPath = path + "/" +entry.name();
    if(entry.isDirectory()){
      entry.close();
      deleteRecursive(entryPath);
    } else {
      entry.close();
      SD.remove((char *)entryPath.c_str());
    }
    yield();
  }

  SD.rmdir((char *)path.c_str());
  file.close();
}

//------------------------------------------------------------
void server_struct::handleDelete(){
  
  if(strlen(filename) == 0) return returnFail("BAD ARGS");
  String path = filename;
  if(path == "/" || !SD.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }
  deleteRecursive(path);
  
  returnOK();
}

//-----------------------------------------------------------
//--------------------- simply 200 --------------------------

void server_struct::returnOK(){
  send(200, textplain, "");
}

//-----------------------------------------------------------
void server_struct::set_table_on_SD(){
  
  if(SD.exists(table_filename)){SD.remove(table_filename);}
  
  File f = SD.open(table_filename, FILE_WRITE);
  f.write(TABLE_FILE_HEADER);
  f.write(' ');
  f.write(arg("t").c_str(),arg("t").length());
  f.close();

  send(200, textplain, arg("t").c_str());
  
}


//-----------------------------------------------------------
void server_struct::get_table_from_SD(){

  if(!SD.exists(table_filename)){return;}

  File f = SD.open(table_filename);

    if( ((byte)f.read()) != TABLE_FILE_HEADER ){
      SD.remove(table_filename);
    }else{
      f.read();
    }

    String msg = "";
    while(f.available()){
      msg += (char)f.read();
    }
  f.close();
  send(200, textplain, msg.c_str());
}

//-----------------------------------------------------------
void server_struct::get_setup_from_SD(){
  char result[45] = "";
  char substr[5] = "";
  itoa(HARD.ip[0], substr, 10);  strcat(result, substr);   strcat(result, ".");
  itoa(HARD.ip[1], substr, 10);  strcat(result, substr);   strcat(result, ".");
  itoa(HARD.ip[2], substr, 10);  strcat(result, substr);   strcat(result, ".");
  itoa(HARD.ip[3], substr, 10);  strcat(result, substr);   strcat(result, "-");

  itoa(HARD.myDns[0], substr, 10);  strcat(result, substr);   strcat(result, ".");
  itoa(HARD.myDns[1], substr, 10);  strcat(result, substr);   strcat(result, ".");
  itoa(HARD.myDns[2], substr, 10);  strcat(result, substr);   strcat(result, ".");
  itoa(HARD.myDns[3], substr, 10);  strcat(result, substr);   strcat(result, "-");
  
  itoa(HARD.max_position, substr, 10);  strcat(result, substr);   strcat(result, "-");
  itoa(HARD.min_position, substr, 10);  strcat(result, substr);   strcat(result, "-");
  itoa(HARD.delta, substr, 10);  strcat(result, substr);   strcat(result, "-0");

  send(200, textplain, result);
  // "192.168.0.177-192.168.0.1-1024-250-3-0"
}


//-----------------------------------------------------------
void server_struct::set_setup_on_SD(){
  
  char subtext[16] = "";

  strcpy(subtext, arg("ip").c_str());  str_point_to_ip(HARD.ip, 4, subtext);
  strcpy(subtext, arg("dns").c_str());  str_point_to_ip(HARD.myDns, 4, subtext);
  strcpy(subtext, arg("pmax").c_str());  HARD.max_position = atoi(subtext);
  strcpy(subtext, arg("pmin").c_str());  HARD.min_position = atoi(subtext);
  strcpy(subtext, arg("del").c_str());  HARD.delta = atoi(subtext);
  
  HARD.mega_sets_save();
  returnOK();
}


//-----------------------------------------------------------
void str_point_to_ip(IPAddress &target, byte len, char* text){

  char *point = strstr(text,".");
  byte i = 0;
  while(point != NULL){
    char substr[4] = "";
    strncpy(substr, text, point - text);
    target[i] = (byte)atoi(substr);
    i++;
    if(i == len){return;}
    strcpy(text,point + 1);
    point = strstr(text,".");
  }
  target[i] = (byte)atoi(text);
}

//-----------------------------------------------------------
 void open_handle(){   
    byte request = (byte)atoi(HTTP.arg("h").c_str());

    HARD.con_position = HARD.value_on_potentiometer();
  
    char responce[4] = "";
    switch(request){
      case 0: strcpy(responce, "wc1"); break;
      case 25: strcpy(responce, "wc2"); break;
      case 50: strcpy(responce, "wc3"); break;
      case 75: strcpy(responce, "wc4"); break;
      case 100: strcpy(responce, "wc5"); break;
    }
   HTTP.send(200, HTTP.textplain, responce);
 }


//-----------------------------------------------------------
//--------------------- выполняет запросы -------------------
void server_struct::reqst_run(){
  if((String(url_address) == "/list")&(method == HTTP_GET)){     printDirectory(); }else
  if((String(url_address) == "/edit")&(method == HTTP_DELETE)){  handleDelete();   }else
  if((String(url_address) == "/edit")&(method == HTTP_PUT)){     handleCreate();   }else
  if((String(url_address) == "/gettabl")&(method == HTTP_GET)){  get_table_from_SD();   }else
  if((String(url_address) == "/settabl")&(method == HTTP_GET)){   set_table_on_SD();   }else
  if((String(url_address) == "/getdata")&(method == HTTP_GET)){   get_setup_from_SD();   }else
  if((String(url_address) == "/setdata")&(method == HTTP_GET)){   set_setup_on_SD();   }else
  if((String(url_address) == "/open")&(method == HTTP_GET)){   open_handle();   }else

  { handleNotFound(); }

}
