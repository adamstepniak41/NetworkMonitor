#pragma once

namespace pcpp{ class RawPacket; };

class IPacketHandler{
public:
    virtual ~IPacketHandler(){}
    virtual void Handle(std::unique_ptr<pcpp::RawPacket> packet) = 0;
};

template<class T>
class IObserver{
public:
    virtual ~IObserver(){}
    virtual void Update(T subject) = 0;
    virtual std::string ToString(){ return "Undefined"; };
};

template<class T>
class IObservedSubject{
public:
    bool Attach(T observer) noexcept;
    bool Detach(T observer) noexcept;
};

template<class T>
class IPacketFilter{
public:
    bool AddFilter(T index, const pcpp::ProtocolType protocol) noexcept;
    bool RemoveFilter(T index, const pcpp::ProtocolType protocol) noexcept;
};