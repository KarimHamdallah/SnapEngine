#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Core/asserts.h>
#include <Snap/Renderer/Texture.h>
#include <glm/glm.hpp>
#include <filesystem>

namespace SnapEngine
{
    struct MSDFData;



    struct Glyph
    {
        uint8_t* TextureBitmapData;
        glm::vec2 Size; // bitmap width, raws
        glm::vec2 Bearing; // bitmap left, top
        uint32_t Advance;
    };

    class Font
    {
    public:
        Font(const std::filesystem::path& FontPath, uint32_t GlyphWidth, uint32_t GlyphHeight);
        ~Font();
        
        const Glyph& GetGlyph(char character);
        SnapPtr<Texture2D> GetFontAtlas() { return m_FontAtlas; }
        const glm::vec4& GetCharacterAtlasTexCoords(char character) const;
    private:

        /// FreeTypeLIB///////////

        std::unordered_map<char, Glyph> m_CharacterMap;
        std::unordered_map<char, glm::vec4> m_AtlasTexCoords; // vec4 TexCoords >> minx, miny, maxx, maxy
        SnapPtr<Texture2D> m_FontAtlas = nullptr;
        uint32_t m_AtlasWidth = 0;
        uint32_t m_AtlasHeight = 0;
        /////////////////////////



    ///MSDFGenLIB///////////////////////////////////////////////////////////
    public:
        void LoadFont(const std::filesystem::path& FontPath);
        SnapPtr<Texture2D> m_AtlasTexture = nullptr;
    private:
        MSDFData* m_Data;
    ////////////////////////////////////////////////////////////////////
    };
}