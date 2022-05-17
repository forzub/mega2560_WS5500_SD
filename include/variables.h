#ifndef variables_h
#define variables_h

#include <Arduino.h>
#include "hard.h"


extern uint32_t timer1;





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



#endif