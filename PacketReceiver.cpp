//
// Created by adam on 30/11/2019.
//

#include <PcapLiveDevice.h>
#include <iostream>
#include <functional>
#include "PacketReceiver.h"
#include "TcpLayer.h"

PacketReceiver::PacketReceiver(PacketQueue& packetQueue, zmq::context_t& context)
:  m_packetQueue(packetQueue), m_context(context), m_socket(m_context, ZMQ_REQ) {
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
        std::string message = "Give me configuration!";
        zmq::message_t request(message.size());
        memcpy(request.data(), message.data(), message.size());
        m_socket.send(request);

        zmq::message_t reply;
        m_socket.recv(&reply);
        std::cout << "Received replay: " << static_cast<const char*>(reply.data()) << std::endl;
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

