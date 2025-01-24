#include "./headers/DriverManager.h"
#include "./headers/Writer.h"
#include "./headers/Reciever.h"
#include "./headers/Flags.h"

#include <fstream>
#include <string.h>
#include <thread>

bool isWriter = false;

int main(int argc, char* argv[]) {
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-w") == 0) {
            isWriter = true;
        } else if (strcmp(argv[i], "-r") == 0) {
            isWriter = false;
        }
    }

    std::string text;

    if (argc > 1 && strcmp(argv[1], "-r") != 0 && strcmp(argv[1], "-w") != 0) {
        // If a filename is provided, read from the file
        std::string filename = argv[1];
        std::ifstream OutputFile(filename, std::ios::binary);
        if (!OutputFile) {
            std::cerr << "Main > Error opening file: " << filename << std::endl;
            return 1;
        }
        // Read the entire file into 'text'
        std::ostringstream oss;
        oss << OutputFile.rdbuf();
        text = oss.str();
        std::cerr << "Main > Read " << text.size() << " bytes from file: " << filename << std::endl;
    } else if (isWriter) {
        // No filename provided, read from stdin
        std::ostringstream oss;
        oss << std::cin.rdbuf();
        text = oss.str();
        std::cerr << "Main > Read " << text.size() << " bytes from stdin." << std::endl;
    }

    //Create DriverManager
    B15F &drv = B15F::getInstance();
    drv.setRegister(&PORTA, 0x00);
    DriverManager drvm = DriverManager(drv, isWriter);
    std::string filename = "InputFile.txt";
    if (isWriter) {
        Message msg = Message(text);
        Writer writer = Writer(drvm, isWriter ,msg);
        writer.StartCommunication();
    } 
    else {
        std::string filename = "InputFile.txt";
        Reciever reciever = Reciever(drvm); //File to store recieved data
        reciever.StartCommunication();
    }

    return 0;
}