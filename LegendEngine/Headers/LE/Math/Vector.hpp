#pragma once

#include <array>
#include <type_traits>

namespace le
{
    template <typename T, size_t N>
        requires std::is_arithmetic_v<T>
    struct Vector
    {
        static_assert(N >= 2, "The vector size must be greater than or equal to 2");

        Vector()
            :
            Vector(0.0f)
        {}

        Vector(T value)
        {
            Construct(value, std::make_index_sequence<N>{});
        }

        template <typename... Ts>
            requires (sizeof...(Ts) == N)
        Vector(Ts... value)
        {
            size_t i = 0;
            ((data[i++] = value), ...);
        }

        template <size_t M, typename... Rest>
            requires (sizeof...(Rest) == N - M) && (M >= 2)
        explicit Vector(const Vector<T, M>& vec, Rest... rest)
        {
            std::copy(vec.data.begin(), vec.data.end(), data.begin());

            // Copy the rest
            size_t i = M;
            ((data[i++] = rest), ...);
        }

        constexpr T operator[](size_t index) const
        {
            return data[index];
        }

        constexpr T& operator[](size_t index)
        {
            return data[index];
        }

        Vector operator-() const
        {
            return Vector(0) - *this;
        }

        // Arithmetic (+ - * /)
        template <typename U>
        Vector operator+(const U& value) const
        {
            return Vector(*this) += value;
        }

        template <typename U>
        Vector operator-(const U& value) const
        {
            return Vector(*this) -= value;
        }

        template <typename U>
        Vector operator*(const U& value) const
        {
            return Vector(*this) *= value;
        }

        template <typename U>
        Vector operator/(const U& value) const
        {
            return Vector(*this) /= value;
        }

        // Assignment (+= -= *= /=)
        Vector& operator+=(const Vector& value)
        {
            [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                ((data[Is] += value[Is]), ...);
            }(std::make_index_sequence<N>{});

            return *this;
        }

        Vector& operator-=(const Vector& value)
        {
            [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                ((data[Is] -= value[Is]), ...);
            }(std::make_index_sequence<N>{});

            return *this;
        }

        Vector& operator*=(const Vector& value)
        {
            [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                ((data[Is] *= value[Is]), ...);
            }(std::make_index_sequence<N>{});

            return *this;
        }

        Vector& operator/=(const Vector& value)
        {
            [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                ((data[Is] /= value[Is]), ...);
            }(std::make_index_sequence<N>{});

            return *this;
        }

        Vector& operator+=(T value)
        {
            [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                ((data[Is] += value), ...);
            }(std::make_index_sequence<N>{});

            return *this;
        }

        Vector& operator-=(T value)
        {
            [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                ((data[Is] -= value), ...);
            }(std::make_index_sequence<N>{});

            return *this;
        }

        Vector& operator*=(T value)
        {
            [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                ((data[Is] *= value), ...);
            }(std::make_index_sequence<N>{});

            return *this;
        }

        Vector& operator/=(T value)
        {
            [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                ((data[Is] /= value), ...);
            }(std::make_index_sequence<N>{});

            return *this;
        }

        T Dot(const Vector& other) const
        {
            T result = 0;

            [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                ((result += data[Is] * other[Is]), ...);
            }(std::make_index_sequence<N>{});

            return result;
        }

        T Length() const
        {
            return std::sqrt(Dot(*this));
        }

        Vector& Normalize()
        {
            float length = Length();
            if (length == 0)
                return *this;

            return operator/=(length);
        }

        Vector Normalized() const
        {
            Vector result = *this;
            result.Normalize();

            return result;
        }

        union { std::array<T, N> data{}; struct { T x; T y; T z; T w; }; };
    private:
        template<size_t... dummy_indices>
        constexpr void Construct(T value, std::index_sequence<dummy_indices...>)
        {
            ([&] { data[dummy_indices] = value; }(), ...);
        }
    };
}
