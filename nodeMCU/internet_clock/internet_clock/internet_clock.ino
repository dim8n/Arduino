/*
  ESP8266 mDNS responder clock

  This example demonstrates two features of the LEA MDNSResponder:
  1. The host and service domain negotiation process that ensures
     the uniqueness of the finally choosen host and service domain name.
  2. The dynamic MDNS service TXT feature

  A 'clock' service in announced via the MDNS responder and the current
  time is set as a TXT item (eg. 'curtime=Mon Oct 15 19:54:35 2018').
  The time value is updated every second!

  The ESP is initially announced to clients as 'esp8266.local', if this host domain
  is already used in the local network, another host domain is negociated. Keep an
  eye to the serial output to learn the final host domain for the clock service.
  The service itself is is announced as 'host domain'._espclk._tcp.local.
  As the service uses port 80, a very simple HTTP server is installed also to deliver
  a small web page containing a greeting and the current time (not updated).
  The web server code is taken nearly 1:1 from the 'mDNS_Web_Server.ino' example.
  Point your browser to 'host domain'.local to see this web page.

  Instructions:
  - Update WiFi SSID and password as necessary.
  - Flash the sketch to the ESP8266 board
  - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Use a MDNS/Bonjour browser like 'Discovery' to find the clock service in your local
    network and see the current time updates.

*/


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <time.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET LED_BUILTIN    //4  Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


/*
   Include the MDNSResponder (the library needs to be included also)
   As LEA MDNSResponder is experimantal in the ESP8266 environment currently, the
   legacy MDNSResponder is defaulted in th include file.
   There are two ways to access LEA MDNSResponder:
   1. Prepend every declaration and call to global declarations or functions with the namespace, like:
      'LEAmDNS::MDNSResponder::hMDNSService  hMDNSService;'
      This way is used in the example. But be careful, if the namespace declaration is missing
      somewhere, the call might go to the legacy implementation...
   2. Open 'ESP8266mDNS.h' and set LEAmDNS to default.

*/
#include <ESP8266mDNS.h>
#include <PolledTimeout.h>
/*
   Global defines and vars
*/

#define TIMEZONE_OFFSET     3                                   // CET
#define DST_OFFSET          0                                   // CEST
#define UPDATE_CYCLE        (10 * 1000)                          // every second

#define SERVICE_PORT        80                                  // HTTP port

#ifndef STASSID
//#define STASSID "NG-MT"
//#define STAPSK  "7680050813"
#define STASSID "HGTP"
#define STAPSK  "45674321"
#endif

const char*                   ssid                    = STASSID;
const char*                   password                = STAPSK;

char*                         pcHostDomain            = 0;        // Negociated host domain
bool                          bHostDomainConfirmed    = false;    // Flags the confirmation of the host domain
MDNSResponder::hMDNSService   hMDNSService            = 0;        // The handle of the clock service in the MDNS responder

// HTTP server at port 'SERVICE_PORT' will respond to HTTP requests
ESP8266WebServer              server(SERVICE_PORT);

/*
   getTimeString
*/
const char* getTimeString(void) {

  static char   acTimeString[32];
  time_t now = time(nullptr);
  ctime_r(&now, acTimeString);
  size_t    stLength;
  while (((stLength = strlen(acTimeString))) &&
         ('\n' == acTimeString[stLength - 1])) {
    acTimeString[stLength - 1] = 0; // Remove trailing line break...
  }
  return acTimeString;
}


/*
   setClock

   Set time via NTP
*/
void setClock(void) {
  configTime((TIMEZONE_OFFSET * 3600), (DST_OFFSET * 3600), "pool.ntp.org", "time.nist.gov", "time.windows.com");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);   // Secs since 01.01.1970 (when uninitalized starts with (8 * 3600 = 28800)
  while (now < 8 * 3600 * 2) {  // Wait for realistic value
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  Serial.printf("Current time: %s\n", getTimeString());
}


/*
   setStationHostname
*/
bool setStationHostname(const char* p_pcHostname) {

  if (p_pcHostname) {
    WiFi.hostname(p_pcHostname);
    Serial.printf("setDeviceHostname: Station hostname is set to '%s'\n", p_pcHostname);
  }
  return true;
}


/*
   MDNSDynamicServiceTxtCallback

   Add a dynamic MDNS TXT item 'ct' to the clock service.
   The callback function is called every time, the TXT items for the clock service
   are needed.
   This can be triggered by calling MDNS.announce().

*/
void MDNSDynamicServiceTxtCallback(const MDNSResponder::hMDNSService p_hService) {
  Serial.println("MDNSDynamicServiceTxtCallback");

  if (hMDNSService == p_hService) {
    Serial.printf("Updating curtime TXT item to: %s\n", getTimeString());
    MDNS.addDynamicServiceTxt(p_hService, "curtime", getTimeString());
  }
}


/*
   MDNSProbeResultCallback

   Probe result callback for the host domain.
   If the domain is free, the host domain is set and the clock service is
   added.
   If the domain is already used, a new name is created and the probing is
   restarted via p_pMDNSResponder->setHostname().

*/
void hostProbeResult(String p_pcDomainName, bool p_bProbeResult) {

  Serial.println("MDNSProbeResultCallback");
  Serial.printf("MDNSProbeResultCallback: Host domain '%s.local' is %s\n", p_pcDomainName.c_str(), (p_bProbeResult ? "free" : "already USED!"));
  if (true == p_bProbeResult) {
    // Set station hostname
    setStationHostname(pcHostDomain);

    if (!bHostDomainConfirmed) {
      // Hostname free -> setup clock service
      bHostDomainConfirmed = true;

      if (!hMDNSService) {
        // Add a 'clock.tcp' service to port 'SERVICE_PORT', using the host domain as instance domain
        hMDNSService = MDNS.addService(0, "espclk", "tcp", SERVICE_PORT);
        if (hMDNSService) {
          // Add a simple static MDNS service TXT item
          MDNS.addServiceTxt(hMDNSService, "port#", SERVICE_PORT);
          // Set the callback function for dynamic service TXTs
          MDNS.setDynamicServiceTxtCallback(MDNSDynamicServiceTxtCallback);
        }
      }
    }
  } else {
    // Change hostname, use '-' as divider between base name and index
    if (MDNSResponder::indexDomain(pcHostDomain, "-", 0)) {
      MDNS.setHostname(pcHostDomain);
    } else {
      Serial.println("MDNSProbeResultCallback: FAILED to update hostname!");
    }
  }
}


/*
   handleHTTPClient
*/

void handleHTTPRequest() {
  Serial.println("");
  Serial.println("HTTP Request");

  // Get current time
  time_t now = time(nullptr);;
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  String s;

  s = "<!DOCTYPE HTML>\r\n<html>Hello from ";
  s += WiFi.hostname() + " at " + WiFi.localIP().toString();
  // Simple addition of the current time
  s += "\r\nCurrent time is: ";
  s += getTimeString();
  // done :-)
  s += "</html>\r\n\r\n";
  Serial.println("Sending 200");
  server.send(200, "text/html", s);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(ssid);
  display.setTextSize(1);
  display.println(WiFi.localIP());

  //display.setTextSize(2);             // Draw 2X-scale text
  //display.setTextColor(WHITE);
  //display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
}

/*
   setup
*/
void setup(void) {
  Serial.begin(115200);

  // Sync clock
  setClock();

  // Setup MDNS responder
  MDNS.setHostProbeResultCallback(hostProbeResult);
  // Init the (currently empty) host domain string with 'esp8266'
  if ((!MDNSResponder::indexDomain(pcHostDomain, 0, "esp8266")) ||
      (!MDNS.begin(pcHostDomain))) {
    Serial.println("Error setting up MDNS responder!");
    while (1) { // STOP
      delay(1000);
    }
  }
  Serial.println("MDNS responder started");

  // Setup HTTP server
  server.on("/", handleHTTPRequest);
  server.begin();
  Serial.println("HTTP server started");

  Wire.begin(0,2);  //установка соответствия контактов дисплея SDA->D3, SCL->D4
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  testdrawstyles();    // Draw 'stylized' characters

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  
  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

/*
   loop
*/
void loop(void) {

  // Check if a request has come in
  server.handleClient();
  // Allow MDNS processing
  MDNS.update();

  static esp8266::polledTimeout::periodicMs timeout(UPDATE_CYCLE);
  if (timeout.expired()) {

    if (hMDNSService) {
      // Just trigger a new MDNS announcement, this will lead to a call to
      // 'MDNSDynamicServiceTxtCallback', which will update the time TXT item
      MDNS.announce();
    }
  }
}
