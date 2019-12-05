//
// Created by adam on 05/12/2019.
//

#ifndef NETWORKMONITOR_MESSAGES_H
#define NETWORKMONITOR_MESSAGES_H

#pragma pack(1)

#include <cstdint>

struct PacketMetadata{
    uint8_t m_type = 1;
    uint64_t m_size = 20;
};
#pragma pack(push,1)
#pragma pack(pop)

#endif //NETWORKMONITOR_MESSAGES_H
