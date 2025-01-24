#include "./headers/Reciever.h"
#include "./headers/DriverManager.h"
#include "./headers/Flags.h"

Reciever::Reciever(DriverManager &drvm, std::string InputFileName) : drvm(drvm) {
    InputFile.open(InputFileName, std::ios::binary | std::ios::in);
    if (!InputFile.is_open()) {
        std::cerr << "Reciever > Error opening InputFile " << std::endl;
    }
    drvm.SetToNull();
}

void Reciever::WaitforFlag(const uint8_t &FLAG) {
    std::cerr << "Reciever > Waiting for Flag... " << std::bitset<8>(FLAG) << std::endl;
    bool FlagRecieved = false;
    auto start = std::chrono::steady_clock::now();

    while (!FlagRecieved) {
        if (drvm.ReadData() == FLAG) {
            FlagRecieved = true;
            std::cerr << std::endl;
        } else {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
            if (elapsed > 2) {
                std::cerr << "Reciever > Resending ACK flag after 2 seconds of waiting." << std::endl;
                drvm.SendData(ACK);
                start = std::chrono::steady_clock::now(); // Reset the timer
            }
        }
    }
    drvm.SendData(ACK); // acknowledge flag
    drvm.Wait(3);
    drvm.SetToNull();
}

void Reciever::StartCommunication() {

    std::cerr << "Reciever > Communication protocol started!" << std::endl;

    WaitforFlag(SYC);

    std::cerr << "Reciever > Synchronization done." << std::endl;
    
    std::cerr << "Reciever > Waiting for SOT signal..." << std::endl; 
    WaitforFlag(SOT);
    std::cerr << "Reciever > SOT signal recieved. Starting to read data." << std::endl;
    
    bool isFinished = false;
    while (!isFinished) {
        GetData(isFinished);
    }

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

    int pckg_sz = ((U_R_PCKG << 4) | L_R_PCKG) +1 ;

    std::cerr << std::endl << "Reciever > Package size recieved: "<< pckg_sz  << std::endl;
    return pckg_sz;
}

void Reciever::GetData(bool &isFinished) {
    uint8_t R_CRC = GetCRC();
    std::cerr << "Reciever > Received CRC from Writer: " << std::bitset<8>(R_CRC) << std::endl;
    
    int PackageSize = static_cast<int>(GetPackageSizeFromWriter());
    drvm.SetToNull();
    std::cerr << "Reciever > Reading data..." << std::endl;
    
    std::cerr << "Reciever > Expecting version 1" << std::endl;
    std::vector<uint8_t> Version1 = GetRepetition(PackageSize);
    std::cerr << "Reciever > Version 1 recieved, continue with version 2" << std::endl;
    std::vector<uint8_t> Version2 = GetRepetition(PackageSize);
    std::cerr << "Reciever > Version 2 recieved, continue with version 3" << std::endl;
    std::vector<uint8_t> Version3 = GetRepetition(PackageSize);
    std::cerr << "Reciever > Version 3 recieved" << std::endl;

    std::vector<uint8_t> FinalVersion = CompareReps(Version1, Version2, Version3);

    drvm.SetToNull();
    std::cerr << "Reciever > Package read. Calculating CRC..." << std::endl;

    if (CalculateCRC8(FinalVersion) == R_CRC) {
        drvm.SendData(ACK); //send ACK that data was correctly sent
        drvm.Wait(2);
        drvm.SetToNull();
        std::cerr << "Reciever > RC check passed. Data received correctly." << std::endl;
        
        std::string dataStr(FinalVersion.begin(), FinalVersion.end());
        data.append(dataStr);
        std::cerr << "Reciever > Data stored in data variable." << std::endl;
        std::cout << data << std::endl;
        InputFile << data;

        if (drvm.ReadData() == EOT) {
            std::cerr << std::endl <<"Reciever > EOT flag received. No more data to read." << std::endl;
            isFinished = true;
            return;
        }
    }
    else {
        std::cerr << "Reciever > CRC check failed. Data corrupted. Requesting again" << std::endl;
        drvm.SendData(NACK); //NACK requests message again
        drvm.Wait(2);
        drvm.SetToNull();
        GetData(isFinished); //activate read data
    }
}

uint8_t Reciever::CalculateCRC8(std::vector<uint8_t> &vector) {
    uint8_t crc = 0;
    for (size_t i = 0; i < vector.size(); ++i) {
        crc ^= vector[i];
        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    std::cerr << "Reciever > Calculated CRC: " << std::bitset<8>(crc) << std::endl;
    return crc;
}

std::vector<uint8_t> Reciever::GetRepetition(int &PackageSize) {
    WaitforFlag(RS);
    std::vector<uint8_t> receivedData;

    for (int i = 0; i < PackageSize; i++) {

        uint8_t UBits = drvm.ReadData();
        uint8_t LBits = drvm.ReadData();

        //std::cerr << "Reciever > " << std::bitset<8>(UBits) << " " << std::bitset<8>(LBits) << std::endl;
        //ReverseBits(LBits);

        uint8_t comb = (UBits << 4)| LBits;
        std::cerr << std::bitset<8>(comb) << std::endl;
        receivedData.push_back(comb); // Combine UBits and LBits
    }
    return receivedData;
}

std::vector<uint8_t> Reciever::CompareReps(std::vector<uint8_t> &Version1, std::vector<uint8_t> &Version2, std::vector<uint8_t> &Version3) {
    std::vector<uint8_t> result;
    size_t size = Version1.size(); //All three vectors are the same size

    for (size_t i = 0; i < size; ++i) {
        uint8_t v1 = Version1[i];
        uint8_t v2 = Version2[i];
        uint8_t v3 = Version3[i];

        if (v1 == v2 || v1 == v3) {
            result.push_back(v1);
        } else if (v2 == v3) {
            result.push_back(v2);
        } else {
            //Compare each individual bit of the elements
            uint8_t consensus = 0;
            for (int bit = 0; bit < 8; ++bit) {
                int count1 = (v1 >> bit) & 1;
                int count2 = (v2 >> bit) & 1;
                int count3 = (v3 >> bit) & 1;

                int sum = count1 + count2 + count3;
                if (sum >= 2) {
                    consensus |= (1 << bit);
                }
            }
            result.push_back(consensus);
        }
    }
    return result;
}

void Reciever::ReverseBits(uint8_t &value) {
    uint8_t original = value;
    uint8_t reversed = 0;

    for (int i = 0; i < 8; i++) {
        uint8_t lsb = original & 1;
        reversed = (reversed << 1) | lsb;
        original >>= 1;
    }

    value = reversed;
}