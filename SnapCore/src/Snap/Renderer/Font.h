#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Core/asserts.h>
#include <Snap/Renderer/Texture.h>
#include <glm/glm.hpp>
#include <filesystem>

namespace SnapEngine
{

    struct Glyph
    {
        SnapPtr<Texture2D> Texture;
        glm::vec2 Size; // bitmap width, raws
        glm::vec2 Bearing; // bitmap left, top
        uint32_t Advance;
    };

    class Font
    {
    public:
        Font(const std::filesystem::path& FontPath, uint32_t GlyphWidth, uint32_t GlyphHeight);
        const Glyph& GetGlyph(char character);
    private:

        std::unordered_map<char, Glyph> m_CharacterMap;
    };
}