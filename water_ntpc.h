#include <NTPClient.h>
#include <WiFiUdp.h> //handles sending and receiving of UDP packages

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800,60000);
String arr_days[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
String date_time;


//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

String add_zero(String str) {
  if (str.length()<2) {str="0"+str;};
  return str;
}

String getFormattedDateTime(time_t epochTime) {
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  String monthDay = add_zero(String(ptm->tm_mday));  //день месяца;
  String currentMonth = add_zero(String(ptm->tm_mon+1));
  String currentYear = String(ptm->tm_year+1900);
  String currentHour = add_zero(String(ptm->tm_hour));
  String currentMinute = add_zero(String(ptm->tm_min));
  String currentSecond = add_zero(String(ptm->tm_sec));
  String currentDate = currentYear + "-" + currentMonth + "-" + monthDay + " "+ currentHour+ ":" +currentMinute+ ":"+ currentSecond;
  return currentDate;
}
