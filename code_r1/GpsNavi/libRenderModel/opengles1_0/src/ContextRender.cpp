// ==========================================================================
//
// render.cpp	Rendering Context Class for 3D Rendering Library
//
//				Rendering Operations
//
// --------------------------------------------------------------------------
//
// 08-07-2003	Hans-Martin Will	initial version
// 25-03-2006	Hans-Martin Will	reworked clipping
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
// --------------------------------------------------------------------------
//
// Copyright (C) 2003  David Blythe   All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// DAVID BLYTHE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// ==========================================================================


#include "stdafx.h"
#include <string.h>
#include "fixed.h"
#include "Context.h"
#include "Surface.h"
#include "Utils.h"

using namespace EGL;
#ifdef DENGXU_LOG
void opengles_save_log(const char *format, ...);
#endif


// --------------------------------------------------------------------------
// Setup mesh arrays
// --------------------------------------------------------------------------

void Context :: ToggleClientState(GLenum array, bool value) {
	switch (array) {
	case GL_TEXTURE_COORD_ARRAY:
		m_TexCoordArrayEnabled[m_ClientActiveTexture] = value;
		break;

	case GL_COLOR_ARRAY:
		m_ColorArrayEnabled = value;
		break;

	case GL_NORMAL_ARRAY:
		m_NormalArrayEnabled = value;
		break;

	case GL_VERTEX_ARRAY:
		m_VertexArrayEnabled = value;
		break;

	case GL_POINT_SIZE_ARRAY_OES:
		m_PointSizeArrayEnabled = value;
		break;

	default:
		RecordError(GL_INVALID_ENUM);
	}
}

void Context :: DisableClientState(GLenum array) {
	ToggleClientState(array, false);
}

void Context :: EnableClientState(GLenum array) {
	ToggleClientState(array, true);
}

void Context :: ColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {

	if (type != GL_UNSIGNED_BYTE && type != GL_FIXED && type != GL_FLOAT) {
		RecordError(GL_INVALID_ENUM);
		return;
	}

	if (size != 4) {
		RecordError(GL_INVALID_VALUE);
		return;
	}

	if (stride < 0) {
		RecordError(GL_INVALID_VALUE);
		return;
	}

	if (stride == 0) {
		switch (type) {
		case GL_UNSIGNED_BYTE:
			stride = sizeof (GLubyte) * size;
			break;

		case GL_FIXED:
			stride = sizeof (GLfixed) * size;
			break;

		case GL_FLOAT:
			stride = sizeof (GLfloat) * size;
			break;
		}
	}

	m_ColorArray.pointer = pointer;
	m_ColorArray.stride = stride;
	m_ColorArray.type = type;
	m_ColorArray.size = size;
	m_ColorArray.boundBuffer = m_CurrentArrayBuffer;
}

void Context :: NormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer) {

	if (type != GL_BYTE && type != GL_SHORT && type != GL_FIXED && type != GL_FLOAT) {
		RecordError(GL_INVALID_ENUM);
		return;
	}

	GLsizei size = 3;

	if (stride < 0) {
		RecordError(GL_INVALID_VALUE);
		return;
	}

	if (stride == 0) {
		switch (type) {
		case GL_BYTE:
			stride = sizeof (GLbyte) * size;
			break;

		case GL_SHORT:
			stride = sizeof (GLshort) * size;
			break;

		case GL_FIXED:
			stride = sizeof (GLfixed) * size;
			break;

		case GL_FLOAT:
			stride = sizeof (GLfloat) * size;
			break;

		}
	}

	m_NormalArray.pointer = pointer;
	m_NormalArray.stride = stride;
	m_NormalArray.type = type;
	m_NormalArray.size = size;
	m_NormalArray.boundBuffer = m_CurrentArrayBuffer;
}

void Context :: VertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {

	if (type != GL_BYTE && type != GL_SHORT && type != GL_FIXED && type != GL_FLOAT) {
		RecordError(GL_INVALID_ENUM);
		return;
	}

	if (size < 2 || size > 4) {
		RecordError(GL_INVALID_VALUE);
		return;
	}

	if (stride < 0) {
		RecordError(GL_INVALID_VALUE);
		return;
	}

	if (stride == 0) {
		switch (type) {
		case GL_BYTE:
			stride = sizeof (GLbyte) * size;
			break;

		case GL_SHORT:
			stride = sizeof (GLshort) * size;
			break;

		case GL_FIXED:
			stride = sizeof (GLfixed) * size;
			break;

		case GL_FLOAT:
			stride = sizeof (GLfloat) * size;
			break;

		}
	}

	m_VertexArray.pointer = pointer;
	m_VertexArray.stride = stride;
	m_VertexArray.type = type;
	m_VertexArray.size = size;
	m_VertexArray.boundBuffer = m_CurrentArrayBuffer;
}

void Context :: TexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {

	if (type != GL_BYTE && type != GL_SHORT && type != GL_FIXED && type != GL_FLOAT) {
		RecordError(GL_INVALID_ENUM);
		return;
	}

	if (size < 2 || size > 4) {
		RecordError(GL_INVALID_VALUE);
		return;
	}

	if (stride < 0) {
		RecordError(GL_INVALID_VALUE);
		return;
	}

	if (stride == 0) {
		switch (type) {
		case GL_BYTE:
			stride = sizeof (GLbyte) * size;
			break;

		case GL_SHORT:
			stride = sizeof (GLshort) * size;
			break;

		case GL_FIXED:
			stride = sizeof (GLfixed) * size;
			break;

		case GL_FLOAT:
			stride = sizeof (GLfloat) * size;
			break;

		}
	}

	m_TexCoordArray[m_ClientActiveTexture].pointer = pointer;
	m_TexCoordArray[m_ClientActiveTexture].stride = stride;
	m_TexCoordArray[m_ClientActiveTexture].type = type;
	m_TexCoordArray[m_ClientActiveTexture].size = size;
	m_TexCoordArray[m_ClientActiveTexture].boundBuffer = m_CurrentArrayBuffer;
}


// --------------------------------------------------------------------------
// Default values if corrsponding array is disabled
// --------------------------------------------------------------------------


void Context :: Color4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha) {
	m_DefaultRGBA.r = red;
	m_DefaultRGBA.g = green;
	m_DefaultRGBA.b = blue;
	m_DefaultRGBA.a = alpha;
}


void Context :: MultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q) {

	if (target < GL_TEXTURE0 || target >= GL_TEXTURE0 + EGL_NUM_TEXTURE_UNITS) {
		// only have one texture unit
		RecordError(GL_INVALID_VALUE);
		return;
	}

	size_t unit = target - GL_TEXTURE0;

	m_DefaultTextureCoords[unit] = Vec4D(s, t, r, q);
}


void Context :: Normal3x(GLfixed nx, GLfixed ny, GLfixed nz) {
	m_DefaultNormal = Vec3D(nx, ny, nz);
}


// --------------------------------------------------------------------------
// Actual array rendering
// --------------------------------------------------------------------------


bool Context :: Begin(GLenum mode) {
	m_PrimitiveState = 0;
	m_NextIndex = 0;

	switch (mode) {
	case GL_POINTS:
		m_Rasterizer->PreparePoint();
		m_DrawPrimitiveFunction = &Context::DrawPoint;
		m_EndPrimitiveFunction = 0;
		break;

	case GL_LINES:
		m_Rasterizer->PrepareLine();
		m_DrawPrimitiveFunction = &Context::DrawLine;
		m_EndPrimitiveFunction = 0;
		break;

	case GL_LINE_STRIP:
		m_Rasterizer->PrepareLine();
		m_DrawPrimitiveFunction = &Context::DrawLineStrip;
		m_EndPrimitiveFunction = 0;
		break;

	case GL_LINE_LOOP:
		m_Rasterizer->PrepareLine();
		m_DrawPrimitiveFunction = &Context::DrawLineLoop;
		m_EndPrimitiveFunction = &Context::EndLineLoop;
		break;

	case GL_TRIANGLES:
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif
		m_Rasterizer->PrepareTriangle();
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif
		m_DrawPrimitiveFunction = &Context::DrawTriangle;
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif
		m_EndPrimitiveFunction = 0;
		break;

	case GL_TRIANGLE_STRIP:
		m_Rasterizer->PrepareTriangle();
		m_DrawPrimitiveFunction = &Context::DrawTriangleStrip;
		m_EndPrimitiveFunction = 0;
		break;

	case GL_TRIANGLE_FAN:
		m_Rasterizer->PrepareTriangle();
		m_DrawPrimitiveFunction = &Context::DrawTriangleFan;
		m_EndPrimitiveFunction = 0;
		break;

	default:
		RecordError(GL_INVALID_ENUM);
		return false;
	}

	return true;
}

void Context :: End() {
	if (m_EndPrimitiveFunction)
		(this->*m_EndPrimitiveFunction)();

	m_DrawPrimitiveFunction = 0;
	m_EndPrimitiveFunction = 0;
}

void Context :: DrawArrays(GLenum mode, GLint first, GLsizei count) {

#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif
	if (count < 0) {
		RecordError(GL_INVALID_VALUE);
		return;
	}
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif

	if (!m_VertexArrayEnabled) {
		RecordError(GL_INVALID_OPERATION);
		return;
	}
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif

	PrepareRendering();
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d mode = %d\n", __func__, __FILE__, __LINE__, mode);
#endif
	
	if (Begin(mode)) {

#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d mode = %d\n", __func__, __FILE__, __LINE__, mode);
#endif
		while (count-- > 0) {
			(this->*m_DrawPrimitiveFunction)(first++);
		}
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif

		End();
	}
}


void Context :: DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) {

	if (count < 0) {
		RecordError(GL_INVALID_VALUE);
		return;
	}

	if (!m_VertexArrayEnabled) {
		RecordError(GL_INVALID_OPERATION);
		return;
	}

	if (m_CurrentElementArrayBuffer) {
		U8 * bufferBase =
			static_cast<U8 *>(m_Buffers.GetObject(m_CurrentElementArrayBuffer)->GetData());

		if (!bufferBase) {
			RecordError(GL_INVALID_OPERATION);
			return;
		}

		size_t offset = static_cast<const U8 *>(indices) - static_cast<const U8 *>(0);
		indices = bufferBase + offset;
	}

	if (!indices) {
		return;
	}

	PrepareRendering();

	if (type == GL_UNSIGNED_BYTE) {
		const GLubyte * ptr = reinterpret_cast<const GLubyte *>(indices);

		if (Begin(mode)) {
			while (count-- > 0) {
				(this->*m_DrawPrimitiveFunction)(*ptr++);
			}

			End();
		}
	} else if (type == GL_UNSIGNED_SHORT) {
		const GLushort * ptr = reinterpret_cast<const GLushort *>(indices);

		if (Begin(mode)) {
			while (count-- > 0) {
				(this->*m_DrawPrimitiveFunction)(*ptr++);
			}

			End();
		}
	} else {
		RecordError(GL_INVALID_ENUM);
	}
}


// --------------------------------------------------------------------------
// Load all the current coordinates from either a specific array or from
// the common settings.
//
// Parameters:
//	index		-	The array index from which any array coordinates should
//					be retrieved.
// --------------------------------------------------------------------------
void Context :: SelectArrayElement(int index, Vertex * rasterPos) {

	assert(m_VertexArray.effectivePointer);

	{
		Vec4D currentVertex;

		m_VertexArray.FetchValues(index, currentVertex.getArray());
		m_ModelViewMatrixStack.CurrentMatrix().Multiply(currentVertex, rasterPos->m_EyeCoords);
		rasterPos->m_ClipCoords = m_ProjectionMatrixStack.CurrentMatrix() * rasterPos->m_EyeCoords;

		if (rasterPos->m_ClipCoords.w() < 0) 
			rasterPos->m_ClipCoords = -rasterPos->m_ClipCoords;

		CalcCC(rasterPos);
	}

	//	copy current colors to raster pos
	if (m_RasterizerState.IsEnabledFog()) {
		assert(m_VaryingInfo->fogIndex >= 0);
		rasterPos->m_Varying[m_VaryingInfo->fogIndex] = FogDensity(EGL_Abs(rasterPos->m_EyeCoords.z()));
	}

	if (m_NormalArray.effectivePointer) {
		Vec3D normal;

		m_NormalArray.FetchValues(index, normal.getArray());
		rasterPos->m_EyeNormal = m_InverseModelViewMatrix.Multiply3x3(normal);
	} else {
		rasterPos->m_EyeNormal = m_TransformedDefaultNormal;
	}

	if (m_ColorArray.effectivePointer) {
		m_ColorArray.FetchValues(index, rasterPos->m_Color[Unlit].getArray());
	} else {
		rasterPos->m_Color[Unlit] = m_DefaultRGBA;
	}

	for (size_t unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
		if (m_VaryingInfo->textureBase[unit] >= 0) {
			I32 base = m_VaryingInfo->textureBase[unit];

			if (m_TexCoordArray[unit].effectivePointer) {
				Vec4D texCoords, projectedTexCoords;

				m_TexCoordArray[unit].FetchValues(index, texCoords.getArray());
			
				if (m_TextureMatrixStack[unit].CurrentMatrix().IsIdentity()) {
					projectedTexCoords = texCoords;
				} else {
					m_TextureMatrixStack[unit].CurrentMatrix().Multiply(texCoords, projectedTexCoords);
				}

				// according to Blythe & McReynolds, this should really happen
				// as part of the perspective interpolation; but that would require
				// interpolating another coordinate; which we do not want to afford
				projectedTexCoords.ProjectiveDivision();
				rasterPos->m_Varying[base]     = projectedTexCoords.x();
				rasterPos->m_Varying[base + 1] = projectedTexCoords.y();
			} else {
				rasterPos->m_Varying[base]     = m_DefaultTransformedTextureCoords[unit].x();
				rasterPos->m_Varying[base + 1] = m_DefaultTransformedTextureCoords[unit].y();
			}
		}
	}

	rasterPos->m_Lit = Unlit;
}


void Context :: PrepareArray(VertexArray & array, bool enabled, bool isColor) {

	array.effectivePointer = 0;

	if (enabled) {
		if (array.boundBuffer) {
			if (m_Buffers.IsObject(array.boundBuffer)) {
				U8 * bufferBase =
					static_cast<U8 *>(m_Buffers.GetObject(array.boundBuffer)->GetData());

				if (!bufferBase) {
					return;
				}

				size_t offset = static_cast<const U8 *>(array.pointer) - static_cast<const U8 *>(0);
				array.effectivePointer = bufferBase + offset;
			}
		} else {
			array.effectivePointer = array.pointer;
		}
	}

	array.PrepareFetchValues(isColor);
}


void Context :: PrepareRendering() {

	if (m_LightingEnabled) {
		if (m_ColorMaterialEnabled) {
               m_LightVertexFunction = &Context::LightVertexTrack;
		} else {
			m_LightVertexFunction = &Context::LightVertexNoTrack;
		}

		// for each light that is turned on, init effective color values
		int mask = 1;

		for (int index = 0; index < EGL_NUMBER_LIGHTS; ++index, mask <<= 1) {
			if (m_LightEnabled & mask) {
				m_Lights[index].InitWithMaterial(m_Material);
			}
		}

		// this is used if no color material tracking is enabled
		m_EffectiveLightModelAmbient = m_Material.GetAmbientColor() * m_LightModelAmbient;
		m_EffectiveLightModelAmbient.a = m_Material.GetDiffuseColor().a;
		m_EffectiveLightModelAmbient += m_Material.GetEmissiveColor();

	} else {
		m_LightVertexFunction = 0;
	}

	PrepareArray(m_VertexArray,   m_VertexArrayEnabled);
	PrepareArray(m_NormalArray,	  m_NormalArrayEnabled);
	PrepareArray(m_ColorArray,    m_ColorArrayEnabled, true);

	for (size_t unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
		Vec4D transformedTexCoords;

		PrepareArray(m_TexCoordArray[unit], m_TexCoordArrayEnabled[unit]);
		m_TextureMatrixStack[unit].CurrentMatrix().Multiply(m_DefaultTextureCoords[m_ActiveTexture], 
															m_DefaultTransformedTextureCoords[unit]);
		// according to Blythe & McReynolds, this should really happen
		// as part of the perspective interpolation
		m_DefaultTransformedTextureCoords[unit].ProjectiveDivision();
	}

	PrepareArray(m_PointSizeArray,m_PointSizeArrayEnabled);

	m_TransformedDefaultNormal = m_InverseModelViewMatrix.Multiply3x3(m_DefaultNormal);
}


namespace {
	void DumpVertices(size_t inputCount, Vertex * input[]) {
		printf("Number of vertices: %d\n", inputCount);

		for (size_t index = 0; index < inputCount; ++index) {
			printf("eye: (%x, %x, %x, %x)  clip: (%x, %x, %x, %x)\n",
				input[index]->m_EyeCoords.x(),
				input[index]->m_EyeCoords.y(),
				input[index]->m_EyeCoords.z(),
				input[index]->m_EyeCoords.w(),
				input[index]->m_ClipCoords.x(),
				input[index]->m_ClipCoords.y(),
				input[index]->m_ClipCoords.z(),
				input[index]->m_ClipCoords.w());
		}
	}
}

size_t Context :: ClipPrimitive(size_t inputCount, Vertex * input[], Vertex * output[], Vertex *** result) {
    size_t plane;

    Vertex ** vilist = input;
    Vertex ** volist = output;

    Vertex * vprev, * vnext;
    size_t i, icnt = inputCount, ocnt = 0;
	const size_t numVarying = m_VaryingInfo->numVarying;
	Vertex * nextTemporary = m_Temporary;

	if (m_ClipPlaneEnabled) {
		for (size_t index = 0, mask = 1; index < NUM_CLIP_PLANES; ++index, mask <<= 1) {
			if (m_ClipPlaneEnabled & mask) {
				U32 c;
				ocnt = 0;
				vnext = vilist[icnt - 1];

				EGL_Fixed currentCoeff = vnext->m_EyeCoords * m_ClipPlanes[index];

				for (c = 0; c < icnt; c++) {
					EGL_Fixed prevCoeff = currentCoeff;
					vprev = vnext;
					vnext = vilist[c];
					currentCoeff = vnext->m_EyeCoords * m_ClipPlanes[index];

					if ((currentCoeff > 0) && (prevCoeff < 0) ||
						(currentCoeff <= 0) && (prevCoeff > 0)) {
						Vertex & newVertex = *nextTemporary++;

						if (EGL_Abs(prevCoeff) > EGL_Abs(currentCoeff)) {
							I32 coeff = Coeff4q28(prevCoeff, prevCoeff - currentCoeff);
							InterpolateWithEye(newVertex, *vprev, *vnext, coeff, numVarying);
						} else {
							I32 coeff = Coeff4q28(currentCoeff, currentCoeff - prevCoeff);
							InterpolateWithEye(newVertex, *vnext, *vprev, coeff, numVarying);
						}

						CalcCC(&newVertex);
						volist[ocnt++] = &newVertex;
					}

					if (currentCoeff > 0) {
						volist[ocnt++] = vilist[c];
					}
				}

				{
					// swap input and output lists
					Vertex ** vtlist;
					vtlist = vilist; vilist = volist; volist = vtlist;
				}

				icnt = ocnt;

				if (!icnt)
					return 0;
			}
		}
	}

	U32 cc = 0;

	for (i = 0; i < icnt; ++i) {
		cc |= vilist[i]->m_cc;
	}

    for (plane = 0; plane < 6; plane++) {
		U32 c, p = 1 << plane;
		I32 coord = plane >> 1;
		bool inside, prev_inside;

		if (!(cc & p)) continue;

		cc = 0;
		ocnt = 0;
		vnext = vilist[icnt - 1];

		inside = !(vnext->m_cc & p);

		for (c = 0; c < icnt; c++) {

			Vertex * voutside, *vinside;
			prev_inside = inside;
			vprev = vnext;
			vnext = vilist[c];
			inside = !(vnext->m_cc & p);

			if (inside ^ prev_inside) {
				if (inside) { 
					vinside = vnext; 
					voutside = vprev; 
				} else { 
					voutside = vnext; 
					vinside = vprev; 
				}

				EGL_Fixed ci = vinside->m_ClipCoords[coord];
				EGL_Fixed wi = vinside->m_ClipCoords.w();
				EGL_Fixed co = voutside->m_ClipCoords[coord];
				EGL_Fixed wo = voutside->m_ClipCoords.w();

				if (!(plane & 1)) {
					wi = -wi; wo = -wo;
				}

				EGL_Fixed num = wi - ci;
				EGL_Fixed denom = co - ci - wo + wi;

				Vertex & newVertex = *nextTemporary++;

				I32 coeff = Coeff4q28(num, denom);

				if (coeff < (1 << 27))
					Interpolate(newVertex, *vinside, *voutside, coeff, numVarying);
				else
					Interpolate(newVertex, *voutside, *vinside, (1 << 28) - coeff, numVarying);
				//newVertex.m_ClipCoords[coord] = newVertex.m_ClipCoords.w();
				CalcCC(&newVertex);
				cc |= newVertex.m_cc;

				volist[ocnt++] = &newVertex;
			}

			if (inside /*&& c != icnt-1*/) {
				volist[ocnt++] = vilist[c];
				cc |= vilist[c]->m_cc;
			}
		}

		{
			// swap input and output lists
			Vertex ** vtlist;
			vtlist = vilist; vilist = volist; volist = vtlist;
		}

		icnt = ocnt;

		if (!icnt)
			return 0;

    }

    *result = vilist;

	return icnt;
}


void Context :: ClipCoordsToWindowCoords(Vertex & pos) {

	// perform depth division
	EGL_Fixed x = pos.m_ClipCoords.x();
	EGL_Fixed y = pos.m_ClipCoords.y();
	EGL_Fixed z = pos.m_ClipCoords.z();
	EGL_Fixed w = pos.m_ClipCoords.w();

# if 1
	// don't use this because we will snap to 1/16th pixel further down
	// fix possible rounding problems
	if (x < -w)	x = -w;
	if (x > w)	x = w;
	if (y < -w)	y = -w;
	if (y > w)	y = w;
	if (z < -w)	z = -w;
	if (z > w)	z = w;
#endif

	if ((w >> 24) && (w >> 24) + 1) {
		// keep this value around for perspective-correct texturing
		EGL_Fixed invDenominator = EGL_Inverse(w >> 8);

		// Scale 1/Z by 2^4 to avoid rounding problems during prespective correct
		// interpolation
		// See book by LaMothe for more detailed discussion on this
		pos.m_WindowCoords.invW = invDenominator << 4;

		pos.m_WindowCoords.x = 
			EGL_Mul(EGL_Mul(x >> 8, invDenominator), m_ViewportScale.x()) + m_ViewportOrigin.x();
		//pos.m_WindowCoords.x = ((pos.m_WindowCoords.x + 0x800) & ~0xfff);

		pos.m_WindowCoords.y = 
			EGL_Mul(EGL_Mul(y >> 8, invDenominator), m_ViewportScale.y()) + m_ViewportOrigin.y();
		//pos.m_WindowCoords.y = ((pos.m_WindowCoords.y + 0x800) & ~0xfff);

		pos.m_WindowCoords.depth = 
			EGL_CLAMP(EGL_Mul(z >> 8, EGL_Mul(m_DepthRangeFactor, invDenominator))  + m_DepthRangeBase, 0, 0xffff);

	} else {
		// keep this value around for perspective-correct texturing
		EGL_Fixed invDenominator = w ? EGL_Inverse(w) : 0;

		// Scale 1/Z by 2^12 to avoid rounding problems during prespective correct
		// interpolation
		pos.m_WindowCoords.invW = invDenominator << 12;

		pos.m_WindowCoords.x = 
			EGL_Mul(EGL_Mul(x, invDenominator), m_ViewportScale.x()) + m_ViewportOrigin.x();
		pos.m_WindowCoords.y = 
			EGL_Mul(EGL_Mul(y, invDenominator), m_ViewportScale.y()) + m_ViewportOrigin.y();
		pos.m_WindowCoords.depth = 
			EGL_CLAMP(EGL_Mul(EGL_Mul(z, invDenominator), m_DepthRangeFactor)  + m_DepthRangeBase, 0, 0xffff);

	}
}

void Context :: GetClipPlanex(GLenum plane, GLfixed eqn[4]) {
	if (plane < GL_CLIP_PLANE0 || plane >= GL_CLIP_PLANE0 + NUM_CLIP_PLANES) {
		RecordError(GL_INVALID_ENUM);
		return;
	}

	size_t index = plane - GL_CLIP_PLANE0;
	eqn[0] = m_ClipPlanes[index].x();
	eqn[1] = m_ClipPlanes[index].y();
	eqn[2] = m_ClipPlanes[index].z();
	eqn[3] = m_ClipPlanes[index].w();
}

void Context :: ClipPlanex(GLenum plane, const GLfixed *equation) {
	
	if (plane < GL_CLIP_PLANE0 || plane >= GL_CLIP_PLANE0 + NUM_CLIP_PLANES) {
		RecordError(GL_INVALID_ENUM);
		return;
	}

	size_t index = plane - GL_CLIP_PLANE0;
	m_ClipPlanes[index] = m_FullInverseModelViewMatrix.Transpose() * Vec4D(equation);
}
