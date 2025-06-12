#ifndef UTPTYPES_H
#define UTPTYPES_H

#include <cstdint>

enum utpType {
    ST_DATA = 0,
    ST_FIN,
    ST_STATE,
    ST_RESET,
    ST_SYN
};
#pragma pack(push, 1)
struct utpPacket {
    uint8_t type_ver;
    uint16_t conn_id;
    uint32_t timestamp;
    uint32_t timestamp_diff;
    uint32_t window_size;
    uint16_t seq_num;
    uint16_t ack_num;

    uint8_t* data = nullptr;
    size_t data_len = 0;

    void set_type(uint8_t t) {
        type_ver = (type_ver & 0x0F) | ((t & 0x0F) << 4);
    }

    void set_version(uint8_t v) {
        type_ver = (type_ver & 0xF0) | (v & 0x0F);
    }
    uint8_t get_type() const {
        return (type_ver >> 4) & 0x0F;
    }

    uint8_t get_version() const {
        return type_ver & 0x0F;
    }
};
#pragma pack(pop)

#endif //UTPTYPES_H
