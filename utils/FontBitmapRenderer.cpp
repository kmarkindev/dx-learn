#include "FontBitmapRenderer.h"

FontBitmap FontBitmapRenderer::RenderBitmap(FT_Face face,
    const std::vector<char32_t>& symbols,
    unsigned int fontSize,
    unsigned int padding)
{
    if(face == nullptr)
        throw std::runtime_error("Cannot generate bitmap when face is not loaded");

    if(symbols.empty())
        throw std::runtime_error("Cannot generate bitmap because symbols vector is empty");

    constexpr unsigned int GLYPHS_IN_ROW = 16;
    auto rowsCount = static_cast<unsigned int>(std::ceil(symbols.size() / static_cast<double>(GLYPHS_IN_ROW)));

    const unsigned int glyphWidth = fontSize + padding * 2;
    const unsigned int glyphHeight = glyphWidth;
    const unsigned int bitmapWidth = glyphWidth * GLYPHS_IN_ROW;
    const unsigned int bitmapHeight = glyphHeight * rowsCount;

    FontBitmap result;
    result.bitmapWidth = bitmapWidth;
    result.bitmapHeight = bitmapHeight;
    result.bitmap = new unsigned char[bitmapWidth * bitmapHeight];
    memset(result.bitmap, 0, bitmapWidth * bitmapHeight);

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    for(size_t charIndex = 0; charIndex < symbols.size(); ++charIndex)
    {
        auto glyphIndex = FT_Get_Char_Index(face, symbols[charIndex]);
        FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        auto glyphMetrics = face->glyph->metrics;

        unsigned int x = (charIndex % GLYPHS_IN_ROW) * glyphWidth;
        unsigned int y = (charIndex / GLYPHS_IN_ROW) * glyphHeight;

        FontBitmap::GlyphInfo glyphInfo = {};
        glyphInfo.advance = glyphMetrics.horiAdvance >> 6;
        glyphInfo.width = glyphMetrics.width >> 6;
        glyphInfo.height = glyphMetrics.height >> 6;
        glyphInfo.bearingX = glyphMetrics.horiBearingX >> 6;
        glyphInfo.bearingY = glyphMetrics.horiBearingY >> 6;
        glyphInfo.bitmapX = x + padding;
        glyphInfo.bitmapY = y + padding;
        glyphInfo.bitmapW = face->glyph->bitmap.width;
        glyphInfo.bitmapH = face->glyph->bitmap.rows;

        for(unsigned int xx = 0; xx < face->glyph->bitmap.width; ++xx)
        {
            for(unsigned int yy = 0; yy < face->glyph->bitmap.rows; ++yy)
            {
                result.bitmap[(y + yy + padding) * bitmapWidth + (x + xx + padding)] =
                    face->glyph->bitmap.buffer[yy * face->glyph->bitmap.width + xx];
            }
        }
    }

    return result;
}
