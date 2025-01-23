#include "./headers/DriverManager.h"

DriverManager::DriverManager(B15F &drv, bool isWriter) : drv(drv) {
    isWriter_DM = isWriter;

    drv.setRegister(&DDRA, 0x0F); //left 4 pins input, 4 right pins output

    if(isWriter) {
        std::cerr << "Driver Manager > Writer Instance created!" << std::endl;
    } else {
        std::cerr << "Driver Manager > Reciever Instance created!" << std::endl;
    }
    
}

void DriverManager::SendData(uint8_t data) {

    uint8_t LBits, UBits;
    UBits = (data >> 4) & 0x0F; //upper 4 bits
    LBits = data & 0x0F; //lower 4 bits

    std::cerr << std::bitset<4>(UBits) << " " << std::bitset<4>(LBits);

    drv.setRegister(&PORTA, UBits);
    drv.setRegister(&PORTA, LBits);
}

void DriverManager::SendFlag(const uint8_t FLAG) {
    SetToNull();
    
    bool ACK_Recieved = false;
    std::cerr << "DriverManager > Sending Flag (" << std::bitset<8>(FLAG) << ")" << std::endl;

    std::cerr << "DriverManager > Waiting for ACK..." << std::endl;
    while (!ACK_Recieved) {
        drv.setRegister(&PORTA, FLAG);
        if (ReadData() == ACK) {
            ACK_Recieved = true;
            std::cerr << std::endl << "DriverManager > ACK recieved!" << std::endl;
        }
    }
}

uint8_t DriverManager::ReadData() {

    uint8_t Bits_DRV = drv.getRegister(&PINA);
    drv.reverse(Bits_DRV);
    uint8_t inverted_Bits = Bits_DRV  & 0x0F; //IMPORTANT FOR MIRRORED DATA !!!
    //std::cerr << std::bitset<4>(inverted_Bits) << " ";

    return inverted_Bits; //invert bits so they are not mirrored
}

int DriverManager::GetPackageSize() {
    std::cerr << "DriverManager > Returned package size: " << PackageSize << std::endl;
    return PackageSize;
}

void DriverManager::SetToNull() {
    drv.setRegister(&PORTA, 0x00);
}

void DriverManager::Wait(uint16_t  ms) {
    drv.delay_ms(ms);
}