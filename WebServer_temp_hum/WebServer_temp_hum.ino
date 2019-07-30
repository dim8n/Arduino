/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 */

#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>

dht DHT;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

#define DHT11_PIN 5

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  lcd.begin(20,4);
  Serial.begin(9600);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    
    int chk = DHT.read11(DHT11_PIN);
    switch (chk) {
      case DHTLIB_OK:  Serial.print("DTH OK,\n"); break;
      case DHTLIB_ERROR_CHECKSUM: Serial.print("Checksum error,\n"); break;
      case DHTLIB_ERROR_TIMEOUT: Serial.print("Time out error,\n");  break;
      default: Serial.print("Unknown error,\n"); break; }
    // DISPLAY DATA
  
    lcd.setCursor(0,0); lcd.print(Ethernet.localIP());
    lcd.setCursor(0,1); lcd.print("Vlaghnost:   "); lcd.print(DHT.humidity);
    lcd.setCursor(0,2); lcd.print("Temperatura: "); lcd.print(DHT.temperature);

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
          client.println("Refresh: 10");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("Vlaghnost: "); client.println(DHT.humidity); client.println("<br>");
          Serial.print("Vlaghnost: "); Serial.println(DHT.humidity);
          client.println("Temperatura: : "); client.println(DHT.temperature); client.println("<br>");
          Serial.print("Temperatura: "); Serial.println(DHT.temperature);
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

