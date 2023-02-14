#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Core/asserts.h>
#include <filesystem>

namespace SnapEngine
{
	struct MSDFData;

	class FontEncoder
	{
	public:
		/*
		OutputPath = assets / Fonts / arial >> arial.png, arial.SnapFont
		FontPath = assets/Fonts/arial.ttf
		*/
		static void GenerateFontAtlasAndMetrics(const std::filesystem::path& OutputPath, const std::filesystem::path& FontPath, float Size = 40.0f);



	private:
		static MSDFData* m_Data;
	};
}