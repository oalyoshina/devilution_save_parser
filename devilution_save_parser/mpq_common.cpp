#include "mpq_common.hpp"
#include <libmpq/mpq.h>

namespace mpq_common
{
MpqFileHash CalculateMpqFileHash(std::string_view filename)
{
    MpqFileHash fileHash;
    libmpq__file_hash_s(filename.data(), filename.size(), &fileHash[0], &fileHash[1], &fileHash[2]);
    return fileHash;
}

}
