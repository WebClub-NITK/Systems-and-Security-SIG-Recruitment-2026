#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>
#include <stddef.h>

//Computes standard IEEE 802.3 CRC-32 over an arbitrary byte buffer. data = Pointer to input bytes. length = Number of bytes to process. returns 32-bit CRC digest.
uint32_t crc32_buffer(const uint8_t *data, size_t length);

//Computes CRC-32 over a single 32-bit integer (little-endian byte order). val 32-bit integer input (e.g., Challenge ^ SECRET_KEY). returns 32-bit CRC digest matching Python zlib.crc32.
uint32_t crc32_u32(uint32_t val);

#endif