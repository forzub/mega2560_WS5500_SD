#include "http.h"

//-----------------------------------------------------------
//--------------------- слушает порт ------------------------
void server_struct::listener(){
  
  client = server.available();
  if(client){

    while(client.connected()){
        int len = client.available();
        if(len > 0){
            byte buff[80];
            if (len > 80) len = 80;
            client.read(buff, len);

            Serial.write(buff, len);

          }else{
              Serial.println("");
          }
    }

    //reqstrun();
    client.stop();
    clearvars();

    HARD.Console(F("Client disconnected."));
    HARD.Console(F("Connection Time: "));
  }
}



//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//--------------------- очистка переменных ------------------
void server_struct::clearvars(){

  strcpy(servpath,"");
  strcpy(filename,"");
  strcpy(params,"");
  strcpy(parseserver,"");
  strcpy(boundary,"");

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
    message += servpath;
    message += "\nArguments: ";
    message += params;
    send(404, "text/plain", message.c_str() );
}
//-----------------------------------------------------------
//--------------------- отправка файла с SD карты -----------
bool server_struct::loadFromSdCard(){
  String path = String(servpath);
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
void server_struct::reqstrun(){
  if((String(servpath) == "/list")&(method == HTTP_GET)){     printDirectory(); }else
  if((String(servpath) == "/edit")&(method == HTTP_DELETE)){  handleDelete();   }else
  if((String(servpath) == "/edit")&(method == HTTP_PUT)){     handleCreate();   }else

  { handleNotFound(); }

}
