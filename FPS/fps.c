#include "fps.h"

void fps_transmit_header(){
	//starting code
	putByte(0xEF);
	putByte(0x01);
	//address
	putByte(0xFF);
	putByte(0xFF);
	putByte(0xFF);
	putByte(0xFF);
	//package identifier
	putByte(0x01);
}

void fps_confirmation(uint8_t instruction_code){

	for(int i = 0; i<9; ++i){
		getByte();
	}
	//confirmation code
	tmp[0] = getByte();
	if(instruction_code == 0x1D){
		tmp[1] = getByte();
		tmp[2] = getByte();
	}
	if(instruction_code == 0x04){
		//page_ID
		tmp[1] = getByte();
		tmp[2] = getByte();
		//MatchScore
		tmp[3] = getByte();
		tmp[4] = getByte();
	}
	//checksum
	getByte();
	getByte();
}



void fps_read_finger(){
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
	
	fps_confirmation(0x01);
}

void fps_img2TZ(unsigned char buffer){
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
		
	fps_confirmation(0x02);
}

void fps_search(){
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

	fps_confirmation(0x04);
}

void fps_genModel(){
	//header
	fps_transmit_header();
	//length
	putByte(0x00);
	putByte(0x03);
	//instruction
	putByte(0x05);
	//sum
	putByte(0x00);
	putByte(0x09);
	
	fps_confirmation(0x05);
}

void fps_storeModel(uint16_t storeID){
	//header
	fps_transmit_header();
	//length
	putByte(0x00);
	putByte(0x06);
	//instruction
	putByte(0x06);
	//bufferID
	putByte(0x01);
	//location number
	putByte(storeID & 0xFF00);
	putByte(storeID & 0x00FF);
	//sum
	storeID += 14;
	putByte(storeID & 0xFF00);
	putByte(storeID & 0x00FF);
	
	fps_confirmation(0x06);
}

void fps_deleteModel(uint16_t deleteID){
	//header
	fps_transmit_header();
	//length
	putByte(0x00);
	putByte(0x07);
	//instruction
	putByte(0x0c);
	//pageID
	putByte(deleteID & 0xFF00);
	putByte(deleteID & 0x00FF);
	//number of templates to be deleted
	putByte(0x00);
	putByte(0x01);
	//sum
	deleteID += 21;
	putByte(deleteID & 0xFF00);
	putByte(deleteID & 0x00FF);
	
	fps_confirmation(0x0c);
}

void fps_templateNum(){
	//header
	fps_transmit_header();
	//length
	putByte(0x00);
	putByte(0x03);
	//instruction
	putByte(0x1D);	
	//sum
	putByte(0x00);
	putByte(0x21);
	
	fps_confirmation(0x1D);
	
}