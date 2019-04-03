// ==========================================================================
//
// RasterizerPieces.cpp	
//						Specialized inner loops for triangle rasterizer
//
// --------------------------------------------------------------------------
//
// 03-16-2006		Hans-Martin Will	initial version
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


namespace EGL {

	// Perform just the depth test for a block
	PixelMask RBDepthTestLess(const RasterInfo * rasterInfo, 
							  const Variables * vars,
							  PixelMask * pixelMask) {

		PixelMask * mask = pixelMask, totalMask = 0;
		I32 depth0 = vars->Depth.Value;

		// initialize surface pointers in local info block
		U16 * depthBuffer = rasterInfo->RasterSurface.DepthBuffer;
		I32 iy = EGL_RASTER_BLOCK_SIZE;

		do {
			PixelMask rowMask = 0;
			U16 * innerDepthBuffer = depthBuffer;
			I32 innerDepth = depth0;
			I32 ix = EGL_RASTER_BLOCK_SIZE;

			do {
				rowMask >>= 1;

				U16 writeDepth = innerDepth >> 4;

				if (writeDepth < *innerDepthBuffer) {
					*innerDepthBuffer = writeDepth;
					rowMask |= (1 << (EGL_RASTER_BLOCK_SIZE - 1));
				}

				if (!--ix)
					break;

				++innerDepthBuffer;
				innerDepth += vars->Depth.dX;
			} while (true);

			*mask++ = rowMask;
			totalMask |= rowMask;

			if (!--iy)
				break;

			depth0 += vars->Depth.dY;
			depthBuffer += rasterInfo->RasterSurface.Pitch;
		} while (true);

		return totalMask;
	}

	PixelMask RBEdgeDepthTestLess(const RasterInfo * rasterInfo, 
								  const Variables * vars,
								  const Edges * edges,
								  PixelMask * pixelMask) {

		PixelMask * mask = pixelMask, totalMask = 0;
		I32 depth0 = vars->Depth.Value;

		I32 CY1 = edges->edge12.CY;
		I32 CY2 = edges->edge23.CY;
		I32 CY3 = edges->edge31.CY;

		// initialize surface pointers in local info block
		U16 * depthBuffer = rasterInfo->RasterSurface.DepthBuffer;
		I32 iy = EGL_RASTER_BLOCK_SIZE;

		do {
			GLint CX1 = CY1, CX2 = CY2, CX3 = CY3;
			PixelMask rowMask = 0;
			U16 * innerDepthBuffer = depthBuffer;
			I32 innerDepth = depth0;
			I32 ix = EGL_RASTER_BLOCK_SIZE;
			bool done = false;

			do {
				rowMask >>= 1; 

				if (CX1 > 0 && CX2 > 0 && CX3 > 0) {
					U16 writeDepth = innerDepth >> 4;

					if (writeDepth < *innerDepthBuffer) {
						*innerDepthBuffer = writeDepth;
						rowMask |= (1 << (EGL_RASTER_BLOCK_SIZE - 1));
					}

					done = true;
				} else if (done) {
					rowMask >>= ix - 1;
					break;
				} 

				if (!--ix)
					break;

				CX1 += edges->edge12.FDY;
				CX2 += edges->edge23.FDY;
				CX3 += edges->edge31.FDY;

				++innerDepthBuffer;
				innerDepth += vars->Depth.dX;
			} while (true);

			*mask++ = rowMask;
			totalMask |= rowMask;

			if (!--iy)
				break;

			CY1 -= edges->edge12.FDX;
			CY2 -= edges->edge23.FDX;
			CY3 -= edges->edge31.FDX;

			depth0 += vars->Depth.dY;
			depthBuffer += rasterInfo->RasterSurface.Pitch;
		} while (true);

		return totalMask;
	}

	void RBTextureReplace(const RasterInfo * rasterInfo, 
						  I32 varying[][2][2],
						  const PixelMask * pixelMask) {
		const PixelMask * mask = pixelMask;

		// initialize surface pointers in local info block
		U16 * colorBuffer = rasterInfo->RasterSurface.ColorBuffer;

		I32 iy = EGL_RASTER_BLOCK_SIZE;
		const Texture * texture = rasterInfo->Textures[0] + rasterInfo->MipmapLevel[0];
		I32 shiftU = 16 - texture->GetLogWidth();
		I32 shiftV = 16 - texture->GetLogHeight();
		I32 logWidth = texture->GetLogWidth();
		const U16 * textureData = (const U16 *) texture->GetData();

		do {

			PixelMask rowMask = *mask++;
			U16 * innerColor = colorBuffer;

			I32 tu = varying[0][0][0];
			I32 dtu = (varying[0][1][0] - varying[0][0][0]) >> EGL_LOG_RASTER_BLOCK_SIZE;
			I32 tv = varying[1][0][0];
			I32 dtv = (varying[1][1][0] - varying[1][0][0]) >> EGL_LOG_RASTER_BLOCK_SIZE;

			do {
				if (rowMask & 1) {
					I32 tu0 = (tu & 0xffff) >> shiftU;
					I32 tv0 = (tv & 0xffff) >> shiftV;
					I32 offset = (tv0 << logWidth) + tu0;
					*innerColor = textureData[offset];
				}

				rowMask >>= 1;

				if (rowMask == 0)
					break;

				++innerColor;
				tu += dtu;
				tv += dtv;
			} while(true);

			if (!--iy)
				break;

			varying[0][0][0] += varying[0][0][1];
			varying[0][1][0] += varying[0][1][1];
			varying[1][0][0] += varying[1][0][1];
			varying[1][1][0] += varying[1][1][1];

			colorBuffer += rasterInfo->RasterSurface.Pitch;
		} while (true);
	}
}