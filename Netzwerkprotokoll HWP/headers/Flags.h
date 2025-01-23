#include <cstdint>

#ifndef FLAGS_H //vermeidet multiples rekompilieren
#define FLAGS_H

const uint8_t SYC = 0x0F; //Synchronisation (0b00001111) (15)

const uint8_t DATA_F = 0x07; //data flag (0b00000111) (7)
const uint8_t SOT = 0x01; //start of trasmission  (0b00000001) (1)
const uint8_t EOT = 0x04; //end of transmission (0b00000100) (4)

const uint8_t CRC_F = 0x02; //CRC will be sent (0b00000010) (2)
const uint8_t SIZE_F = 0x08; //size flag (0b00001000) (8)

const uint8_t ACK = 0x06; //acknowlodgement (0b00000110) (6)
const uint8_t NACK = 0x0A; //negative acknowlodgement  (0b00001010) (10)

const uint8_t GS = 0x0C; //group separation (0b00001100) (12)
const uint8_t RS = 0x0E; //record separator (0b00001110) (14)

#endif // FLAGS_H