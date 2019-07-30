/*------------------------------------------------*/
// подключение библиотек для работы с OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
/*------------------------------------------------*/
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET); // создаём объект "дисплей"
/*------------------------------------------------*/
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // инициализация дисплея по интерфейсу I2C, адрес 0x3C
  display.clearDisplay(); // очистка дисплея
  display.setTextSize(1); // установка размера шрифта
  display.setTextColor(WHITE); // установка цвета текста
  display.setCursor(0, 0); // установка курсора в позицию X = 0; Y = 0
  display.print ("Hello, world!"); // записываем в буфер дисплея нашу фразу
  display.display(); // и её выводим на экран
}
/*------------------------------------------------*/
void loop() {
}
/*------------------------------------------------*/
