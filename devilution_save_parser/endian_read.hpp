#pragma once

#include <cstdint>

namespace utils
{

template <typename T>
constexpr uint32_t LoadLE32(const T* b)
{
    return (static_cast<uint8_t>(b[3]) << 24) | (static_cast<uint8_t>(b[2]) << 16) | (static_cast<uint8_t>(b[1]) << 8) | (static_cast<uint8_t>(b[0]));
}

}
