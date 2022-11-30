#include "SnapPCH.h"
#include "SubTexture2D.h"

namespace SnapEngine
{
	SubTexture2D::SubTexture2D(const SnapPtr<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
		: m_Texture(texture)
	{      
		m_TexCoords[0] = { max.x, max.y }; // Top Right
		m_TexCoords[1] = { max.x, min.y }; // Bottom Right
		m_TexCoords[2] = { min.x, min.y }; // Bottom Left
		m_TexCoords[3] = { min.x, max.y }; // Top Left
	}

	SnapPtr<SubTexture2D> SubTexture2D::CreatFromCoords(SnapPtr<Texture2D> texture, const glm::vec2& Coords, const glm::vec2& CellSize)
	{
		glm::vec2 min = { (Coords.x * CellSize.x) / texture->getWidth() , (Coords.y * CellSize.y) / texture->getHeight() };
		glm::vec2 max = { ((Coords.x + 1) * CellSize.x) / texture->getWidth() , ((Coords.y + 1) * CellSize.y) / texture->getHeight() };

		return (SnapPtr<SubTexture2D>)std::make_shared<SubTexture2D>(texture, min, max);
	}
}