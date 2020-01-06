#include "gtest/gtest.h"
#include "RoundTripSniffer.h"
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

TEST(RoundTripSnifferTest, RoundTripDetected){
    RoundTripSniffer roundTripSniffer;
    EXPECT_TRUE(roundTripSniffer.IsEmpty());

    pcpp::RawPacket rawTcpSync(RoundTripTestData::sample1.data(), RoundTripTestData::sample1.size(), {0, 0}, false);
    pcpp::RawPacket rawTcpSyncAck(RoundTripTestData::sample2.data(), RoundTripTestData::sample2.size(), {0, 0}, false);

    pcpp::Packet tcpSync(&rawTcpSync);
    pcpp::Packet tcpSyncAck(&rawTcpSyncAck);

    roundTripSniffer.Process(tcpSync);
    EXPECT_TRUE(roundTripSniffer.IsEmpty());
    roundTripSniffer.Process(tcpSyncAck);
    EXPECT_FALSE(roundTripSniffer.IsEmpty());
}

TEST(RoundTripSnifferTest, TimeCalculatedCorrectly){
    RoundTripSniffer roundTripSniffer;
    EXPECT_TRUE(roundTripSniffer.IsEmpty());

    timeval syncTime{0, 0};
    timeval syncAckTime{0, 1000};
    pcpp::RawPacket rawTcpSync(RoundTripTestData::sample1.data(), RoundTripTestData::sample1.size(), syncTime, false);
    pcpp::RawPacket rawTcpSyncAck(RoundTripTestData::sample2.data(), RoundTripTestData::sample2.size(), syncAckTime, false);

    pcpp::Packet tcpSync(&rawTcpSync);
    pcpp::Packet tcpSyncAck(&rawTcpSyncAck);

    roundTripSniffer.Process(tcpSync);
    EXPECT_TRUE(roundTripSniffer.IsEmpty());
    roundTripSniffer.Process(tcpSyncAck);
    EXPECT_FALSE(roundTripSniffer.IsEmpty());

    auto sample = roundTripSniffer.PopSample();
    EXPECT_TRUE(roundTripSniffer.IsEmpty());

    EXPECT_EQ( std::chrono::microseconds(syncAckTime.tv_usec-syncTime.tv_usec), sample.m_roundTrip);
}

TEST(RoundTripSnifferTest, LongTimeCalculatedCorrectly){
    RoundTripSniffer roundTripSniffer;
    EXPECT_TRUE(roundTripSniffer.IsEmpty());

    timeval syncTime{0, 0};
    timeval syncAckTime{100000, 1000};
    pcpp::RawPacket rawTcpSync(RoundTripTestData::sample1.data(), RoundTripTestData::sample1.size(), syncTime, false);
    pcpp::RawPacket rawTcpSyncAck(RoundTripTestData::sample2.data(), RoundTripTestData::sample2.size(), syncAckTime, false);

    pcpp::Packet tcpSync(&rawTcpSync);
    pcpp::Packet tcpSyncAck(&rawTcpSyncAck);

    roundTripSniffer.Process(tcpSync);
    EXPECT_TRUE(roundTripSniffer.IsEmpty());
    roundTripSniffer.Process(tcpSyncAck);
    EXPECT_FALSE(roundTripSniffer.IsEmpty());

    auto sample = roundTripSniffer.PopSample();
    EXPECT_TRUE(roundTripSniffer.IsEmpty());

    auto uSecDiff = std::chrono::microseconds(syncAckTime.tv_usec-syncTime.tv_usec);
    auto secDiff = std::chrono::seconds(syncAckTime.tv_sec-syncTime.tv_sec);

    EXPECT_EQ( std::chrono::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(secDiff)+uSecDiff), sample.m_roundTrip);
}