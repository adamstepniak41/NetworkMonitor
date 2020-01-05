#include <TcpLayer.h>
#include <IPv6Layer.h>
#include <IPv4Layer.h>
#include "RoundTripSniffer.h"
#include <iostream>

RoundTripSniffer::RoundTripSniffer() : m_syncTimes(ConnectionsEqual)
{}

void RoundTripSniffer::Process(pcpp::Packet& packet) {
    pcpp::TcpLayer* tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
    if(!tcpLayer)
        return;

    if(SyncStarted(*tcpLayer)){
        auto connection = GetConnection(packet);
        if(connection){
            auto timestamp = packet.getRawPacket()->getPacketTimeStamp();
            m_syncTimes.insert(std::pair<Connection, timeval>(std::move(*connection), timestamp));
        }
    }
    else if(SyncAcknowledged(*tcpLayer)){
        auto connection = GetConnection(packet);
        if(connection){
            auto startedSync = m_syncTimes.find(*connection);
            if(startedSync!=m_syncTimes.end()){
                auto syncAckTime = packet.getRawPacket()->getPacketTimeStamp();
                auto&& sample = Create(std::move(*connection), startedSync->second, syncAckTime);
                m_samples.emplace(std::forward<Sample>(sample));
                m_syncTimes.erase(startedSync);
            }
        }
    }
}

std::optional<RoundTripSniffer::Connection>
RoundTripSniffer::GetConnection(pcpp::Packet& packet) const {
    pcpp::TcpLayer* tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
    if(!tcpLayer)
        return std::nullopt;

    std::optional<Connection> connection{std::nullopt};

    uint16_t srcPort = tcpLayer->getTcpHeader()->portSrc;
    uint16_t dstPort = tcpLayer->getTcpHeader()->portDst;

    pcpp::IPv4Layer* ipv4Layer = packet.getLayerOfType<pcpp::IPv4Layer>();
    pcpp::IPv6Layer* ipv6Layer = packet.getLayerOfType<pcpp::IPv6Layer>();

    if(ipv4Layer && ipv6Layer){
        std::cout << "Any type of packet encapsulation is not supported. Ignoring packet." << std::endl;
    }
    else{
        if(ipv4Layer){
            Endpoint srcEndpoint(std::make_unique<pcpp::IPv4Address>(ipv4Layer->getIPv4Header()->ipSrc), srcPort);
            Endpoint dstEndpoint(std::make_unique<pcpp::IPv4Address>(ipv4Layer->getIPv4Header()->ipDst), dstPort);

            connection = Connection(std::move(srcEndpoint), std::move(dstEndpoint));
        }
        else if(ipv6Layer){
            Endpoint srcEndpoint(std::make_unique<pcpp::IPv6Address>(ipv6Layer->getIPv6Header()->ipSrc), srcPort);
            Endpoint dstEndpoint(std::make_unique<pcpp::IPv6Address>(ipv6Layer->getIPv6Header()->ipDst), dstPort);

            connection = Connection(std::move(srcEndpoint), std::move(dstEndpoint));
        }
    }

    return connection;
}

bool RoundTripSniffer::SyncStarted(pcpp::TcpLayer &tcpLayer) const {
    return tcpLayer.getTcpHeader()->synFlag && !tcpLayer.getTcpHeader()->ackFlag;
}

bool RoundTripSniffer::SyncAcknowledged(pcpp::TcpLayer &tcpLayer) const {
    return tcpLayer.getTcpHeader()->synFlag && tcpLayer.getTcpHeader()->ackFlag;
}

bool RoundTripSniffer::ConnectionsEqual(const Connection& connection1, const Connection& connection2) {
    return  connection1.first.first->equals(&(*connection2.first.first)) &&
            connection1.second.first->equals(&(*connection2.second.first)) &&
            connection1.first.second == connection2.first.second &&
            connection1.second.second == connection2.first.second ;
}

RoundTripSniffer::Sample RoundTripSniffer::Create(Connection&& connection, timeval syncTime, timeval syncAckTime) {
    std::chrono::seconds seconds(syncAckTime.tv_sec - syncTime.tv_sec);
    std::chrono::microseconds microseconds(syncAckTime.tv_usec - syncTime.tv_usec);

    Sample measureResult{
        std::chrono::system_clock::now(),
        std::move(connection),
        std::chrono::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(seconds)+microseconds)
    };

    return measureResult;
}

bool RoundTripSniffer::IsEmpty() {
    return m_samples.empty();
}

RoundTripSniffer::Sample RoundTripSniffer::PopSample() {
    auto sample = std::move(m_samples.front());
    m_samples.pop();
    return sample;
}
