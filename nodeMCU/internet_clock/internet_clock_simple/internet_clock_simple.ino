#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET LED_BUILTIN    //4  Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//const char *ssid     = "HGTP";
//const char *password = "45674321";

const char *ssid     = "NG-MT";
const char *password = "7680050813";

const long utcOffsetInSeconds = 3*3600;
#define UPDATE_CYCLE        (1 * 1000)

//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
int days, months, years;
String formDate;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
ESP8266WebServer server(80);

void handleHTTPRequest() {
  Serial.println("");
  Serial.println("HTTP Request");

  String s;
  s = "<!DOCTYPE HTML>\r\n";
  s += "<head>\n\t<title>Simple Internet clock</title>\n\t<meta http-equiv='refresh' content='5' />\n</head>\n";
  s += "<html>\n\tHello from ";
  s += WiFi.hostname() + " at " + WiFi.localIP().toString() + "("+ ssid +")";
  // Simple addition of the current time
  s += "<br>\r\n\tCurrent date/time is: ";
  s += daysOfTheWeek[timeClient.getDay()]; s += " ";
  s += timeClient.getFormattedTime(); s += " ";
  s += formDate;
  s += "\n</html>\r\n\r\n";
  Serial.println("Sending 200");
  server.send(200, "text/html", s);
}

void display_on_display(void) {
  display.clearDisplay();
  display.setTextColor(WHITE);        // Draw white text
  
  display.setCursor(15,0); display.setTextSize(1); display.print(daysOfTheWeek[timeClient.getDay()]); display.print(" \/ "); display.println(formDate);     // displaying day of week and date
  display.setCursor(16,14); display.setTextSize(2,5); display.print(timeClient.getFormattedTime());     // displaying time
  //display.setCursor(0,14); display.setTextSize(3,5); display.print(timeClient.getFormattedTime());     // displaying time
  //display.setCursor(32,30); display.setTextSize(1);       // displaying date

  display.setCursor(0,56); display.setTextSize(1); display.print(WiFi.localIP());  // displaying wi-fi info
  display.setTextSize(1); display.print(" ("); display.print(ssid); display.print(")");
  
  display.display();    // draw display buffer
}

void convertdate(void){
  time_t t = timeClient.getEpochTime();
  tm* dt = gmtime(&t);
  dt->tm_isdst = 0;
  years = dt->tm_year + 1900;
  months = dt->tm_mon + 1;
  days = dt->tm_mday;

  formDate = "";
  if(days<10) formDate = "0"; formDate += String(days); formDate += "-";
  if(months<10) formDate += "0"; formDate += String(months); formDate += "-";
  formDate += String(years);
}

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay (500); Serial.print (".");
  }

  timeClient.begin();
  convertdate();

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
  display_on_display();    // Draw 'stylized' characters
}

void loop() {
  // Check if a request has come in
  server.handleClient();

  timeClient.update();
  convertdate();
  display_on_display();    // Draw 'stylized' characters

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getFormattedTime());
  Serial.print(", ");
  Serial.println(formDate);

  delay(1000);
}
