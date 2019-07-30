#include "U8glib.h"

// setup u8g object, please remove comment from one of the following constructor calls
// IMPORTANT NOTE: The following list is incomplete. The complete list of supported 
// devices with all constructor calls is here: https://github.com/olikraus/u8glib/wiki/device

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI 

const uint8_t rook_bitmap[] U8G_PROGMEM = {
  0x00,         // 00000000 
  0x55,         // 01010101
  0x7f,         // 01111111
  0x3e,         // 00111110
  0x3e,         // 00111110 
  0x3e,         // 00111110
  0x3e,         // 00111110 
  0x7f          // 01111111
};

int i = 0;

void setup(void) {
}

void loop(void) {
  // picture loop
  u8g.firstPage();
  do {
      u8g.setFont(u8g_font_osb18);
      u8g.drawStr(0, 20, "ABC");
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  
  delay(250);

}
