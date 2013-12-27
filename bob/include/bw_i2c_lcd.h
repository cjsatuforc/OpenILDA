#ifndef BW_I2C_LCD_H_
#define BW_I2C_LCD_H_

#include <stdint.h>

extern int bw_i2c_lcd_start (char);
extern void bw_i2c_lcd_end (void);

extern void bw_i2c_lcd_reinit(void);

extern void bw_i2c_lcd_text_line_1(char *, uint8_t);
extern void bw_i2c_lcd_text_line_2(char *, uint8_t);
extern void bw_i2c_lcd_text_line_3(char *, uint8_t);
extern void bw_i2c_lcd_text_line_4(char *, uint8_t);

extern void bw_i2c_lcd_cls(void);
extern void bw_i2c_lcd_set_contrast(uint8_t);
extern void bw_i2c_lcd_set_backlight(uint8_t);

extern void bw_i2c_lcd_read_id(void);

#endif /* BW_I2C_LCD_H_ */
