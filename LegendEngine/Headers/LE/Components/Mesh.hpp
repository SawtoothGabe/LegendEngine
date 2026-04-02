#pragma once

#include <LE/Components/Component.hpp>
#include <LE/Resources/Material.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    struct Mesh : Component
    {
        Resource::ID<MeshData> data;
        Resource::ID<Material> material;
    };
}
