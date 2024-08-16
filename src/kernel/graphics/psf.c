#include <graphics/psf.h>
#include <stdint.h>

uint8_t _psf_font_version;

uint8_t 
psf_get_version(char *font) {
    PSF1_font_t *v1_font = (PSF1_font_t*)font;
    if (v1_font->magic == PSF1_FONT_MAGIC) 
        return PSF_V1;

    PSF2_font_t *v2_font = (PSF2_font_t*)font;
    if (v2_font->magic == PSF2_FONT_MAGIC)
        return PSF_V2;

    return 0;
}

uint8_t *
psf_get_glyph(uint8_t symnum, uint8_t version) {
    switch (version) {
        case PSF_V1: {
            PSF1_font_t *loaded = (PSF1_font_t*)&_binary_fonts_default_psf_start;
            return (uint8_t *)loaded + sizeof(PSF1_font_t) + (symnum * loaded->charsize);
        }
        case PSF_V2: {
            PSF2_font_t *loaded = (PSF2_font_t*)&_binary_fonts_default_psf_start;
            return (uint8_t *)loaded + loaded->headersize + (symnum * loaded->bytesperglyph);
        }
        default: 
            return 0;
    }
}

uint32_t 
psf_get_height(uint8_t version) {
    if (version == PSF_V1)
        return ((PSF1_font_t *)&_binary_fonts_default_psf_start)->charsize;

    return ((PSF2_font_t *)&_binary_fonts_default_psf_start)->height;

}

uint32_t 
psf_get_width(uint8_t version) {
    if (version == PSF_V1)
        return 8;

    return ((PSF2_font_t *)&_binary_fonts_default_psf_start)->width;
}

