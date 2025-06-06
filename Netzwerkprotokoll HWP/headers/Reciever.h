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
        std::string data;
        
    public:
        Reciever(DriverManager &drvm);
        void SaveStringInFile(std::string text);
        void StartCommunication();
        void Synch();
        void getFlag();
        //uint8_t CalculateCRC8(const std::vector<uint8_t>& data);
        uint8_t CalculateCRC8(std::vector<uint8_t> &vector);
        void WaitforFlag(const uint8_t &FLAG);
        uint8_t GetCRC();
        int GetPackageSizeFromWriter();
        void GetData(bool &isFinished);
        void ReverseBits(uint8_t &value);
        std::vector<uint8_t> GetRepetition(int &PackageSize);
        std::vector<uint8_t> CompareReps(std::vector<uint8_t> &Version1, std::vector<uint8_t> &Version2, std::vector<uint8_t> &Version3);
};

#endif