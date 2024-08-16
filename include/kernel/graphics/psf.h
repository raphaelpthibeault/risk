#ifndef PSF_H
#define PSF_H

#include <stdint.h>

/* see https://wiki.osdev.org/PC_Screen_Font */

// PSF1 font magic: 0x0436
#define PSF1_FONT_MAGIC 0x0436

#define PSF2_FONT_MAGIC 0x864ab572

#define PSF_V1 0x01
#define PSF_V2 0x02

typedef struct {
  uint16_t magic;
  uint8_t mode;
  uint8_t charsize; // witdth is always 0 so height is equal to charsize
} PSF1_font_t;


typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF2_font_t;

extern char _binary_fonts_default_psf_start[];
extern char _binary_fonts_default_psf_end;
extern char _binary_fonts_default_psf_size;

uint8_t psf_get_version(char *);
uint8_t *psf_get_glyph(uint8_t,uint8_t);
uint32_t psf_get_height(uint8_t);
uint32_t psf_get_width(uint8_t);

#endif /* ifndef PSF_H */
