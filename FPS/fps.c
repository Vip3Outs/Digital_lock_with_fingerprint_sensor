#include "fps.h"
#define F_CPU 16000000
#include <avr/delay.h>
#include <stdbool.h>
#include <string.h>
  
  uint16_t fingerID; //The matching location that is set by fingerFastSearch()
  uint16_t confidence; //The confidence of the fingerFastSearch() match, higher numbers are more confidents
  uint16_t templateCount; //The number of stored templates in the sensor, set by getTemplateCount()
  uint16_t status_reg = 0x0; //The status register (set by getParameters)
  uint16_t system_id = 0x0; //The system identifier (set by getParameters)
  uint16_t capacity = 64; //The fingerprint capacity (set by getParameters)
  uint16_t security_level = 0; //The security level (set by getParameters)
  uint32_t device_addr = 0xFFFFFFFF; //The device address (set by getParameters)
  uint16_t packet_len = 64; //The max packet length (set by getParameters)
  uint16_t baud_rate = 57600; //The UART baud rate (set by getParameters)
  
/*!
 * @brief Gets the command packet
 */
#define GET_CMD_PACKET(...) uint8_t data[] = {__VA_ARGS__};											\
	struct fps_Packet packet;																		\
	fill_fps_Packet(FINGERPRINT_COMMANDPACKET, sizeof(data), data, packet);							\
	writeStructuredPacket(packet);																	\
	if (getStructuredPacket(&packet, DEFAULTTIMEOUT) != FINGERPRINT_OK)								\
	return FINGERPRINT_PACKETRECIEVEERR;															\
	if (packet.s_type != FINGERPRINT_ACKPACKET)														\
	return FINGERPRINT_PACKETRECIEVEERR;															\

/*!
 * @brief Sends the command packet
 */
#define SEND_CMD_PACKET(...) GET_CMD_PACKET(__VA_ARGS__); return packet.s_data[0];



/**************************************************************************/
/*!
    @brief  Get the sensors parameters, fills in the member variables
    status_reg, system_id, capacity, security_level, device_addr, packet_len
    and baud_rate
    @returns True if password is correct
*/
/**************************************************************************/
/*
uint8_t getParameters(void) {
  GET_CMD_PACKET(FINGERPRINT_READSYSPARAM);

  status_reg = ((uint16_t)packet.s_data[1] << 8) | packet.s_data[2];
  system_id = ((uint16_t)packet.s_data[3] << 8) | packet.s_data[4];
  capacity = ((uint16_t)packet.s_data[5] << 8) | packet.s_data[6];
  security_level = ((uint16_t)packet.s_data[7] << 8) | packet.s_data[8];
  device_addr = ((uint32_t)packet.s_data[9] << 24) |
                ((uint32_t)packet.s_data[10] << 16) |
                ((uint32_t)packet.s_data[11] << 8) | (uint32_t)packet.s_data[12];
  packet_len = ((uint16_t)packet.s_data[13] << 8) | packet.s_data[14];
  if (packet_len == 0) {
    packet_len = 32;
  } else if (packet_len == 1) {
    packet_len = 64;
  } else if (packet_len == 2) {
    packet_len = 128;
  } else if (packet_len == 3) {
    packet_len = 256;
  }
  baud_rate = (((uint16_t)packet.s_data[15] << 8) | packet.s_data[16]) * 9600;

  return packet.s_data[0];
}

*/

/**************************************************************************/
/*!
    @brief   Ask the sensor to take an image of the finger pressed on surface
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_NOFINGER</code> if no finger detected
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
    @returns <code>FINGERPRINT_IMAGEFAIL</code> on imaging error
*/
/**************************************************************************/
uint8_t getImage(void) {
  SEND_CMD_PACKET(FINGERPRINT_GETIMAGE);
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to convert image to feature template
    @param slot Location to place feature template (put one in 1 and another in
   2 for verification to create model)
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_IMAGEMESS</code> if image is too messy
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
    @returns <code>FINGERPRINT_FEATUREFAIL</code> on failure to identify
   fingerprint features
    @returns <code>FINGERPRINT_INVALIDIMAGE</code> on failure to identify
   fingerprint features
*/
uint8_t image2Tz(uint8_t slot) {
  SEND_CMD_PACKET(FINGERPRINT_IMAGE2TZ, slot);
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to take two print feature template and create a
   model
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
    @returns <code>FINGERPRINT_ENROLLMISMATCH</code> on mismatch of fingerprints
*/
uint8_t createModel(void) {
  SEND_CMD_PACKET(FINGERPRINT_REGMODEL);
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to store the calculated model for later matching
    @param   location The model location #
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_BADLOCATION</code> if the location is invalid
    @returns <code>FINGERPRINT_FLASHERR</code> if the model couldn't be written
   to flash memory
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
uint8_t storeModel(uint16_t location) {
  SEND_CMD_PACKET(FINGERPRINT_STORE, 0x01, (uint8_t)(location >> 8), (uint8_t)(location & 0xFF));
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to load a fingerprint model from flash into buffer 1
    @param   location The model location #
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_BADLOCATION</code> if the location is invalid
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
uint8_t loadModel(uint16_t location) {
  SEND_CMD_PACKET(FINGERPRINT_LOAD, 0x01, (uint8_t)(location >> 8), (uint8_t)(location & 0xFF));
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to transfer 256-byte fingerprint template from the
   buffer to the UART
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
uint8_t getModel(void) {
  SEND_CMD_PACKET(FINGERPRINT_UPLOAD, 0x01);
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to delete a model in memory
    @param   location The model location #
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_BADLOCATION</code> if the location is invalid
    @returns <code>FINGERPRINT_FLASHERR</code> if the model couldn't be written
   to flash memory
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
uint8_t deleteModel(uint16_t location) {
  SEND_CMD_PACKET(FINGERPRINT_DELETE, (uint8_t)(location >> 8), (uint8_t)(location & 0xFF), 0x00, 0x01);
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to delete ALL models in memory
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_BADLOCATION</code> if the location is invalid
    @returns <code>FINGERPRINT_FLASHERR</code> if the model couldn't be written
   to flash memory
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
uint8_t emptyDatabase(void) {
  SEND_CMD_PACKET(FINGERPRINT_EMPTY);
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to search the current slot 1 fingerprint features to
   match saved templates. The matching location is stored in <b>fingerID</b> and
   the matching confidence in <b>confidence</b>
    @returns <code>FINGERPRINT_OK</code> on fingerprint match success
    @returns <code>FINGERPRINT_NOTFOUND</code> no match made
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
/**************************************************************************/
uint8_t fingerFastSearch(void) {
  // high speed search of slot #1 starting at page 0x0000 and page #0x00A3
  GET_CMD_PACKET(FINGERPRINT_HISPEEDSEARCH, 0x01, 0x00, 0x00, 0x00, 0xA3);
  fingerID = 0xFFFF;
  confidence = 0xFFFF;

  fingerID = packet.s_data[1];
  fingerID <<= 8;
  fingerID |= packet.s_data[2];

  confidence = packet.s_data[3];
  confidence <<= 8;
  confidence |= packet.s_data[4];

  return packet.s_data[0];
}




/**************************************************************************/
/*!
    @brief   Ask the sensor to search the current slot fingerprint features to
   match saved templates. The matching location is stored in <b>fingerID</b> and
   the matching confidence in <b>confidence</b>
   @param slot The slot to use for the print search, defaults to 1
    @returns <code>FINGERPRINT_OK</code> on fingerprint match success
    @returns <code>FINGERPRINT_NOTFOUND</code> no match made
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
/**************************************************************************/
uint8_t fingerSearch(uint8_t slot) {
  // search of slot starting thru the capacity
  GET_CMD_PACKET(FINGERPRINT_SEARCH, slot, 0x00, 0x00, capacity >> 8, capacity & 0xFF);

  fingerID = 0xFFFF;
  confidence = 0xFFFF;

  fingerID = packet.s_data[1];
  fingerID <<= 8;
  fingerID |= packet.s_data[2];

  confidence = packet.s_data[3];
  confidence <<= 8;
  confidence |= packet.s_data[4];

  return packet.s_data[0];
}

/**************************************************************************/
/*!
    @brief   Ask the sensor for the number of templates stored in memory. The
   number is stored in <b>templateCount</b> on success.
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
/**************************************************************************/
uint8_t getTemplateCount(void) {
	  GET_CMD_PACKET(FINGERPRINT_TEMPLATECOUNT);

	templateCount = packet.s_data[1];
	templateCount <<= 8;
	templateCount |= packet.s_data[2];
	return packet.s_data[0];
}



/**************************************************************************/
/*!
    @brief   Helper function to process a packet and send it over UART to the
   sensor
    @param   packet A structure containing the bytes to transmit
*/
/**************************************************************************/

void writeStructuredPacket(const struct fps_Packet packet) {

  putByte((uint8_t)(packet.start_code >> 8));
  putByte((uint8_t)(packet.start_code & 0xFF));
  putByte(packet.address[0]);
  putByte(packet.address[1]);
  putByte(packet.address[2]);
  putByte(packet.address[3]);
  putByte(packet.s_type);

  uint16_t wire_length = packet.s_length + 2;
  putByte((uint8_t)(wire_length >> 8));
  putByte((uint8_t)(wire_length & 0xFF));

//#ifdef FINGERPRINT_DEBUG
//  Serial.print("-> 0x");
//  Serial.print((uint8_t)(packet.start_code >> 8), HEX);
//  Serial.print(", 0x");
//  Serial.print((uint8_t)(packet.start_code & 0xFF), HEX);
//  Serial.print(", 0x");
//  Serial.print(packet.address[0], HEX);
//  Serial.print(", 0x");
//  Serial.print(packet.address[1], HEX);
//  Serial.print(", 0x");
//  Serial.print(packet.address[2], HEX);
//  Serial.print(", 0x");
//  Serial.print(packet.address[3], HEX);
//  Serial.print(", 0x");
//  Serial.print(packet.type, HEX);
//  Serial.print(", 0x");
//  Serial.print((uint8_t)(wire_length >> 8), HEX);
//  Serial.print(", 0x");
//  Serial.print((uint8_t)(wire_length & 0xFF), HEX);
//#endif

  uint16_t sum = ((wire_length) >> 8) + ((wire_length)&0xFF) + packet.s_type;
  for (uint8_t i = 0; i < packet.s_length; i++) {
    putByte(packet.s_data[i]);
    sum += packet.s_data[i];
//#ifdef FINGERPRINT_DEBUG
//    Serial.print(", 0x");
//    Serial.print(packet.data[i], HEX);
//#endif
  }

  putByte((uint8_t)(sum >> 8));
  putByte((uint8_t)(sum & 0xFF));

//#ifdef FINGERPRINT_DEBUG
//  Serial.print(", 0x");
//  Serial.print((uint8_t)(sum >> 8), HEX);
//  Serial.print(", 0x");
//  Serial.println((uint8_t)(sum & 0xFF), HEX);
//#endif

 // return;
}

/**************************************************************************/
/*!
    @brief   Helper function to receive data over UART from the sensor and
   process it into a packet
    @param   packet A structure containing the bytes received
    @param   timeout how many milliseconds we're willing to wait
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_TIMEOUT</code> or
   <code>FINGERPRINT_BADPACKET</code> on failure
*/
/**************************************************************************/
uint8_t getStructuredPacket(struct fps_Packet *packet, uint16_t timeout) {
  uint8_t byte;
  uint16_t idx = 0, timer = 0;

//#ifdef FINGERPRINT_DEBUG
//  Serial.print("<- ");
//#endif

  while (true) {
    while (checkEmptyRegister() == false) {
      _delay_ms(1);
      timer++;
      if (timer >= timeout) {
//#ifdef FINGERPRINT_DEBUG
//        Serial.println("Timed out");
//#endif
        return FINGERPRINT_TIMEOUT;
      }
    }
    byte = getByte();
//#ifdef FINGERPRINT_DEBUG
//    Serial.print("0x");
//    Serial.print(byte, HEX);
//    Serial.print(", ");
//#endif
    switch (idx) {
    case 0:
      if (byte != (FINGERPRINT_STARTCODE >> 8))
        continue;
      packet->start_code = (uint16_t)byte << 8;
      break;
    case 1:
      packet->start_code |= byte;
      if (packet->start_code != FINGERPRINT_STARTCODE)
        return FINGERPRINT_BADPACKET;
      break;
    case 2:
    case 3:
    case 4:
    case 5:
      packet->address[idx - 2] = byte;
      break;
    case 6:
      packet->s_type = byte;
      break;
    case 7:
      packet->s_length = (uint16_t)byte << 8;
      break;
    case 8:
      packet->s_length |= byte;
      break;
    default:
      packet->s_data[idx - 9] = byte;
      if ((idx - 8) == packet->s_length) {
//#ifdef FINGERPRINT_DEBUG
//       Serial.println(" OK ");
//#endif
        return FINGERPRINT_OK;
      }
      break;
    }
    idx++;
  }
  // Shouldn't get here so...
  return FINGERPRINT_BADPACKET;
}

	void fill_fps_Packet(uint8_t type, uint16_t length, uint8_t *data, struct fps_Packet packet) {
	packet.start_code = FINGERPRINT_STARTCODE;
	packet.s_type = type;
	packet.s_length = length;
	packet.address[0]= 0xFF;
	packet.address[1]= 0xFF;
	packet.address[2]= 0xFF;
	packet.address[3]= 0xFF;
	if (length < 64)
	memcpy(packet.s_data, data, length);
	else
	memcpy(packet.s_data, data, 64);
	}