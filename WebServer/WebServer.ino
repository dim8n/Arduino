#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <DS1302.h>

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Enter a MAC address and IP address for your controller below.
byte ip[] = { 192, 168, 137, 177 };
EthernetServer server(80); // server port at 8345

unsigned int localPort = 8888;       // local port to listen for UDP packets
char timeServer[] = "www.belgim.by"; // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
EthernetUDP Udp; // A UDP instance to let us send and receive packets over UDP

int timeOffset = 3; // GMT +3 hours

dht DHT; // Object of temp and humidity sensor
//DHT dht;
#define DHT11_PIN 5  // Pin 5 used for temp and humidity sensor

DS1302 RTC(8, 7, 6);

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

String str = "";

void setup() {
  Serial.begin(9600);   // Open serial communications and wait for port to open:

  lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines, turn on backlight
  for(int i = 0; i< 3; i++) {lcd.backlight(); delay(250); lcd.noBacklight(); delay(250); } lcd.backlight(); // Мигаем дисплеем 3 раза
  
  lcd.setCursor(0,0); lcd.print("Getting IP from DHCP"); Serial.println("Getting IP from DHCP");
  Ethernet.begin(mac); // Получение IP по DHCP
  lcd.setCursor(0,1); lcd.print("IP: "); lcd.print(Ethernet.localIP()); Serial.print("IP: "); Serial.println(Ethernet.localIP());
  server.begin();
  delay(2000);
  
  Udp.begin(localPort);
  if(Ethernet.localIP()!=0) {GetInternetTime(); delay(5000);}
  lcd.clear();
}


void loop() {
  lcd.setCursor(0,0);
  //Serial.println(RTC.getTimeStr());
  str = RTC.getTimeStr();
  if(str=="00:00:00") {lcd.clear(); lcd.setCursor(0,0); } //  Полное обновление дисплея когда спеняются сутки
  lcd.print(RTC.getTimeStr()); lcd.print("  "); lcd.print(RTC.getDateStr());
  lcd.setCursor(0,1); lcd.print("      "); lcd.print(RTC.getDOWStr(FORMAT_LONG));
  
  int chk = DHT.read11(DHT11_PIN); lcd.setCursor(0,2); lcd.print("T:"); lcd.print(DHT.temperature); lcd.print("C H:"); lcd.print(DHT.humidity); lcd.print("%");
  lcd.setCursor(0,3); lcd.print("IP: "); lcd.print(Ethernet.localIP());
  delay(100);
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          // output the value of each analog input pin

          client.print(RTC.getTimeStr()); client.print("  "); client.print(RTC.getDateStr()); client.print("   "); client.println(RTC.getDOWStr(FORMAT_LONG));
          client.println("<br><br>");
          client.println("<table border=1>");
          client.println("<tr>");
          client.println("<td width=70>Temp</td><td width=70>Hum</td>");
          client.println("</tr>");
          client.println("<tr>");
          //int chk = DHT.read11(DHT11_PIN);
          //if (chk == DHTLIB_OK){
            client.print("<td>"); client.print(DHT.temperature); client.println(" C</td>");
            client.print("<td>"); client.print(DHT.humidity); client.println(" %</td>");
          //}
          client.println("</tr>");
          client.println("</table>");
          client.println("<br /><br />");
          
          client.println("<table border=1>"); // Print the analog inputs state
          client.println("<tr>");
          for (int analogChannel=0; analogChannel<4; analogChannel++){
            client.print("<td width=50>"); client.print("A"); client.print(analogChannel); client.println("</td>"); }
          client.println("</tr>");

          client.println("<tr>");
          for (int analogChannel = 0; analogChannel < 4; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("<td>"); client.print(sensorReading); client.println("</td>");
          }
          client.println("</tr>");
          
          client.println("</table>");

          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

// send an NTP request to the time server at the given address
void sendNTPpacket(char* address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void GetInternetTime() {
 sendNTPpacket(timeServer); // send an NTP packet to a time server
 Serial.print("Getting time from "); Serial.println(timeServer);
 lcd.clear();
 lcd.setCursor(0,0); lcd.print("Getting time from"); lcd.setCursor(0,1); lcd.print(timeServer);
 delay(1000);
 lcd.setCursor(0,2); lcd.print("Result:"); Serial.println("Result: ");

  if (Udp.parsePacket()) {
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = "); // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;  // subtract seventy years:
    Serial.println(epoch);  // print Unix time:

    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10) { // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ((epoch % 60) < 10) { // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');    }
    Serial.println(epoch % 60); // print the second

    RTC.setTime((epoch  % 86400L) / 3600 + timeOffset, (epoch  % 3600) / 60, (epoch % 60));  //  Setting time from time server
    lcd.setCursor(0,3); lcd.print((epoch  % 86400L) / 3600 + timeOffset); lcd.print(":"); lcd.print((epoch  % 3600) / 60); lcd.print(":"); lcd.print((epoch % 60));
  } else { lcd.setCursor(0,3); lcd.print("Can't get time"); Serial.println("Can't get time");}
  RTC.setDate(2,18,2018);
}
