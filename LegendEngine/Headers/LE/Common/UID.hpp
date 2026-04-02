#pragma once

#include <bitset>
#include <cstdint>

namespace le
{
	class UID
	{
	public:
		UID();
		UID(uint64_t id);

		explicit operator uint64_t() const;
		bool operator==(const UID& rhs) const;

		[[nodiscard]] uint64_t Get() const;
	private:
		uint64_t m_UID = 0;
	};
}

template<>
struct std::hash<le::UID>
{
	size_t operator()(const le::UID& uid) const noexcept;
};
