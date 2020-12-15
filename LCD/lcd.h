#ifndef __LCD_H__
#define __LCD_H__


#define I2C_ADDR 0x3F
#define SLA_WRITE_ADDR I2C_ADDR << 1

void lcd_init(void);
void lcd_send_byte(char c);
void lcd_send_char(char c);
void lcd_clear(void);
void lcd_setCursor(char col, char row);
void lcd_send_string(const char *str);
void lcd_send_info(const char *line1, const char *line2);

#endif /* __LCD_H__ */
