#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Core/asserts.h>
#include <Snap/Renderer/Texture.h>
#include <glm/glm.hpp>
#include <filesystem>

namespace SnapEngine
{
    struct GlyphMetrics
    {
        glm::vec2 TexCoordMin = { 0.0f, 0.0f };
        glm::vec2 TexCoordMax = { 0.0f, 0.0f };
        glm::vec2 QuadMin = { 0.0f, 0.0f };
        glm::vec2 QuadMax = { 0.0f, 0.0f };
        float AssenderY = 0.0f;
        float DessenderY = 0.0f;
        float Advance = 0.0f;
    };

    class Font
    {
    public:
        Font(const std::filesystem::path& FontTextureAtlas, const std::filesystem::path& SnapFontFile);
        ~Font();

        SnapPtr<Texture2D> GetFontAtlas() const;
        GlyphMetrics GetGlyphMetrics(char Character) const;
    private:
        SnapPtr<Texture2D> m_AtlasTexture = nullptr;
        std::map<char, GlyphMetrics> m_FontMetrics;
    };
}