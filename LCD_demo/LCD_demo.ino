#include <LiquidCrystal.h>

/* Константа: высота символа в точках */
enum { SYMBOL_HEIGHT = 8 };

/* Описываем наши собственные символы в виде массивов битовых масок.
 * Каждый символ - это восемь масок по пять бит.
 */
byte arnie[SYMBOL_HEIGHT] =
{
  B10101,
  B11111,
  B10111,
  B11111,
  B11111,
  B00111,
  B11110,
  B00000,
};

byte skull_and_bones[SYMBOL_HEIGHT] =
{
  B01110,
  B10101,
  B11011,
  B01110,
  B00000,
  B10001,
  B01110,
  B10001,
};

byte lightning[SYMBOL_HEIGHT] =
{
  B00010,
  B00100,
  B01000,
  B11111,
  B00010,
  B00100,
  B01000,
  B00000,
};

/* Константы размеров экрана. Нужны для того, чтобы размер экрана
 * менять только в одном месте, а не по всему коду. Везде, где
 * результат вывода на дисплей должен зависеть от размера экрана,
 * следует использовать эти константы.
 */
enum { LCD_WIDTH = 16, LCD_HEIGHT = 2 };

/* Объявляем объект нашего дисплея */
LiquidCrystal lcd(12, 11, 10, 4, 5, 6, 7);


void setup()
{
  /* Регистрируем собственные символы с кодами 0, 1 и 2 */
  lcd.createChar(3, arnie);
  lcd.createChar(1, skull_and_bones);
  lcd.createChar(2, lightning);
  /* Начинаем работу с дисплееем */
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
}


typedef void (*LCD_demo)();

/* Это список демо-функций. Добавляйте свои или меняйте их порядок,
 * если хотите разнообразить этот скетч (:
 */
LCD_demo const demos[] =
{
  showArnie,
  showWarning,
  showScrolling,
  showAutoscroll,
  showRTL
};

/* Этот макрос - обычный "синтаксический сахар". Мне просто влом писать
 * по 10 почти одинаковых циклов for. Конечно, в серьёзном коде такое лучше не писать.
 */
#define dotimes(n, code) for (int i = 0; i < (n); ++i) code;


void loop()
{
  dotimes(sizeof(demos) / sizeof(demos[0]),
  {
    demos[i](); // запускаем очередную демонстрацию

    /* Даём насладиться её последними "кадрами", после чего очищаем экран */
    delay(2000);
    lcd.clear();
    delay(1000);
  });
}


void showArnie()
{
  lcd.cursor(); // показываем курсор
  lcd.blink(); // заставляем его мигать
  delay(1000);

  lcd.write("\3"); // выводим наш собственный символ с кодом 0 (ноль)
  lcd.write(' ');
  delay(1000);

  lcd.print("I'll be back"); // угадай, кто на экране (:
  delay(1000);

  lcd.noBlink(); // больше мигать не нужно
  lcd.noCursor(); // прячем курсор
}


void showWarning()
{
  /* Так как коды наших собственных символов - от 0 до 7,
   * можно смело использовать запись символов в строке при помощи
   * восьмеричных кодов: \1, \2 и т.д.
   * НО: код 0 (ноль) в C и C++ используется в качестве маркера конца строки,
   * так что используйте lcd.write(0) для вывода нулевого символа.
   */
  lcd.print("Smoke kills \1"); // в конце строки будет черепок с костями
  delay(2000);

  lcd.setCursor(0, 1); // переходим на следующую строку
  /* Можно вывести молнию как в начале, так и в середине */
  lcd.print("\2 AC/DC \2  rocks");
}


void showScrolling()
{
  lcd.print("I'm scrolling");
  delay(1000);

  /* Прокручиваем текст вправо за экран */
  dotimes(16,
  {
    lcd.scrollDisplayRight();
    delay(150);
  });

  /* И возвращаем на место, прокручивая влево */
  dotimes(16,
  {
    lcd.scrollDisplayLeft();
    delay(150);
  });
}

void showAutoscroll()
{
  lcd.setCursor(LCD_WIDTH - 2, 0);
  lcd.autoscroll(); // включаем автопрокрутку

  const char *text = "autoscroll";

  /* Печатаем строку буква за буквой. Текст будет "выползать" справа. */
  dotimes(strlen(text),
  {
    lcd.write(text[i]);
    delay(200);
  });

  lcd.noAutoscroll(); // выключаем автопрокрутку
}


void showRTL()
{
  lcd.setCursor(LCD_WIDTH - 1, 0);
  lcd.rightToLeft(); // теперь пишем справа налево

  const char *text = "tfel-ot-thgir"; // похоже на заклинание

  /* Печатаем строку буква за буквой. Текст будет выводиться задом наперёд. */
  dotimes(strlen(text),
  {
    lcd.write(text[i]);
    delay(200);
  });

  lcd.leftToRight(); // пишем слева направо
}
