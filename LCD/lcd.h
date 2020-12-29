#ifndef __LCD_H__
#define __LCD_H__


#define SLAVE_ADDRESS 0x3F	//adres slave
#define SLAVE_WRITE_ADDRESS SLAVE_ADDRESS << 1 //adres slave podczas wysylania pakietu

void lcd_init();
void lcd_transmit_header(char tmp);
void lcd_send_byte(char tmp);
void lcd_send_char(char tmp);
void lcd_clear();
void lcd_setCursor(char col, char row);
void lcd_send_string(const char *str);
void lcd_send_info(const char *line1, const char *line2);

#endif /* __LCD_H__ */
