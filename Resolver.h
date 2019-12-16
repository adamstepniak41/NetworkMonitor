#pragma once

#include <memory>
#include <Packet.h>
#include <set>
#include <IPv4Layer.h>
#include <IPv6Layer.h>
#include <google/protobuf/stubs/mutex.h>
#include <condition_variable>
#include <map>
#include "Interfaces.h"
#include "TcpLayer.h"
#include "Thread.h"
#include "ThreadSafeQueue.h"
#include <optional>

namespace RoundTrip{
    using Endpoint = std::pair<std::unique_ptr<pcpp::IPAddress>, uint16_t>;
    using Connection = std::pair<Endpoint ,Endpoint>;

    struct MeasureResult{
        Connection m_connection;
        int m_roundTrip;
        int m_roundTripUSec;
    };

    using MeasureResultShPtr = std::shared_ptr<MeasureResult>;
    using MeasureObserverShpPtr = std::shared_ptr<IObserver<MeasureResultShPtr > >;

    class Resolver :
            public Thread,
            public IObserver<std::shared_ptr<pcpp::Packet> >,
            public IObservedSubject<MeasureObserverShpPtr > {
    public:
        Resolver();
        void Update(std::shared_ptr<pcpp::Packet> packet) override;
        bool Attach(MeasureObserverShpPtr observer) noexcept;
        bool Detach(MeasureObserverShpPtr observer) noexcept;
    private:
        std::optional<Connection> GetConnection(pcpp::Packet& packet) const;
        MeasureResultShPtr  Create(Connection connection, timeval syncTime, timeval syncAckTime);
        bool SyncStarted(pcpp::TcpLayer& tcpLayer) const;
        bool SyncAcknowledged(pcpp::TcpLayer& tcpLayer) const;
        static bool ConnectionsEqual(const Connection& connection1, const Connection& connection2);
        using ConnectionsComparatorType = bool (*)(const Connection&, const Connection&);
        void MainLoop() override ;

        std::set<MeasureObserverShpPtr > m_measureObservers;
        ThreadSafeQueue<MeasureResultShPtr> m_measureResults;
        std::map<Connection, timeval, ConnectionsComparatorType> m_syncTimes;
        std::mutex m_mutex;
    };
}

