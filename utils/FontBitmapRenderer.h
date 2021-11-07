#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <vector>
#include <unordered_map>
#include <cmath>
#include <stdexcept>

struct FontBitmap
{
    struct GlyphInfo
    {
        unsigned int bitmapX;
        unsigned int bitmapY;
        unsigned int bitmapW;
        unsigned int bitmapH;

        unsigned int bearingX;
        unsigned int bearingY;
        unsigned int advance;
        unsigned int width;
        unsigned int height;
    };

    unsigned char* bitmap;
    unsigned int bitmapWidth;
    unsigned int bitmapHeight;

    std::unordered_map<char32_t, GlyphInfo> glyphs;
};

class FontBitmapRenderer
{
public:

    /*
     * Draws a bitmap with all symbols using given face, font size and padding between glyphs
     * Bitmap is R8 format (only red channel with 8 bits per each pixel)
     */
    FontBitmap RenderBitmap(FT_Face face, const std::vector<char32_t>& symbols, unsigned int fontSize, unsigned int padding);

};


