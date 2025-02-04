#include "mpq_reader.hpp"

#include <libmpq/mpq.h>

namespace mpq_reader
{

MpqArchive &MpqArchive::operator=(MpqArchive &&other) noexcept
{
    path = std::move(other.path);
    if (archive)
    {
        libmpq__archive_close(archive);
    }
    archive = other.archive;
    other.archive = nullptr;

    tmpBuf = std::move(other.tmpBuf);
    return *this;
}

MpqArchive::~MpqArchive()
{
    if (archive)
    {
        libmpq__archive_close(archive);
    }
}

std::optional<MpqArchive> MpqArchive::Open(const char *path, int32_t &error)
{
    mpq_archive_s *archive;
    error = libmpq__archive_open(&archive, path, -1);
    if (error)
    {
        return std::nullopt;
    }
    return MpqArchive{ std::string(path), archive };
}

std::optional<MpqArchive> MpqArchive::Clone(int32_t& error)
{
    mpq_archive_s *copy;
    error = libmpq__archive_dup(archive, path.c_str(), &copy);
    if (error)
    {
        return std::nullopt;
    }
    return MpqArchive{ path, copy };
}

const char* MpqArchive::GetErrorMessage(int32_t errorCode)
{
    return libmpq__strerror(errorCode);
}

bool MpqArchive::GetFileNumber(mpq_common::MpqFileHash fileHash, uint32_t &fileNumber)
{
    return libmpq__file_number_from_hash(archive, fileHash[0], fileHash[1], fileHash[2], &fileNumber) == 0;
}

std::unique_ptr<std::byte[]> MpqArchive::ReadFile(std::string_view filename, size_t &fileSize, int32_t &error)
{
    std::unique_ptr<std::byte[]> result;
    std::uint32_t fileNumber;

    error = libmpq__file_number_s(archive, filename.data(), filename.size(), &fileNumber);
    if (error)
    {
        return result;
    }

    libmpq__off_t unpackedSize;
    error = libmpq__file_size_unpacked(archive, fileNumber, &unpackedSize);
    if (error)
    {
        return result;
    }

    error = OpenBlockOffsetTable(fileNumber, filename);
    if (error)
    {
        return result;
    }

    result = std::make_unique<std::byte[]>(static_cast<size_t>(unpackedSize));

    const size_t blockSize = GetBlockSize(fileNumber, 0, error);
    if (error)
    {
        return result;
    }

    std::vector<uint8_t>& tmp = GetTemporaryBuffer(blockSize);
    if (error)
    {
        return result;
    }

    error = libmpq__file_read_with_filename_and_temporary_buffer_s(archive,
                                                                   fileNumber,
                                                                   filename.data(),
                                                                   filename.size(),
                                                                   reinterpret_cast<uint8_t *>(result.get()),
                                                                   unpackedSize,
                                                                   tmp.data(),
                                                                   static_cast<libmpq__off_t>(blockSize),
                                                                   nullptr);

    CloseBlockOffsetTable(fileNumber);
    if (error)
    {
        return nullptr;
    }

    fileSize = static_cast<size_t>(unpackedSize);
    return result;
}

int32_t MpqArchive::ReadBlock(uint32_t fileNumber, uint32_t blockNumber, uint8_t *out, size_t outSize)
{
    std::vector<uint8_t> &tmpBuf = GetTemporaryBuffer(outSize);
    return libmpq__block_read_with_temporary_buffer(archive,
                                                    fileNumber,
                                                    blockNumber,
                                                    out,
                                                    static_cast<libmpq__off_t>(outSize),
                                                    tmpBuf.data(),
                                                    outSize,
                                                    nullptr);
}

size_t MpqArchive::GetUnpackedFileSize(uint32_t fileNumber, int32_t &error)
{
    libmpq__off_t unpackedSize;
    error = libmpq__file_size_unpacked(archive, fileNumber, &unpackedSize);
    return static_cast<size_t>(unpackedSize);
}

uint32_t MpqArchive::GetNumBlocks(uint32_t fileNumber, int32_t& error)
{
    uint32_t numBlocks;
    error = libmpq__file_blocks(archive, fileNumber, &numBlocks);
    return numBlocks;
}

int32_t MpqArchive::OpenBlockOffsetTable(uint32_t fileNumber, std::string_view filename)
{
    return libmpq__block_open_offset_with_filename_s(archive, fileNumber, filename.data(), filename.size());
}

int32_t MpqArchive::CloseBlockOffsetTable(uint32_t fileNumber)
{
    return libmpq__block_close_offset(archive, fileNumber);
}

size_t MpqArchive::GetBlockSize(uint32_t fileNumber, uint32_t blockNumber, int32_t &error)
{
    libmpq__off_t blockSize;
    error = libmpq__block_size_unpacked(archive, fileNumber, blockNumber, &blockSize);
    return static_cast<size_t>(blockSize);
}

bool MpqArchive::HasFile(std::string_view filename) const
{
    uint32_t fileNumber;
    return libmpq__file_number_s(archive, filename.data(), filename.size(), &fileNumber) == 0;
}

}
