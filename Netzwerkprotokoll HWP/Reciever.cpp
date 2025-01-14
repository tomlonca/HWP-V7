#include <Reciever.h>

Reciever::Reciever(DriverManager &drvm, std::fstream InputFile) {
    isWriter_W = false;
    InputFile = InputFile;
    drvm = drvm;
}

/* void Reciever::StandBy() {
    // Waits for SYC from the other PC, which indicates it is ready to write
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this]{ return NowReading; });

    std::cout << "Reciever > Waiting for SYC FLAG..." << std::endl;
    WaitforFlat(SYC);

    NowWriting = true;
    NowReading = false;
    cv.notify_all();

    StartCommunication();
} */

void Reciever::StartCommunication() {

    std::cout << "Reciever > Communication protocol started!" << std::endl;

    WaitforFlag(SYC);

    std::cout << "Reciever > Synchronization done." << std::endl;
    
    std::cout << "Reciever > Waiting for SOT signal..." << std::endl; 
    WaitforFlag(SOT);
    std::cout << "Reciever > SOT signal recieved. Starting to read data." << std::endl;
    
    GetData();

    std::cou << "Reciever > Communication ended. " << std::endl;
}

void Reciever::WaitforFlag(const uint8_t &FLAG) {
    bool FlagRecieved = false;

    while (!FlagRecieved) {
        if (drvm.ReadData() == FLAG)
            FlagRecieved = true;
    }

    drvm.SendData(ACK); //acknowledge flag
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

    uint8_t ((U_R_PCKG << 4) | L_R_PCKG);
}

void Reciever::GetData() {

    uint8_t R_CRC = GetCRC();
    int PackageSize = static_cast<int>(GetPackageSizeFromWriter());
    
    std::string dataStr;

    for (int i = 0; i < drvm.PackageSize; i++) {
        uint8_t UBits = drvm.ReadData();
        uint8_t LBits = drvm.ReadData();

        dataStr.push_back(std::bitset<4>(UBits));
        dataStr.push_back(std::bitset<4>(LBits));
    }

    std::cout << "Reciever > Package read. Calculating CRC..." << std::endl;

    uint8_t CalculatedCRC = CalculateCRC(dataStr);

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