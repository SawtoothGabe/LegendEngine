#pragma once

#include <typeindex>
#include <type_traits>
#include <vector>

#include <LE/Common/Assert.hpp>
#include <LE/Common/Defs.hpp>
#include <LE/Common/Types.hpp>

namespace le
{
    class IComponentStorage
    {
    public:
        virtual ~IComponentStorage() = default;

        virtual void MoveFrom(size_t index, IComponentStorage& other) = 0;
        // A swap-and-pop operation that destroys the object
        virtual void Remove(size_t index) = 0;

        [[nodiscard]] virtual size_t GetSize() const = 0;
        [[nodiscard]] virtual std::type_index GetElementType() const = 0;
        [[nodiscard]] virtual Scope<IComponentStorage> CloneEmpty() const = 0;
    };

    struct Component;

    template <typename T>
        requires std::is_move_assignable_v<T>
    class ComponentStorage final : public IComponentStorage
    {
    public:
        ComponentStorage() = default;
        ~ComponentStorage() override = default;
        LE_NO_COPY(ComponentStorage);

        template <typename... Args>
        T& Push(Args&&... args)
        {
            return m_data.emplace_back(std::forward<Args>(args)...);
        }

        void MoveFrom(const size_t index, IComponentStorage& otherStorage) override
        {
            ComponentStorage& other = Cast(otherStorage);

            // Move the component
            Push(std::move(other.Get(index)));
            // Pop and swap from the old storage
            other.Remove(index);
        }

        void Remove(size_t index) override
        {
            // Only swap if the index isn't the last
            if (index < m_data.size() - 1)
                std::swap(m_data.at(index), m_data.back());

            m_data.pop_back();
        }

        [[nodiscard]] size_t GetSize() const override
        {
            return m_data.size();
        }

        [[nodiscard]] std::type_index GetElementType() const override
        {
            return typeid(T);
        }

        [[nodiscard]] T& Get(size_t index)
        {
            return m_data[index];
        }

        [[nodiscard]] Scope<IComponentStorage> CloneEmpty() const override
        {
            return std::make_unique<ComponentStorage>();
        }

        static ComponentStorage& Cast(const Scope<IComponentStorage>& storage)
        {
            return Cast(*storage);
        }

        static ComponentStorage& Cast(IComponentStorage& storage)
        {
            LE_ASSERT(storage.GetElementType() == typeid(T), "Tried to cast component storage to incorrect component type");
            return *static_cast<ComponentStorage*>(&storage);
        }
    private:
        std::vector<T> m_data;
    };
}
