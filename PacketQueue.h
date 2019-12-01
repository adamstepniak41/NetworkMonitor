//
// Created by adam on 28/11/2019.
//

#ifndef NETWORKMONITOR_PACKETQUEUE_H
#define NETWORKMONITOR_PACKETQUEUE_H


#include <memory>
#include <Packet.h>
#include <queue>
#include <condition_variable>

//Thread safe packet queue
class PacketQueue {
public:
    void Enqueue(std::unique_ptr<pcpp::Packet> packet);
    std::unique_ptr<pcpp::Packet> Dequeue();
private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<std::unique_ptr<pcpp::Packet> > m_packets;
};

#endif //NETWORKMONITOR_PACKETQUEUE_H
