#include <gtest/gtest.h>
#include <LE/LegendEngine.hpp>
#include <print>

using namespace le;

struct SmartPtrData
{
    SmartPtrData()
    {
        LE_INFO("Constructed SmartPtrData");
    }

    ~SmartPtrData()
    {
        LE_INFO("Destroyed SmartPtrData");
    }

    size_t data = 1234;
};

struct SmartPtrComponent : Component
{
    SmartPtrComponent()
    {
        LE_INFO("Constructed SmartPtrComponent");
    }

    SmartPtrComponent(const SmartPtrComponent& other)
        :
        Component(other)
    {
        LE_INFO("Copied SmartPtrComponent");
    }

    SmartPtrComponent(SmartPtrComponent&& other) noexcept
        :
        Component(other)
    {
        LE_INFO("Moved SmartPtrComponent");
        m_moved = true;
    }

    ~SmartPtrComponent() override
    {
        if (m_moved)
            return;

        LE_INFO("Destroyed SmartPtrComponent");
    }

    Ref<SmartPtrData> data;
    bool m_moved = false;
};

void EnqueueCreation(Scene& scene)
{
    EntityCreator creator;

    SmartPtrComponent component;
    component.data = std::make_shared<SmartPtrData>();

    creator.AddComponent(component);
    scene.EnqueueEntityCreation(std::move(creator));
}

TEST(ECS, SmartPtrComponent)
{
    Scene scene;
    EnqueueCreation(scene);

    scene.ProcessEntityChanges();
}

class Environment : public testing::Environment
{
public:
    void SetUp() override
    {
        Application::Create(GraphicsAPI::NONE, "ECS test");
    }

    void TearDown() override
    {
        Application::Destroy();
    }
};

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new Environment());

    return RUN_ALL_TESTS();
}