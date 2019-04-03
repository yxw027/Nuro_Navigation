// ==========================================================================
//
// triangles.cpp	Rendering Context Class for 3D Rendering Library
//
//					Rendering Operations for Triangles
//
// --------------------------------------------------------------------------
//
// 08-12-2003	Hans-Martin Will	initial version
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
#include "Context.h"
#include "fixed.h"
#include "Rasterizer.h"
#include "Utils.h"

using namespace EGL;


void Context :: FrontFace(GLenum mode) {

	switch (mode) {
		case GL_CW:
			m_ReverseFaceOrientation = true;
			break;

		case GL_CCW:
			m_ReverseFaceOrientation = false;
			break;

		default:
			RecordError(GL_INVALID_ENUM);
			break;
	}
}

void Context :: CullFace(GLenum mode) {

	switch (mode) {
		case GL_FRONT:
			m_CullMode = CullModeFront;
			break;

		case GL_BACK:
			m_CullMode = CullModeBack;
			break;

		case GL_FRONT_AND_BACK:
			m_CullMode = CullModeBackAndFront;
			break;

		default:
			RecordError(GL_INVALID_ENUM);
			break;
	}
}

namespace {
	inline I64 MulLong(EGL_Fixed a, EGL_Fixed b) {
		return (((I64) a * (I64) b)  >> EGL_PRECISION);
	}

	inline EGL_Fixed Round(EGL_Fixed value) {
		return (value + 8) >> 4;
	}
}

void Context :: RenderTriangle(Vertex& a, Vertex& b, Vertex& c) {

	if (a.m_cc & b.m_cc & c.m_cc)
		return;

	EGL_Fixed x0 = a.m_ClipCoords.w();
	EGL_Fixed x1 = a.m_ClipCoords.x();
	EGL_Fixed x2 = a.m_ClipCoords.y();
								
	EGL_Fixed y0 = b.m_ClipCoords.w();
	EGL_Fixed y1 = b.m_ClipCoords.x();
	EGL_Fixed y2 = b.m_ClipCoords.y();
								
	EGL_Fixed z0 = c.m_ClipCoords.w();
	EGL_Fixed z1 = c.m_ClipCoords.x();
	EGL_Fixed z2 = c.m_ClipCoords.y();

	bool cw;
	I64 sign;
	
	sign = 
			+ (x0 >> 12) * (static_cast<I64>(y1 >> 12) * (z2 >> 12) - static_cast<I64>(z1 >> 12) * (y2 >> 12))
			- (y0 >> 12) * (static_cast<I64>(x1 >> 12) * (z2 >> 12) - static_cast<I64>(z1 >> 12) * (x2 >> 12))
			+ (z0 >> 12) * (static_cast<I64>(x1 >> 12) * (y2 >> 12) - static_cast<I64>(y1 >> 12) * (x2 >> 12));

	if (sign <= -(1 << 6))
		cw = true;
	else if (sign >= (1 << 6))
		cw = false;
	else {
		// This code assumes that truncation is handled properly, e.g. the sign is always correct,
		// and the lower 63 bits are the properly truncated result of the overall product.
		sign = 
				+ x0 * (static_cast<I64>(y1) * z2 - static_cast<I64>(z1) * y2)
				- y0 * (static_cast<I64>(x1) * z2 - static_cast<I64>(z1) * x2)
				+ z0 * (static_cast<I64>(x1) * y2 - static_cast<I64>(y1) * x2);

		cw = sign < 0;
	}

	bool backFace = cw ^ m_ReverseFaceOrientation;

	if (m_CullFaceEnabled) {
		if ((m_CullMode == CullModeFront) ^ backFace)
			return;
	}

	if (m_VaryingInfo->colorIndex >= 0) {
		LightMode mode;

		if (m_LightingEnabled) { 
			mode = (backFace && m_TwoSidedLightning) ? Back : Front;
			LightVertex(&c, mode);

			if (m_RasterizerState.GetShadeModel() == RasterizerState::ShadeModelSmooth) {
				LightVertex(&a, mode);
				LightVertex(&b, mode);
			}
		} else {
			mode = Unlit;
		}

		if (m_RasterizerState.GetShadeModel() == RasterizerState::ShadeModelSmooth) {
			a.m_Color[mode].toArray(a.m_Varying + m_VaryingInfo->colorIndex);
			b.m_Color[mode].toArray(b.m_Varying + m_VaryingInfo->colorIndex);
		} else {
			c.m_Color[mode].toArray(a.m_Varying + m_VaryingInfo->colorIndex);
			c.m_Color[mode].toArray(b.m_Varying + m_VaryingInfo->colorIndex);
		}

		c.m_Color[mode].toArray(c.m_Varying + m_VaryingInfo->colorIndex);
	}

	Vertex * array1[16], *array2[16], ** result = 0;
	array1[0] = &a;

	if (!cw) {
		array1[1] = &b;
		array1[2] = &c;
	} else {
		array1[2] = &b;
		array1[1] = &c;
	}

	size_t numVertices = ClipPrimitive(3, array1, array2, &result);

	if (numVertices >= 3) {
		ClipCoordsToWindowCoords(*result[0]);
		ClipCoordsToWindowCoords(*result[1]);

		for (size_t index = 2; index < numVertices; ++index) {
			ClipCoordsToWindowCoords(*result[index]);
			m_Rasterizer->RasterTriangle(*result[0], *result[index - 1], *result[index]);
		}
	}

}


void Context :: DrawTriangle(int index) {
	SelectArrayElement(index, &m_Input[m_NextIndex]);

	if (++m_NextIndex == 3) {
		RenderTriangle(m_Input[0], m_Input[1], m_Input[2]);
		m_NextIndex = 0;
	}
}

void Context :: DrawTriangleStrip(int index) {
	SelectArrayElement(index, &m_Input[m_NextIndex]);

	if (m_PrimitiveState == 3) {
		// even triangle
		I32 prevIndex = (m_NextIndex - 1) > m_NextIndex ? m_NextIndex + 2 : m_NextIndex - 1;
		I32 prevIndex2 = (m_NextIndex - 2) > m_NextIndex ? m_NextIndex + 1 : m_NextIndex - 2;
		RenderTriangle(m_Input[prevIndex], m_Input[prevIndex2], m_Input[m_NextIndex]);
		m_PrimitiveState = 2;
	} else if (m_PrimitiveState == 2) {
		// odd triangle
		I32 prevIndex = (m_NextIndex - 1) > m_NextIndex ? m_NextIndex + 2 : m_NextIndex - 1;
		I32 prevIndex2 = (m_NextIndex - 2) > m_NextIndex ? m_NextIndex + 1 : m_NextIndex - 2;
		RenderTriangle(m_Input[prevIndex2], m_Input[prevIndex], m_Input[m_NextIndex]);
		m_PrimitiveState = 3;
	} else {
		// remember seen a vertex
		++m_PrimitiveState;
	}

	if (++m_NextIndex == 3)
		m_NextIndex = 0;
}

void Context :: DrawTriangleFan(int index) {
	SelectArrayElement(index, &m_Input[m_NextIndex]);

	if (m_PrimitiveState == 3) {
		// even triangle
		I32 prevIndex = (m_NextIndex - 1) > m_NextIndex ? m_NextIndex + 2 : m_NextIndex - 1;
		I32 prevIndex2 = (m_NextIndex - 2) > m_NextIndex ? m_NextIndex + 1 : m_NextIndex - 2;
		RenderTriangle(m_Input[prevIndex], m_Input[prevIndex2], m_Input[m_NextIndex]);
		m_PrimitiveState = 2;
	} else if (m_PrimitiveState == 2) {
		// odd triangle
		I32 prevIndex = (m_NextIndex - 1) > m_NextIndex ? m_NextIndex + 2 : m_NextIndex - 1;
		I32 prevIndex2 = (m_NextIndex - 2) > m_NextIndex ? m_NextIndex + 1 : m_NextIndex - 2;
		RenderTriangle(m_Input[prevIndex2], m_Input[prevIndex], m_Input[m_NextIndex]);
		m_PrimitiveState = 3;
	} else if (m_PrimitiveState == 1) {
		// remember seen second vertex
		m_PrimitiveState = 2;
	} else if (m_PrimitiveState == 0) {
		// remember seen first vertex
		m_PrimitiveState = 1;
	}

	if (++m_NextIndex == 3)
		m_NextIndex = 1;
}

