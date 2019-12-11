//
// Created by adam on 30/11/2019.
//

#include <PcapLiveDevice.h>
#include <iostream>
#include <functional>
#include "PacketReceiver.h"
#include "TcpLayer.h"

bool PacketReceiver::AddFilter(PacketObserverShPtr observer, const pcpp::ProtocolType protocol) noexcept{

    auto attachedObserver = m_packetObservers.find(observer);
    if(attachedObserver==m_packetObservers.end()) {
        return false;
    }

    {
        std::unique_lock<std::mutex> lock(m_mutex);

        auto& subscribedObservers = m_protocolToObserver[protocol];
        subscribedObservers.insert(observer);
    }

    return true;
}

bool PacketReceiver::RemoveFilter(PacketObserverShPtr observer, const pcpp::ProtocolType protocol)  noexcept{
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
        std::unique_lock<std::mutex> lock(m_mutex);
        subscribedObservers->second.erase(protocolObserver);
    }

    return true;
}

bool PacketReceiver::Attach(PacketObserverShPtr observer) noexcept{
    std::unique_lock<std::mutex> lock(m_mutex);

    auto result = m_packetObservers.insert(observer);
    if(!result.second)
        return false;

    return true;
}

bool PacketReceiver::Detach(PacketObserverShPtr observer) noexcept{
    auto searchResult = m_packetObservers.find(observer);
    if(searchResult==m_packetObservers.end()) {
        return  false;
    }

    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_packetObservers.erase(searchResult);
        for(auto& protocol: m_protocolToObserver){
            protocol.second.erase(observer);
        }
    }

    return true;
}

void PacketReceiver::MainLoop() {
    while(m_runThread){
        auto packet = m_packetQueue.TryDequeue();
        if(packet){
            OnPacketDistributing(*packet);
            DistributePacket(std::move(packet));
        }
        else{
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
        }
    }
}

void PacketReceiver::DistributePacket(std::shared_ptr<pcpp::Packet> packet) {
    std::unique_lock<std::mutex> lock(m_mutex);
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