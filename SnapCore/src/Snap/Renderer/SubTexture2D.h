#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Renderer/Texture.h>

#include <glm/glm.hpp>

namespace SnapEngine
{
	class SubTexture2D
	{
	public:
		SubTexture2D(const SnapPtr<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		SnapPtr<Texture2D> GetTexture() { return m_Texture; }
		const glm::vec2* GetTexCoords() const { return m_TexCoords; }

		static SnapPtr<SubTexture2D> CreatFromCoords(SnapPtr<Texture2D> texture, const glm::vec2& Coords, const glm::vec2& CellSize);
	private:
		SnapPtr<Texture2D> m_Texture;
		glm::vec2 m_TexCoords[4];
	};
}