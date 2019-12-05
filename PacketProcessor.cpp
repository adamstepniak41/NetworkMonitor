//
// Created by adam on 01/12/2019.
//

#include <functional>
#include "PacketProcessor.h"
#include <iostream>
#include "Messages.h"

PacketProcessor::PacketProcessor(PacketQueue &packetQueue, zmq::context_t& context)
: m_packetQueue(packetQueue), m_context(context), m_socket(m_context, ZMQ_SUB)
{}


void PacketProcessor::MainLoop() {
    while(m_runThread){
        auto packet = m_packetQueue.Dequeue();
        std::cout << packet->toString() << std::endl;

        //Wait for next request from client
        zmq::message_t request;
        m_socket.recv(&request);

        PacketMetadata* metadata = static_cast<PacketMetadata*>(request.data());
        std::cout << "Received metadata object with type: " << +metadata->m_type << " and size: " << metadata->m_size << std::endl;
    }
}

void PacketProcessor::OnThreadStarting() {
    std::cout << "Binding inproc://my_publisher" << std::endl;
    uint8_t subscription = 1;
    m_socket.setsockopt(ZMQ_SUBSCRIBE, &subscription, sizeof(subscription));
    m_socket.bind("inproc://my_publisher");
}