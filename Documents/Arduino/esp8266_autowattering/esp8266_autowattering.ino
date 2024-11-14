 #include <WiFiManager.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
  
#include <FS.h>

#include <EEPROM.h>
#include "water_ntpc.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "GyverTimer.h"

#include <FastBot.h>

#include "index.h"
#include "time.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ESP8266WebServer server(80);

int PIN_W = 0;  // 13 для ESP8266, 2 для ESP01 и 0 для ESPреле
bool display_en = true;
bool alarm_level = true;

Time time_clock(0, 0, 0);

struct {
byte d_watering;
byte h_watering;
byte m_watering;
word w_watering;
byte d_clock;
byte pin_watering;
bool enable_watering;
} timer_watering[4];

GTimer timerClock(MS, 60000);
GTimer timerPoliv(MS);

byte lock_watering=0;
bool ap_connect;

bool NC = false;

FastBot bot("5751256260:AAH619IJAMmp-zytgAGAYvieBtXVL-5Vo0I");

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("FIELD CONNECT");
  display.println("Start AP: AutoConnect");
  display.println("Password: 88889999");
  display.println("IP: 192.168.4.1");
  display.display(); // actually display all of the above
  
  delay(200); 
}

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void log_file() {
  File f=SPIFFS.open("/file.txt","r");
  server.send(200, "text/plain",f.readStringUntil('EOF'));
  f.close();
}

void remove_file() {
SPIFFS.remove("/file.txt");
server.send(200, "text/plain", "File delete done");
}


void enable_invert() {
  byte id = server.arg("invert").toInt();
  if (id == 0) {NC=false;Serial.println("save NC - false");}
    else {NC=true;Serial.println("save NC - true");};
  EEPROM.put(0, NC);
  EEPROM.commit();   // Сохранение изменений
}

void system_status() {
  String message = "Settings:\n";
  for (byte i=0; i<4; i++) {
    message += "Timer #"+String(i);
    message += " - d_watering="+String(timer_watering[i].d_watering);
    message += ", time="+String(timer_watering[i].h_watering)+":"+String(timer_watering[i].m_watering);
    message += ", w_watering(sec)="+String(timer_watering[i].w_watering);
    message += ", d_clock(days)="+String(timer_watering[i].d_clock);
    message += ", pin_watering="+String(timer_watering[i].pin_watering);
    message += ", enable_watering=";
    if (timer_watering[i].enable_watering==1){message += "true\n";} else {message += "false\n";};
  }
  message += "time_clock: days=" + String(time_clock.GetDays());
  message += ", time=" + String(time_clock.GetHours()) + ":" + String(time_clock.GetMinutes());
  message += "\ntimeClient=" +  getFormattedDateTime(timeClient.getEpochTime());

  server.send(200, "text/plain", message);
}

void save_time() {
  byte id = server.arg("id").toInt();

  timer_watering[id].enable_watering = server.arg("enable").toInt();
  timer_watering[id].d_watering = server.arg("days").toInt();
  timer_watering[id].h_watering = server.arg("hours").toInt(); 
  timer_watering[id].m_watering = server.arg("minutes").toInt();
  timer_watering[id].w_watering = server.arg("seconds").toInt();
  timer_watering[id].pin_watering = server.arg("pin").toInt();
  
  Serial.print("save settings[");
  Serial.print(id);
  Serial.print("]");
  Serial.print(timer_watering[id].enable_watering);
  Serial.print(" - days: ");
  Serial.print(timer_watering[id].d_watering);
  Serial.print(", hours: ");
  Serial.print(timer_watering[id].h_watering);
  Serial.print(", minutes: ");
  Serial.print(timer_watering[id].m_watering);
  Serial.print(", s: ");
  Serial.print(timer_watering[id].w_watering);
  Serial.print(", pin: ");
  Serial.println(timer_watering[id].pin_watering);

  EEPROM.put(1, timer_watering);
  EEPROM.commit();   // Сохранение изменений
  server.send(200, "text/html", "OK");
}

void get_time() {
  String message ="";
  for (byte i=0; i<4; i++) {
    message+=String(timer_watering[i].d_watering)+",";
    if (timer_watering[i].h_watering<10) {message+="0";};
    message+=String(timer_watering[i].h_watering)+":";
    if (timer_watering[i].m_watering<10) {message+="0";};
    message+=String(timer_watering[i].m_watering)+","+String(timer_watering[i].w_watering)+","+String(timer_watering[i].enable_watering)+","+String(timer_watering[i].pin_watering)+";";
  };
  Serial.println("Message: "+message);
  server.send(200, "text/plane", message);
}

void put_water() {
  if (lock_watering==0) {
    lock_watering=1;
    byte id = server.arg("id").toInt();
    PIN_W = server.arg("pin").toInt();
    digitalWrite(PIN_W, !NC);
    File file=SPIFFS.open("/file.txt","a");
    file.print(getFormattedDateTime(timeClient.getEpochTime()));
    file.println(" - Start watering");
    file.close();
    Serial.println("полив начат");
    timerPoliv.setTimeout(timer_watering[id].w_watering*1000);
  }
}

void start_water() {
  if (lock_watering==0) {
    lock_watering=1;
    PIN_W = server.arg("pin").toInt();
    Serial.print("Ручной полив без таймера начат, пин=");
    Serial.println(PIN_W);
    File file=SPIFFS.open("/file.txt","a");
    file.print(getFormattedDateTime(timeClient.getEpochTime()));
    file.print(" - Start handmode watering without timer, pin=");
    file.println(PIN_W);
    file.close();
    digitalWrite(PIN_W, !NC);
  }
}

void stop_water() {
  if (lock_watering==1) {
    digitalWrite(PIN_W, NC);
    Serial.print("Полив окончен, пин=");
    Serial.println(PIN_W);
    File file=SPIFFS.open("/file.txt","a");
    file.print(getFormattedDateTime(timeClient.getEpochTime()));
    file.print(" - Stop watering, pin=");
    file.println(PIN_W);
    file.close();
    lock_watering=0;
  }
}

void setup(void) {
  
  EEPROM.begin(sizeof(timer_watering)+1);  // Инициализация EEPROM с размером 
  EEPROM.get(0,NC);
  EEPROM.get(1,timer_watering);
  
  
  for (byte i=0; i<4; i++) {
    pinMode(timer_watering[i].pin_watering, OUTPUT);
    digitalWrite(timer_watering[i].pin_watering, NC);
  }

  pinMode(14, INPUT);
  digitalWrite(14, HIGH);
  
  Serial.begin(115200);
  

  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    display_en = false;
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.display(); // actually display all of the above
  
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Try connecting... ");
  display.print("SSID: ");
  display.println(wm.getWiFiSSID(true));
  display.display(); // actually display all of the above 
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  wm.setAPCallback(configModeCallback);
  res = wm.autoConnect("AutoConnectAP","88889999"); // password protected ap
  delay(200);
  ap_connect=false;
    if(!res) {
        Serial.println("Failed to connect");
    }
    else {
        //если удалось подключиться к точке доступа
        ap_connect=true;
        //Serial.println(WiFiManager->getConfigPortalSSID());
        
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.println("connect success   ");
        display.print("SSID: ");
        display.println(wm.getWiFiSSID(true));    
        display.print("ip: ");
        display.println(WiFi.localIP());
        display.display(); // actually display all of the above
        delay(200);
        
        timeClient.begin();
        server.begin();
        timeClient.update();
        
        Serial.print("Time: ");
        Serial.println(getFormattedDateTime(timeClient.getEpochTime()));
        display.print("Time: ");
        display.println(timeClient.getFormattedTime());
        time_clock=timeClient.getHours() *60 + timeClient.getMinutes();
        display.display();
        delay(200);
         
        bool success = SPIFFS.begin();
        Serial.println("");
        if (success) {
            Serial.println("File system mounted with success");
        } else {
            Serial.println("Error mounting the file system");
            return;
        }
        File file = SPIFFS.open("/file.txt", "a");
        if (!file) {
          Serial.println("Error opening file for writing");
          return;
        }
        
        file.print("\nStart server");
        file.print(" - Time server: ");
        file.println(getFormattedDateTime(timeClient.getEpochTime()));
        file.close();
  
        server.on("/", handleRoot);
        server.on("/read", get_time);
        server.on("/save_time", save_time);
        server.on("/put_water", put_water);
        server.on("/start", start_water);
        server.on("/stop", stop_water);
        server.on("/enable_invert", enable_invert);
        server.on("/log.txt", log_file);
        server.on("/remove_file", remove_file);
        server.on("/system_status", system_status);
        server.begin();

        for(byte id=0; id<4; id++) // цикл для переменной i от 1 до k с шагом 1
        {
          timer_watering[id].d_clock=0;
          Serial.print("save settings[");
          Serial.print(id);
          Serial.print("]");
          Serial.print(timer_watering[id].enable_watering);
          Serial.print(" - days: ");
          Serial.print(timer_watering[id].d_watering);
          Serial.print(", hours: ");
          Serial.print(timer_watering[id].h_watering);
          Serial.print(", minutes: ");
          Serial.print(timer_watering[id].m_watering);
          Serial.print(", s: ");
          Serial.print(timer_watering[id].w_watering);
          Serial.print(", pin: ");
          Serial.println(timer_watering[id].pin_watering);
          
        }
        if (NC) Serial.println("Inverse - TRUE"); else Serial.println("Inverse - FALSE");
    }
}

void loop(void) {
  if (ap_connect) {

    if (digitalRead(14))  {
      //Serial.print("малый уровень воды");
      if (!alarm_level) {bot.sendMessage("low water","1571134916");Serial.println("Send message to TG");};
      alarm_level = true;
    } else alarm_level = false;
    
    if (timerPoliv.isReady()) {
        digitalWrite(PIN_W, NC);
        Serial.println("полив завершен");
        lock_watering=0;
        File file=SPIFFS.open("/file.txt","a");
        file.print(getFormattedDateTime(timeClient.getEpochTime()));
        file.println(" - Stop watering");
        file.close();
        }; //
         
    if (timerClock.isReady()) {
        time_clock+=1; //к часам добавляем минуту
        
        Serial.println(time_clock.TotalMinutes()); // вывод минут дня debug
        
        for (byte i=0; i<4; i++) {
          if ((time_clock.GetHours() == timer_watering[i].h_watering) and (time_clock.GetMinutes() == timer_watering[i].m_watering)){
            timer_watering[i].d_clock+=1; //к таймеру совпавшему добавляем сутки
            Serial.print("Совпало время таймера #");
            Serial.print(i);
            Serial.print(" день ");
            Serial.println(timer_watering[i].d_clock);
            
            File file=SPIFFS.open("/file.txt","a");
            //file.print(getFormattedDateTime(timeClient.getEpochTime()));
            //file.println(" - Совпало время таймера #" + String(i) + " день " + String(timer_watering[i].d_clock));
            //file.close();
            
            if (timer_watering[i].enable_watering==1){
              Serial.println("таймер для полива включен");
              Serial.print("d_clock=");
              Serial.println(timer_watering[i].d_clock);
              Serial.print(", d_watering=");
              Serial.println(timer_watering[i].d_watering);

              File file=SPIFFS.open("/file.txt","a");
              //file.print(getFormattedDateTime(timeClient.getEpochTime()));
              //file.println(" - таймер для полива включен d_clock=" + String(timer_watering[i].d_clock) + ", d_watering=" + String(timer_watering[i].d_watering));
              //file.close();
              
              if (lock_watering==0 and timer_watering[i].w_watering>0 and timer_watering[i].d_clock>=timer_watering[i].d_watering)
              {
                  lock_watering=1;
                  PIN_W=timer_watering[i].pin_watering;
                  timer_watering[i].d_clock=0;
                  pinMode(PIN_W, OUTPUT);
                  digitalWrite(PIN_W, !NC);
                  File file=SPIFFS.open("/file.txt","a");
                  file.print(getFormattedDateTime(timeClient.getEpochTime()));
                  file.print(" - start watering, number timer: ");
                  file.print(i);
                  file.print(", pin: ");
                  file.println(PIN_W);
                  file.close();
                  
                  Serial.print("полив начат, таймер: ");
                  Serial.print(i);
                  Serial.print(", пин: ");
                  Serial.println(PIN_W);
                  timerPoliv.setTimeout(timer_watering[i].w_watering*1000);
            }}
          } 
        }
        
    };   // тикаем каждую минуту
  };
  
  server.handleClient();
}
