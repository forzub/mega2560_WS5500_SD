#include "console.h"




//---------------------------------------------------------------------
void Console(const char ss){
    Serial.print(ss);
}

//---------------------------------------------------------------------
void Console(String ss){
    Serial.print(ss);
}
//---------------------------------------------------------------------
void Consoleln(const char ss){
    Serial.println(ss);
}

//---------------------------------------------------------------------
void Consoleln(String ss){
    Serial.println(ss);
}

