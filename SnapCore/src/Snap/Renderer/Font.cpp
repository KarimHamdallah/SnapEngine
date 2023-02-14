#include "SnapPCH.h"
#include "Font.h"

#include <stb_image.h>

#include <Snap/Core/asserts.h>
#include <yaml-cpp/yaml.h>


namespace SnapEngine
{

    Font::Font(const std::filesystem::path& FontTextureAtlas, const std::filesystem::path& SnapFontFile)
    {
        int Width, Height;
        const char* Pixels = (const char*)stbi_load(FontTextureAtlas.string().c_str(), &Width, &Height, nullptr, 3);

        Textureprops TextTextureProps;
        TextTextureProps.m_MinFilter = FilterMode::Linear;
        TextTextureProps.m_MagFilter = FilterMode::Linear;
        TextTextureProps.m_WrapS = WrapMode::ClampToEdge;
        TextTextureProps.m_WrapT = WrapMode::ClampToEdge;

        TextureSpecification specs;
        specs.Width = Width;
        specs.Height = Height;
        specs.ImageFormat = ImageFormat::RGB8;
        specs.GenerateMips = false;



        m_AtlasTexture = SnapPtr<Texture2D>(Texture2D::Creat(specs, TextTextureProps, nullptr));
        m_AtlasTexture->SetData((void*)Pixels, Width * Height * 3);

        ////////// Get Font Metrics //////////
        std::ifstream file(SnapFontFile.string().c_str());
        std::stringstream strstream;
        strstream << file.rdbuf();

        YAML::Node data;
        try
        {
            data = YAML::Load(strstream.str());
        }
        catch (const YAML::ParserException& e)
        {
            SNAP_ERROR("Yaml Failed to load {0}.SnapFont file, {1}", SnapFontFile.string().c_str(), e.what());
            return;
        }

        auto& Data = data["FontGlyphMetrics"];
        if (Data)
        {
            for (uint32_t Character = 33; Character < 127; Character++)
            {
                std::string CharacterString = std::string(1, (char)Character);
                auto& CharNode = Data[CharacterString.c_str()];

                float TexCoordMinX = CharNode["TexCoordMinX"].as<float>();
                float TexCoordMinY = CharNode["TexCoordMinY"].as<float>();

                float TexCoordMaxX = CharNode["TexCoordMaxX"].as<float>();
                float TexCoordMaxY = CharNode["TexCoordMaxY"].as<float>();

                float QuadMinX = CharNode["QuadMinX"].as<float>();
                float QuadMinY = CharNode["QuadMinY"].as<float>();

                float QuadMaxX = CharNode["QuadMaxX"].as<float>();
                float QuadMaxY = CharNode["QuadMaxY"].as<float>();

                float AssenderY = CharNode["AssenderY"].as<float>();
                float DessenderY = CharNode["DessenderY"].as<float>();

                float Advance = CharNode["Advance"].as<float>();

                GlyphMetrics Metrics
                {
                    {TexCoordMinX, TexCoordMinY},
                    {TexCoordMaxX, TexCoordMaxY},
                    {QuadMinX, QuadMinY},
                    {QuadMaxX, QuadMaxY},
                    AssenderY,
                    DessenderY,
                    Advance
                };

                m_FontMetrics[(char)Character] = Metrics;
            }
        }
    }

    Font::~Font()
    { 
    }

    SnapPtr<Texture2D> Font::GetFontAtlas() const
    {
        return m_AtlasTexture;
    }

    GlyphMetrics Font::GetGlyphMetrics(char Character) const
    {
        SNAP_ASSERT(m_FontMetrics.find(Character) != m_FontMetrics.end());
        return m_FontMetrics.at(Character);
    }
}





////////////////////////////////////////////////////////////////////////////////////////
/*
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_write.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>


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
        std::unordered_map<char, Glyph> m_CharacterMap;
        std::unordered_map<char, glm::vec4> m_AtlasTexCoords; // vec4 TexCoords >> minx, miny, maxx, maxy
        SnapPtr<Texture2D> m_FontAtlas = nullptr;
        uint32_t m_AtlasWidth = 0;
        uint32_t m_AtlasHeight = 0;
    }




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
                float minx = (float)(xOffset + 1) / (float)m_AtlasWidth;
                float miny = 1.0f / (float)m_AtlasHeight;
                float maxx = (float)(xOffset + GlyphWidth - 1) / (float)m_AtlasWidth;
                float maxy = (float)(GlyphHeight - 1) / (float)m_AtlasHeight;
                m_AtlasTexCoords[glyph.first] = { minx, miny, maxx, maxy };

                xOffset += GlyphWidth;
                AtlasBuffer_XPointer = xOffset;
                AtlasBuffer_YPointer = 0;
            }

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
*/