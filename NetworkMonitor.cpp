#include <iostream>
#include "CaptureReceiver.h"

void DumpBuffer(const uint8_t* buffPtr, size_t length){
    std::cout << "BUFFER_BEGIN" << std::endl;
    for(int packetIndex = 0; length;  ++packetIndex){
        printf("%02X ", buffPtr[packetIndex]);
    }
    std::cout << "BUFFER_END" << std::endl;
}

int main(int argc, char **argv)
{
    CaptureReceiver captureReceiver("wlp7s0");
    captureReceiver.Start();

    while(true){ }

	return 0;
}
