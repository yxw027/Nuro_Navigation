// ==========================================================================
//
// RasterizerTraingles.cpp	Rasterizer Class for 3D Rendering Library
//
// The rasterizer converts transformed and lit primitives and creates a
// raster image in the current rendering surface.
//
// This files contains the triangle rasterization code, which was
// previously in the Rasterizer.cpp source file.
//
// --------------------------------------------------------------------------
//
// 05-22-2004		Hans-Martin Will	initial version
// 02-19-2006		Hans-Martin Will	rewrite using edge equations
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
#include "Rasterizer.h"
#include "Surface.h"
#include "Texture.h"
#include "Utils.h"
#include "arm/FunctionCache.h"

using namespace EGL;
#ifdef DENGXU_LOG
void opengles_save_log(const char *format, ...);
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace {
	inline I32 min(I32 a, I32 b, I32 c) {
		I32 d = (a < b) ? a : b;
		return (d < c) ? d : c;
	}

	inline I32 max(I32 a, I32 b, I32 c) {
		I32 d = (a > b) ? a : b;
		return (d > c) ? d : c;
	}

	// 32-bit determinant with 64-bit intermediate
	inline I64 det2x2(I32 a00, I32 a01, I32 a10, I32 a11) {
		I64 prod1 = static_cast<I64>(a00) * static_cast<I64>(a11);
		I64 prod2 = static_cast<I64>(a10) * static_cast<I64>(a01);
		return static_cast<I64>(prod1 - prod2);
	}

	bool HasAlpha(RasterizerState::TextureFormat format) {
		switch (format) {
		case RasterizerState::TextureFormatAlpha:
		case RasterizerState::TextureFormatLuminanceAlpha:
		case RasterizerState::TextureFormatRGBA8:
		case RasterizerState::TextureFormatRGBA4444:
		case RasterizerState::TextureFormatRGBA5551:
			return true;

		default:
			return false;
		}
	}

	inline I32 Mul24(I32 a, I32 b) {
		I64 product = static_cast<I64>(a) * static_cast<I64>(b);
		return static_cast<I32>((product + 0x400000) >> 23);
	}

	inline I32 MulRoundShift(I32 a, I32 b, I32 shift) {
		I64 product = static_cast<I64>(a) * static_cast<I64>(b);
		return static_cast<I32>((product + (1 << (shift - 1))) >> shift);
	}

	inline I32 Mul(I32 a, I32 b, const I32 shift) {
		I64 product = static_cast<I64>(a) * static_cast<I64>(b);
		return static_cast<I32>(product >> shift);
	}

	inline I32 Mul64(I64 a, I32 b, const I32 shift) {
		I64 product = a * b;
		return static_cast<I32>(product >> shift);
	}

	inline I32 InvNewtonRaphson4q28(I32 a, I32 x) {
		x = EGL_InverseIterQ(a, x, 28);
		x = EGL_InverseIterQ(a, x, 28);

		return x;
	}
}


// ---------------------------------------------------------------------------
// Prepare rasterizer object for triangles
// ---------------------------------------------------------------------------

namespace EGL {
	PixelMask RBDepthTestLess(const RasterInfo * rasterInfo, 
							  const Variables * vars,
							  PixelMask * pixelMask);

	
	PixelMask RBEdgeDepthTestLess(const RasterInfo * rasterInfo, 
								  const Variables * vars,
								  const Edges * edges,
								  PixelMask * pixelMask);

	void RBTextureReplace(const RasterInfo * rasterInfo, 
						  I32 varying[][2][2],
						  const PixelMask * pixelMask);

}

void Rasterizer :: PrepareTriangle() {
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif
	Prepare();
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif

	memset(m_RasterInfo.MipmapLevel, 0, sizeof(m_RasterInfo.MipmapLevel));
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif

	// initialize block rasterization here

	do {
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif
		m_FunctionCache->Begin();

#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif
		m_BlockDepthStencilFunction = (BlockDepthStencilFunction *) //&RBDepthTestLess;
			m_FunctionCache->GetFunction(FunctionCache::FunctionTypeBlockDepthStencil,
										 *m_State, &m_VaryingInfo);

#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif
		m_BlockEdgeDepthStencilFunction = (BlockEdgeDepthStencilFunction *) //&RBEdgeDepthTestLess;
			m_FunctionCache->GetFunction(FunctionCache::FunctionTypeBlockEdgeDepthStencil,
										 *m_State, &m_VaryingInfo);

#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif
		m_BlockColorAlphaFunction = (BlockColorAlphaFunction *) //&RBTextureReplace;
			m_FunctionCache->GetFunction(FunctionCache::FunctionTypeBlockColorAlpha,
										 *m_State, &m_VaryingInfo);
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif

	} while (m_FunctionCache->End());
#ifdef DENGXU_LOG
    opengles_save_log("func:%s | file:%s:%d\n", __func__, __FILE__, __LINE__);
#endif
}

#if !EGL_USE_JIT

PixelMask Rasterizer :: RasterBlockDepthStencil(const Variables * vars,
												PixelMask * pixelMask) {

	PixelMask * mask = pixelMask, totalMask = 0;
	I32 depth0 = vars->Depth.Value;

	// initialize surface pointers in local info block
	SurfaceInfo surfaceInfo = m_RasterInfo.RasterSurface;

    for (I32 iy = 0; iy < EGL_RASTER_BLOCK_SIZE; iy++) {

		PixelMask rowMask = 0;

        for (I32 ix = 0; ix < EGL_RASTER_BLOCK_SIZE; ix++) {

			bool written = false;

            // test and write depth and stencil
			if (!m_State->IsEnabledScissorTest() ||
				m_State->m_ScissorTest.X <= vars->x + ix &&
				m_State->m_ScissorTest.X + m_State->m_ScissorTest.Width > vars->x + ix &&
				m_State->m_ScissorTest.Y <= vars->y + iy &&
				m_State->m_ScissorTest.Y + m_State->m_ScissorTest.Height > vars->y + iy) {
				written = FragmentDepthStencil(&m_RasterInfo, &surfaceInfo, ix, depth0 >> 4);
			}

			rowMask = (rowMask >> 1) | (written ? (1 << (EGL_RASTER_BLOCK_SIZE - 1)) : 0);
			depth0 += vars->Depth.dX;
        }

		*mask++ = rowMask;
		totalMask |= rowMask;

		depth0 += vars->Depth.dY - (vars->Depth.dX << EGL_LOG_RASTER_BLOCK_SIZE);

		surfaceInfo.DepthBuffer += surfaceInfo.Pitch;
		surfaceInfo.StencilBuffer += surfaceInfo.Pitch;
    }

	return totalMask;
}

PixelMask Rasterizer :: RasterBlockEdgeDepthStencil(const Variables * vars,
													const Edges * edges,
													PixelMask * pixelMask) {

	PixelMask * mask = pixelMask, totalMask = 0;
	I32 depth0 = vars->Depth.Value;

	// initialize surface pointers in local info block
	SurfaceInfo surfaceInfo = m_RasterInfo.RasterSurface;

	I32 CY1 = edges->edge12.CY;
	I32 CY2 = edges->edge23.CY;
	I32 CY3 = edges->edge31.CY;

    for (I32 iy = 0; iy < EGL_RASTER_BLOCK_SIZE; iy++) {
        GLint CX1 = CY1, CX2 = CY2, CX3 = CY3;
		I32 depth1 = depth0;
		PixelMask rowMask = 0;
		bool done = false;

        for(I32 ix = 0; ix < EGL_RASTER_BLOCK_SIZE; ix++) {
            if (CX1 > 0 && CX2 > 0 && CX3 > 0) {
				// test and write depth and stencil
				bool written = false;

				// test and write depth and stencil
				if (!m_State->IsEnabledScissorTest() ||
					m_State->m_ScissorTest.X <= vars->x + ix &&
					m_State->m_ScissorTest.X + m_State->m_ScissorTest.Width > vars->x + ix &&
					m_State->m_ScissorTest.Y <= vars->y + iy &&
					m_State->m_ScissorTest.Y + m_State->m_ScissorTest.Height > vars->y + iy) {
					written = FragmentDepthStencil(&m_RasterInfo, &surfaceInfo, ix, depth1 >> 4);
				}

				rowMask >>= 1;
				rowMask |= written ? (1 << (EGL_RASTER_BLOCK_SIZE - 1)) : 0;
				done = true;
            } else if (done) {
				rowMask >>= EGL_RASTER_BLOCK_SIZE - ix;
				break;
			} else {
				rowMask >>= 1;
			}

            CX1 += edges->edge12.FDY;
			CX2 += edges->edge23.FDY;
			CX3 += edges->edge31.FDY;
			depth1 += vars->Depth.dX;
        }

		*mask++ = rowMask;
		totalMask |= rowMask;

        CY1 -= edges->edge12.FDX;
		CY2 -= edges->edge23.FDX;
		CY3 -= edges->edge31.FDX;

		depth0 += vars->Depth.dY;

		surfaceInfo.DepthBuffer += surfaceInfo.Pitch;
		surfaceInfo.StencilBuffer += surfaceInfo.Pitch;
    }

	return totalMask;
}

void Rasterizer :: RasterBlockColorAlpha(I32 varying[][2][2],
										 const PixelMask * pixelMask) {
	const PixelMask * mask = pixelMask;

	// initialize surface pointers in local info block
	SurfaceInfo surfaceInfo = m_RasterInfo.RasterSurface;

    for (I32 iy = 0; iy < EGL_RASTER_BLOCK_SIZE; iy++) {

		I32 index;
		I32 varying0[EGL_MAX_NUM_VARYING][2];
		PixelMask rowMask = *mask++;

		for (index = 0; index < m_VaryingInfo.numVarying; ++index) {
			varying0[index][0] = varying[index][0][0];
			varying0[index][1] = (varying[index][1][0] - varying[index][0][0]) >> EGL_LOG_RASTER_BLOCK_SIZE;
		}

        for (I32 ix = 0; ix < EGL_RASTER_BLOCK_SIZE; ix++) {

			if (rowMask & 1) {
				I32 tu[EGL_NUM_TEXTURE_UNITS], tv[EGL_NUM_TEXTURE_UNITS];
				Color baseColor;
				I32 fog;

				for (I32 unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
					size_t textureBase = m_VaryingInfo.textureBase[unit];

					if (textureBase >= 0) {
						tu[unit] = varying0[textureBase][0];
						tv[unit] = varying0[textureBase + 1][0];
					}
				}

				if (m_VaryingInfo.colorIndex >= 0) {
					//baseColor = FractionalColor(varying0 + m_VaryingInfo.colorIndex);
					baseColor = FractionalColor(
						varying0[m_VaryingInfo.colorIndex][0],
						varying0[m_VaryingInfo.colorIndex + 1][0],
						varying0[m_VaryingInfo.colorIndex + 2][0],
						varying0[m_VaryingInfo.colorIndex + 3][0]);
				}

				if (m_VaryingInfo.fogIndex >= 0) {
					fog = varying0[m_VaryingInfo.fogIndex][0];
				}

				FragmentColorAlpha(&m_RasterInfo, &surfaceInfo, ix, tu, tv, baseColor, fog);
			}

			if (!(rowMask >>= 1))
				break;

			for (index = 0; index < m_VaryingInfo.numVarying; ++index) {
				varying0[index][0] += varying0[index][1];
			}
        }

		for (index = 0; index < m_VaryingInfo.numVarying; ++index) {
			varying[index][0][0] += varying[index][0][1];
			varying[index][1][0] += varying[index][1][1];
		}

		surfaceInfo.ColorBuffer += surfaceInfo.Pitch;
		surfaceInfo.AlphaBuffer += surfaceInfo.Pitch;
    }
}

#endif

void Rasterizer :: RasterTriangle(const Vertex& a, const Vertex& b,
								  const Vertex& c) {

	I32 index, unit;			// index into varying variable array & texture unit

	Variables vars;

	Edges edges;

	// 16.16 -> 28.4 fixed-point coordinates
	const I32 X1 = ((a.m_WindowCoords.x + (1 << 12)) >> 12) & ~1;
	const I32 X2 = ((b.m_WindowCoords.x + (1 << 12)) >> 12) & ~1;
	const I32 X3 = ((c.m_WindowCoords.x + (1 << 12)) >> 12) & ~1;

    // Deltas
    const I32 DX12 = X1 - X2;
	const I32 DX23 = X2 - X3;
	const I32 DX31 = X3 - X1;

    // 24.8 Fixed-point deltas
    edges.edge12.FDX = DX12 << 4;
	edges.edge23.FDX = DX23 << 4;
	edges.edge31.FDX = DX31 << 4;

	// 16.16 -> 28.4 fixed-point coordinates
	const I32 Y1 = ((a.m_WindowCoords.y + (1 << 12)) >> 12) & ~1;
	const I32 Y2 = ((b.m_WindowCoords.y + (1 << 12)) >> 12) & ~1;
	const I32 Y3 = ((c.m_WindowCoords.y + (1 << 12)) >> 12) & ~1;

    const I32 DY12 = Y1 - Y2;
	const I32 DY23 = Y2 - Y3;
	const I32 DY31 = Y3 - Y1;

    edges.edge12.FDY = DY12 << 4;
	edges.edge23.FDY = DY23 << 4;
	edges.edge31.FDY = DY31 << 4;

	// setup of interpolation of varying vars goes here
	// area in 24.8
	I32 area = DX31 * DY12 - DX12 * DY31;

	if (area <= 0xf)
		return;

	// inv arera as 8.24
	I32 invArea = EGL_InverseQ(area, 8);

    // Bounding rectangle; round lower bound down to block size
    const I32 minx = ((min(X1, X2, X3) + 0x7) >> 4) & ~(EGL_RASTER_BLOCK_SIZE - 1);
    const I32 miny = ((min(Y1, Y2, Y3) + 0x7) >> 4) & ~(EGL_RASTER_BLOCK_SIZE - 1);
    const I32 maxx = ((max(X1, X2, X3) + 0x8) >> 4) + (EGL_RASTER_BLOCK_SIZE - 1) & ~(EGL_RASTER_BLOCK_SIZE - 1);
    const I32 maxy = ((max(Y1, Y2, Y3) + 0x8) >> 4) + (EGL_RASTER_BLOCK_SIZE - 1) & ~(EGL_RASTER_BLOCK_SIZE - 1);
	const I32 span = maxx - minx;

	m_RasterInfo.Init(m_Surface, miny, minx);
	I32 blockStride = EGL_RASTER_BLOCK_SIZE * m_RasterInfo.RasterSurface.Pitch - span;

	const U32 numVarying = m_VaryingInfo.numVarying;
	U32 usedNumVarying = 0;


    // Half-edge constants
    I32 C1 = Y2 * X1 - X2 * Y1;
    I32 C2 = Y3 * X2 - X3 * Y2;
    I32 C3 = Y1 * X3 - X1 * Y3;

    // Correct for fill convention
	if (DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
	if (DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
	if (DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

	I32 XMin1 = (minx << 4) + (1 << 3) - X1;
	I32 YMin1 = (miny << 4) + (1 << 3) - Y1;

	// Deltas
    const I32 DD12 = (a.m_WindowCoords.depth - b.m_WindowCoords.depth) << 4;
    const I32 DD23 = (b.m_WindowCoords.depth - c.m_WindowCoords.depth) << 4;
    const I32 DD31 = (c.m_WindowCoords.depth - a.m_WindowCoords.depth) << 4;

	vars.Depth.dX =  Mul64(det2x2(DY12, DD12, DY31, DD31), invArea, 28);
	vars.Depth.dY = -Mul64(det2x2(DX12, DD12, DX31, DD31), invArea, 28);
	I32 depthSlope=  EGL_Max(EGL_Abs(vars.Depth.dX), EGL_Abs(vars.Depth.dY));
	I32 factor    =  EGL_Mul(depthSlope, m_State->GetPolygonOffsetFactor());
	vars.Depth.dBlockLine = vars.Depth.dY * EGL_RASTER_BLOCK_SIZE - vars.Depth.dX * span;
	vars.Depth.Value = (a.m_WindowCoords.depth << 4) 
							+ ((m_State->GetPolygonOffsetUnits() + (1 << 11)) >> 12)
							+ factor
							+ Mul(XMin1, vars.Depth.dX, 4)
							+ Mul(YMin1, vars.Depth.dY, 4);

    const I32 DW12 = a.m_WindowCoords.invW - b.m_WindowCoords.invW;
    const I32 DW23 = b.m_WindowCoords.invW - c.m_WindowCoords.invW;
    const I32 DW31 = c.m_WindowCoords.invW - a.m_WindowCoords.invW;

	// dWdX, dWdY is 4.28
	vars.InvW.dX =  Mul64(det2x2(DY12, DW12, DY31, DW31), invArea, 28);
	vars.InvW.dY = -Mul64(det2x2(DX12, DW12, DX31, DW31), invArea, 28);
	vars.InvW.dBlockLine = vars.InvW.dY * EGL_RASTER_BLOCK_SIZE - vars.InvW.dX * span;
	vars.InvW.Value = a.m_WindowCoords.invW
							  + Mul(XMin1, vars.InvW.dX, 4)
					   		  + Mul(YMin1, vars.InvW.dY, 4);

    // Loop through blocks
    for (vars.y = miny; vars.y < maxy; vars.y += EGL_RASTER_BLOCK_SIZE) {

        for (vars.x = minx; vars.x < maxx; vars.x += EGL_RASTER_BLOCK_SIZE) {

            // Corners of block as 28.4; move to pixel centers
            GLint x0 = (vars.x << 4) | (1 << 3);
            GLint y0 = (vars.y << 4) | (1 << 3);

            // Evaluate half-space functions
            edges.edge12.CY = C1 + DY12 * x0 - DX12 * y0;

            GLint pass1 = edges.edge12.CY > 0;
            pass1 += edges.edge12.CY + (edges.edge12.FDY << EGL_LOG_RASTER_BLOCK_SIZE) > 0;
            pass1 += edges.edge12.CY - (edges.edge12.FDX << EGL_LOG_RASTER_BLOCK_SIZE) > 0;
            pass1 += edges.edge12.CY + (edges.edge12.FDY << EGL_LOG_RASTER_BLOCK_SIZE)
						 - (edges.edge12.FDX << EGL_LOG_RASTER_BLOCK_SIZE) > 0;

            edges.edge23.CY = C2 + DY23 * x0 - DX23 * y0;
            GLint pass2 = edges.edge23.CY > 0;
            pass2 += edges.edge23.CY + (edges.edge23.FDY << EGL_LOG_RASTER_BLOCK_SIZE) > 0;
            pass2 += edges.edge23.CY - (edges.edge23.FDX << EGL_LOG_RASTER_BLOCK_SIZE) > 0;
            pass2 += edges.edge23.CY + (edges.edge23.FDY << EGL_LOG_RASTER_BLOCK_SIZE)
						 - (edges.edge23.FDX << EGL_LOG_RASTER_BLOCK_SIZE) > 0;

            edges.edge31.CY = C3 + DY31 * x0 - DX31 * y0;
            GLint pass3 = edges.edge31.CY > 0;
            pass3 += edges.edge31.CY + (edges.edge31.FDY << EGL_LOG_RASTER_BLOCK_SIZE) > 0;
            pass3 += edges.edge31.CY - (edges.edge31.FDX << EGL_LOG_RASTER_BLOCK_SIZE) > 0;
            pass3 += edges.edge31.CY + (edges.edge31.FDY << EGL_LOG_RASTER_BLOCK_SIZE)
						 - (edges.edge31.FDX << EGL_LOG_RASTER_BLOCK_SIZE) > 0;

			PixelMask pixelMask[EGL_RASTER_BLOCK_SIZE];
			PixelMask totalMask;

            // Skip block when outside an edge
			if (pass1 == 0x0 || pass2 == 0x0 || pass3 == 0x0) {
				goto cont;
			}

			// Accept whole block when totally covered
            if (pass1 + pass2 + pass3 == 12) {
#if !EGL_USE_JIT
				totalMask = RasterBlockDepthStencil(&vars, pixelMask);
#else
				totalMask = m_BlockDepthStencilFunction(&m_RasterInfo, &vars, pixelMask);
				//totalMask = RBDepthTestLess(&m_RasterInfo, &vars, pixelMask);
#endif
            } else {
				// Partially covered block
				// initialize surface pointers in local info block
#if !EGL_USE_JIT
				totalMask = RasterBlockEdgeDepthStencil(&vars, &edges, pixelMask);
#else
				totalMask = m_BlockEdgeDepthStencilFunction(&m_RasterInfo, &vars, &edges, pixelMask);
				//totalMask = RBEdgeDepthTestLess(&m_RasterInfo, &vars, &edges, pixelMask);
#endif
            }

			if (totalMask) {
				if (numVarying != usedNumVarying) {
					I32 XMin2 = x0 - X1;
					I32 YMin2 = y0 - Y1;

					usedNumVarying = numVarying;

					for (index = usedNumVarying; --index >= 0; ) {
						// 4.28
						I32 V1OverW = Mul(a.m_Varying[index], a.m_WindowCoords.invW, 16);
						I32 V2OverW = Mul(b.m_Varying[index], b.m_WindowCoords.invW, 16);
						I32 V3OverW = Mul(c.m_Varying[index], c.m_WindowCoords.invW, 16);

						I32 IVW12 = V1OverW - V2OverW;
						I32 IVW31 = V3OverW - V1OverW;

						// dVaryingDx, dVaryingDy is 4.28
						vars.VaryingInvW[index].dX =  Mul64(det2x2(DY12, IVW12, DY31, IVW31), invArea, 28);
						vars.VaryingInvW[index].dY = -Mul64(det2x2(DX12, IVW12, DX31, IVW31), invArea, 28);
						vars.VaryingInvW[index].dBlockLine =
							vars.VaryingInvW[index].dY * EGL_RASTER_BLOCK_SIZE - vars.VaryingInvW[index].dX * span;

						// varyingStart is 4.28
						vars.VaryingInvW[index].Value =
							V1OverW
								+ Mul(XMin2, vars.VaryingInvW[index].dX, 4)
								+ Mul(YMin2, vars.VaryingInvW[index].dY, 4);
					}

				}

				// interpolation values for the four corners
				I32 w[2][2];				// w in all four corners of block 4.28 (?)

				I32 varying[EGL_MAX_NUM_VARYING][2][2];	//

				// compute w in all four corners
				w[0][0] = EGL_InverseQ(vars.InvW.Value, 28);		// 4.28
				w[0][1] = InvNewtonRaphson4q28(vars.InvW.Value + vars.InvW.dX * EGL_RASTER_BLOCK_SIZE, w[0][0]);
				w[1][0] = InvNewtonRaphson4q28(vars.InvW.Value + vars.InvW.dY * EGL_RASTER_BLOCK_SIZE, w[0][0]);
				w[1][1] = InvNewtonRaphson4q28(vars.InvW.Value + vars.InvW.dX * EGL_RASTER_BLOCK_SIZE
												+ vars.InvW.dY * EGL_RASTER_BLOCK_SIZE, w[1][0]);

				// compute values of varying at all four corners
				for (index = usedNumVarying; --index >= 0; ) {
					varying[index][0][0] = Mul(vars.VaryingInvW[index].Value, w[0][0], 16);
					varying[index][0][1] =
						(Mul(vars.VaryingInvW[index].Value
										+ (vars.VaryingInvW[index].dY << EGL_LOG_RASTER_BLOCK_SIZE), w[1][0], 16)
						 - varying[index][0][0]) >> EGL_LOG_RASTER_BLOCK_SIZE;

					varying[index][1][0] =
						Mul(vars.VaryingInvW[index].Value
										+ (vars.VaryingInvW[index].dX << EGL_LOG_RASTER_BLOCK_SIZE), w[0][1], 16);
					varying[index][1][1] =
						(Mul(vars.VaryingInvW[index].Value
										+ (vars.VaryingInvW[index].dX << EGL_LOG_RASTER_BLOCK_SIZE)
										+ (vars.VaryingInvW[index].dY << EGL_LOG_RASTER_BLOCK_SIZE), w[1][1], 16)
						 - varying[index][1][0]) >> EGL_LOG_RASTER_BLOCK_SIZE;
				}

				// perform Mipmap selection; initialize local RasterInfo structure
				unit = EGL_NUM_TEXTURE_UNITS - 1; 
				do {
					I32 textureBase = m_VaryingInfo.textureBase[unit];

					if (textureBase >= 0 && m_UseMipmap[unit]) {
						I32 dUdX = ((varying[textureBase][1][0] << 1)
									+ (varying[textureBase][1][1] << EGL_LOG_RASTER_BLOCK_SIZE)
									- (varying[textureBase][0][0] << 1)
									- (varying[textureBase][0][1] << EGL_LOG_RASTER_BLOCK_SIZE))
									>> (EGL_LOG_RASTER_BLOCK_SIZE + 1);
						I32 dUdY = (varying[textureBase][0][1] + varying[textureBase][1][1]) 
									>> 1;
						I32 dVdX = ((varying[textureBase + 1][1][0] << 1)
									+ (varying[textureBase + 1][1][1] << EGL_LOG_RASTER_BLOCK_SIZE)
									- (varying[textureBase + 1][0][0] << 1)
									- (varying[textureBase + 1][0][1] << EGL_LOG_RASTER_BLOCK_SIZE))
									>> (EGL_LOG_RASTER_BLOCK_SIZE + 1);
						I32 dVdY = (varying[textureBase + 1][0][1] + varying[textureBase + 1][1][1]) 
									>> 1;

						I32 maxDu = EGL_Max(EGL_Abs(dUdX), EGL_Abs(dUdY)) >> (16 - m_Texture[unit]->GetTexture(0)->GetLogWidth());
						I32 maxDv = EGL_Max(EGL_Abs(dVdX), EGL_Abs(dVdY)) >> (16 - m_Texture[unit]->GetTexture(0)->GetLogHeight());

						I32 rho = maxDu + maxDv;

						// should actually plug in approximation formula from Blythe & McReynolds

						// we start with nearest/minification only selection; will add LINEAR later
						m_RasterInfo.MipmapLevel[unit] = EGL_Min(Log2(rho), m_RasterInfo.MaxMipmapLevel[unit]);
						m_RasterInfo.Textures[unit] = m_Texture[unit]->GetTexture(m_RasterInfo.MipmapLevel[unit]);
					}
				} while (--unit >= 0);

#if !EGL_USE_JIT
				RasterBlockColorAlpha(varying, pixelMask);
#else
				m_BlockColorAlphaFunction(&m_RasterInfo, varying, pixelMask);
				//RBTextureReplace(&m_RasterInfo, varying, pixelMask);
#endif
			}
cont:
			vars.Depth.Value += vars.Depth.dX << EGL_LOG_RASTER_BLOCK_SIZE;
			vars.InvW.Value += vars.InvW.dX << EGL_LOG_RASTER_BLOCK_SIZE;

			for (index = usedNumVarying; --index >= 0; ) {
				vars.VaryingInvW[index].Value += vars.VaryingInvW[index].dX << EGL_LOG_RASTER_BLOCK_SIZE;
			}

			m_RasterInfo.RasterSurface.ColorBuffer   += EGL_RASTER_BLOCK_SIZE;
			m_RasterInfo.RasterSurface.DepthBuffer   += EGL_RASTER_BLOCK_SIZE;
			m_RasterInfo.RasterSurface.AlphaBuffer   += EGL_RASTER_BLOCK_SIZE;
			m_RasterInfo.RasterSurface.StencilBuffer += EGL_RASTER_BLOCK_SIZE;
        }

		vars.Depth.Value += vars.Depth.dBlockLine;
		vars.InvW.Value += vars.InvW.dBlockLine;

		for (index = usedNumVarying; --index >= 0; ) {
			vars.VaryingInvW[index].Value += vars.VaryingInvW[index].dBlockLine;
		}

		m_RasterInfo.RasterSurface.ColorBuffer   += blockStride;
		m_RasterInfo.RasterSurface.DepthBuffer   += blockStride;
		m_RasterInfo.RasterSurface.AlphaBuffer   += blockStride;
		m_RasterInfo.RasterSurface.StencilBuffer += blockStride;
    }
}

