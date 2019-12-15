#include <iostream>
#include <algorithm>
#include <functional>
#include "Resolver.h"
#include "IPv4Layer.h"
#include "IPv6Layer.h"

namespace RoundTrip{
    Resolver::Resolver() : m_syncTimes(ConnectionsEqual) {
    }

    void Resolver::Update(std::shared_ptr<pcpp::Packet> packet) {
        pcpp::TcpLayer* tcpLayer = packet->getLayerOfType<pcpp::TcpLayer>();
        if(!tcpLayer)
            return;

        if(SyncStarted(*tcpLayer)){
            auto connection = GetConnection(*packet);
            if(connection){
                std::lock_guard<std::mutex> lock(m_mutex);
                auto timestamp = packet->getRawPacket()->getPacketTimeStamp();
                m_syncTimes.insert(std::pair<std::unique_ptr<Connection>, timeval>(std::move(connection), timestamp));
            }
        }
        else if(SyncAcknowledged(*tcpLayer)){
            auto connection = GetConnection(*packet);
            if(connection){
                std::lock_guard<std::mutex> lock(m_mutex);
                auto startedSync = m_syncTimes.find(connection);
                if(startedSync!=m_syncTimes.end()){
                    auto syncAckTime = packet->getRawPacket()->getPacketTimeStamp();
                    std::cout << "Round trip time is: " << syncAckTime.tv_sec - startedSync->second.tv_sec << std::endl;
                }
            }
        }
    }

    bool Resolver::Attach(MeasureObserverShpPtr observer) noexcept{
        std::lock_guard<std::mutex> lock(m_mutex);

        auto result = m_measureObservers.insert(observer);
        if(!result.second)
            return false;

        return true;
    }

    bool Resolver::Detach(MeasureObserverShpPtr observer) noexcept{
        std::lock_guard<std::mutex> lock(m_mutex);

        auto searchResult = m_measureObservers.find(observer);
        if(searchResult==m_measureObservers.end()) {
            return  false;
        }

        m_measureObservers.erase(searchResult);
        return true;
    }

    std::unique_ptr<Connection>
    Resolver::GetConnection(pcpp::Packet& packet) const {
        pcpp::TcpLayer* tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
        if(!tcpLayer)
            return nullptr;

        std::unique_ptr<Connection> connection;

        uint16_t srcPort = tcpLayer->getTcpHeader()->portSrc;
        uint16_t dstPort = tcpLayer->getTcpHeader()->portDst;

        pcpp::IPv4Layer* ipv4Layer = packet.getLayerOfType<pcpp::IPv4Layer>();
        pcpp::IPv6Layer* ipv6Layer = packet.getLayerOfType<pcpp::IPv6Layer>();

        if(ipv4Layer && ipv6Layer){
            std::cout << "Any type of packet encapsulation is not supported. Ignoring packet." << std::endl;
            connection.reset(nullptr);
        }
        else{
            if(ipv4Layer){
                Endpoint srcEndpoint(std::make_unique<pcpp::IPv4Address>(ipv4Layer->getIPv4Header()->ipSrc), srcPort);
                Endpoint dstEndpoint(std::make_unique<pcpp::IPv4Address>(ipv4Layer->getIPv4Header()->ipDst), dstPort);

                connection.reset(new Connection(std::move(srcEndpoint), std::move(dstEndpoint)));
            }
            else if(ipv6Layer){
                Endpoint srcEndpoint(std::make_unique<pcpp::IPv6Address>(ipv6Layer->getIPv6Header()->ipSrc), srcPort);
                Endpoint dstEndpoint(std::make_unique<pcpp::IPv6Address>(ipv6Layer->getIPv6Header()->ipDst), dstPort);

                connection.reset(new Connection(std::move(srcEndpoint), std::move(dstEndpoint)));
            }
        }

        return connection;
    }

    void Resolver::MainLoop() {
        using namespace std::chrono_literals;
        while(m_runThread){
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                //Refresh statistics
            }

            std::this_thread::sleep_for(100ms);
        }
    }

    bool Resolver::SyncStarted(pcpp::TcpLayer &tcpLayer) const {
        return tcpLayer.getTcpHeader()->synFlag && !tcpLayer.getTcpHeader()->ackFlag;
    }

    bool Resolver::SyncAcknowledged(pcpp::TcpLayer &tcpLayer) const {
        return tcpLayer.getTcpHeader()->synFlag && tcpLayer.getTcpHeader()->ackFlag;
    }

    bool Resolver::ConnectionsEqual(const std::unique_ptr<Connection>& connection1, const std::unique_ptr<Connection>& connection2) {
        return  connection1->first.first->equals(&(*connection2->first.first)) &&
                connection1->second.first->equals(&(*connection2->second.first)) &&
                connection1->first.second == connection2->first.second &&
                connection1->second.second == connection2->first.second ;
    }
}
