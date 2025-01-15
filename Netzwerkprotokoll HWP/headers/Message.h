#include <string>
#include <vector>
#include <stdint.h>
#include <bitset>
#include <iostream>

#include "Flags.h"

class Message {
    private:
        std::string text;
        std::vector<uint8_t> BinVector;
        int MessageSize; //Size is amount of chars
        uint8_t CRC;

    public:
        Message(const std::string &text);
        std::vector<uint8_t> ProcessMessage(const std::string &text);
        std::vector<uint8_t> getPackageData(int i, int j);
        uint8_t CalculateCRC8(const std::string &binaryString);

        std::vector<uint8_t> getBinVector();
        uint8_t getCRC();
        int getMessageSize();

}