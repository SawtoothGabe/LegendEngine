#pragma once

#include <LE/Graphics/ShaderManager.hpp>
#include <LE/Math/Types.hpp>
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
	private:
	    const ShaderManager& m_ShaderManager;

		Ref<Shader> m_shaderId;
		Ref<Texture> m_textureId;
		Uniforms m_uniformData;

		size_t m_lastUpdatedFrame = std::numeric_limits<size_t>::max();
	};
}
