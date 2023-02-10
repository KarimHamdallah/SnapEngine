#pragma once
#include <Snap/Renderer/Texture.h>

namespace SnapEngine
{
	class OpenGLTexture : public Texture2D
	{
	public:
		OpenGLTexture(const std::string& filepath, const Textureprops& props = Textureprops(), bool flip = true);
		OpenGLTexture(const TextureSpecification& Specs, const Textureprops& props, void* Data);
		virtual ~OpenGLTexture();

		virtual bool Is_null() const override;

		virtual void Bind(uint32_t Slot = 0) const override;
		virtual void UnBind() const override;
		virtual void Delete() override;

		virtual void SetData(void* data, uint32_t size) override;
	};
}