#include "./headers/Message.h"

Message::Message(const std::string &msg) {
    text = msg;
    BinVector = ProcessMessage(msg);
    MessageSize = BinVector.size();
}

std::vector<uint8_t> Message::ProcessMessage(const std::string &text) {
    std::vector<uint8_t> BinVector;
    for (char c : text) {
        //Turn each char in uint8_t
        uint8_t bin = static_cast<uint8_t>(c);
        //std::cerr << "Message > " << c << ": " << std::bitset<8>(bin) << std::endl;
        BinVector.push_back(bin);
    }
    if (BinVector.empty())
        std::cerr << "Message > Binary Vector is empty!" << std::endl;

    return BinVector;
}

std::vector<uint8_t> Message::getPackageData(int i, int j) {
    std::vector<uint8_t> subVector;
    int BinVectorSize = BinVector.size();

    if (i >= 0 && j < BinVectorSize && i <= j) {
        subVector.insert(subVector.end(), BinVector.begin() + i, BinVector.begin() + j - 1);
    } else {
        std::cerr << "Message > Invalid indices for getPackageData: i = " << i << ", j = " << j << std::endl;
    }
    return subVector;
}

int Message::getMessageSize() {
    return BinVector.size();
}

std::vector<uint8_t> Message::getBinVector() {
    return BinVector;
}

