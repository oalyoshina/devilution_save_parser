#include "load_save_helper.h"

#include <cstddef>
#include <memory>
#include <numeric>
#include <optional>

using SaveReader = MpqArchive;
using SaveWriter = MpqWriter;

class LoadHelper
{
	std::unique_ptr<std::byte[]> m_buffer;
	size_t m_cur = 0, m_size;

	template<class T>
	T Next()
	{
		const auto size = sizeof(T);
		if (!WithinBounds(size))
		{
			return {};
		}

		T value;
		memcpy(&value, &m_buffer[m_cur], size);
		m_cur += size;

		return value;
	}
public:
	LoadHelper(std::optional<SaveReader> archive, const char* filename)
	{

	}

	constexpr bool WithinBounds(const size_t size = 1)
	{
		return m_buffer != nullptr && m_size >= (m_cur + size);
	}
};

void LoadGame()
{
	LoadHelper loader;
}
