#include <iostream>
#include "PacketQueue.h"
#include "PacketReceiverThread.h"

void DumpBuffer(const uint8_t* buffPtr, size_t length){
    std::cout << "BUFFER_BEGIN" << std::endl;
    for(int packetIndex = 0; length;  ++packetIndex){
        printf("%02X ", buffPtr[packetIndex]);
    }
    std::cout << "BUFFER_END" << std::endl;
}

int main(int argc, char **argv)
{
    PacketQueue packetQueue;
    PacketReceiverThread packetReceiver(packetQueue);
    packetReceiver.Start();

    while(true){ }

	return 0;
}
