#pragma once

#include <cstring>
#include <vector>

#include <LE/Common/Assert.hpp>

namespace le
{
    struct Component;
    class ComponentStorage final
    {
    public:
        explicit ComponentStorage(size_t componentSize);
        ComponentStorage(ComponentStorage&& other) noexcept;
        ~ComponentStorage();

        template <typename T, typename... Args>
        void Push(Args&&... args)
        {
            LE_ASSERT(sizeof(T) == m_componentSize, "Component size mismatch");

            void* data = Allocate();
            new (data) T(std::forward<Args>(args)...);
        }

        template <typename T>
        T* Get(const size_t index)
        {
            return std::launder(static_cast<T*>(GetData(index)));
        }

        void* GetData(size_t index);
        void* Allocate();
        void SwapAndPop(size_t index, bool destructor = true);
        [[nodiscard]] size_t GetElementSize() const;
    private:
        size_t m_componentSize = 0;
        size_t m_count = 0;
        std::vector<std::byte> m_componentData;
    };
}