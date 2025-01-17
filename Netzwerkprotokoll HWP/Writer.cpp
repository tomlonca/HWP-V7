#include "./headers/Writer.h"
#include "./headers/DriverManager.h"

Writer::Writer(DriverManager &drvm, bool isWriter, Message &msg) : drvm(drvm), msg(msg) { // Ändern Sie den Typ des ersten Parameters
    this->isWriter_W = isWriter;
    drvm.SetToNull();
}

void Writer::StartCommunication() {
    
    std::cerr << "Writer > Communication protocol started!" << std::endl;
    drvm.SendFlag(SYC);
    
    //drvm.Synch(isWriter_W);
    
    if (msg.getMessageSize() == 0) {
        std::cerr << "Writer > There is nothing to send!" << std::endl;
        return;
    }

    bool isFinished = false;
    drvm.SendFlag(SOT); //start of transmission
    while (!isFinished) {
        SendMessage(isFinished);
    }
    drvm.SendFlag(EOT); //end of transmission
}

int Writer::CalculatePackagesAmount() {
    std::cerr << "Writer > Calculating packages amount. " << std::endl;
    int MsgSize = msg.getMessageSize();
    int PckgSize = drvm.GetPackageSize();
    int counter = 0;
    while (MsgSize > 0) {
        if (MsgSize <= PckgSize) {
            counter++;
            MsgSize = 0;
        } else {
            MsgSize -= PckgSize;
            counter++;
        }
    }
    std::cerr << "Writer > total packages: " << counter << std::endl;
    return counter;
}

uint8_t CalculateCRC(std::vector<uint8_t> vector) {
    std::cerr << "Writer > Calculating CRC" << std::endl;
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
    std::cerr << "Writer > Calculated CRC: " << crc << std::endl;
    return crc;
}

void Writer::SendMessage(bool &isFinished) {
    int PackagesAmount = CalculatePackagesAmount();
    std::vector<int> packageSizes = CalculatePackageSizes(PackagesAmount);

    std::cerr << "Writer > Packages Amount: " << PackagesAmount << std::endl;

    for (int i = 0; i < PackagesAmount; i++) {
        std::cerr << "Writer > Sending package Nr. " << i+1 << std::endl;
        std::vector<uint8_t> packageData = msg.getPackageData(i, packageSizes[i]);

        std::cerr << "Writer > Package size: " << packageData.size() << std::endl;

        uint8_t crc = CalculateCRC(packageData);
        std::cerr << "Writer > Calculated CRC for next package: " << static_cast<int>(crc) << std::endl;

        SendPackage(packageData, packageSizes[i], crc, i);
    }

    isFinished = true;
}

std::vector<int> Writer::CalculatePackageSizes(int PackagesAmount) {
    std::cerr << "Writer > Calculating package sizes" << std::endl;
    int remainingSize = msg.getMessageSize();
    int currentPackageSize = 0;
    std::vector<int> packageSizes;

    for (int i = 0; i < PackagesAmount; i++) {
        if (remainingSize >= drvm.GetPackageSize()) {
            currentPackageSize = drvm.GetPackageSize();
        } else {
            currentPackageSize = remainingSize;
        }
        packageSizes.push_back(currentPackageSize);
        remainingSize -= currentPackageSize;
    }
    std::cerr << "Writer > Package sizes calculated successfully!" << std::endl;

    return packageSizes;
}

void Writer::SendPackage(const std::vector<uint8_t>& packageData, int packageSize, uint8_t crc, int i) {
    
    drvm.SendFlag(CRC_F);
    drvm.SendData(crc);

    drvm.SendFlag(SIZE_F);
    drvm.SendData(static_cast<uint8_t>(packageSize));
    
    drvm.SendFlag(GS);
    for (size_t i = 0; i < packageData.size(); i++) {
        drvm.SendData(packageData.at(i));
    }

    drvm.SetToNull();
    HandleAcknowledgement(packageData, packageSize, crc, i);
}

void Writer::HandleAcknowledgement(const std::vector<uint8_t>& packageData, int packageSize, uint8_t crc, int i) {
    bool ackReceived = false;

    while (!ackReceived) {
        uint8_t response = drvm.ReadData();
        if (response == ACK) {
            std::cerr << "Writer > Package recieved correctly. continue..." << std::endl;
            ackReceived = true;
        } else if (response == NACK) {
            std::cerr << "Writer > NACK recieved. Resending last package!" << std::endl;
            std::cerr << "Writer > Resending package Nr. " << i << std::endl;
            std::cerr << "Writer > Resending CRC" << std::endl;
            drvm.SendFlag(CRC_F);
            drvm.SendData(crc);
            std::cerr << "Writer > Resending package size" << std::endl;
            drvm.SendFlag(SIZE_F);
            drvm.SendData(static_cast<uint8_t>(packageSize));
            std::cerr << "Writer > Ready to send data!" << std::endl;
            drvm.SendFlag(GS);
            for (size_t i = 0; i < packageData.size(); i++) {
                drvm.SendData(packageData.at(i));
            }
        }
    }
}