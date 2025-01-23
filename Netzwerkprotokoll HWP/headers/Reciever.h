#ifndef RCVR_H
#define RCVR_H

#include "Flags.h"
#include "DriverManager.h"
//#include "Message.h"

#include <chrono>
#include <string.h>
#include <fstream>
#include <condition_variable>
#include <b15f/b15f.h>
#include <mutex>

class Reciever {
    private:
        DriverManager& drvm;
        std::ifstream InputFile;
        std::string data;
        
    public:
        Reciever(DriverManager &drvm, std::string InputFileName);
        void SaveStringInFile(std::string text);
        void StartCommunication();
        void Synch();
        void getFlag();
        uint8_t CalculateCRC8(const std::vector<uint8_t>& data);
        void WaitforFlag(const uint8_t &FLAG);
        uint8_t GetCRC();
        int GetPackageSizeFromWriter();
        void GetData(bool &isFinished);
        void Invert(uint8_t &value);
};



#endif