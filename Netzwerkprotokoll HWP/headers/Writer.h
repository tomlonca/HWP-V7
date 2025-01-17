#ifndef WRTR_H
#define WRTR_H

#include "Flags.h"
#include "DriverManager.h"
#include "Message.h"

#include <fstream>
#include <condition_variable>
#include <mutex>

class Writer {
    private:
        bool isWriter_W;
        DriverManager& drvm;
        Message& msg;

    public:
        Writer(DriverManager &drvm, bool isWriter, Message &msg); // Ändern Sie den Typ des ersten Parameters
        void StartCommunication();
        void Synch();
        void Write(uint8_t CRC, Message msg);
        uint8_t getCRC();
        int CalculatePackagesAmount();
        void SendMessage(bool &isFinished);
        std::vector<int> CalculatePackageSizes(int PackagesAmount);
        void SendPackage(const std::vector<uint8_t>& packageData, int packageSize, uint8_t crc, int i);
        void HandleAcknowledgement(const std::vector<uint8_t>& packageData, int packageSize, uint8_t crc, int i);
};

#endif