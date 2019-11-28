//
// Created by adam on 28/11/2019.
//

#include <mutex>
#include "PacketQueue.h"

void PacketQueue::Enqueue(std::unique_ptr<pcpp::Packet> packet) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_packets.push(std::move(packet));
    m_cv.notify_all();
}

std::unique_ptr<pcpp::Packet> PacketQueue::Dequeue(std::unique_ptr<pcpp::Packet> packet) {
    if(m_packets.empty()){
        std::unique_lock<std::mutex> cvLock(m_mutex);
        m_cv.wait(cvLock, [this]{ return m_notEmpty; });
    }

    std::lock_guard<std::mutex> packetsLock(m_mutex);
    auto outPacket = std::move(m_packets.front());
    m_packets.pop();

    return outPacket;
}

PacketQueue::PacketQueue() : m_notEmpty(false) {}
