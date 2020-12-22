#ifndef FPS_H_
#define FPS_H_
#include <stdint.h>
uint8_t tmp[5];
void fps_transmit_header();
void fps_confirmation(uint8_t instruction_code);
void fps_read_finger();
void fps_img2TZ(uint8_t buffer);
void fps_search();
void fps_genModel();
void fps_storeModel(uint16_t storeID);
void fps_deleteModel(uint16_t deleteID);
void fps_templateNum();



#endif /* FPS_H_ */