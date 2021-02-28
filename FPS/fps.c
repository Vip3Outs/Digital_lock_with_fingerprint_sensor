#include "fps.h"

void fps_transmit_header(){
	//starting code
	uart_putByte(0xEF);
	uart_putByte(0x01);
	//address
	uart_putByte(0xFF);
	uart_putByte(0xFF);
	uart_putByte(0xFF);
	uart_putByte(0xFF);
	//package identifier
	uart_putByte(0x01);
}

void fps_confirmation(uint8_t instruction_code){

	for(uint8_t i = 0; i<9; ++i){
		uart_getByte();
	}
	//confirmation code
	tmp[0] = uart_getByte();
	if(instruction_code == 0x1D){
		tmp[1] = uart_getByte();
		tmp[2] = uart_getByte();
	}
	if(instruction_code == 0x04){
		//page_ID
		tmp[1] = uart_getByte();
		tmp[2] = uart_getByte();
		//MatchScore
		tmp[3] = uart_getByte();
		tmp[4] = uart_getByte();
	}
	//checksum
	uart_getByte();
	uart_getByte();
}



void fps_read_finger(){
	//header
	fps_transmit_header();
	//length
	uart_putByte(0x00);
	uart_putByte(0x03);
	//instruction code
	uart_putByte(0x01);
	//sum
	uart_putByte(0x00);
	uart_putByte(0x05);
	
	fps_confirmation(0x01);
}

void fps_img2TZ(uint8_t buffer){
	//header
	fps_transmit_header();
	//length
	uart_putByte(0x00);
	uart_putByte(0x04);
	//instruction code
	uart_putByte(0x02);
	//bufferID
	uart_putByte(buffer);
	//sum
	buffer+=7;
	uart_putByte(buffer & 0xFF00);
	uart_putByte(buffer & 0x00FF);
		
	fps_confirmation(0x02);
}

void fps_search(){
	//header
	fps_transmit_header();
	//length
	uart_putByte(0x00);
	uart_putByte(0x08);
	//instruction
	uart_putByte(0x04);
	//buffer_ID
	uart_putByte(0x01);
	//start location
	uart_putByte(0x00);
	uart_putByte(0x01);
	//end location
	uart_putByte(0x00);
	uart_putByte(0xFF);
	//sum
	uart_putByte(0x01);
	uart_putByte(0x0E);

	fps_confirmation(0x04);
}

void fps_genModel(){
	//header
	fps_transmit_header();
	//length
	uart_putByte(0x00);
	uart_putByte(0x03);
	//instruction
	uart_putByte(0x05);
	//sum
	uart_putByte(0x00);
	uart_putByte(0x09);
	
	fps_confirmation(0x05);
}

void fps_storeModel(uint16_t storeID){
	//header
	fps_transmit_header();
	//length
	uart_putByte(0x00);
	uart_putByte(0x06);
	//instruction
	uart_putByte(0x06);
	//bufferID
	uart_putByte(0x01);
	//location number
	uart_putByte(storeID & 0xFF00);
	uart_putByte(storeID & 0x00FF);
	//sum
	storeID += 14;
	uart_putByte(storeID & 0xFF00);
	uart_putByte(storeID & 0x00FF);
	
	fps_confirmation(0x06);
}

void fps_deleteModel(uint16_t deleteID){
	//header
	fps_transmit_header();
	//length
	uart_putByte(0x00);
	uart_putByte(0x07);
	//instruction
	uart_putByte(0x0c);
	//pageID
	uart_putByte(deleteID & 0xFF00);
	uart_putByte(deleteID & 0x00FF);
	//number of templates to be deleted
	uart_putByte(0x00);
	uart_putByte(0x01);
	//sum
	deleteID += 21;
	uart_putByte(deleteID & 0xFF00);
	uart_putByte(deleteID & 0x00FF);
	
	fps_confirmation(0x0c);
}

void fps_templateNum(){
	//header
	fps_transmit_header();
	//length
	uart_putByte(0x00);
	uart_putByte(0x03);
	//instruction
	uart_putByte(0x1D);	
	//sum
	uart_putByte(0x00);
	uart_putByte(0x21);
	
	fps_confirmation(0x1D);
	
}