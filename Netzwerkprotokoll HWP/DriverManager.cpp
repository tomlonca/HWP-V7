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

    drv.setRegister(&PORTA, UBits);
    //drv.delay_ms(5);  //useful? who knows idc
    drv.setRegister(&PORTA, LBits);
}

void DriverManager::SendFlag(const uint8_t FLAG) {
    
    bool ACK_Recieved = false;
    std::cerr << "DriverManager > Sending Flag (" << static_cast<int>(FLAG) << ")" << std::endl;

    std::cerr << "DriverManager > Waiting for ACK..." << std::endl;
    while (!ACK_Recieved) {
        drv.setRegister(&PORTA, FLAG);
        if (ReadData() == ACK)
            ACK_Recieved = true;
    }
}

uint8_t DriverManager::ReadData() {

    uint8_t Bits_DRV = drv.getRegister(&PINA);
    drv.reverse(Bits_DRV);
    uint8_t inverted_Bits = Bits_DRV  & 0x0F; //IMPORTANT FOR MIRRORED DATA !!!
    std::cerr << "DriverManager > Recieved Data: " << std::bitset<8>(inverted_Bits) << std::endl;

    return inverted_Bits; //invert bits so they are not mirrored
}

int DriverManager::GetPackageSize() {
    return PackageSize;
}