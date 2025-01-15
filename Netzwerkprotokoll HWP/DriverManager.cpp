#include "DriverManager.h"

DriverManager::DriverManager(B15F & drv, bool isWriter) {
    this->drv = drv;
    isWriter_DM = isWriter;
    PackageSize = 32;

    if(isWriter) {
        drv.setRegister(&DDRA, 0x0F); //left 4 pins input, 4 right pins output
        NowWriting = true;
        NowReading = false;
        std::cout << "Driver Manager > Writer Instance created!" << std::endl;
    } else {
        drv.setRegister(&DDRA, 0xF0); //left 4 pins output, 4 right pins input
        NowWriting = false;
        NowReading = true
        std::cout << "Driver Manager > Reader Instance created!" << std::endl;
    }
}

/* void DriverManager::Synch(bool isWriter) {
    std::cout << "DriverManager > Starting Synchronization..." << std::endl;
    if (isWriter) {
        try {
            SendFlag(SYC);
            std::cout << "DriverManager > Synchronization done." << std::endl;
        } catch (const std::runtime_error& e) {
            std::cerr << "DriverManager > Synchronization failed: " << e.what() << std::endl;
        }
    }
    else {
        StandBy();
    }
} */

void DriverManager::SendData(uint8_t data) {

    /* std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this]{ return NowWriting; }); */

    uint8_t LBits, UBits;
    UBits = (data >> 4) & 0x0F; //upper 4 bits
    LBits = data & 0x0F; //lower 4 bits

    drv.setRegister(&PORTA, UBits);
    drv.delay_ms(5);  //useful? who knows idc
    drv.setRegister(&PORTA, LBits);

    /* NowWriting = false;
    NowReading = true;
    cv.notify_all(); */
}

void DriverManager::SendFlag(const uint8_t FLAG) {
    /* std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this]{ return NowWriting; }); */
    
    bool ACK_Recieved = false;
    std::cout << "DriverManager > Sending Flag (" << static_cast<int>(FLAG) << ")" << std::endl;

    while (!ACK_Recieved) {
        drv.setRegister(&PORTA, FLAG);
        drv.delay_ms(10);
        drv.setRegister(&PORTA, 0x00);
        std::cout << "DriverManager > Waiting for ACK..." << std::endl;
        if (drvm.ReadData() == ACK)
            ACK_Recieved = true;
    }

    /* NowWriting = false;
    NowReading = true; */
    /* cv.notify_all(); */
}

uint8_t DriverManager::ReadData() {
    /* std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this]{ return NowReading; }); */

    uint8_t Bits_DRV = drv.getRegister(&PORTA):
    uint8_t inverted_Bits = drvm.reverse(Bits_DRV) & 0x0F; //IMPORTANT FOR MIRRORED DATA !!!
    std::cout << "DriverManager > Recieved Data: " << inverted_Bits << std::endl;

    /* NowReading = false;
    NowWriting = true;
    cv.notify_all(); */

    return inverted_Bits; //invert bits so they are not mirrored
}


int DriverManager::GetPackageSize() {
    return PackageSize;
}