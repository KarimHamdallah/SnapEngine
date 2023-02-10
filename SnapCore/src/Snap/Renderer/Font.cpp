#include "SnapPCH.h"
#include "Font.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_write.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>

#undef INFINITE;
#include "msdf-atlas-gen.h"
#include "FontGeometry.h"
#include "GlyphGeometry.h"
namespace SnapEngine
{
    struct MSDFData
    {
        std::vector<msdf_atlas::GlyphGeometry> Glyphs;
        msdf_atlas::FontGeometry FontGeometry;
    };


    Font::Font(const std::filesystem::path& FontPath, uint32_t GlyphWidth, uint32_t GlyphHeight)
        : m_Data(new MSDFData())
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

                //stbi_write_png(("Glyphs/output" + std::to_string(i) + ".png").c_str(), glyph.Size.x, glyph.Size.y, 1, glyph.TextureBitmapData, glyph.Size.x * 1);
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

            TextureSpecification specs;
            specs.Width = m_AtlasWidth;
            specs.Height = m_AtlasHeight;
            specs.ImageFormat = ImageFormat::R8;
            specs.DisableByteAligment = true;
            specs.GenerateMips = false;

            m_FontAtlas = SnapPtr<Texture2D>(Texture2D::Creat(specs, TextTextureProps, AtlasBuffer));
            
            
            delete[] AtlasBuffer;


            FT_Done_Face(face);
            FT_Done_FreeType(ft);
        }

        // Creat Atlas
    }

    Font::~Font()
    {
        delete m_Data;
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





    ////////////////// MSDFGENLIB ///////////////////////////







    template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
    static SnapPtr<Texture2D> CreateAndCacheAtlas(const std::string& fontName, float fontSize, const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
        const msdf_atlas::FontGeometry& fontGeometry, uint32_t width, uint32_t height)
    {
        msdf_atlas::GeneratorAttributes attributes;
        attributes.config.overlapSupport = true;
        attributes.scanlinePass = true;

        msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
        generator.setAttributes(attributes);
        generator.setThreadCount(8);
        generator.generate(glyphs.data(), (int)glyphs.size());

        msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

        Textureprops TextTextureProps;
        TextTextureProps.m_MinFilter = FilterMode::Linear;
        TextTextureProps.m_MagFilter = FilterMode::Linear;
        TextTextureProps.m_WrapS = WrapMode::ClampToEdge;
        TextTextureProps.m_WrapT = WrapMode::ClampToEdge;

        TextureSpecification specs;
        specs.Width = bitmap.width;
        specs.Height = bitmap.height;
        specs.ImageFormat = ImageFormat::RGB8;
        specs.GenerateMips = false;


        SnapPtr<Texture2D> texture = SnapPtr<Texture2D>(Texture2D::Creat(specs, TextTextureProps, nullptr));
        texture->SetData((void*)bitmap.pixels, bitmap.width * bitmap.height * 3);
        return texture;
    }















    void Font::LoadFont(const std::filesystem::path& FontPath)
    {
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
        SNAP_ASSERT(ft);

        std::string fileString = FontPath.string();

        // TODO(Yan): msdfgen::loadFontData loads from memory buffer which we'll need 
        msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
        if (!font)
        {
            SNAP_ERROR("Failed to load font: {}", fileString);
            return;
        }

        struct CharsetRange
        {
            uint32_t Begin, End;
        };

        // From imgui_draw.cpp
        static const CharsetRange charsetRanges[] =
        {
            { 0x0020, 0x00FF }
        };

        msdf_atlas::Charset charset;
        for (CharsetRange range : charsetRanges)
        {
            for (uint32_t c = range.Begin; c <= range.End; c++)
                charset.add(c);
        }

        double fontScale = 1.0;
        m_Data->FontGeometry = msdf_atlas::FontGeometry(&m_Data->Glyphs);
        int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);
        SNAP_INFO("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());


        double emSize = 40.0;

        msdf_atlas::TightAtlasPacker atlasPacker;
        // atlasPacker.setDimensionsConstraint()
        atlasPacker.setPixelRange(2.0);
        atlasPacker.setMiterLimit(1.0);
        atlasPacker.setPadding(0);
        atlasPacker.setScale(emSize);
        int remaining = atlasPacker.pack(m_Data->Glyphs.data(), (int)m_Data->Glyphs.size());
        SNAP_ASSERT(remaining == 0);

        int width, height;
        atlasPacker.getDimensions(width, height);
        emSize = atlasPacker.getScale();

        m_AtlasTexture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test", (float)emSize, m_Data->Glyphs, m_Data->FontGeometry, width, height);

        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(ft);
    }
}