#ifndef WRTR_H
#define WRTR_H

#include <Flags.h>
#include <DriverManager.h>
#include <fstream>
#include <Message.h>
#include <condition_variable>
#include <mutex>

class Writer {
    private:
        bool isWriter_W;
        DriverManager& drvm;
        Message& msg;
        //std::vector<std::string> packages;

    public:
        Writer(B15F & drvm, bool isWriter, std::fstream OutputFile, Message &msg);
        void StartCommunication();
        void Synch();
        void Write(uint8_t CRC, Message msg);
        uint8_t getCRC();
};

#endif