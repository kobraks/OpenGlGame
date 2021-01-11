#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "Glyph.h"
#include "Texture.h"

namespace Game
{
	class Font
	{
	public:
		struct Info
		{
			std::string Family;
		};

	private:
		struct Row
		{
			uint32_t Width;
			uint32_t Top;
			uint32_t Height;

			Row(uint32_t rowTop, uint32_t rowHeight) : Width(0),
			                                           Top(rowTop),
			                                           Height(rowHeight)
			{}
		};

		typedef std::unordered_map<uint32_t, Glyph> GlyphTable;

		struct Page
		{
			Page();

			GlyphTable Glyphs;
			Texture Texture;
			uint32_t NextRow;
			std::vector<Row> Rows;
		};

		typedef std::unordered_map<uint32_t, Page> PageTable;

		void *m_Library   = nullptr;
		void *m_Face      = nullptr;
		void *m_StreamRec = nullptr;
		void *m_Stroker   = nullptr;
		int *m_RefCount   = nullptr;

		Info m_Info;

		mutable PageTable m_Pages;
		mutable std::vector<float> m_PixelBuffer;
		
		Font();
	public:
		Font(const std::string &fileName);
		Font(const Font& font);
		Font(Font&& font);
		
		~Font();

		Font& operator=(const Font& font);
		Font& operator=(Font&& font) noexcept;
		
		const Info GetInfo() const { return m_Info; }

		const Glyph& GetGlyph(uint32_t codePoint, uint32_t characterSize, bool bold, float outlineThickness = 0) const;

		float GetKerning(uint32_t first, uint32_t second, uint32_t characterSize) const;
		float GetLineSpacing(uint32_t characterSize) const;
		float GetUnderlinePosition(uint32_t characterSize) const;
		float GetUnderLineThickness(uint32_t characterSize) const;

		const Texture& GetTexture(uint32_t characterSize) const { return m_Pages[characterSize].Texture; }
	private:
		void Clean();

		Glyph LoadGlyph(uint32_t codePoint, uint32_t characterSize, bool bold, float outlineThickness) const;

		IntRect FindGlyphRect(Page &page, uint32_t width, uint32_t height) const;
		bool SetCurrentSize(uint32_t characterSize) const;
	};
}
