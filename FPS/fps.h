#ifndef FPS_H_
#define FPS_H_

#include <stdint.h>
#include <stdbool.h>
#define FINGERPRINT_OK 0x00					//Command execution is complete
#define FINGERPRINT_PACKETRECIEVEERR 0x01	//Error when receiving data package
#define FINGERPRINT_NOFINGER 0x02			//No finger on the sensor
#define FINGERPRINT_IMAGEFAIL 0x03			//Failed to enroll the finger
#define FINGERPRINT_IMAGEMESS 0x06			//Failed to generate character file due to overly disorder fingerprint image
#define FINGERPRINT_FEATUREFAIL 0x07		//Failed to generate character file due to the lack of character point or small fingerprint image
#define FINGERPRINT_NOMATCH 0x08			//Finger doesn't match
#define FINGERPRINT_NOTFOUND 0x09			//Failed to find matching finger
#define FINGERPRINT_ENROLLMISMATCH 0x0A		//Failed to combine the character files
#define FINGERPRINT_BADLOCATION 0x0B		//Addressed PageID is beyond the finger library
#define FINGERPRINT_DBRANGEFAIL 0x0C		//Error when reading template from library or invalid template
#define FINGERPRINT_UPLOADFEATUREFAIL 0x0D	//Error when uploading template
#define FINGERPRINT_PACKETRESPONSEFAIL 0x0E //Module failed to receive the following data packages
#define FINGERPRINT_UPLOADFAIL 0x0F			//Error when uploading image
#define FINGERPRINT_DELETEFAIL 0x10			//Failed to delete the template
#define FINGERPRINT_DBCLEARFAIL 0x11		//Failed to clear finger library
#define FINGERPRINT_PASSFAIL 0x13			//Find whether the fingerprint passed or failed
#define FINGERPRINT_INVALIDIMAGE 0x15		//Failed to generate image because of lac of valid primary image
#define FINGERPRINT_FLASHERR 0x18			//Error when writing flash
#define FINGERPRINT_INVALIDREG 0x1A			//Invalid register number
#define FINGERPRINT_ADDRCODE 0x20			//Address code
#define FINGERPRINT_PASSVERIFY 0x21			//Verify the fingerprint passed
#define FINGERPRINT_STARTCODE 0xEF01		//Fixed falue of EF01H; High byte transferred first

#define FINGERPRINT_COMMANDPACKET 0x1		//Command packet
#define FINGERPRINT_DATAPACKET 0x2			//Data packet, must follow command packet or acknowledge packet
#define FINGERPRINT_ACKPACKET 0x7			//Acknowledge packet
#define FINGERPRINT_ENDDATAPACKET 0x8		//End of data packet

#define FINGERPRINT_TIMEOUT 0xFF			//Timeout was reached
#define FINGERPRINT_BADPACKET 0xFE			//Bad packet was sent

#define FINGERPRINT_GETIMAGE 0x01			//Collect finger image
#define FINGERPRINT_IMAGE2TZ 0x02			//Generate character file from image
#define FINGERPRINT_SEARCH 0x04				//Search for fingerprint in slot
#define FINGERPRINT_REGMODEL 0x05			//Combine character files and generate template
#define FINGERPRINT_STORE 0x06				//Store template
#define FINGERPRINT_LOAD 0x07				//Read/load template
#define FINGERPRINT_UPLOAD 0x08				//Upload template
#define FINGERPRINT_DELETE 0x0C				//Delete templates
#define FINGERPRINT_EMPTY 0x0D				//Empty library
#define FINGERPRINT_READSYSPARAM 0x0F		//Ready system parameters
#define FINGERPRINT_SETPASSWORD 0x12		//Sets passwords
#define FINGERPRINT_HISPEEDSEARCH 0x1B		//Asks the sensor to search for a matching fingerprint template to the last model generated
#define FINGERPRINT_TEMPLATECOUNT 0x1D		//!< Read finger template numbers



//#define FINGERPRINT_DEBUG

#define DEFAULTTIMEOUT 1000 //!< UART reading timeout in milliseconds

struct fps_Packet {
  
	uint16_t start_code;
	uint8_t s_type;
	uint16_t s_length;
	uint8_t address[4];
	uint8_t s_data[64];
};

void fill_fps_Packet(uint8_t type, uint16_t length, uint8_t *data, struct fps_Packet packet);
bool verifyPassword(void);
uint8_t getParameters(void);

uint8_t getImage(void);
uint8_t image2Tz(uint8_t slot);
uint8_t createModel(void);

uint8_t emptyDatabase(void);
uint8_t storeModel(uint16_t id);
uint8_t loadModel(uint16_t id);
uint8_t getModel(void);
uint8_t deleteModel(uint16_t id);
uint8_t fingerFastSearch(void);
uint8_t fingerSearch(uint8_t slot);
uint8_t getTemplateCount(void);


void writeStructuredPacket(struct fps_Packet p);
uint8_t getStructuredPacket(struct fps_Packet *p, uint16_t timeout);

  




#endif /* FPS_H_ */