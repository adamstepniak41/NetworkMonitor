#include "gtest/gtest.h"
#include "Resolver.h"
#include "Interfaces.h"
#include <chrono>

namespace RoundTripTestData{
    //TCP SYNC
    constexpr  std::array<uint8_t, 74> sample1 = {   0x00, 0x1f, 0x29, 0x5e, 0x4d, 0x26, 0x00, 0x50, 0x56, 0xbb, 0x3a, 0xa0, 0x08, 0x00, 0x45, 0x10
            , 0x00, 0x3c, 0x83, 0x1b, 0x40, 0x00, 0x40, 0x06, 0x15, 0x0a, 0xc0, 0xa8, 0x14, 0x46, 0x4a, 0x7d
            , 0x83, 0x1b, 0xd5, 0x1d, 0x00, 0x19, 0x6b, 0x7f, 0xc7, 0x2d, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02
            , 0x72, 0x10, 0xa2, 0xb5, 0x00, 0x00, 0x02, 0x04, 0x05, 0xb4, 0x04, 0x02, 0x08, 0x0a, 0x0a, 0x99
            , 0x44, 0x36, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x07 };
    pcpp::ProtocolType sample1Layers[] = {pcpp::Ethernet, pcpp::IPv4, pcpp::TCP};

    //TCP SYNC/ACK
    constexpr  std::array<uint8_t, 74> sample2 = {   0x00, 0x50, 0x56, 0xbb, 0x3a, 0xa0, 0x00, 0x1f, 0x29, 0x5e, 0x4d, 0x26, 0x08, 0x00, 0x45, 0x00
            , 0x00, 0x3c, 0x51, 0x49, 0x00, 0x00, 0x31, 0x06, 0x95, 0xec, 0x4a, 0x7d, 0x83, 0x1b, 0xc0, 0xa8
            , 0x14, 0x46, 0x00, 0x19, 0xd5, 0x1d, 0x99, 0x56, 0xad, 0x80, 0x6b, 0x7f, 0xc7, 0x2e, 0xa0, 0x12
            , 0xa6, 0x2c, 0xd5, 0xc1, 0x00, 0x00, 0x02, 0x04, 0x05, 0x96, 0x04, 0x02, 0x08, 0x0a, 0x03, 0xa5
            , 0x88, 0x68, 0x0a, 0x99, 0x44, 0x36, 0x01, 0x03, 0x03, 0x07 };
    pcpp::ProtocolType sample2Layers[] = {pcpp::Ethernet, pcpp::IPv4, pcpp::TCP};

    constexpr auto timeDiff = std::chrono::microseconds(29195);
}

class RoundTripStatistics : public IObserver<RoundTrip::MeasureResultShPtr > {
public:
    void Update(RoundTrip::MeasureResultShPtr packet) override{
        m_roundTripTime = std::chrono::seconds(packet->m_roundTrip) + std::chrono::microseconds(packet->m_roundTripUSec);
        ++counter;
    }

    int counter = 0;
    std::chrono::high_resolution_clock::duration m_roundTripTime;
};


TEST (ResolverTest, RoundTripIsDetected) {
    RoundTrip::Resolver resolver;
    auto roundTripStatistics = std::make_shared<RoundTripStatistics>();
    EXPECT_TRUE(resolver.Attach(roundTripStatistics));
    resolver.Start();

    pcpp::RawPacket rawTcpSync(RoundTripTestData::sample1.data(), RoundTripTestData::sample1.size(), {0, 0}, false);
    pcpp::RawPacket rawTcpSyncAck(RoundTripTestData::sample2.data(), RoundTripTestData::sample2.size(), {0, 0}, false);

    auto tcpSync = std::make_shared<pcpp::Packet>(&rawTcpSync);
    auto tcpSyncAck = std::make_shared<pcpp::Packet>(&rawTcpSyncAck);

    resolver.Update(tcpSync);

    //The waiting is brittle and will be replaced by event handling when implemented
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);

    EXPECT_EQ(0, roundTripStatistics->counter);

    resolver.Update(tcpSyncAck);

    //The waiting is brittle and will be replaced by event handling when implemented
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);

    resolver.Stop();
    EXPECT_EQ(1, roundTripStatistics->counter);
}

TEST (ResolverTest, TimeIsCalculated) {
    RoundTrip::Resolver resolver;
    auto roundTripStatistics = std::make_shared<RoundTripStatistics>();
    EXPECT_TRUE(resolver.Attach(roundTripStatistics));
    resolver.Start();

    timeval syncTime{0, 0};
    timeval syncAckTime{0, 1000};
    pcpp::RawPacket rawTcpSync(RoundTripTestData::sample1.data(), RoundTripTestData::sample1.size(), syncTime, false);
    pcpp::RawPacket rawTcpSyncAck(RoundTripTestData::sample2.data(), RoundTripTestData::sample2.size(), syncAckTime, false);

    auto tcpSync = std::make_shared<pcpp::Packet>(&rawTcpSync);
    auto tcpSyncAck = std::make_shared<pcpp::Packet>(&rawTcpSyncAck);

    resolver.Update(tcpSync);
    resolver.Update(tcpSyncAck);

    //The waiting is brittle and will be replaced by event handling when implemented
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);

    resolver.Stop();
    EXPECT_EQ( std::chrono::microseconds(syncAckTime.tv_usec-syncTime.tv_usec), roundTripStatistics->m_roundTripTime);
}