#include <random>
#include <LE/Common/UID.hpp>

namespace le
{
    UID::UID()
    {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dist;

        m_UID = dist(gen);
    }

    UID::UID(const uint64_t id)
        :
        m_UID(id)
    {}

    UID::operator uint64_t() const
    {
        return Get();
    }

    bool UID::operator==(const UID& rhs) const
    {
        return m_UID == rhs.m_UID;
    }

    uint64_t UID::Get() const
    {
        return m_UID;
    }
}

size_t std::hash<le::UID>::operator()(const le::UID& uid) const noexcept
{
    return std::hash<size_t>{}(uid.Get());
}
