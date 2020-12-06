#ifndef FPS_H_
#define FPS_H_
#include <stdint.h>

void fps_transmit_header();
uint16_t fps_confirmation(uint8_t instruction_code);
uint8_t fps_read_finger();



#endif /* FPS_H_ */