#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "mpq_common.hpp"

struct mpq_archive;
using mpq_archive_s = struct mpq_archive;

namespace mpq_reader
{

class MpqArchive
{
public:
	MpqArchive(MpqArchive&& other) noexcept
		: path(std::move(other.path))
		, archive(other.archive)
		, tmpBuf(std::move(other.tmpBuf))
	{
		other.archive = nullptr;
	}

	MpqArchive& operator=(MpqArchive&& other) noexcept;
	~MpqArchive();

	static std::optional<MpqArchive>    Open(const char *path, int32_t &error);
	std::optional<MpqArchive>           Clone(int32_t &error);
	static const char                  *GetErrorMessage(int32_t errorCode);

	bool                         GetFileNumber(mpq_common::MpqFileHash fileHash, uint32_t& fileNumber);
	std::unique_ptr<std::byte[]> ReadFile(std::string_view filename, std::size_t& fileSize, int32_t& error);
	int32_t                      ReadBlock(uint32_t fileNumber, uint32_t blockNumber, uint8_t* out, size_t outSize);
	size_t                       GetUnpackedFileSize(uint32_t fileNumber, int32_t& error);
	uint32_t                     GetNumBlocks(uint32_t fileNumber, int32_t& error);
	int32_t                      OpenBlockOffsetTable(uint32_t fileNumber, std::string_view filename);
	int32_t                      CloseBlockOffsetTable(uint32_t fileNumber);
	size_t                       GetBlockSize(uint32_t fileNumber, uint32_t blockNumber, int32_t& error);
	bool                         HasFile(std::string_view filename) const;
private:
	MpqArchive(std::string path, mpq_archive_s* archive)
		: path(std::move(path))
		, archive(archive)
	{
	}

	std::vector<std::uint8_t>& GetTemporaryBuffer(size_t size)
	{
		if (tmpBuf.size() < size)
		{
			tmpBuf.resize(size);
		}
		return tmpBuf;
	}

	std::string path;
	mpq_archive_s* archive;
	std::vector<std::uint8_t> tmpBuf;
};

}
