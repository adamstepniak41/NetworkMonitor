#ifndef NETWORKMONITOR_THREADSAFEQUEUE_H
#define NETWORKMONITOR_THREADSAFEQUEUE_H

#include <memory>
#include <Packet.h>
#include <queue>
#include <condition_variable>

template<class QueueDataType>
class ThreadSafeQueue {
public:
    void Enqueue(QueueDataType data);
    QueueDataType Dequeue();
    QueueDataType TryDequeue();
private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<QueueDataType > m_packets;
};

template<class QueueDataType>
void ThreadSafeQueue<QueueDataType>::Enqueue(QueueDataType data) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_packets.push(std::move(data));
    m_cv.notify_one();
}

template<class QueueDataType>
QueueDataType ThreadSafeQueue<QueueDataType>::Dequeue() {
    std::unique_lock<std::mutex> cvLock(m_mutex);

    while(m_packets.empty()){
        m_cv.wait(cvLock);
    }

    auto outPacket = std::move(m_packets.front());
    m_packets.pop();

    return std::move(outPacket);
}

template<class QueueDataType>
QueueDataType ThreadSafeQueue<QueueDataType>::TryDequeue() {
    std::unique_lock<std::mutex> cvLock(m_mutex);
    if(m_packets.empty())
        return std::shared_ptr<pcpp::Packet>();

    auto outPacket = std::move(m_packets.front());
    m_packets.pop();

    return std::move(outPacket);
}

#endif //NETWORKMONITOR_THREADSAFEQUEUE_H
