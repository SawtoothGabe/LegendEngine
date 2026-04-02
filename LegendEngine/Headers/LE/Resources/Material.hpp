#pragma once

#include <LE/Graphics/GraphicsResources.hpp>
#include <LE/Graphics/ShaderManager.hpp>
#include <LE/Graphics/API/DynamicUniforms.hpp>
#include <LE/Resources/Resource.hpp>
#include <LE/Resources/Shader.hpp>
#include <LE/Resources/Texture.hpp>

namespace le
{
	class GraphicsContext;
	class Material final : public Resource
	{
	public:
		struct Uniforms
		{
			Color color = Color(1.0f);
		};

		Material();

		void SetTexture(const ID<Texture>& toSet);
		void SetColor(const Color& toSet);
		[[nodiscard]] ID<Texture> GetTexture() const;
	    [[nodiscard]] ID<Shader> GetShader() const;
		[[nodiscard]] Color GetColor() const;

		[[nodiscard]] bool HasChanged() const;

		void UpdateUniforms();
		void CopyUniformData() const;

		[[nodiscard]] DynamicUniforms& GetUniforms() const;

		ID<Material> id = ID<Material>(m_uid);
	private:
		GraphicsContext& m_context;
		ResourceManager& m_resourceManager;
		GraphicsResources& m_graphicsResources;
	    const ShaderManager& m_ShaderManager;

		ID<Shader> m_shaderId;
		ID<Texture> m_textureId;
		Uniforms m_uniformData;

		Scope<Buffer> m_buffer;
		Scope<DynamicUniforms> m_uniforms;

		size_t m_lastUpdatedFrame = std::numeric_limits<size_t>::max();
	};
}
