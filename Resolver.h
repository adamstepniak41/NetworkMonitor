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

namespace RoundTrip{
    using Endpoint = std::pair<std::unique_ptr<pcpp::IPAddress>, uint16_t>;
    using Connection = std::pair<Endpoint ,Endpoint>;

    struct MeasureResult{
        Connection m_connection;
        int m_roundTrip;
    };

    using MeasureResultShPtr = std::shared_ptr<IObserver<std::shared_ptr<MeasureResult> > >;
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
        std::unique_ptr<Connection> GetConnection(pcpp::Packet& packet) const;
        bool SyncStarted(pcpp::TcpLayer& tcpLayer) const;
        bool SyncAcknowledged(pcpp::TcpLayer& tcpLayer) const;
        static bool ConnectionsEqual(const std::unique_ptr<Connection>& connection1, const std::unique_ptr<Connection>& connection2);
        using ConnectionsComparatorType = bool (*)(const std::unique_ptr<Connection>&, const std::unique_ptr<Connection>&);
        void MainLoop() override ;

        std::set<MeasureObserverShpPtr > m_measureObservers;
        std::map<std::unique_ptr<Connection>, timeval, ConnectionsComparatorType> m_syncTimes;
        std::mutex m_mutex;
    };
}

