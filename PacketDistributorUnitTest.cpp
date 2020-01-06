#include "gtest/gtest.h"
#include "PacketDistributor.h"

constexpr  std::array<uint8_t, 93> sample1 = { 0x00, 0x1f, 0xf3, 0x3c, 0xe1, 0x13, 0xf8, 0x1e, 0xdf, 0xe5, 0x84, 0x3a, 0x08, 0x00, 0x45, 0x00,
                                               0x00, 0x4f, 0xde, 0x53, 0x40, 0x00, 0x40, 0x06, 0x47, 0xab, 0xac, 0x10, 0x0b, 0x0c, 0x4a, 0x7d,
                                               0x13, 0x11, 0xfc, 0x35, 0x01, 0xbb, 0xc6, 0xd9, 0x14, 0xd0, 0xc5, 0x1e, 0x2d, 0xbf, 0x80, 0x18,
                                               0xff, 0xff, 0xcb, 0x8c, 0x00, 0x00, 0x01, 0x01, 0x08, 0x0a, 0x1a, 0x7d, 0x84, 0x2c, 0x37, 0xc5,
                                               0x58, 0xb0, 0x15, 0x03, 0x01, 0x00, 0x16, 0x43, 0x1a, 0x88, 0x1e, 0xfa, 0x7a, 0xbc, 0x22, 0x6e,
                                               0xe6, 0x32, 0x7a, 0x53, 0x47, 0x00, 0xa7, 0x5d, 0xcc, 0x64, 0xea, 0x8e, 0x92};
pcpp::ProtocolType sample1Layers[] = {pcpp::Ethernet, pcpp::IPv4, pcpp::TCP, pcpp::SSL};

constexpr  std::array<uint8_t, 81> sample2 = {0x00, 0x1f, 0xf3, 0x3c, 0xe1, 0x13, 0xf8, 0x1e, 0xdf, 0xe5, 0x84, 0x3a, 0x08, 0x00, 0x45, 0x00,
                                              0x00, 0x43, 0x5b, 0x2c, 0x00, 0x00, 0xff, 0x11, 0xf2, 0x4f, 0xac, 0x10, 0x0b, 0x0c, 0xac, 0x10,
                                              0x0b, 0x01, 0xd5, 0x6f, 0x00, 0x35, 0x00, 0x2f, 0x75, 0x84, 0x13, 0xa2, 0x01, 0x00, 0x00, 0x01,
                                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x65, 0x38, 0x37, 0x32, 0x01, 0x67, 0x0a, 0x61, 0x6b,
                                              0x61, 0x6d, 0x61, 0x69, 0x65, 0x64, 0x67, 0x65, 0x03, 0x6e, 0x65, 0x74, 0x00, 0x00, 0x01, 0x00,
                                              0x01};
pcpp::ProtocolType sample2Layers[] = {pcpp::Ethernet, pcpp::IPv4, pcpp::UDP, pcpp::DNS};

constexpr  std::array<uint8_t, 70> sample3 = {0x00, 0x1f, 0xf3, 0x3c, 0xe1, 0x13, 0xf8, 0x1e, 0xdf, 0xe5, 0x84, 0x3a, 0x08, 0x00, 0x45, 0x00,
                                              0x00, 0x38, 0xf3, 0x61, 0x00, 0x00, 0x40, 0x01, 0x19, 0x36, 0xac, 0x10, 0x0b, 0x0c, 0xac, 0x10,
                                              0x0b, 0x01, 0x03, 0x03, 0x14, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x43, 0xc1, 0x80,
                                              0x00, 0x00, 0x40, 0x11, 0x4a, 0xfc, 0xac, 0x10, 0x0b, 0x01, 0xac, 0x10, 0x0b, 0x0c, 0x00, 0x35,
                                              0xe7, 0xe8, 0x00, 0x2f, 0x00, 0x00};
pcpp::ProtocolType sample3Layers[] = {pcpp::Ethernet, pcpp::IPv4, pcpp::UDP, pcpp::ICMP};

class LayerCounter : public IObserver<std::shared_ptr<pcpp::Packet> > {
public:
    void Update(std::shared_ptr<pcpp::Packet> packet) override{
        counter++;
    }
    uint64_t counter = 0;
};

TEST (PacketDistributorTest, SubscribingToPacketLayer) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;
    pcpp::RawPacket rawPacket1(sample1.data(), sample1.size(), {0, 0}, false);
    auto packet1 = std::make_shared<pcpp::Packet>(&rawPacket1);

    // Prepare test objects
    PacketDistributor packetDistributor;
    auto tcpCounter = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetDistributor.Attach(tcpCounter));
    EXPECT_TRUE(packetDistributor.AddFilter(tcpCounter, pcpp::TCP));

    packetDistributor.DistributePacket(packet1);

    EXPECT_EQ (1, tcpCounter->counter);
}

TEST (PacketDistributorTest, SubscribingToMultipleLayersWithMultipleObservers) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;
    pcpp::RawPacket rawPacket1(sample1.data(), sample1.size(), {0, 0}, false);
    auto packet1 = std::make_shared<pcpp::Packet>(&rawPacket1);

    // Prepare test objects
    PacketDistributor packetDistributor;
    auto tcpCounter = std::make_shared<LayerCounter>();
    auto ipv4Counter = std::make_shared<LayerCounter>();
    auto sslCounter = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetDistributor.Attach(tcpCounter));
    EXPECT_TRUE(packetDistributor.Attach(ipv4Counter));
    EXPECT_TRUE(packetDistributor.Attach(sslCounter));
    EXPECT_TRUE(packetDistributor.AddFilter(ipv4Counter, pcpp::IPv4));
    EXPECT_TRUE(packetDistributor.AddFilter(tcpCounter, pcpp::TCP));
    EXPECT_TRUE(packetDistributor.AddFilter(sslCounter, pcpp::SSL));

    packetDistributor.DistributePacket(packet1);

    EXPECT_EQ (1, tcpCounter->counter);
    EXPECT_EQ (1, ipv4Counter->counter);
    EXPECT_EQ (1, sslCounter->counter);
}

TEST (PacketDistributorTest, SubscribingToMultipleLayersWithOneObserver) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;
    pcpp::RawPacket rawPacket1(sample1.data(), sample1.size(), {0, 0}, false);
    auto packet1 = std::make_shared<pcpp::Packet>(&rawPacket1);

    // Prepare test objects
    PacketDistributor packetDistributor;
    auto filteredPacketsCounter = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetDistributor.Attach(filteredPacketsCounter));
    EXPECT_TRUE(packetDistributor.AddFilter(filteredPacketsCounter, pcpp::Ethernet));
    EXPECT_TRUE(packetDistributor.AddFilter(filteredPacketsCounter, pcpp::TCP));
    EXPECT_TRUE(packetDistributor.AddFilter(filteredPacketsCounter, pcpp::IPv4));
    EXPECT_TRUE(packetDistributor.AddFilter(filteredPacketsCounter, pcpp::SSL));

    packetDistributor.DistributePacket(packet1);

    EXPECT_EQ (4, filteredPacketsCounter->counter);
}

TEST (PacketDistributorTest, SubscribingToTheSameLayerWithMultipleObservers) {
    // Prepare test data
    pcpp::RawPacket rawPacket1(sample1.data(), sample1.size(), {0, 0}, false);
    auto packet1 = std::make_shared<pcpp::Packet>(&rawPacket1);

    // Prepare test objects
    PacketDistributor packetDistributor;
    auto observer1 = std::make_shared<LayerCounter>();
    auto observer2 = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetDistributor.Attach(observer1));
    EXPECT_TRUE(packetDistributor.Attach(observer2));

    EXPECT_TRUE(packetDistributor.AddFilter(observer1, pcpp::TCP));
    EXPECT_TRUE(packetDistributor.AddFilter(observer2, pcpp::TCP));

    packetDistributor.DistributePacket(packet1);

    EXPECT_EQ (1, observer1->counter);
    EXPECT_EQ (1, observer2->counter);
}

TEST (PacketDistributorTest, AttachingAlreadyAttachedObserver) {
    // Prepare test objects
    PacketDistributor packetDistributor;
    auto observer1 = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetDistributor.Attach(observer1));
    EXPECT_FALSE(packetDistributor.Attach(observer1));
}

TEST (PacketDistributorTest, DetachingUnknownObserver) {
    // Prepare test objects
    PacketDistributor packetDistributor;
    auto observer1 = std::make_shared<LayerCounter>();

    EXPECT_FALSE(packetDistributor.Detach(observer1));
}

TEST (PacketDistributorTest, AttachDetach) {
    // Prepare test objects
    PacketDistributor packetDistributor;
    auto observer1 = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetDistributor.Attach(observer1));
    EXPECT_TRUE(packetDistributor.Detach(observer1));

    EXPECT_FALSE(packetDistributor.AddFilter(observer1, pcpp::TCP));
}

TEST (PacketDistributorTest, DetachWithFilterAndSend) {
    // Prepare test objects
    PacketDistributor packetDistributor;
    auto observer1 = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetDistributor.Attach(observer1));
    EXPECT_TRUE(packetDistributor.AddFilter(observer1, pcpp::TCP));
    EXPECT_TRUE(packetDistributor.Detach(observer1));

    EXPECT_EQ (0, observer1->counter);
}

TEST (PacketDistributorTest, MultiplePacketsForTheSameLayer) {
    // Prepare test data
    pcpp::RawPacket rawPacket1(sample1.data(), sample1.size(), {0, 0}, false);
    pcpp::RawPacket rawPacket2(sample2.data(), sample2.size(), {0, 0}, false);
    pcpp::RawPacket rawPacket3(sample3.data(), sample3.size(), {0, 0}, false);
    auto packet1 = std::make_shared<pcpp::Packet>(&rawPacket1);
    auto packet2 = std::make_shared<pcpp::Packet>(&rawPacket2);
    auto packet3 = std::make_shared<pcpp::Packet>(&rawPacket3);

    // Prepare test objects
    PacketDistributor packetDistributor;
    auto observer1 = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetDistributor.Attach(observer1));
    EXPECT_TRUE(packetDistributor.AddFilter(observer1, pcpp::Ethernet));

    packetDistributor.DistributePacket(packet1);
    packetDistributor.DistributePacket(packet2);
    packetDistributor.DistributePacket(packet3);

    EXPECT_EQ (3, observer1->counter);
}