//
// Created by adam on 01/12/2019.
//

#include <functional>
#include "PacketProcessor.h"
#include <iostream>

PacketProcessor::PacketProcessor(PacketQueue &packetQueue, zmq::context_t& context)
: m_packetQueue(packetQueue), m_context(context), m_socket(m_context, ZMQ_REP)
{}

void PacketProcessor::MainLoop() {
    while(m_runThread){
        auto packet = m_packetQueue.Dequeue();
        std::cout << packet->toString() << std::endl;

        //Wait for next request from client
        zmq::message_t request;
        m_socket.recv(&request);
        std::cout << "PacketProcessor: Received request: " << static_cast<const char*>(request.data()) << std::endl;

        //Processing request
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        zmq::message_t reply;
        std::string message = "Response with configuration";
        memcpy(reply.data(), message.data(), message.size());
        m_socket.send(reply);
    }
}

void PacketProcessor::OnThreadStarting() {
    std::cout << "Binding inproc://my_publisher" << std::endl;
    m_socket.bind("inproc://my_publisher");
}