#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include "esp_adc_cal.h"
#include "bmp.h"
#include <NTPClient.h>
#include <time.h>
#include <WiFiUdp.h>

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL          4  // Display backlight control pin
#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0

char ssid[5][30] = {"iPhone (Dmitriy)","HGTP","HUAWEI-K05","dd-wrt","NG-MT"};
char password[5][30] = {"12348765","45674321","194419491983","7680050813","7680050813"};
int wifiselect = 0;

const long utcOffsetInSeconds = 3*3600;
char daysOfTheWeek[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
int days, months, years;
String formDate;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

char buff[512];
int vref = 1100;
int btnCick = false;
String voltage = "";

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{   
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void getVoltage()
{
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 1000) {
        timeStamp = millis();
        uint16_t v = analogRead(ADC_PIN);
        float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
        voltage = String(battery_voltage) + "V";
    }
}

void convertdate(void){
  time_t t = timeClient.getEpochTime();
  tm* dt = gmtime(&t);
  dt->tm_isdst = 0;
  years = dt->tm_year + 1900;
  months = dt->tm_mon + 1;
  days = dt->tm_mday;

  formDate = "";
  if(days<10) formDate = "0"; formDate += String(days); formDate += "/";
  if(months<10) formDate += "0"; formDate += String(months); formDate += "/";
  formDate += String(years);
}

void mainScreen()
{
    tft.setTextDatum(MC_DATUM);  // отображение даты
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    getVoltage();
    tft.drawString(voltage, 0, 0);  //отображение напряжения питания

    tft.setTextColor(TFT_GREEN, TFT_BLACK); // отображение дня недели
    tft.drawString(daysOfTheWeek[timeClient.getDay()], tft.width() - 20, 0);
    
    tft.setTextSize(5);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // отображение времени
    tft.drawString(timeClient.getFormattedTime(),  tft.width() / 2, tft.height() / 2 - 20);

    tft.setTextSize(3);
    tft.setTextColor(TFT_RED, TFT_BLACK); // отображение даты
    tft.drawString(formDate,  tft.width() / 2, tft.height() / 2 + 20);

    tft.setTextSize(2);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK); // нижняя строка
    tft.drawString("WiFi is "+WiFi.SSID(), tft.width() / 2, tft.height());
}

void secondScreen()
{
  tft.fillScreen(TFT_BLACK);
}

void button_init()
{
    btn1.setLongClickHandler([](Button2 & b) {
        btnCick = false;
        int r = digitalRead(TFT_BL);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Press again to wake up",  tft.width() / 2, tft.height() / 2 );
        espDelay(6000);
        digitalWrite(TFT_BL, !r);

        tft.writecommand(TFT_DISPOFF);
        tft.writecommand(TFT_SLPIN);
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
        esp_deep_sleep_start();
    });
    btn1.setPressedHandler([](Button2 & b) {
        Serial.println("\nDetect Voltage..");
        tft.fillScreen(TFT_BLACK);
        btnCick = true;
    });

    btn2.setPressedHandler([](Button2 & b) {
        btnCick = false;
        Serial.println("\nbtn press wifi scan");
        tft.fillScreen(TFT_BLACK);
        secondScreen();
    });
}

void button_loop()
{
    btn1.loop();
    btn2.loop();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Start");

    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    //tft.setTextSize(1);

    if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
         pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
         digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }

    tft.setSwapBytes(true);
    button_init();
    tft.pushImage(0, 0,  240, 135, ttgo);
    espDelay(1000);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // строка перед подключением к сети

    getVoltage();
    
    int i = 0;
    //WiFi.begin(ssid[wifiselect], password[wifiselect]);
    //delay (1000);
    //Serial.print (".");
    while (WiFi.status() != WL_CONNECTED)  {
        Serial.println("\nTrying to connect "+String(ssid[wifiselect])+"\n");
        WiFi.begin(ssid[wifiselect], password[wifiselect]);
        tft.fillScreen(TFT_BLACK);
        tft.drawString(ssid[wifiselect],  tft.width() / 2, tft.height() / 2);
        delay (5000); //Serial.print (".");
        i++;
        if (i>1) {i=0; wifiselect++;} if(wifiselect > 5) break;
    } 
    Serial.println("\nConnect OK!!!");
    
    tft.fillScreen(TFT_BLACK);

    timeClient.begin();
    timeClient.update();
    convertdate();

    mainScreen();

    tft.setRotation(3);

    //esp_adc_cal_characteristics_t adc_chars;
    //esp_adc_cal_value_t val_type = esp_adc_cal_characterize((adc_unit_t)ADC_UNIT_1, (adc_atten_t)ADC1_CHANNEL_6, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &adc_chars);
    //Check type of calibration value used to characterize ADC
    //if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    //    Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
    //    vref = adc_chars.vref;
    //} else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
    //    Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
    //} else {
    //    Serial.println("Default Vref: 1100mV");
    //}
}



void loop()
{
    //if (btnCick) {
    //    mainScreen();
    //}
    //button_loop();
    mainScreen();
}
