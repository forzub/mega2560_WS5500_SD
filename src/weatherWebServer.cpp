#include "weatherWebServer.h"

bool isWeather = false;
bool isWeaterRequest = false;
uint32_t weather_watchdog = 0;
EthernetClient weather;

void weather_send_request(EthernetClient &weather){
    
    if (weather.connect(weather_url,80)){
        #ifdef DEBUG

            Serial.print(F("\nweater: connected to "));
            Serial.println(weather.remoteIP());

        #endif
        weather.println(weather_params);
        weather.println("Host: api.openweathermap.org");
        weather.println("Connection: close");
        weather.println();

        weather_watchdog = millis();
        isWeather = true;

  } else {
        #ifdef DEBUG
            Serial.println("weater: connection failed");
        #endif
        isWeather = false;
  }
    
    
}

void weather_handler(EthernetClient &weather){

    if (weather.available()) {
        Serial.println(F("weater: read request"));
        while (weather.connected()){
            if (weather.available()){
                int len = weather.available();
                byte buffer[80];
                if (len > 80) len = 80;
                weather.read(buffer, len);

                #ifdef DEBUG
                    Serial.write(buffer, len); // show in the serial monitor (slows some boards)
                #endif

            }
        }
        isWeather = false;
        weather.stop();
    }
}