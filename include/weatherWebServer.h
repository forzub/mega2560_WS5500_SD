#ifndef weatherWebServer_h
#define weatherWebServer_h

#define DEBUG

#include <Arduino.h>
#include "Ethernet.h"

extern uint32_t weather_watchdog;
extern bool isWeather, isWeaterRequest;
extern EthernetClient weather;

// const char url[] = "http://api.openweathermap.org/data/2.5/onecall?lat=50.0&lon=36.25&exclude=current,minutely,daily,alerts&units=metric&appid=577e97e93b979b2a3722ddd04f41716b";
const char weather_url[] = "api.openweathermap.org";
const char weather_params[] = "GET /data/2.5/onecall?lat=50.0&lon=36.25&exclude=current,minutely,daily,alerts&units=metric&appid=577e97e93b979b2a3722ddd04f41716b HTTP/1.1";
void weather_send_request(EthernetClient &weather);
void weather_handler(EthernetClient &weather);

#define WEATHER_WATCHDOG 5000


// ------------------------------------------



#endif