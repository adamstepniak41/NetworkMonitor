#pragma  once

#include <PcapLiveDeviceList.h>
#include <set>
#include <map>
#include "ThreadSafeQueue.h"
#include "Thread.h"
#include "Interfaces.h"

using PacketObserverShPtr = std::shared_ptr<IObserver<std::shared_ptr<pcpp::Packet> > >;

class PacketReceiver :
        public Thread,
        public IObservedSubject<PacketObserverShPtr>,
        public IPacketFilter<PacketObserverShPtr>{
public:
    bool Attach(PacketObserverShPtr observer) noexcept;
    bool Detach(PacketObserverShPtr observer) noexcept;

    bool AddFilter(PacketObserverShPtr observer, const pcpp::ProtocolType protocol) noexcept;
    bool RemoveFilter(PacketObserverShPtr observer, const pcpp::ProtocolType protocol) noexcept;
protected:
    virtual void OnPacketDistributing(pcpp::Packet& packet){};
    ThreadSafeQueue<std::shared_ptr<pcpp::Packet> > m_packetQueue;
private:
    void MainLoop() override;
    void DistributePacket(std::shared_ptr<pcpp::Packet> packet);
    std::set<PacketObserverShPtr > m_packetObservers;
    std::map<pcpp::ProtocolType, std::set<PacketObserverShPtr> > m_protocolToObserver;
    std::mutex m_mutex;
};