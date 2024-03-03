#include <stdio.h>
#include <stdint.h>
#include <string.h>


static uint32_t crc32_table[256];

// Generate the CRC-32 lookup table
static void generateCRCTable() {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc = crc >> 1;
            }
        }
        crc32_table[i] = crc;
    }
}

// Calculate the CRC-32 checksum for data of the given length
uint32_t calculateCRC32(const char* data, size_t length) {
    if (!data) return 0;

    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ data[i]];
    }

    return ~crc;
}

int main() {
    // Initialize the CRC-32 table
    generateCRCTable();

    // Example binary string
    const char* binaryString = "110110101";

    // Calculate CRC-32
    uint32_t crcResult = calculateCRC32(binaryString, strlen(binaryString));

    // Print the result
    printf("CRC-32 Value: %u\n", crcResult);

    return 0;
}
