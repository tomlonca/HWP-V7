#include <cstdint>

#ifndef FLAGS_H //vermeidet multiples rekompilieren
#define FLAGS_H

const uint8_t SYC = 0x0F; //Synchronisation (0b00001111)
//const uint8_t SYC_ACK = 0xF0; //Synchronisation (0b11110000)

const uint8_t SOT = 0x01; //start of trasmission  (0b00000001)
const uint8_t EOT = 0x04; //end of transmission (0b00000100)

const uint8_t CRC_F = 0x02; //CRC will be sent (0b00000010)
const uint8_t SIZE_F = 0x08; //size flag (0b00001000)

const uint8_t ACK = 0x06; //acknowlodgement (0b00000110)
const uint8_t NACK = 0x0A; //negative acknowlodgement  (0b00001010)

//const uint8_t EOTB = 0x0B; //end of transmission block (0b00001011)
const uint8_t GS = 0x0C; //group separation (0b00001100)

#endif // FLAGS_H