#include "./headers/Reciever.h"
#include "./headers/DriverManager.h"
#include "./headers/Flags.h"

Reciever::Reciever(DriverManager &drvm, std::string InputFileName) : drvm(drvm) {
    InputFile.open(InputFileName, std::ios::binary);
    if (!InputFile.is_open()) {
        std::cerr << "Reciever > Error opening InputFile " << std::endl;
    }
}

void Reciever::WaitforFlag(const uint8_t &FLAG) {
    std::cerr << "Reciever > Waiting for Flag " << static_cast<int>(FLAG) << std::endl;
    bool FlagRecieved = false;

    while (!FlagRecieved) {
        if (drvm.ReadData() == FLAG)
            FlagRecieved = true;
    }

    drvm.SendData(ACK); //acknowledge flag
    //drvm.SetToNull();
}

void Reciever::StartCommunication() {

    std::cerr << "Reciever > Communication protocol started!" << std::endl;

    WaitforFlag(SYC);

    std::cerr << "Reciever > Synchronization done." << std::endl;
    
    std::cerr << "Reciever > Waiting for SOT signal..." << std::endl; 
    WaitforFlag(SOT);
    std::cerr << "Reciever > SOT signal recieved. Starting to read data." << std::endl;
    
    GetData();

    std::cerr << "Reciever > Communication ended. " << std::endl;
    std::cout << data << std::endl;
}


uint8_t Reciever::GetCRC() {
    std::cerr << "Reciever > Expecting CRC" << std::endl;
    WaitforFlag(CRC_F);
    uint8_t U_R_CRC = drvm.ReadData();
    uint8_t L_R_CRC = drvm.ReadData();

    std::cerr << "Reciever > CRC recieved." << std::endl;
    return ((U_R_CRC << 4) | L_R_CRC); //combine both signals recieved
}

int Reciever::GetPackageSizeFromWriter() {
    std::cerr << "Reciever > Expecting Package size" << std::endl;
    WaitforFlag(SIZE_F);
    uint8_t U_R_PCKG = drvm.ReadData();
    uint8_t L_R_PCKG = drvm.ReadData();

    std::cerr << "Reciever > Package size recieved" << std::endl;
    return static_cast<int>(((U_R_PCKG << 4) | L_R_PCKG));
}

void Reciever::GetData() {

    uint8_t R_CRC = GetCRC();
    int PackageSize = static_cast<int>(GetPackageSizeFromWriter());
    WaitforFlag(GS);
    
    std::string dataStr;
    
    std::cerr << "Reciever > Reading data..." << std::endl;
    for (int i = 0; i < PackageSize; i++) {
        uint8_t UBits = drvm.ReadData();
        uint8_t LBits = drvm.ReadData();

        dataStr.append(std::bitset<4>(UBits).to_string());
        dataStr.append(std::bitset<4>(LBits).to_string());
    }

    std::cerr << "Reciever > Package read. Calculating CRC..." << std::endl;
    drvm.SetToNull();

    uint8_t CalculatedCRC = CalculateCRC8(dataStr);

    if (CalculatedCRC == R_CRC) {
        drvm.SendData(ACK); //send ACK that data was correctly sent
        std::cerr << "Reciever > RC check passed. Data received correctly." << std::endl;
        
        data.append(dataStr);
        std::cerr << "Reciever > Data stored in data variable." << std::endl;

        if (drvm.ReadData() == EOT) {
            std::cerr << "Reciever > EOT flag received. No more data to read." << std::endl;
            return;
        }
    }
    else {
        std::cerr << "Reciever > CRC check failed. Data corrupted. Requesting again" << std::endl;
        drvm.SendData(NACK); //NACK requests message again
        GetData(); //activate read data
    }
    std::cerr << "Reciever > Nothing else to receive. Terminating" << std::endl;
}

uint8_t Reciever::CalculateCRC8(const std::string &binaryStr) {
    std::cerr << "Reciever > Calculating own CRC" << std::endl;
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
    std::cerr << "Reciever > CRC Calculated: " << CRC << "(" << static_cast<int>(CRC) << ")" << std::endl;
    return CRC;
}