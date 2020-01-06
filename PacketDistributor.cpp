//
// Created by adam on 30/11/2019.
//

#include <PcapLiveDevice.h>
#include <iostream>
#include "PacketDistributor.h"
#include "TcpLayer.h"

bool PacketDistributor::AddFilter(PacketObserverShPtr observer, const pcpp::ProtocolType protocol) noexcept{
    auto attachedObserver = m_packetObservers.find(observer);
    if(attachedObserver==m_packetObservers.end()) {
        return false;
    }

    auto& subscribedObservers = m_protocolToObserver[protocol];
    subscribedObservers.insert(observer);

    return true;
}

bool PacketDistributor::RemoveFilter(PacketObserverShPtr observer, const pcpp::ProtocolType protocol)  noexcept{
    auto attachedObserver = m_packetObservers.find(observer);
    if(attachedObserver==m_packetObservers.end()) {
        return false;
    }

    auto subscribedObservers = m_protocolToObserver.find(protocol);
    if(subscribedObservers==m_protocolToObserver.end()) {
        return false;
    }

    auto protocolObserver = subscribedObservers->second.find(observer);
    if(protocolObserver!=subscribedObservers->second.end()){
        subscribedObservers->second.erase(protocolObserver);
    }

    return true;
}

bool PacketDistributor::Attach(PacketObserverShPtr observer) noexcept{
    auto result = m_packetObservers.insert(observer);
    if(!result.second)
        return false;

    return true;
}

bool PacketDistributor::Detach(PacketObserverShPtr observer) noexcept{
    auto searchResult = m_packetObservers.find(observer);
    if(searchResult==m_packetObservers.end()) {
        return  false;
    }

    m_packetObservers.erase(searchResult);
    for(auto& protocol: m_protocolToObserver){
        protocol.second.erase(observer);
    }

    return true;
}

void PacketDistributor::DistributePacket(std::shared_ptr<pcpp::Packet> packet) {
    for(auto currentLayer = packet->getFirstLayer(); currentLayer!=NULL; currentLayer=currentLayer->getNextLayer()){
        auto protocol = currentLayer->getProtocol();
        auto observers = m_protocolToObserver.find(protocol);

        if(observers!=m_protocolToObserver.end()){
            for(auto& observer : observers->second){
                observer->Update(std::move(packet));
            }
        }
    }
}