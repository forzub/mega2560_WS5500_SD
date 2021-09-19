#include "http.h"

//-----------------------------------------------------------
//--------------------- слушает порт ------------------------
void server_struct::listener(){
  
   client = server.available();
   if(client){
      HARD.Console(F("\n\nNew Client.\n"));
      boolean read_title = true;
      http_wachdog_timer = millis();
      
      
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

                    Serial.print("prn> "); Serial.print(str_for_search);Serial.println("<");

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
                      content_length = atoi(str_for_search);  
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
            if( (millis() - http_wachdog_timer) > HTTP_WACHDOG_TIME ){HARD.Consoleln(F("http_wachdog_timer")); break;}
            
            //-------------- запрос PUT DELETE ------------------------------
            if((!read_title) & (content_length > 0) & (method != HTTP_POST)){
                if(!is_content_count){
                  content_count = http_buf.len;
                  is_content_count = true;
                }

                if((content_count) >= content_length){
                  char str_for_search[255] = "";
                  HARD.Consoleln(F("r-body:"));
                  HARD.Consoleln((char*)http_buf.buf);
                  
                  char *point = strstr((char *)http_buf.buf,"\r\n");
                  while(point != NULL){
                    clear_str((char *)str_for_search, (int)sizeof(str_for_search));
                    strncpy(str_for_search, (char *)http_buf.buf, point - (char *)http_buf.buf);
                    bytecpy(http_buf.buf, &http_buf.len, (byte *)point + strlen("\r\n"), http_buf.len - (point - (char*)http_buf.buf + strlen("\r\n") ) );
                    point = strstr((char *)http_buf.buf,"\r\n");
                    if(strlen(str_for_search) < 2){
                      break;
                      }
                  }
                  strncat(filename,(char *)http_buf.buf,point - (char *)http_buf.buf);
                  break;
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
      HARD.Consoleln(F("client disconnected"));
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
  HARD.Console(F("last len> ")); HARD.Consoleln(String(http_buf.len));
  HARD.Console(F("method> ")); HARD.Consoleln(method_tx[method]);
  HARD.Console(F("url_address> "));HARD.Consoleln(url_address);
  HARD.Console(F("params> "));HARD.Consoleln(params);
  HARD.Console(F("boundary> "));HARD.Consoleln(boundary);
  HARD.Console(F("filename> "));HARD.Consoleln(filename);
  HARD.Console(F("content count> ")); HARD.Consoleln(String(content_count));
  HARD.Console(F("content length> ")); HARD.Consoleln(String(content_length));
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
    send(404, "text/plain", message.c_str() );
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
  if (hasArg((char*)F("download"))) dataType = F("application/octet-stream");
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
  send(500, "text/plain", message);
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
  send(200, "text/plain", "");
}

//-----------------------------------------------------------
//--------------------- выполняет запросы -------------------
void server_struct::reqst_run(){
  if((String(url_address) == "/list")&(method == HTTP_GET)){     printDirectory(); }else
  if((String(url_address) == "/edit")&(method == HTTP_DELETE)){  handleDelete();   }else
  if((String(url_address) == "/edit")&(method == HTTP_PUT)){     handleCreate();   }else

  { handleNotFound(); }

}
