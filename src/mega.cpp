#include "mega.h"

//---------------------------------------------------------------------
void mega_dev_all_off(){

  digitalWrite(BUT_DOWN,HIGH);
  digitalWrite(BUT_UP,HIGH);
  digitalWrite(POWER_CONTROL,HIGH);

}

//---------------------------------------------------------------------
void mega_dev_init(){
    
    // analogReference(INTERNAL2V56);
    pinMode(W_POSITION,INPUT);
    pinMode(BUT_SET,INPUT);

    pinMode(BUT_UP,OUTPUT);
    pinMode(BUT_DOWN,OUTPUT);
    pinMode(POWER_CONTROL,OUTPUT);
    pinMode(ERR_SIGNAL,OUTPUT);

    digitalWrite(BUT_UP,HIGH);
    digitalWrite(BUT_DOWN,HIGH);
    digitalWrite(POWER_CONTROL,HIGH);
    digitalWrite(ERR_SIGNAL,HIGH);

}

