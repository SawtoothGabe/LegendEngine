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
	class Material : public Resource
	{
	public:
		struct Uniforms
		{
			Color color = Color(1.0f);
		};

		Material();

		void SetTexture(const Ref<Texture>& toSet);
		void SetColor(const Color& toSet);
		[[nodiscard]] Ref<Texture> GetTexture() const;
	    [[nodiscard]] Ref<Shader> GetShader() const;
		[[nodiscard]] Color GetColor() const;

		[[nodiscard]] bool HasChanged() const;

		void UpdateUniforms();
		void CopyUniformData() const;

		[[nodiscard]] DynamicUniforms& GetUniforms() const;
	private:
		GraphicsContext& m_context;
		ResourceManager& m_resourceManager;
		GraphicsResources& m_graphicsResources;
	    const ShaderManager& m_ShaderManager;

		Ref<Shader> m_shaderId;
		Ref<Texture> m_textureId;
		Uniforms m_uniformData;

		Scope<Buffer> m_buffer;
		Scope<DynamicUniforms> m_uniforms;

		size_t m_lastUpdatedFrame = std::numeric_limits<size_t>::max();
	};
}
