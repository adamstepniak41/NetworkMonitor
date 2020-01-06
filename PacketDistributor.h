#pragma  once

#include <PcapLiveDeviceList.h>
#include <set>
#include <map>
#include "ThreadSafeQueue.h"
#include "Thread.h"
#include "Interfaces.h"

using PacketObserverShPtr = std::shared_ptr<IObserver<std::shared_ptr<pcpp::Packet> > >;

class PacketDistributor :
        public IObservedSubject<PacketObserverShPtr>,
        public IPacketFilter<PacketObserverShPtr>{
public:
    void DistributePacket(std::shared_ptr<pcpp::Packet> packet);

    bool Attach(PacketObserverShPtr observer) noexcept;
    bool Detach(PacketObserverShPtr observer) noexcept;

    bool AddFilter(PacketObserverShPtr observer, const pcpp::ProtocolType protocol) noexcept;
    bool RemoveFilter(PacketObserverShPtr observer, const pcpp::ProtocolType protocol) noexcept;
private:
    std::set<PacketObserverShPtr > m_packetObservers;
    std::map<pcpp::ProtocolType, std::set<PacketObserverShPtr> > m_protocolToObserver;
};