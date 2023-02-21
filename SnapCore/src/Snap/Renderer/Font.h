#pragma once

#include <filesystem>

#include "Snap/Core/Core.h"
#include "Snap/Renderer/Texture.h"

namespace SnapEngine
{

	struct MSDFData;

	class Font
	{
	public:
		Font(const std::filesystem::path& font);
		~Font();

		const MSDFData* GetMSDFData() const { return m_Data; }
		SnapPtr<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }

		static SnapPtr<Font> GetDefault();
	private:
		MSDFData* m_Data;
		SnapPtr<Texture2D> m_AtlasTexture;
	};
}