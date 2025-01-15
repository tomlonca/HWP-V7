#include "./headers/Reciever.h"
#include "./headers/DriverManager.h"
#include "./headers/Flags.h"
#include <fstream>

Reciever::Reciever(DriverManager &drvm, std::string InputFileName) : drvm(drvm) {
    std::fstream InputFile(InputFileName, std::ios::binary);
    if (!InputFile) {
        std::cerr << "Error opening file: " << InputFileName << std::endl;
    }
}

void Reciever::WaitforFlag(const uint8_t &FLAG) {
    bool FlagRecieved = false;

    while (!FlagRecieved) {
        if (drvm.ReadData() == FLAG)
            FlagRecieved = true;
    }

    drvm.SendData(ACK); //acknowledge flag
}

void Reciever::StartCommunication() {

    std::cout << "Reciever > Communication protocol started!" << std::endl;

    WaitforFlag(SYC);

    std::cout << "Reciever > Synchronization done." << std::endl;
    
    std::cout << "Reciever > Waiting for SOT signal..." << std::endl; 
    WaitforFlag(SOT);
    std::cout << "Reciever > SOT signal recieved. Starting to read data." << std::endl;
    
    GetData();

    std::cout << "Reciever > Communication ended. " << std::endl;
}


uint8_t Reciever::GetCRC() {
    WaitforFlag(CRC_F);
    uint8_t U_R_CRC = drvm.ReadData();
    uint8_t L_R_CRC = drvm.ReadData();

    return ((U_R_CRC << 4) | L_R_CRC); //combine both signals recieved
}

int Reciever::GetPackageSizeFromWriter() {
    WaitforFlag(SIZE_F);
    uint8_t U_R_PCKG = drvm.ReadData();
    uint8_t L_R_PCKG = drvm.ReadData();

    return static_cast<int>(((U_R_PCKG << 4) | L_R_PCKG));
}

void Reciever::GetData() {

    uint8_t R_CRC = GetCRC();
    int PackageSize = static_cast<int>(GetPackageSizeFromWriter());
    
    std::string dataStr;

    for (int i = 0; i < PackageSize; i++) {
        uint8_t UBits = drvm.ReadData();
        uint8_t LBits = drvm.ReadData();

        dataStr.append(std::bitset<4>(UBits).to_string());
        dataStr.append(std::bitset<4>(LBits).to_string());
    }

    std::cout << "Reciever > Package read. Calculating CRC..." << std::endl;

    uint8_t CalculatedCRC = CalculateCRC8(dataStr);

    if (CalculatedCRC == R_CRC) {
        drvm.SendFlag(ACK); //send ACK that data was correctly sent
        std::cout << "Reciever > RC check passed. Data received correctly." << std::endl;
        if (InputFile.is_open()) {
            InputFile << dataStr;
            std::cout << "Reciever > Data stored in InputFile." << std::endl;
        } else {
        std::cerr << "Reciever > Error: InputFile is not open." << std::endl;
    }
    } else {
        drvm.SendFlag(NACK); //NACK requests message again
        std::cerr << "Reciever > CRC check failed. Data corrupted. Requesting again" << std::endl;
        GetData(); //activate read data
    }
}

uint8_t Reciever::CalculateCRC8(const std::string &binaryStr) {
    uint8_t polynomial = 0x07; //Standard polynomial for CRC-8 (e.g., x^8 + x^2 + x + 1 => 0x07)
    uint8_t CRC = 0x00; // Initial CRC value

    for (size_t i = 0; i < binaryStr.length(); ++i) {
        bool inputBit = (binaryStr[i] == '1'); // Convert '1'/'0' to boolean
        CRC = (CRC << 1) | inputBit; //Shift CRC to the left and add the input bit
    
        //If the most significant bit is 1, apply the polynomial
        if ((CRC & 0x80) != 0) {
            CRC ^= polynomial; //Apply polynomial
        }
    }
    std::cout << "Reciever > CRC Calculated: " << CRC << "(" << static_cast<int>(CRC) << ")" << std::endl;
    return CRC;
}