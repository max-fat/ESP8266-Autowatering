#include <WiFiManager.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <EEPROM.h>
#include "water_ntpc.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "GyverTimer.h"

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

const int PIN_W = 0;  // 13 для ESP8266, 2 для ESP01 и 0 для ESPреле
bool display_en = true;

Time time_clock(0, 0, 0);
byte d_watering;
byte h_watering;
byte m_watering;
word w_watering;
byte d_clock;
GTimer timerClock(MS, 60000);
GTimer timerPoliv(MS);

bool watering;
bool ap_connect;



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
  String s = webpage;
  server.send(200, "text/html", s);
}

void save_time() {
  d_watering = server.arg("days").toInt();
  h_watering = server.arg("hours").toInt(); 
  m_watering = server.arg("minutes").toInt();
  w_watering = server.arg("seconds").toInt();
  
  Serial.print("save settings - days: ");
  Serial.print(d_watering);
  Serial.print(", hours: ");
  Serial.print(h_watering);
  Serial.print(", minutes: ");
  Serial.print(m_watering);
  Serial.print(", s: ");
  Serial.println(w_watering);
  
  EEPROM.write(0, d_watering);  // Запись данных
  EEPROM.write(1, h_watering);  // Запись данных
  EEPROM.write(2, m_watering);  // Запись данных
  EEPROM.put(3,w_watering);
  EEPROM.commit();   // Сохранение изменений
  server.send(200, "text/html", "OK");
}

void get_time() {
  String message = String(d_watering)+",";
  if (h_watering<10) {message=message+"0";};
  message=message+String(h_watering)+":";
  if (m_watering<10) {message=message+"0";};
  message=message+String(m_watering)+","+String(w_watering);
  Serial.println("Message: "+message);
  server.send(200, "text/plane", message);
}

void put_water() {
  if (!watering) {
    watering=true;
    digitalWrite(PIN_W, HIGH);
    Serial.println("полив начат");
    timerPoliv.setTimeout(w_watering*1000);
  }
}

void setup(void) {
  Serial.begin(115200);

  pinMode(PIN_W, OUTPUT);
  digitalWrite(PIN_W, LOW);
  
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
        display.print("Time: ");
        display.println(timeClient.getFormattedTime());
        time_clock=timeClient.getHours() *60 + timeClient.getMinutes();
        display.display();
        delay(200); 

        server.on("/", handleRoot);
        server.on("/read", get_time);
        server.on("/save_time", save_time);
        server.on("/put_water", put_water);
        server.begin();
      
        EEPROM.begin(5);  // Инициализация EEPROM с размером 512 байт
        d_watering = EEPROM.read(0);
        h_watering = EEPROM.read(1); 
        m_watering = EEPROM.read(2);
        EEPROM.get(3,w_watering);  
        Serial.print("days: ");
        Serial.print(d_watering);
        Serial.print(", hours: ");
        Serial.print(h_watering);
        Serial.print(", minutes: ");
        Serial.print(m_watering);
        Serial.print(", s: ");
        Serial.println(w_watering);
    }
}

void loop(void) {
  if (ap_connect) {
    if (timerPoliv.isReady()) {digitalWrite(PIN_W, LOW); Serial.println("полив завершен");}; // 
    if (timerClock.isReady()) {time_clock+=1; Serial.println(time_clock.TotalMinutes());};   // тикаем каждую минуту
     
    if (time_clock.TotalMinutes() == (h_watering*60+m_watering)) {
      if (!watering & (d_clock==d_watering)) {
        watering=true;
        digitalWrite(PIN_W, HIGH);
        Serial.println("полив начат");
        timerPoliv.setTimeout(w_watering*1000);
      }
    }
    else watering=false;
  };
  
  server.handleClient();
}
