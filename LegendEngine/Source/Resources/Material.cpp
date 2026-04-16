#include <LE/Application.hpp>
#include <LE/Math/Types.hpp>
#include <LE/Resources/Material.hpp>
#include <LE/Resources/Texture.hpp>

namespace le
{
    class Shader;

    Material::Material(Passkey)
        :
        Material(Application::Get().GetGraphicsContext().GetResources(), Passkey{})
    {}

    Material::Material(GraphicsResources& resources, Passkey)
        :
        m_resources(resources)
    {
        m_handle = m_resources.CreateMaterial();
    }

    Material::~Material()
    {
        m_resources.DestroyMaterial(m_handle);
    }

    void Material::SetTexture(const Ref<Texture>& toSet) const
    {
        m_resources.SetMaterialTexture(m_handle, toSet);
    }

    void Material::SetColor(const Color& toSet) const
    {
        m_resources.SetMaterialColor(m_handle, toSet);
    }

    void Material::SetShader(const Ref<Shader>& toSet) const
    {
        m_resources.SetMaterialShader(m_handle, toSet);
    }

    MaterialID Material::GetHandle() const
    {
        return m_handle;
    }

    Ref<Material> Material::Create()
    {
        return std::make_shared<Material>(Passkey{});
    }

    Ref<Material> Material::Create(GraphicsResources& resources)
    {
        return std::make_shared<Material>(resources, Passkey{});
    }
}
