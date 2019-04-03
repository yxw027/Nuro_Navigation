// ==========================================================================
//
// arithmetic.cpp	Arithmetic base types for implementation of OpenGL ES
//					library.
//
//					This file contains declarations of numeric types
//
// --------------------------------------------------------------------------
//
// 01-07-2006	Hans-Martin Will	initial version
//
// --------------------------------------------------------------------------
//
// Copyright (c) 2004, Hans-Martin Will. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
//	 *  Redistributions of source code must retain the above copyright
// 		notice, this list of conditions and the following disclaimer. 
//   *	Redistributions in binary form must reproduce the above copyright
// 		notice, this list of conditions and the following disclaimer in the 
// 		documentation and/or other materials provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.
//
// ==========================================================================


#include "stdafx.h"
#include "arithmetic.h"


using namespace EGL;


namespace {
	inline I32 CountLeadingZeros(I32 x) {
#ifdef EGL_USE_CLZ
		return _CountLeadingZeros(x);
#else
		U8 zeros = 31;
		if (x & 0xffff0000) { zeros -= 16; x >>= 16; }
		if (x & 0xff00) { zeros -= 8; x >>= 8; }
		if (x & 0xf0) { zeros -= 4; x >>= 4; }
		if (x & 0xc) { zeros -= 2; x >>= 2; }
		if (x & 0x2) { zeros -= 1; }
		return zeros;
#endif
	}

	inline I32 Mul24(I32 a, I32 b) {
		I64 product = static_cast<I64>(a) * static_cast<I64>(b);
		return static_cast<I32>((product + 0x200000) >> 22);
	}
}

inline Float::Float(I32 mantissa, U8 exponent)
{
	if (mantissa) {
		I32 zeros = CountLeadingZeros(mantissa < 0 ? -mantissa : mantissa);
		I32 shift = zeros - 9;
		m_mantissa = shift == 0 ? mantissa : shift > 0 ? (mantissa << shift) : mantissa + (1 << (shift - 1)) >> -shift;
		m_exponent = exponent - shift;
	} else {
		m_value = 0;
	}
}

Float Float::fromX(GLfixed value) {
	return Float(value, 128 - 16);
}

GLfixed Float::toX() const {
	I32 mantissa = Mantissa();

	if (!mantissa)
		return 0;

	I32 diff = static_cast<I32>(Exponent()) - (128 - 16);

	if (diff < 0)
		return (mantissa + (1 << (-diff - 1))) >> -diff;
	else if (diff > 0)
		return mantissa << diff;
	else 
		return mantissa;
}

Float Float::Normalized(Float value) {
	return Float(value.Mantissa(), value.Exponent());
}

Float Float::operator+(const Float& other) const {
	I32 diff = static_cast<I32>(Exponent()) - static_cast<I32>(other.Exponent());
	I32 mant1 = Mantissa();
	I32 mant2 = other.Mantissa();

	if (diff == 0) {
		return Float(mant1 + mant2, Exponent());
	} else if (diff > 0) {
		return Float(mant1 + (mant2 + (1 << diff - 1) >> diff), Exponent());
	} else {
		return Float((mant1 + (1 << -diff - 1) >> -diff) + mant2, other.Exponent());
	}
}

Float Float::operator-(const Float& other) const {
	I32 mant1 = Mantissa();
	I32 mant2 = -other.Mantissa();
	I32 diff = static_cast<I32>(Exponent()) - static_cast<I32>(other.Exponent());

	if (diff == 0) {
		return Float(mant1 + mant2, Exponent());
	} else if (diff > 0) {
		return Float(mant1 + (mant2 + (1 << diff - 1) >> diff), Exponent());
	} else {
		return Float((mant1 + (1 << -diff - 1) >> -diff) + mant2, other.Exponent());
	}
}

Float Float::operator*(const Float& other) const {
	if (m_value == 0 || other.m_value == 0)
		return Float(0, 0);
	else {
		I32 result = static_cast<I32>(static_cast<I64>(Mantissa()) * static_cast<I64>(other.Mantissa()) + 0x80000 >> 20);
		U8 exp = static_cast<I32>(Exponent()) + static_cast<I32>(other.Exponent()) - (128 - 20);
		return Float(result, exp);
	}
}


Float Float::Inverse(Float value) {
	I32 a = value.Mantissa();
    I32 x;

    /* 1/(4x) */
    static const I32 __gl_rcp_tab[] = { /* domain 0.5 .. 1.0-1/16 */
		0x8000, 0x71c7, 0x6666, 0x5d17, 0x5555, 0x4ec4, 0x4924, 0x4444
    };
	bool sign = false;

    if (a == 0) return Float(0x7fffff, 255);

	if (a < 0) {
		sign = true;
		a = -a;
	}

    x = __gl_rcp_tab[(a >> 20)&0x7] << 7;

	//printf("est %f\n", __GL_F_2_FLOAT(x));
    /* four iterations of newton-raphson  x = x(2-ax) */
	x = Mul24(x,(0x800000 - Mul24(a,x)));
	x = Mul24(x,(0x800000 - Mul24(a,x)));
	x = Mul24(x,(0x800000 - Mul24(a,x)));
	x = Mul24(x,(0x800000 - Mul24(a,x)));
	x = Mul24(x,(0x800000 - Mul24(a,x)));
	x = Mul24(x,(0x800000 - Mul24(a,x)));
//printf("recip %f %f\n", __GL_F_2_FLOAT(a), __GL_F_2_FLOAT(x));

	if (sign)
		x = -x;

	return Float(x, (256 - 44) - static_cast<I32>(value.Exponent()));
}
