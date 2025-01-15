#ifndef RCVR_H
#define RCVR_H

#include "Flags.h"
#include "DriverManager.h"
#include "Message.h"

#include <fstream>
#include <condition_variable>
#include <mutex>

class Reciever {
    private:
        bool isWriter_R;
        DriverManager& drvm;
        Message r_msg;
        std::fstream InputFile;
        int package_counter;
        int package_amount;
        
    public:
        Reciever(DriverManager &drvm, std::fstream InputFile);
        void SaveStringInFile(std::string text);
        void StartCommunication();
        void Synch();
        void getFlag();
        void GetData();
}

#endif