//
// Created by adam on 30/11/2019.
//

#include <PcapLiveDevice.h>
#include <iostream>
#include <functional>
#include "PacketReceiver.h"
#include "TcpLayer.h"
#include "Messages.h"

PacketReceiver::PacketReceiver(PacketQueue& packetQueue, zmq::context_t& context)
:  m_packetQueue(packetQueue), m_context(context), m_socket(m_context, ZMQ_PUB) {
}

void PacketReceiver::OnPacketArrived(pcpp::RawPacket* pPacket, pcpp::PcapLiveDevice* pDevice, void* userCookie){
    auto packet = std::make_unique<pcpp::Packet>(pPacket);
    auto packetReceiver = static_cast<PacketReceiver*>(userCookie);

    if(packetReceiver->PacketTypeSupported(*packet)){
        std::cout << "Packet inserted to the queue" << std::endl;
        packetReceiver->m_packetQueue.Enqueue(std::move(packet));
    }
}

bool PacketReceiver::PacketTypeSupported(pcpp::Packet &packet) {
    if(packet.isPacketOfType(pcpp::TCP)){
        return true;
    }

    return false;
}

void PacketReceiver::MainLoop() {
    while(m_runThread){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        //Send a request to packet receiver
        PacketMetadata metadata;
        zmq::message_t request(sizeof(PacketMetadata));
        memcpy(request.data(), &metadata, sizeof(PacketMetadata));
        m_socket.send(request);

        //Send a request with another parameters
        PacketMetadata metadata2;
        metadata2.m_size = 30;
        metadata2.m_type = 40;
        zmq::message_t request2(sizeof(PacketMetadata));
        memcpy(request2.data(), &metadata2, sizeof(PacketMetadata));
        m_socket.send(request2);
    }
}

void PacketReceiver::StartCapturing() {
    auto& devices = pcpp::PcapLiveDeviceList::getInstance();
    auto deviceList = devices.getPcapLiveDevicesList();

    pcpp::PcapLiveDevice* wlp7s0 = devices.getPcapLiveDeviceByName("wlp7s0");
    if(!wlp7s0)
        std::runtime_error("Device: wlp7s0 was not found.");

    if(!wlp7s0->open())
        std::runtime_error("Device wlp7s0 could not be opened.");

    if(!wlp7s0->startCapture(OnPacketArrived, this))
        std::runtime_error("Unable to start capturing.");
}

void PacketReceiver::OnThreadStarting() {
    std::cout << "Connecting inproc://my_publisher" << std::endl;
    m_socket.connect("inproc://my_publisher");

    StartCapturing();
}

