#ifndef __LCD_H__
#define __LCD_H__


#define I2C_ADDR 0x3F
#define SLA_WRITE_ADDR I2C_ADDR << 1


/*
 * LCD initialization
 */
void lcd_init(void);

/**
 * Sending byte to lcd
 * @c: byte
 * @rs: type of data
 */
void lcd_send_byte(char c);

/**
 * Sending char to lcd
 * @c: char
 */
void lcd_send_char(char c);

/*
 * Clear LCD
 */
void lcd_clear(void);

/**
 * Move cursor on lcd
 * @col: X
 * @row: Y
 */
void lcd_setCursor(char col, char row);

/**
 * Writing string on the lcd
 * @str: string
 */
void lcd_send_string(const char *str);


#endif
