#ifndef GLES_ARITHMETIC_H
#define GLES_ARITHMETIC_H 1

// ==========================================================================
//
// arithmetic.h		Arithmetic base types for implementation of OpenGL ES
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


#include "OGLES.h"
#include <math.h>
#include <assert.h>


// --------------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------------



// --------------------------------------------------------------------------
// Types
// --------------------------------------------------------------------------

namespace EGL {

	// ----------------------------------------------------------------------
	// A non-IEEE floating point type
	//
	// The floating point number is represented as a signed 32-bit word,
	// where the upper 24 bits form a signed mantissa, and the lower 8 bits
	// contain an unsigned exponent.
	//
	// The exponent is adjusted such that a value of 2^30 corresponds to
	// the pattern, e.g. the integer value is truncated and the exponent 
	// is 128.
	//
	//				24-bit signed mantissa			 exponent
	//				-------------------------------  ---------
	//				0100 0000  0000 0000  0000 0000  1000 0000
	//
	// A value of zero is represented as an all zero word.
	// ----------------------------------------------------------------------

	class Float	{
	private:
		Float(I32 mantissa, U8 exponent);

		I32 Mantissa() const {
			return m_mantissa;
		}

		U8 Exponent() const {
			return m_exponent;
		}

	public:
		Float() {
			m_value = 0;
		}

		Float(const Float& other) {
			m_value = other.m_value;
		}

		Float& operator=(const Float& other) {
			m_value = other.m_value;
			return *this;
		}

		Float& operator+=(const Float& other) {
			*this = *this + other;
			return *this;
		}

		Float& operator-=(const Float& other) {
			*this = *this - other;
			return *this;
		}

		Float& operator*=(const Float& other) {
			*this = *this * other;
			return *this;
		}

		Float& operator/=(const Float& other) {
			*this = *this / other;
			return *this;
		}

		Float operator+(const Float& other) const;

		Float operator-(const Float& other) const;

		Float operator*(const Float& other) const;

		Float operator/(const Float& other) const {
			return *this * Inverse(other);
		}

		Float operator-() const {
			return Float(-Mantissa(), Exponent());
		}

		Float operator+() const {
			return *this;
		}

		bool operator==(const Float& other) const {
			return m_value == other.m_value;
		}

		bool operator!=(const Float& other) const {
			return m_value != other.m_value;
		}

		bool operator<(const Float& other) const {
			return (*this - other).Mantissa() < 0;
		}

		bool operator<=(const Float& other) const {
			return (*this - other).Mantissa() <= 0;
		}

		bool operator>(const Float& other) const {
			return (*this - other).Mantissa() > 0;
		}

		bool operator>=(const Float& other) const {
			return (*this - other).Mantissa() >= 0;
		}

		static Float fromX(GLfixed value);

		static Float fromI(GLint value) {
			return Float(value, 128);
		}

		GLfixed toX() const;

		GLint   toI() const {
			I32 diff = static_cast<I32>(Exponent()) - 128;

			if (diff < 0)
				return Mantissa() >> -diff;
			else 
				return Mantissa() << diff;
		}

		static Float fromF(GLfloat value) {
			int exp;
			double mant = frexp(value, &exp);
			return Float(static_cast<I32>(mant * (1 << 23)), exp + 128 - 23);
		}

		GLfloat toF() const {
			return static_cast<GLfloat>(ldexp(static_cast<float>(Mantissa()), static_cast<I32>(Exponent()) - 128));
		}


		static Float Normalized(Float value);

		void Normalize() {
			*this = Normalized(*this);
		}

		static Float Inverse(Float value);

		static Float Zero() {
			return Float::fromI(0);
		}

		static Float One() {
			return Float::fromI(1);
		}

		static Float Two() {
			return Float::fromI(2);
		}

	private:
		union {
			struct {
				unsigned	m_exponent: 8;
				signed		m_mantissa: 24;			// the lower 8 bits contain the exponent
													// for 0, all bits are 0
			};
			I32 m_value;
		};
	};
}

#endif //ndef GLES_ARITHMETIC_H
