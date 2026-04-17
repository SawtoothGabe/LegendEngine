#pragma once

#include <LE/Common/UID.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
    class Scene;
    class Entity final
    {
        friend Scene;
    public:
        Entity() = default;

        // ReSharper disable once CppNonExplicitConversionOperator
        operator UID() const;

        template <typename T, typename... Args>
        void AddComponent(Args&&... args)
        {
            m_pScene->AddComponent<T>(uid, std::forward<Args>(args)...);
        }

        template <typename T>
        void RemoveComponent() const
        {
            m_pScene->RemoveComponent<T>(uid);
        }

        template <typename T>
        T GetComponentData()
        {
            return m_pScene->GetComponentData<T>(uid);
        }

        template <typename T>
        void ReplaceComponent(T&& component)
        {
            m_pScene->ReplaceComponent<T>(uid, std::forward<T>(component));
        }

        template <typename... Ts, typename Fn>
        void QueryComponents(Fn update)
        {
            m_pScene->QueryEntityComponents<Ts...>(uid, update);
        }

        UID uid;
    protected:
        explicit Entity(Scene& scene);

        Scene* m_pScene = nullptr;
    };
}
