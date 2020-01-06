#ifndef NETWORKMONITOR_ROUNDTRIPSNIFFER_H
#define NETWORKMONITOR_ROUNDTRIPSNIFFER_H

#include <memory>
#include <Packet.h>
#include <optional>
#include <map>
#include "Interfaces.h"
#include <vector>
#include <queue>
#include <chrono>
#include <IpAddress.h>
#include <TcpLayer.h>

class RoundTripSniffer{
public:
    using Endpoint = std::pair<std::unique_ptr<pcpp::IPAddress>, uint16_t>;
    using Connection = std::pair<Endpoint ,Endpoint>;

    struct Sample{
        std::chrono::time_point<std::chrono::high_resolution_clock> m_timestamp;
        Connection m_connection;
        std::chrono::microseconds m_roundTrip;
    };

    RoundTripSniffer();
    void Process(pcpp::Packet& packet);
    Sample PopSample();
    bool IsEmpty();
private:
    std::optional<Connection> GetConnection(pcpp::Packet& packet) const;
    Sample  Create(Connection&& connection, timeval syncTime, timeval syncAckTime);
    bool SyncStarted(pcpp::TcpLayer& tcpLayer) const;
    bool SyncAcknowledged(pcpp::TcpLayer& tcpLayer) const;
    static bool ConnectionsEqual(const Connection& connection1, const Connection& connection2);
    using ConnectionsComparatorType = bool (*)(const Connection&, const Connection&);
    std::map<Connection, timeval, ConnectionsComparatorType> m_syncTimes;

    std::queue<Sample> m_samples;
};


#endif //NETWORKMONITOR_ROUNDTRIPSNIFFER_H
