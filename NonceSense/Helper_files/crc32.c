#include "crc32.h"

#define CRC32_POLYNOMIAL 0xEDB88320UL

uint32_t crc32_buffer(const uint8_t *data, size_t length) {
    uint32_t crc = 0xFFFFFFFFUL;

    for (size_t i = 0; i < length; i++) {
        crc ^= (uint32_t)data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 1UL) {
                crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
            } else {
                crc = (crc >> 1);
            }
        }
    }

    return ~crc;
}

uint32_t crc32_u32(uint32_t val) {
    uint8_t bytes[4];
    bytes[0] = (uint8_t)(val & 0xFF);
    bytes[1] = (uint8_t)((val >> 8) & 0xFF);
    bytes[2] = (uint8_t)((val >> 16) & 0xFF);
    bytes[3] = (uint8_t)((val >> 24) & 0xFF);

    return crc32_buffer(bytes, 4);
}