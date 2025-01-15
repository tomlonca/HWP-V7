#ifndef DRVMNGR_H
#define DRVMNGR_H

#include <b15f/b15f.h>
#include <stdint.h>

#include "Flags.h"

class DriverManager {
    private:
        bool isWriter_DM;
        B15F& drv;
        bool NowWriting;
        bool NowReading;
        const int PackageSize;
        std::mutex mutex;
        std::condition_variable cv;

    public:
        DriverManager(B15F &drv, bool isWriter);
        
        uint8_t ReadData();
        void SendData(uint8_t data);
        
        void Synch();
        bool SendFlag(const uint8_t FLAG); //return true when ACK is recieved. false when NACK is recieved
        uint8_t GetFlag();
}

#endif