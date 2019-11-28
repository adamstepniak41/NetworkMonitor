#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>
#include <EthLayer.h>
#include <Packet.h>
#include <queue>
#include "PacketQueue.h"


void DumpBuffer(const uint8_t* buffPtr, size_t length){
    std::cout << "BUFFER_BEGIN" << std::endl;
    for(int packetIndex = 0; length;  ++packetIndex){
        printf("%02X ", buffPtr[packetIndex]);
    }
    std::cout << "BUFFER_END" << std::endl;
}

PacketQueue packetQueue;

bool PacketTypeSupported(const pcpp::Packet& packet){
    //#TODO: To Implement
    return true;
}

void OnPacketArrived(pcpp::RawPacket* pPacket, pcpp::PcapLiveDevice* pDevice, void* userCookie){
    auto packet = std::make_unique<pcpp::Packet>(pPacket);

    if(PacketTypeSupported(*packet)){
        packetQueue.Enqueue(std::move(packet));
    }
}

struct CaptureStats{
    uint64_t m_packetsCount;
};


int main(int argc, char **argv)
{
    auto& devices = pcpp::PcapLiveDeviceList::getInstance();
    auto deviceList = devices.getPcapLiveDevicesList();
    
    for(auto pcapDevice : deviceList){
        std::cout << "Device name: " << pcapDevice->getName() << std::endl;
    }

    pcpp::PcapLiveDevice* wlp7s0 = devices.getPcapLiveDeviceByName("wlp7s0");
    if(!wlp7s0){
        std::cout << "Device: wlp7s0 was not found. Aborting ... " << std::endl;
        return 1;
    }

    if(!wlp7s0->open()){
        std::cout << "Device wlp7s0 could not be opened" << std::endl;
        return 1;
    }

    CaptureStats captureStats;

    if(!wlp7s0->startCapture(OnPacketArrived, &captureStats)){
        std::cout << "Unable to start capturing" << std::endl;
        return 1;
    }

    while(true){ }

	return 0;
}