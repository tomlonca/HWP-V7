#include "DriverManager.h"
#include "Message.h"
#include "Writer.h"
#include "Reciever.h"
#include "Flags.h"

#include <fstream>
#include <string.h>
#include <thread>

bool isWriter = false;

int main(int argc, char* argv[]) {

    std::string text;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-w") == 0) {
            isWriter = true;
        } else if (strcmp(argv[i], "-r") == 0) {
            isWriter = false;
        }
    }

    if (argc > 1 && strcmp(argv[1], "-r") != 0 && strcmp(argv[1], "-w") != 0) {
        // If a filename is provided, read from the file
        std::string filename = argv[1];
        std::ifstream OutputFile(filename, std::ios::binary);
        if (!OutputFile) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return 1;
        }
        // Read the entire file into 'text'
        std::ostringstream oss;
        oss << OutputFile.rdbuf();
        text = oss.str();
        std::cerr << "Read " << text.size() << " bytes from file: " << filename << std::endl;
    } else {
        // No filename provided, read from stdin
        std::ostringstream oss;
        oss << std::cin.rdbuf();
        text = oss.str();
        std::cerr << "Read " << text.size() << " bytes from stdin." << std::endl;
    }

    //Create DriverManager
    B15F &drv = B15F::getInstance();
    DriverManager drvm = DriverManager(drv, isWriter);

    if (isWriter) {
        Message msg = Message(text);
        Writer writer = Writer(drvm, msg);
        writer.StartCommunication();
    } 
    else {
        std::string filename = "InputFile.txt"
        std::ifstream InputFile(filename, std::ios::binary);
        if (!InputFile) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return 1;
        }
        Reciever reciever = Reviever(drvm, InputFile); //File to store recieved data
        reciever.StartCommunication();
    }
}