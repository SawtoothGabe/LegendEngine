#pragma once

namespace le
{
    template <typename BitType>
    class Flags
    {
    public:
        using MaskType = std::underlying_type_t<BitType>;

        constexpr Flags() noexcept = default;
        constexpr Flags(const Flags& rhs) noexcept = default;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr Flags(BitType bit) noexcept
            :
            m_mask(static_cast<MaskType>(bit))
        {}

        constexpr explicit Flags(MaskType flags) noexcept
            :
            m_mask(flags)
        {}

        auto operator<=>(const Flags&) const = default;

        constexpr bool operator!() const noexcept
        {
            return !m_mask;
        }

        constexpr Flags operator&(const Flags& rhs) const noexcept
        {
            return Flags(m_mask & rhs.m_mask);
        }

        constexpr Flags operator|(const Flags& rhs) const noexcept
        {
            return Flags(m_mask | rhs.m_mask);
        }

        constexpr Flags operator^(const Flags& rhs) const noexcept
        {
            return Flags(m_mask ^ rhs.m_mask);
        }

        constexpr Flags & operator=(const Flags& rhs) noexcept = default;
        constexpr Flags & operator|=(const Flags& rhs) noexcept
        {
            m_mask |= rhs.m_mask;
            return *this;
        }

        constexpr Flags & operator&=(const Flags& rhs) noexcept
        {
            m_mask &= rhs.m_mask;
            return *this;
        }

        constexpr Flags & operator^=(const Flags& rhs) noexcept
        {
            m_mask ^= rhs.m_mask;
            return *this;
        }

        explicit constexpr operator bool() const noexcept
        {
            return !!m_mask;
        }

        explicit constexpr operator MaskType() const noexcept
        {
            return m_mask;
        }
    private:
        MaskType m_mask = 0;
    };

    template <typename BitType>
    constexpr Flags<BitType> operator&( BitType bit, Flags<BitType> const & flags ) noexcept
    {
        return flags.operator&( bit );
    }

    template <typename BitType>
    constexpr Flags<BitType> operator|( BitType bit, Flags<BitType> const & flags ) noexcept
    {
        return flags.operator|( bit );
    }

    template <typename BitType>
    constexpr Flags<BitType> operator^( BitType bit, Flags<BitType> const & flags ) noexcept
    {
        return flags.operator^( bit );
    }

    template<typename>
    struct BitmaskTag
    {
        static constexpr bool BITMASK = false;
    };

#define LE_DEFINE_BITMASK(type) \
    template<> \
    struct BitmaskTag<type> \
    { \
        static constexpr bool BITMASK = true; \
    };

    template <typename BitType>
        requires BitmaskTag<BitType>::BITMASK
    constexpr Flags<BitType> operator&( BitType lhs, BitType rhs ) noexcept
    {
        return Flags<BitType>( lhs ) & rhs;
    }

    template <typename BitType>
        requires BitmaskTag<BitType>::BITMASK
    constexpr Flags<BitType> operator|( BitType lhs, BitType rhs ) noexcept
    {
        return Flags<BitType>( lhs ) | rhs;
    }

    template <typename BitType>
        requires BitmaskTag<BitType>::BITMASK
    constexpr Flags<BitType> operator^( BitType lhs, BitType rhs ) noexcept
    {
        return Flags<BitType>( lhs ) ^ rhs;
    }
}