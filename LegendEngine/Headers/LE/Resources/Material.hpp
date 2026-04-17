#pragma once

#include <LE/Math/Types.hpp>
#include <LE/Resources/Resource.hpp>
#include <LE/Resources/Texture.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
	class GraphicsContext;
	class Material : public Resource
	{
		struct Passkey {};
	public:
		struct Uniforms
		{
			Color color = Color(1.0f);
		};

		explicit Material(Passkey);
		explicit Material(GraphicsResources& resources, Passkey);
		~Material() override;

		void SetTexture(const Ref<Texture>& toSet);
		void SetColor(const Color& toSet) const;
		void SetShader(const Ref<Shader>& toSet);

		[[nodiscard]] Ref<Shader> GetShader() const;

		[[nodiscard]] MaterialID GetHandle() const;

		static Ref<Material> Create();
		static Ref<Material> Create(GraphicsResources& resources);
	private:
		GraphicsResources& m_resources;
		MaterialID m_handle;

		uint64_t m_shaderFeatures = static_cast<uint64_t>(Features::SOLID_COLOR);
		Ref<Shader> m_customShader;
	};
}
