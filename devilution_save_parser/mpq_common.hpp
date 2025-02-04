#pragma once

#include <array>
#include <cstdint>
#include <string_view>

#include "endian_read.hpp"

namespace mpq_common
{
constexpr size_t MaxMpqPathSize = 256;

#pragma pack(push, 1)
struct MpqFileHeader
{
    // Diablo header settings
    static constexpr uint32_t Signature = utils::LoadLE32("MPQ\x1A");
    static constexpr uint32_t HeaderSize = 32;

    uint32_t signature;

    // The size of the header in bytes, always 32 for Diablo MPQs
    uint32_t headerSize;

    // The size of the MPQ file in bytes
    uint32_t fileSize;

    // Version of the header
    uint16_t version;

    // Block size is '512 * 2 ^ blockSizeFactor'
    uint16_t blockSizeFactor;

    // Location of the hash entries table
    uint32_t hashEntriesOffset;

    // Location of the block entries table
    uint32_t blockEntriesOffset;

    // Size of the hash entries table (number of entries)
    uint32_t hashEntriesCount;

    // Size of the block entries table (number of entries)
    uint32_t blockEntriesCount;

    // Empty space after the header. Not included into 'headerSize'
    uint8_t pad[72];
};

struct MpqHashEntry
{
    // Special values for the 'block' field.
    // Doesn't point to a block (unassigned hash entry)
    static constexpr uint32_t NullBlock = -1;

    // Used to point to a block, but now the block is deleted (could be reclaimed)
    static constexpr uint32_t DeletedBlock = -2;

    // 'hashA' and 'hashB' are used to resolve hash index collisions
    uint32_t hashA, hashB;

    uint16_t locale, platform;

    // Index of the first block in the block entries table,
    // or -1 for an unused entry, -2 for a deleted entry
    uint32_t block;
};

struct MpqBlockEntry
{
    static constexpr uint32_t FlagExists = 0x80000000;
    static constexpr uint32_t CompressPkZip = 0x00000100;

    // Offset to the start of the block
    uint32_t offset;

    // Size in the MPQ
    uint32_t packedSize;

    // Uncompressed size
    uint32_t unpackedSize;

    // Flags indicating compression type, encryption, etc.
    uint32_t flags;
};
#pragma pack(pop)

using MpqFileHash = std::array<uint32_t, 3>;
MpqFileHash CalculateMpqFileHash(std::string_view filename);

}

