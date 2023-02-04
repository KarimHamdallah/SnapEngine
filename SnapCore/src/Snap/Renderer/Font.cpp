#include "SnapPCH.h"
#include "Font.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_write.h>

/*
#undef INFINITE;
#include "msdfgen.h"
#include "msdfgen-ext.h"
*/

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>

namespace SnapEngine
{
    Font::Font(const std::filesystem::path& FontPath, uint32_t GlyphWidth, uint32_t GlyphHeight)
    {
        FT_Library ft;
        SNAP_ASSERT(!FT_Init_FreeType(&ft));
        if (ft)
        {
            FT_Face face; // .ttf font
            SNAP_ASSERT(!FT_New_Face(ft, FontPath.string().c_str(), 0, &face));
            FT_Set_Pixel_Sizes(face, GlyphWidth, GlyphHeight);

            for (size_t i = 32; i < 128; i++)
            {
                char character = (char)i;
                if (FT_Load_Char(face, (char)i, FT_LOAD_RENDER))
                {
                    SNAP_ERROR("ERROR::FREETYTPE: Failed to load Glyph With Character {}", char(i));
                    continue;
                }

                Glyph glyph =
                {
                    nullptr,
                    { (float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows },
                    { (float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top },
                    face->glyph->advance.x
                };
                // CopyBuffer Data
                uint32_t BufferLength = face->glyph->bitmap.width * face->glyph->bitmap.rows;
                glyph.TextureBitmapData = new uint8_t[BufferLength];
                memcpy(glyph.TextureBitmapData, face->glyph->bitmap.buffer, BufferLength * sizeof(uint8_t));
                m_CharacterMap[character] = glyph;


                m_AtlasWidth += face->glyph->bitmap.width;
                m_AtlasHeight = std::max(m_AtlasHeight, face->glyph->bitmap.rows);
            }


            // Atlas Generation
            uint8_t* AtlasBuffer = new uint8_t[m_AtlasWidth * m_AtlasHeight];


            int AtlasBuffer_XPointer = 0;
            int AtlasBuffer_YPointer = 0;
            int xOffset = 0;
            for (auto& glyph : m_CharacterMap)
            {
                uint32_t GlyphWidth = (uint32_t)glyph.second.Size.x;
                uint32_t GlyphHeight = (uint32_t)glyph.second.Size.y;
                uint8_t* GlyphBuffer = glyph.second.TextureBitmapData;

                for (size_t y = 0; y < GlyphHeight; y++)
                {
                    for (size_t x = 0; x < GlyphWidth; x++)
                    {
                        int AtlasBufferPixel = AtlasBuffer_YPointer * m_AtlasWidth + AtlasBuffer_XPointer;
                        int GlyphBufferPixel = y * GlyphWidth + x;
                        
                        AtlasBuffer[AtlasBufferPixel] = GlyphBuffer[GlyphBufferPixel];
                        AtlasBuffer_XPointer++;
                    }
                    AtlasBuffer_XPointer = xOffset;
                    AtlasBuffer_YPointer++;
                }

                // Note:: texture Flipped!
                float minx = (float)xOffset / (float)m_AtlasWidth;
                float miny = 0.0f;
                float maxx = (float)(xOffset + GlyphWidth) / (float)m_AtlasWidth;
                float maxy = (float)(GlyphHeight) / (float)m_AtlasHeight;
                m_AtlasTexCoords[glyph.first] = { minx, miny, maxx, maxy };

                xOffset += GlyphWidth;
                AtlasBuffer_XPointer = xOffset;
                AtlasBuffer_YPointer = 0;
            }

            //stbi_write_png("outputatlas.png", m_AtlasWidth, m_AtlasHeight, 1, AtlasBuffer, m_AtlasWidth * 1);
            

            Textureprops TextTextureProps;
            TextTextureProps.m_MinFilter = FilterMode::Linear;
            TextTextureProps.m_MagFilter = FilterMode::Linear;
            TextTextureProps.m_WrapS = WrapMode::ClampToEdge;
            TextTextureProps.m_WrapT = WrapMode::ClampToEdge;
            m_FontAtlas = SnapPtr<Texture2D>(Texture2D::Creat(m_AtlasWidth, m_AtlasHeight, AtlasBuffer, TextTextureProps));

            
            //delete[] AtlasBuffer;


            FT_Done_Face(face);
            FT_Done_FreeType(ft);
        }

        // Creat Atlas
    }
    const Glyph& Font::GetGlyph(char character)
    {
        if (m_CharacterMap.find(character) != m_CharacterMap.end())
            return m_CharacterMap.at(character);
        else
            SNAP_ASSERT_MSG(false, "charcater {0} not found in CharacterMap");
    }

    const glm::vec4& Font::GetCharacterAtlasTexCoords(char character) const
    {
        if (m_CharacterMap.find(character) != m_CharacterMap.end())
            return m_AtlasTexCoords.at(character);
        else
            SNAP_ASSERT_MSG(false, "charcater {0} not found in CharacterMap");
    }
}