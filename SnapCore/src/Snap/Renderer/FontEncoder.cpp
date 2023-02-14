#include "SnapPCH.h"
#include "FontEncoder.h"

#include <glm/glm.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_write.h>

#undef INFINITE;
#include "msdf-atlas-gen.h"
#include "FontGeometry.h"
#include "GlyphGeometry.h"

#include <Snap/Core/asserts.h>
#include <yaml-cpp/yaml.h>

using namespace msdfgen;

namespace SnapEngine
{

    struct MSDFData
    {
        std::vector<msdf_atlas::GlyphGeometry> Glyphs;
        msdf_atlas::FontGeometry FontGeometry;
    };

    MSDFData* FontEncoder::m_Data = nullptr;

    template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
    static void CreateAndCacheAtlas(const std::string& OutputPath, const std::string& fontName, float fontSize, const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
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

        stbi_write_png(OutputPath.c_str(), (int)bitmap.width, (int)bitmap.height, 3, (const void*)bitmap.pixels, (int)bitmap.width * 3);

        return;

        /*
        ///////////////////////////////////////
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
        ///////////////////////////////
        */
    }










	void FontEncoder::GenerateFontAtlasAndMetrics(const std::filesystem::path& OutputPath, const std::filesystem::path& FontPath, float Size)
	{
        m_Data = new MSDFData();

        {
            std::string Extension = FontPath.extension().string();
            if (Extension != ".ttf" && Extension != ".tof")
            {
                SNAP_ERROR("{} is not a font file", FontPath.string().c_str());
                delete m_Data;
                return;
            }

            msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
            SNAP_ASSERT(ft);

            std::string fileString = FontPath.string();

            // TODO(Karim): msdfgen::loadFontData loads from memory buffer which we'll need
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


            double emSize = Size;

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

            // if MSDF || MTSDF
#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8

            // Edge coloring
            uint64_t coloringSeed = 0;
            bool expensiveColoring = false;
                if (expensiveColoring)
                {
                    msdf_atlas::Workload([&glyphs = m_Data->Glyphs, &coloringSeed](int i, int threadNo) -> bool
                        {
                        unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
                        glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
                        return true;
                        }, m_Data->Glyphs.size()).finish(THREAD_COUNT);
                }
                else
                {
                    unsigned long long glyphSeed = coloringSeed;
                    for (msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs)
                    {
                        glyphSeed *= LCG_MULTIPLIER;
                        glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
                    }
                }



            CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>((OutputPath.string() + ".png"), "Test", (float)emSize, m_Data->Glyphs, m_Data->FontGeometry, width, height);

            msdfgen::destroyFont(font);
            msdfgen::deinitializeFreetype(ft);

            ///////////////// Save Font Data ////////////////////
            YAML::Emitter out;

            const auto& FontGeometry = m_Data->FontGeometry;
            const auto& FontMetrics = m_Data->FontGeometry.getMetrics();

            out << YAML::BeginMap;
            out << YAML::Key << "FontGlyphMetrics" << YAML::Value;


            out << YAML::BeginMap;
            for (uint32_t Character = 33; Character < 127; Character++ /*auto Character : charset*/)
            {
                out << YAML::Key << std::string(1, (char)Character) << YAML::Value;
                {
                    out << YAML::BeginMap;

                    auto glyph = FontGeometry.getGlyph((char)Character);
                    double al, ab, ar, at;
                    glyph->getQuadAtlasBounds(al, ab, ar, at);
                    glm::vec2 TexCoordMin = { (float)al, (float)ab };
                    glm::vec2 TexCoordMax = { (float)ar, (float)at };

                    double pl, pb, pr, pt;
                    glyph->getQuadPlaneBounds(pl, pb, pr, pt);
                    glm::vec2 QuadMin = { (float)pl, (float)pb };
                    glm::vec2 QuadMax = { (float)pr, (float)pt };

                    double AssenderY = 0.0, DessenderY = 0.0;
                    AssenderY = FontMetrics.ascenderY;
                    DessenderY = FontMetrics.descenderY;

                    double Advance = glyph->getAdvance();
                    FontGeometry.getAdvance(Advance, (char)Character, (char)(Character + 1));

                    out << YAML::Key << "TexCoordMinX" << YAML::Value << TexCoordMin.x;
                    out << YAML::Key << "TexCoordMinY" << YAML::Value << TexCoordMin.y;

                    out << YAML::Key << "TexCoordMaxX" << YAML::Value << TexCoordMax.x;
                    out << YAML::Key << "TexCoordMaxY" << YAML::Value << TexCoordMax.y;

                    out << YAML::Key << "QuadMinX" << YAML::Value << QuadMin.x;
                    out << YAML::Key << "QuadMinY" << YAML::Value << QuadMin.y;

                    out << YAML::Key << "QuadMaxX" << YAML::Value << QuadMax.x;
                    out << YAML::Key << "QuadMaxY" << YAML::Value << QuadMax.y;

                    out << YAML::Key << "AssenderY" << YAML::Value << AssenderY;
                    out << YAML::Key << "DessenderY" << YAML::Value << DessenderY;

                    out << YAML::Key << "Advance" << YAML::Value << Advance;

                    out << YAML::EndMap;
                }
            }
            out << YAML::EndMap;


            out << YAML::EndMap;

            std::string OutputFontDataFile = OutputPath.string() + ".SnapFont";

            std::ofstream fout(OutputFontDataFile.c_str());
            fout << out.c_str();

        }

		delete m_Data;
	}
}