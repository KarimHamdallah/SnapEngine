#include "SnapPCH.h"
#include "Font.h"

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

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

            Textureprops TextTextureProps;
            TextTextureProps.m_MinFilter = FilterMode::Linear;
            TextTextureProps.m_MagFilter = FilterMode::Linear;
            TextTextureProps.m_WrapS = WrapMode::ClampToEdge;
            TextTextureProps.m_WrapT = WrapMode::ClampToEdge;

            for (size_t i = 0; i < 128; i++)
            {
                char character = (char)i;
                if (FT_Load_Char(face, (char)i, FT_LOAD_RENDER))
                {
                    SNAP_ERROR("ERROR::FREETYTPE: Failed to load Glyph With Character {}", char(i));
                    continue;
                }

                SnapPtr<Texture2D> Tex = SnapPtr<Texture2D>(Texture2D::Creat(
                    (float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows, face->glyph->bitmap.buffer));

                Glyph glyph =
                {
                    Tex,
                    { (float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows },
                    { (float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top },
                    face->glyph->advance.x
                };
                m_CharacterMap[character] = glyph;
            }
            FT_Done_Face(face);
            FT_Done_FreeType(ft);
        }
    }
    const Glyph& Font::GetGlyph(char character)
    {
        if (m_CharacterMap.find(character) != m_CharacterMap.end())
            return m_CharacterMap.at(character);
        else
            SNAP_ASSERT_MSG(false, "charcater {0} not found in CharacterMap");
    }
}