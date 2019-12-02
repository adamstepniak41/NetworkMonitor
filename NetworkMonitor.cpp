#include <iostream>
#include "PacketQueue.h"
#include "PacketReceiver.h"
#include "PacketProcessor.h"
#include <zmq.h>

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
    zmq::context_t context(1);

    PacketProcessor packetProcessor(packetQueue, context);
    packetProcessor.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    PacketReceiver packetReceiver(packetQueue,context);
    packetReceiver.Start();

    while(true){ }

	return 0;
}
