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
	//confirmation code
	tmp = getByte();
	if(tmp == 0x00 && instruction_code == 0x1D){
		tmp = getByte()<< 8;
		tmp += getByte();
	}
	else if(!(tmp == 0x00) && instruction_code == 0x1D){
		tmp = 0xFFFF;
		getByte();
		getByte();
	}
	if(tmp == 0x00 && instruction_code == 0x04){
		//page_ID
		getByte();
		getByte();
		//MatchScore
		tmp = getByte() << 8;
		tmp += getByte();
	}
	else if(!(tmp == 0x00) && instruction_code == 0x04){
		tmp = 0xFFFF;
		//page_ID
		getByte();
		getByte();
		//MatchScore
		getByte();
		getByte();
	}
	//checksum
	getByte();
	getByte();
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

uint8_t fps_img2TZ(unsigned char buffer){
	//header
	fps_transmit_header();
	//length
	putByte(0x00);
	putByte(0x04);
	//instruction code
	putByte(0x02);
	//sum
	buffer+=7;
	putByte(buffer & 0xFF00);
	putByte(buffer & 0x00FF);
		
	return fps_confirmation(0x02);
}

uint16_t fps_search(){
	//header
	fps_transmit_header();
	//length
	putByte(0x00);
	putByte(0x08);
	//instruction
	putByte(0x04);
	//buffer_ID
	putByte(0x01);
	//start location
	putByte(0x00);
	putByte(0x01);
	//end location
	putByte(0x00);
	putByte(0xFF);
	//sum
	putByte(0x01);
	putByte(0x0E);

	return fps_confirmation(0x04);
}


