#pragma once

#include <LE/Math/Types.hpp>
#include <LE/Resources/Resource.hpp>
#include <LE/Resources/Texture.hpp>

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

		void SetTexture(const Ref<Texture>& toSet) const;
		void SetColor(const Color& toSet) const;
		void SetShader(const Ref<Shader>& toSet) const;

		[[nodiscard]] MaterialID GetHandle() const;

		static Ref<Material> Create();
		static Ref<Material> Create(GraphicsResources& resources);
	private:
		GraphicsResources& m_resources;
		MaterialID m_handle;
	};
}
