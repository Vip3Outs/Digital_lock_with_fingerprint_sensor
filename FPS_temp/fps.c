#include "fps.h"

void fps_transmit_header(){
	putByte(0xEF);
	putByte(0x01);
	
	putByte(0xFF);
	putByte(0xFF);
	putByte(0xFF);
	putByte(0xFF);
	
	putByte(0x01);
}

uint16_t fps_confirmation(uint8_t instruction_code){
	uint16_t tmp;
	for(int i = 0; i<9; ++i){
		getByte();
	}
	tmp = getByte();
	if(tmp == 0x00 && instruction_code == 0x1D){
		uint16_t template_count; 
		tmp = getByte()<< 8;
		tmp = getByte();
	}
	
	return tmp;
}



uint8_t fps_read_finger(){
	//header
	fps_transmit_header();
	//length
	putByte(0x00);
	putByte(0x03);
	//instruction code
	putByte(0x01);
	//sum
	putByte(0x00);
	putByte(0x05);
	
	return fps_confirmation(0x01);
}