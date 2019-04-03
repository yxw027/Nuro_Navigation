#ifndef NURO_COLOR_H_FADKJFKAJSKN34NFA
#define NURO_COLOR_H_FADKJFKAJSKN34NFA

#include "nurotypes.h"
class nuroColor
{
public:
    enum
    {
        COLORTYPE_RGBA32 = 0,
            COLORTYPE_RGB565
    };
public:
    inline static nuroU32 RGBA32(nuroS32 r_, nuroS32 g_, nuroS32 b_, nuroS32 a_)
    {
        return r_ | (g_<<8) | (b_<<16) | (a_<<24);
    }
    inline static nuroU16 RGB565(nuroS32 r_, nuroS32 g_, nuroS32 b_)
    {
        return ( (r_&0xf8)<<8 ) | ( (g_&0xfc)<<3 ) | ( b_>>3 );
    }
    inline static nuroU16 RGB565FromRGBA32(nuroU32 color)
    {
        nuroU16 r = 0xff & (color);
        nuroU16 g = 0xff & (color>>8);
        nuroU16 b = 0xff & (color>>16);
        return RGB565(r, g, b);
    }
    inline static nuroU32 RGBA32FromRGB565(nuroU16 color565)
    {
        nuroS32 r = 0x07 | (color565>>8);
        nuroS32 g = 0x03 | ((color565&0x07E0)>>3);
        nuroS32 b = 0x07 | ((color565&0x001F)<<3);
        return RGBA32(r, g, b, 0xff);
    }
public:
    nuroColor()
        : _r(0), _g(0), _b(0), _a(0), _rgb565(0)//, _rgba32(0)
    {
    }
    explicit nuroColor(nuroU32 r, nuroU32 g, nuroU32 b, nuroU32 a)
        : _r(r), _g(g), _b(b), _a(a)
    {
        //_rgba32 = RGBA32(_r, _g, _b, _a);
        _rgb565 = RGB565(_r, _g, _b);
    }
    explicit nuroColor(nuroU32 rgb, nuroS32 type)
    {
        SetValue(rgb, type);
    }
    inline void SetValue(nuroU32 rgb, nuroS32 type)
    {
        switch (type)
        {
        case COLORTYPE_RGB565:
            _r = 0x07 | (rgb>>8);
            _g = 0x03 | ((rgb&0x07E0)>>3);
            _b = 0x07 | ((rgb&0x001F)<<3);
            //_a = 0xff;
            _rgb565 = rgb;
            //_rgba32 = RGBA32(_r, _g, _b, _a);
            break;
        case COLORTYPE_RGBA32:
            _r = 0xff & (rgb);
            _g = 0xff & (rgb>>8);
            _b = 0xff & (rgb>>16);
            _a = 0xff & (rgb>>24);
            //_rgba32 = rgb;
            _rgb565 = RGB565(_r, _g, _b);
        default:
            break;
        }
    }
    inline nuroS32 RValue()
    {
        return _r;
    }
    inline nuroS32 GValue()
    {
        return _g;
    }
    inline nuroS32 BValue()
    {
        return _b;
    }
    inline nuroS32 AValue()
    {
        return _a;
    }
	inline void SetAlpha(nuroS32 a)
	{
		_a = a;
	}
    inline nuroU32 RGBA32Data()
    {
        return RGBA32(_r, _g, _b, _a);//_rgba32;
    }
    inline nuroU16 RGB565Data()
    {
        return _rgb565;
    }
private:
    nuroU8 _r;
    nuroU8 _g;
    nuroU8 _b;
    nuroU8 _a;
    //nuroU32 _rgba32;
    nuroU16 _rgb565;
    nuroU16 _res;
};

#endif //NURO_COLOR_H_FADKJFKAJSKN34NFA
