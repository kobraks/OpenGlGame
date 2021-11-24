#include "pch.h"
#include "Font.h"

#include <cstdlib>
#include <cstring>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H

#include "Image.h"

namespace Game
{
	template <typename T, typename U>
	static T Reinterpret(const U &input)
	{
		T output;
		std::memcpy(&output, &input, sizeof(U));
		return output;
	}

	static uint64_t Combine(float outlineThickness, bool bold, uint32_t index)
	{
		return (static_cast<uint64_t>(Reinterpret<uint32_t>(outlineThickness)) << 32) | (static_cast<uint64_t>(bold) <<
			31) | index;
	}

	Font::Page::Page() : NextRow(3)
	{
		Image image;
		image.Create(128, 128, Color(1.f, 1.f, 1.f, 0.f));

		for(int x = 0; x < 2; ++x)
			for(int y = 0; y < 2; ++y) image.SetPixel(x, y, Color(1.f, 1.f, 1.f, 1.f));

		Texture.Create(image);
		Texture.SetFilters(Filter::Linear, Filter::Linear);
	}

	Font::Font()
	{
		m_RefCount = new int(1);
	}

	Font::Font(const std::string &fileName) : Font()
	{
		FT_Library lib;
		if(FT_Init_FreeType(&lib))
		{
			throw std::runtime_error("Unable to initialize FreeType library");
		}

		FT_Face face;
		if(FT_New_Face(lib, fileName.c_str(), 0, &face))
		{
			FT_Done_FreeType(lib);
			throw std::runtime_error("Unable to initialize font face");
		}

		FT_Stroker stroker;
		if(FT_Stroker_New(lib, &stroker))
		{
			FT_Done_Face(face);
			FT_Done_FreeType(lib);
			throw std::runtime_error("Unable to create stroker");
		}

		if(FT_Select_Charmap(face, FT_ENCODING_UNICODE))
		{
			FT_Stroker_Done(stroker);
			FT_Done_Face(face);
			FT_Done_FreeType(lib);
			throw std::runtime_error("Unable to set Unicode character set for font");
		}

		m_Library = lib;
		m_Face    = face;
		m_Stroker = stroker;

		m_Info.Family = face->family_name ? face->family_name : std::string();
	}

	Font::Font(const Font &font)
	{
		(*this) = font;
	}

	Font::Font(Font &&font)
	{
		(*this) = std::move(font);
	}

	Font::~Font()
	{
		Clean();
	}

	Font& Font::operator=(const Font &font)
	{
		Clean();

		m_RefCount  = font.m_RefCount;
		m_Library   = font.m_Library;
		m_Face      = font.m_Face;
		m_Info      = font.m_Info;
		m_Pages     = font.m_Pages;
		m_StreamRec = font.m_StreamRec;
		m_Stroker   = font.m_Stroker;

		++(*m_RefCount);

		return *this;
	}

	Font& Font::operator=(Font &&font) noexcept
	{
		Clean();

		m_RefCount  = font.m_RefCount;
		m_Library   = font.m_Library;
		m_Face      = font.m_Face;
		m_Info      = font.m_Info;
		m_Pages     = font.m_Pages;
		m_StreamRec = font.m_StreamRec;
		m_Stroker   = font.m_Stroker;

		return *this;
	}

	const Glyph& Font::GetGlyph(uint32_t codePoint, uint32_t characterSize, bool bold, float outlineThickness) const
	{
		GlyphTable &glyphs = m_Pages[characterSize].Glyphs;

		uint64_t key = Combine(outlineThickness, bold, FT_Get_Char_Index(static_cast<FT_Face>(m_Face), codePoint));

		GlyphTable::const_iterator it = glyphs.find(key);
		if(it != glyphs.end()) return it->second;
		Glyph glyph = LoadGlyph(codePoint, characterSize, bold, outlineThickness);
		return glyphs.insert(std::make_pair(key, glyph)).first->second;
	}

	float Font::GetKerning(uint32_t first, uint32_t second, uint32_t characterSize) const
	{
		if(first == 0 || second == 0) return 0.f;

		FT_Face face = static_cast<FT_Face>(m_Face);

		if(face && FT_HAS_KERNING(face) && SetCurrentSize(characterSize))
		{
			FT_UInt index1 = FT_Get_Char_Index(face, first);
			FT_UInt index2 = FT_Get_Char_Index(face, second);

			FT_Vector kerning;
			FT_Get_Kerning(face, index1, index2, FT_KERNING_DEFAULT, &kerning);

			if(!FT_IS_SCALABLE(face)) return static_cast<float>(kerning.x);

			return static_cast<float>(kerning.x) / static_cast<float>(1 << 6);
		}

		return 0.f;
	}

	float Font::GetLineSpacing(uint32_t characterSize) const
	{
		FT_Face face = static_cast<FT_Face>(m_Face);
		if(face && SetCurrentSize(characterSize)) return static_cast<float>(face->size->metrics.height) / static_cast<
			float>(1 << 6);

		return 0.f;
	}

	float Font::GetUnderlinePosition(uint32_t characterSize) const
	{
		FT_Face face = static_cast<FT_Face>(m_Face);

		if(face && SetCurrentSize(characterSize))
		{
			if(!FT_IS_SCALABLE(face)) return characterSize / 10.f;

			return -static_cast<float>(FT_MulFix(face->underline_position, face->size->metrics.y_scale)) / static_cast<
				float>(1 << 6);
		}

		return 0.f;
	}

	float Font::GetUnderLineThickness(uint32_t characterSize) const
	{
		FT_Face face = static_cast<FT_Face>(m_Face);

		if(face && SetCurrentSize(characterSize))
		{
			if(!FT_IS_SCALABLE(face)) return characterSize / 14.f;


			return static_cast<float>(FT_MulFix(face->underline_thickness, face->size->metrics.y_scale)) / static_cast<
				float>(1 << 6);
		}
		return 0.f;
	}

	void Font::Clean()
	{
		if(m_RefCount)
		{
			(*m_RefCount)--;

			if(*m_RefCount == 0)
			{
				delete m_RefCount;

				if(m_Stroker) FT_Stroker_Done(static_cast<FT_Stroker>(m_Stroker));
				if(m_Face) FT_Done_Face(static_cast<FT_Face>(m_Face));
				if(m_StreamRec) delete static_cast<FT_StreamRec*>(m_StreamRec);

				if(m_Library) FT_Done_FreeType(static_cast<FT_Library>(m_Library));
			}
		}

		m_Library   = nullptr;
		m_Face      = nullptr;
		m_Stroker   = nullptr;
		m_StreamRec = nullptr;
		m_RefCount  = nullptr;
		m_Pages.clear();
		std::vector<uint8_t>().swap(m_PixelBuffer);
	}

	Glyph Font::LoadGlyph(uint32_t codePoint, uint32_t characterSize, bool bold, float outlineThickness) const
	{
		Glyph glyph;
		
		FT_Face face = static_cast<FT_Face>(m_Face);
		if (!face)
			return glyph;
		if (!SetCurrentSize(characterSize))
			return glyph;

		FT_Int32 flags = FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT;
		if (outlineThickness != 0)
			flags |= FT_LOAD_NO_BITMAP;

		if (FT_Load_Char(face, codePoint, flags) != 0)
			return glyph;

		FT_Glyph glyphDesc;
		if (FT_Get_Glyph(face->glyph, &glyphDesc) != 0)
			return glyph;

		FT_Pos weight = 1 << 6;
		bool outline = (glyphDesc->format == FT_GLYPH_FORMAT_OUTLINE);
		if (outline)
		{
			if (bold)
			{
				FT_OutlineGlyph outlineGlyph = (FT_OutlineGlyph)glyphDesc;
				FT_Outline_Embolden(&outlineGlyph->outline, weight);
			}

			if (outlineThickness != 0)
			{
				FT_Stroker stroker = static_cast<FT_Stroker>(m_Stroker);

				FT_Stroker_Set(stroker, static_cast<FT_Fixed>(outlineThickness * static_cast<float>(1 << 6)), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
				FT_Glyph_Stroke(&glyphDesc, stroker, true);
			}
		}

		FT_Glyph_To_Bitmap(&glyphDesc, FT_RENDER_MODE_NORMAL, 0, 1);
		FT_Bitmap& bitmap = reinterpret_cast<FT_BitmapGlyph>(glyphDesc)->bitmap;

		if (!outline)
		{
			if (bold)
				FT_Bitmap_Embolden(static_cast<FT_Library>(m_Library), &bitmap, weight, weight);

			if (outlineThickness != 0)
				LOG_ERROR("Failed to outline glyph (no fallback available)");
		}

		glyph.Advance = static_cast<float>(face->glyph->metrics.horiAdvance) / static_cast<float>(1 << 6);
		if (bold)
			glyph.Advance += static_cast<float>(weight) / static_cast<float>(1 << 6);

		int width = bitmap.width;
		int height = bitmap.rows;

		if ((width > 0) && (height > 0))
		{
			const uint32_t padding = 1;

			width += 2 * padding;
			height += 2 * padding;

			Page& page = m_Pages[characterSize];

			glyph.TextureRect = FindGlyphRect(page, width, height);

			glyph.TextureRect.X += padding;
			glyph.TextureRect.Y += padding;
			glyph.TextureRect.Width += padding;
			glyph.TextureRect.Height += padding;

			glyph.Bounds.X = static_cast<float>(face->glyph->metrics.horiBearingX) / static_cast<float>(1 << 6);
			glyph.Bounds.Y = static_cast<float>(face->glyph->metrics.horiBearingY) / static_cast<float>(1 << 6);
			
			glyph.Bounds.Width = static_cast<float>(face->glyph->metrics.width) / static_cast<float>(1 << 6) + outlineThickness * 2;
			glyph.Bounds.Height = static_cast<float>(face->glyph->metrics.height) / static_cast<float>(1 << 6) + outlineThickness * 2;

			m_PixelBuffer.resize(width * height * 4);

			uint8_t* current = &m_PixelBuffer[0];
			uint8_t* end = current + width * height * 4;

			while(current != end)
			{
				(*current++) = 255;
				(*current++) = 255;
				(*current++) = 255;
				(*current++) = 0;
			}

			const uint8_t* pixels = bitmap.buffer;
			if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
			{
				for (uint32_t y = padding; y < height - padding; ++y)
				{
					for (uint32_t x = padding; x < width - padding; ++x)
					{
						std::size_t index = x + y * width;
						m_PixelBuffer[index * 4 + 3] = ((pixels[(x - padding) / 8]) & (1 << (7-((x - padding) % 8)))) ? 255 : 0;
					}
					pixels += bitmap.pitch;
				}
			}
			else
			{
				for (uint32_t y = padding; y < height - padding; ++y)
				{
					for (uint32_t x = padding; x < width - padding; ++x)
					{
						std::size_t index = x + y * width;
						m_PixelBuffer[index * 4 + 3] = pixels[x - padding];
					}
					pixels += bitmap.pitch;
				}
			}

			uint32_t x = glyph.TextureRect.X - padding;
			uint32_t y = glyph.TextureRect.Y - padding;
			uint32_t w = glyph.TextureRect.Width + 2 * padding;
			uint32_t h = glyph.TextureRect.Height + 2 * padding;
			page.Texture.Update(&m_PixelBuffer[0], w, h, x, y);
		}
		
		FT_Done_Glyph(glyphDesc);
		
		return glyph;
	}
	
	IntRect Font::FindGlyphRect(Page &page, uint32_t width, uint32_t height) const
	{
		Row* row = nullptr;
		float bestRatio = 0;
		for (std::vector<Row>::iterator it = page.Rows.begin(); it != page.Rows.end() && !row; ++it)
		{
			float ratio = static_cast<float>(height) / it->Height;

			if ((ratio < 0.7f) || (ratio > 1.f))
				continue;

			if (width > page.Texture.Width() - it->Width)
				continue;

			if (ratio < bestRatio)
				continue;;

			row = &*it;
			bestRatio = ratio;
		}

		if (!row)
		{
			int rowHeight = height + height / 10;
			while((page.NextRow + rowHeight >= page.Texture.Height()) || (width >= page.Texture.Width()))
			{
				uint32_t textureWidth = page.Texture.Width();
				uint32_t textureHeight = page.Texture.Height();

				if (textureWidth * 2 <= Texture::GetMaximumSize() && textureHeight * 2 <= Texture::GetMaximumSize())
				{
					Texture newTexture;
					newTexture.Create(textureWidth, textureHeight * 2);
					newTexture.SetFilters(Filter::Linear, Filter::Linear);
					newTexture.Update(page.Texture);
					page.Texture.Swap(newTexture);
				}
				else
				{
					LOG_ERROR("Failed to add new character to the font: the maximum size has been reached");
					return IntRect(0, 0, 2, 2);
				}
			}

			page.Rows.push_back(Row(page.NextRow, rowHeight));
			page.NextRow += rowHeight;
			row = &page.Rows.back();
		}
		
		IntRect rect(row->Width, row->Top, width, height);
		row->Width += width;
		
		return rect;
	}
	
	bool Font::SetCurrentSize(uint32_t characterSize) const
	{
		FT_Face face = static_cast<FT_Face>(m_Face);
		FT_UShort currentSize = face->size->metrics.x_ppem;

		if (currentSize != characterSize)
		{
			FT_Error result = FT_Set_Pixel_Sizes(face, 0, characterSize);
			if (result == FT_Err_Invalid_Pixel_Size)
			{
				if (!FT_IS_SCALABLE(face))
				{
					LOG_ERROR("Failed to set bitmap font size to: {}", characterSize);

					fmt::memory_buffer buffer;
					fmt::format_to(std::back_inserter(buffer), "Available sizes are: ");

					for (int i = 0; i < face->num_fixed_sizes; ++i)
					{
						const uint32_t size = (face->available_sizes[i].y_ppem + 32) >> 6;
						fmt::format_to(std::back_inserter(buffer), "{} ", size);
					}
					LOG_INFO(buffer.data());
				}
				else
				{
					LOG_ERROR("Failed to set font size to {}", characterSize);
				}
			}

			return result == FT_Err_Ok;
		}

		return true;
	}
}
