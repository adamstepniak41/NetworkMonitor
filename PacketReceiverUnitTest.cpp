#include "gtest/gtest.h"
#include "PacketReceiver.h"
#include "PacketReceiverTestData.h"

class SpecificPacketReceiver : public PacketReceiver {
public:
    SpecificPacketReceiver(std::vector<pcpp::RawPacket>& samples) : m_samples(samples){}
protected:
    void OnThreadStarting() override{
        auto readingThread = std::thread([](PacketQueue& packetQueue, std::vector<pcpp::RawPacket>& samples){
            for(auto& rawPacket : samples){
                auto packet = std::make_shared<pcpp::Packet>(&rawPacket);
                packetQueue.Enqueue(std::move(packet));
            }
        }, std::ref(m_packetQueue),  std::ref(m_samples));
        readingThread.join();
    }

    std::vector<pcpp::RawPacket>& m_samples;
};

class LayerCounter : public IObserver<std::shared_ptr<pcpp::Packet> > {
public:
    void Update(std::shared_ptr<pcpp::Packet> packet) override{
        counter++;
    }
    uint64_t counter = 0;
};

TEST (PacketReceiverTest, SubscribingToPacketLayer) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;
    pcpp::RawPacket packet1(sample1.data(), sample1.size(), {0, 0}, false);
    samples.push_back(packet1);

    // Prepare test objects
    SpecificPacketReceiver packetMock(samples);
    auto tcpCounter = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetMock.Attach(tcpCounter));
    EXPECT_TRUE(packetMock.AddFilter(tcpCounter, pcpp::TCP));

    packetMock.Start();

    //The waiting is brittle and will be replaced by event handling when implemented
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);

    packetMock.Stop();

    EXPECT_EQ (1, tcpCounter->counter);
}

TEST (PacketReceiverTest, SubscribingToMultipleLayersWithMultipleObservers) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;
    pcpp::RawPacket packet1(sample1.data(), sample1.size(), {0, 0}, false);
    samples.push_back(packet1);

    // Prepare test objects
    SpecificPacketReceiver packetMock(samples);
    auto tcpCounter = std::make_shared<LayerCounter>();
    auto ipv4Counter = std::make_shared<LayerCounter>();
    auto sslCounter = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetMock.Attach(tcpCounter));
    EXPECT_TRUE(packetMock.Attach(ipv4Counter));
    EXPECT_TRUE(packetMock.Attach(sslCounter));
    EXPECT_TRUE(packetMock.AddFilter(ipv4Counter, pcpp::IPv4));
    EXPECT_TRUE(packetMock.AddFilter(tcpCounter, pcpp::TCP));
    EXPECT_TRUE(packetMock.AddFilter(sslCounter, pcpp::SSL));

    packetMock.Start();

    //The waiting is brittle and will be replaced by event handling when implemented
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);

    packetMock.Stop();

    EXPECT_EQ (1, tcpCounter->counter);
    EXPECT_EQ (1, ipv4Counter->counter);
    EXPECT_EQ (1, sslCounter->counter);
}

TEST (PacketReceiverTest, SubscribingToMultipleLayersWithOneObserver) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;
    pcpp::RawPacket packet1(sample1.data(), sample1.size(), {0, 0}, false);
    samples.push_back(packet1);

    // Prepare test objects
    SpecificPacketReceiver packetMock(samples);
    auto filteredPacketsCounter = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetMock.Attach(filteredPacketsCounter));
    EXPECT_TRUE(packetMock.AddFilter(filteredPacketsCounter, pcpp::Ethernet));
    EXPECT_TRUE(packetMock.AddFilter(filteredPacketsCounter, pcpp::TCP));
    EXPECT_TRUE(packetMock.AddFilter(filteredPacketsCounter, pcpp::IPv4));
    EXPECT_TRUE(packetMock.AddFilter(filteredPacketsCounter, pcpp::SSL));

    packetMock.Start();

    //The waiting is brittle and will be replaced by event handling when implemented
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);

    packetMock.Stop();

    EXPECT_EQ (4, filteredPacketsCounter->counter);
}

TEST (PacketReceiverTest, SubscribingToTheSameLayerWithMultipleObservers) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;
    pcpp::RawPacket packet1(sample1.data(), sample1.size(), {0, 0}, false);
    samples.push_back(packet1);

    // Prepare test objects
    SpecificPacketReceiver packetMock(samples);
    auto observer1 = std::make_shared<LayerCounter>();
    auto observer2 = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetMock.Attach(observer1));
    EXPECT_TRUE(packetMock.Attach(observer2));

    EXPECT_TRUE(packetMock.AddFilter(observer1, pcpp::TCP));
    EXPECT_TRUE(packetMock.AddFilter(observer2, pcpp::TCP));

    packetMock.Start();

    //The waiting is brittle and will be replaced by event handling when implemented
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);

    packetMock.Stop();

    EXPECT_EQ (1, observer1->counter);
    EXPECT_EQ (1, observer2->counter);
}

TEST (PacketReceiverTest, AttachingAlreadyAttachedObserver) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;

    // Prepare test objects
    SpecificPacketReceiver packetMock(samples);
    auto observer1 = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetMock.Attach(observer1));
    EXPECT_FALSE(packetMock.Attach(observer1));
}

TEST (PacketReceiverTest, DetachingUnknownObserver) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;

    // Prepare test objects
    SpecificPacketReceiver packetMock(samples);
    auto observer1 = std::make_shared<LayerCounter>();

    EXPECT_FALSE(packetMock.Detach(observer1));
}

TEST (PacketReceiverTest, AttachDetach) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;
    pcpp::RawPacket packet1(sample1.data(), sample1.size(), {0, 0}, false);
    samples.push_back(packet1);

    // Prepare test objects
    SpecificPacketReceiver packetMock(samples);
    auto observer1 = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetMock.Attach(observer1));
    EXPECT_TRUE(packetMock.Detach(observer1));

    EXPECT_FALSE(packetMock.AddFilter(observer1, pcpp::TCP));
}

TEST (PacketReceiverTest, DetachWithFilterAndSend) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;
    pcpp::RawPacket packet1(sample1.data(), sample1.size(), {0, 0}, false);
    samples.push_back(packet1);

    // Prepare test objects
    SpecificPacketReceiver packetMock(samples);
    auto observer1 = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetMock.Attach(observer1));
    EXPECT_TRUE(packetMock.AddFilter(observer1, pcpp::TCP));
    EXPECT_TRUE(packetMock.Detach(observer1));

    packetMock.Start();

    //The waiting is brittle and will be replaced by event handling when implemented
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);

    packetMock.Stop();

    EXPECT_EQ (0, observer1->counter);
}

TEST (PacketReceiverTest, MultiplePacketsForTheSameLayer) {
    // Prepare test data
    std::vector<pcpp::RawPacket> samples;
    pcpp::RawPacket packet1(sample1.data(), sample1.size(), {0, 0}, false);
    pcpp::RawPacket packet2(sample2.data(), sample2.size(), {0, 0}, false);
    pcpp::RawPacket packet3(sample3.data(), sample3.size(), {0, 0}, false);

    samples.push_back(packet1);
    samples.push_back(packet2);
    samples.push_back(packet3);

    // Prepare test objects
    SpecificPacketReceiver packetMock(samples);
    auto observer1 = std::make_shared<LayerCounter>();

    EXPECT_TRUE(packetMock.Attach(observer1));
    EXPECT_TRUE(packetMock.AddFilter(observer1, pcpp::Ethernet));

    packetMock.Start();

    //The waiting is brittle and will be replaced by event handling when implemented
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);

    packetMock.Stop();

    EXPECT_EQ (3, observer1->counter);
}

int main(int argc, char **argv) {
::testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}