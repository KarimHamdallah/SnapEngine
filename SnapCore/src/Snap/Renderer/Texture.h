#pragma once

namespace SnapEngine
{
	enum class FilterMode { None = 0, Linear, Nearest };
	enum class WrapMode { None = 0, Repeat, ClampToEdge };
	enum class ByteFormat { None = 0, RGBA, RGBA8, RGB, RGB8, R32UI, RED, GREEN, BLUE, RED_INTEGER, DEPTH24_STENCIL8 /* Depth/Stencil formats */ };

	class Textureprops
	{
	public:
		Textureprops(FilterMode MinFilter = FilterMode::Nearest, FilterMode MagFilter = FilterMode::Nearest,
			WrapMode WrapS = WrapMode::Repeat, WrapMode WrapT = WrapMode::Repeat,
			ByteFormat InternalFormat = ByteFormat::None, ByteFormat ExternalFormat = ByteFormat::None)
			: m_MinFilter(MinFilter), m_MagFilter(MagFilter), m_WrapS(WrapS), m_WrapT(WrapT),
		      m_InternalFormat(InternalFormat), m_ExternalFormat(ExternalFormat)
		{}

	public:
		FilterMode m_MinFilter;
		FilterMode m_MagFilter;
		WrapMode m_WrapS;
		WrapMode m_WrapT;
		ByteFormat m_InternalFormat;
		ByteFormat m_ExternalFormat;
	};

	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat ImageFormat = ImageFormat::RGBA8;
		bool GenerateMips = true;
		bool DisableByteAligment = false; // For Font Rendering
	};


	class Texture
	{
	public:
		virtual ~Texture() {}

		virtual bool Is_null() const = 0;

		virtual void Bind(uint32_t Slot = 0) const = 0;
		virtual void UnBind() const = 0;
		virtual void Delete() = 0;

		virtual void SetData(void* data, uint32_t size) = 0;


		// get
		inline uint32_t getID() const { return m_ID; }
		inline uint32_t getWidth() const { return m_Width; }
		inline uint32_t getHeight() const { return m_Height; }


		inline bool getIsDefault() const { return m_IsDefault; }
		inline std::string getFilePath() const { return m_Filepath; }

		// set
		inline void setID(uint32_t id) { m_ID = id; }
		inline void setWidth(uint32_t w) { m_Width = w; }
		inline void setHeight(uint32_t h) { m_Height = h; }
		inline void setIsDefault(bool is_default) { m_IsDefault = is_default; }

		inline bool IsEqual(const Texture& other) { return m_ID == other.m_ID; }

		// get and set
		Textureprops& getTextureProps() { return m_Props; }

	protected:
		uint32_t m_ID = 0u;
		int m_Width = 0;
		int m_Height = 0;
		Textureprops m_Props = Textureprops();
		std::string m_Filepath;
		bool m_IsDefault = false;
	};


	class Texture2D : public Texture
	{
	public:
		static Texture2D* Creat(const std::string& filepath, const Textureprops& props = Textureprops(), bool flip = true);
		static Texture2D* Creat(const TextureSpecification& Specs, const Textureprops& props = Textureprops(), void* Data = nullptr);
	};
}