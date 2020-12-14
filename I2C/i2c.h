#ifndef __I2C_H__
#define __I2C_H__

#include <avr/io.h>


void i2c_init();
void i2c_start_condition();
void i2c_stop_condition();
void i2c_send_byte(unsigned char byte);
void i2c_send_packet(unsigned char value, unsigned char address);

#endif /* __I2C_H__ */
