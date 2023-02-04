#pragma once
#include <Snap/Renderer/Texture.h>

namespace SnapEngine
{
	class OpenGLTexture : public Texture2D
	{
	public:
		OpenGLTexture(const std::string& filepath, const Textureprops& props = Textureprops(), bool flip = true);
		OpenGLTexture(uint32_t Width, uint32_t Height, const Textureprops& props = Textureprops(), bool flip = true);
		OpenGLTexture(uint32_t Width, uint32_t Height, void* Data, const Textureprops& props = Textureprops()); // Balck White Texture For Font Rendering
		virtual ~OpenGLTexture();

		virtual bool Is_null() const override;

		virtual void Bind(uint32_t Slot = 0) const override;
		virtual void UnBind() const override;
		virtual void Delete() override;

		virtual void SetData(void* data, uint32_t size) override;
	};
}