#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET LED_BUILTIN    //4  Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const char *ssid     = "HGTP";
const char *password = "45674321";

const long utcOffsetInSeconds = 3*3600;
#define UPDATE_CYCLE        (1 * 1000)

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
ESP8266WebServer server(80);

void handleHTTPRequest() {
  Serial.println("");
  Serial.println("HTTP Request");

  String s;

  s = "<!DOCTYPE HTML>\r\n<html>Hello from ";
  s += WiFi.hostname() + " at " + WiFi.localIP().toString();
  // Simple addition of the current time
  s += "\r\n<br>Current time is: ";
  s += timeClient.getHours(); s += ":";
  s +=timeClient.getMinutes(); s +=":";
  s += timeClient.getSeconds();
  s += "</html>\r\n\r\n";
  Serial.println("Sending 200");
  server.send(200, "text/html", s);
}

void testdrawstyles(void) {
  display.clearDisplay();
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  
  display.setTextSize(1);
  display.println(daysOfTheWeek[timeClient.getDay()]);
  display.println();
  display.setTextSize(2);
  if(timeClient.getHours()<10) display.print("0"); display.print(timeClient.getHours()); display.print(":");
  if(timeClient.getMinutes()<10) display.print("0"); display.print(timeClient.getMinutes()); display.print(":");
  if(timeClient.getSeconds()<10) display.print("0"); display.println(timeClient.getSeconds());

  display.setTextSize(1); display.println(); display.println(); display.println(WiFi.localIP());
  display.setTextSize(1); display.print("wifi: "); display.println(ssid);
  
  display.display();
}

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay (500); Serial.print (".");
  }

  timeClient.begin();

  // Setup HTTP server
  server.on("/", handleHTTPRequest);
  server.begin();
  Serial.println("\nHTTP server started");

  Wire.begin(0,2);  //установка соответствия контактов дисплея SDA->D3, SCL->D4
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  testdrawstyles();    // Draw 'stylized' characters
}

void loop() {
  // Check if a request has come in
  server.handleClient();

  timeClient.update();
  testdrawstyles();    // Draw 'stylized' characters

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  //Serial.println(timeClient.getFormattedTime());

  delay(1000);
}
