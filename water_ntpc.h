#include <NTPClient.h>
#include <WiFiUdp.h> //handles sending and receiving of UDP packages

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800,60000);
String arr_days[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
String date_time;
