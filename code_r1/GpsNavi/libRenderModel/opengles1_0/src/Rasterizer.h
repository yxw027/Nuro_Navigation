#ifndef EGL_RASTERIZER_H
#define EGL_RASTERIZER_H 1

// ==========================================================================
//
// Rasterizer.h		Rasterizer Class for 3D Rendering Library
//
//					The rasterizer converts transformed and lit
//					primitives and creates a raster image in the
//					current rendering surface.
//
// --------------------------------------------------------------------------
//
// 10-06-2003		Hans-Martin Will	initial version
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
#include "fixed.h"
#include "linalg.h"
#include "RasterizerState.h"
#include "FractionalColor.h"
#include "Surface.h"


namespace EGL {

	class Texture;
	class FunctionCache;

	// ----------------------------------------------------------------------
	// allocation of varying variables within parameter array
	// ----------------------------------------------------------------------

	struct VaryingInfo {
		// varying variable will be allocated based on the current state
		// settings
		size_t					numVarying;

		// the following variables specify the index base for the different
		// varying variables; a negative value indicates that there is no
		// such varying variable in use
		I32						colorIndex;
		I32						fogIndex;
		I32						textureBase[EGL_NUM_TEXTURE_UNITS];
	};

	// ----------------------------------------------------------------------
	// u and v coordinates for texture mapping
	// ----------------------------------------------------------------------
	struct TexCoord {
		EGL_Fixed		tu, tv;			// texture coordinates between 0 and 1

#if EGL_MIPMAP_PER_TEXEL
		EGL_Fixed		dtudx, dtvdx;	// partial derivatives
		EGL_Fixed		dtudy, dtvdy;	// partial derivatives
#endif
	};

	struct ScreenCoord {
		EGL_Fixed		x, y;		// x, y window coords
		EGL_Fixed		invW;		// 1/Z (w) from depth division

		// TO DO: once the rasterizer works properly, change this to binary 0..2^n-1
		EGL_Fixed		depth;		// depth coordinate after transformation
	};

	// ----------------------------------------------------------------------
	// Vertex information as input for rasterizer
	// ----------------------------------------------------------------------

	enum LightMode {
		Unlit = 0,
		Front = 1,
		Back  = 2
		// BackAndFront = 3
	};

	struct Vertex {
		Vec4D				m_EyeCoords;
		Vec4D				m_ClipCoords;
		ScreenCoord			m_WindowCoords;
		Vec3D				m_EyeNormal;
		FractionalColor		m_Color[3];	// base color, front color, back color		
		EGL_Fixed			m_Varying[EGL_MAX_NUM_VARYING];

		unsigned			m_Lit : 2;	// unlit or lit vertex?
		unsigned			m_cc : 6;	// culling flags
	};

	struct SurfaceInfo {
		U16 *		DepthBuffer;
		U16 *		ColorBuffer;
		U32 *		StencilBuffer;
		U8 *		AlphaBuffer;
		U16			Width;
		U16			Height;
		I32			Pitch;
	};

	struct RasterInfo {
		// surface info
		SurfaceInfo	RasterSurface;
		const I32 *	InversionTablePtr;

		// TODO: will need to add a minimum texture level here
		// TODO:

		// texture info
		Texture *	Textures[EGL_NUM_TEXTURE_UNITS];
		U32			MipmapLevel[EGL_NUM_TEXTURE_UNITS];
		U32			MaxMipmapLevel[EGL_NUM_TEXTURE_UNITS];

        void Init(Surface * surface, I32 y, I32 x = 0);
	};

	struct Interpolant {
		// parameters for a single value to be interpolated during rasterization
		I32		Value;					// initial value
		I32		dX, dY;					// dX and dY increments
		I32		dBlockLine;				// increment for end of block/line to beginning of next
	};

	struct Variables {
		// variables to be interpolated during rasterization
		Interpolant	InvW;								// w^-1 (as 4.28)
		Interpolant Depth;								// Depth (as 28.4)
		Interpolant	VaryingInvW[EGL_MAX_NUM_VARYING];	// varying[idx] * w^-1 (as 4.28)
		I32			x, y;								// x and y integer screen coordinates
	};

	struct Edge {
		I32	CY, FDX, FDY;
	};

	struct Edges {
		Edge edge12;
		Edge edge23;
		Edge edge31;
	};

#if EGL_RASTER_BLOCK_SIZE <= 8
	typedef U8 PixelMask;
#elif EGL_RASTER_BLOCK_SIZE <= 16
	typedef U16 PixelMask;
#else
	typedef U32 PixelMask;
#endif

	// signature for generated scanline functions
	typedef void (LineFunction)(const RasterInfo * info, const Vertex * from, const Vertex * to);
	typedef void (PointFunction)(const RasterInfo * info, const Vertex * pos, EGL_Fixed size);
	typedef PixelMask (BlockDepthStencilFunction)(const RasterInfo * info, const Variables * variables, PixelMask * pixelMask);
	typedef PixelMask (BlockEdgeDepthStencilFunction)(const RasterInfo * info, const Variables * variables, const Edges * edges, PixelMask * pixelMask);
	typedef void (BlockColorAlphaFunction)(const RasterInfo * info, I32 varying[][2][2], const PixelMask * pixelMask);

	class Rasterizer {

	public:
		enum {
			PolygonOffsetUnitSize = 1,		// how to determine this?
			DepthRangeMax = 0xffff			// 31 bits
		};

		enum PixelFormat {
			PixelFormatRGBA,
			PixelFormatRGB,
			PixelFormatLuminanceAlpha,
			PixelFormatLuminance,
			PixelFormatAlpha
		};

		enum PixelType {
			PixelTypeUnsignedByte,
			PixelTypeUnsignedShort_4_4_4_4,
			PixelTypeUnsignedShort_5_5_5_1,
			PixelTypeUnsignedShort_5_6_5
		};


	public:
		Rasterizer(RasterizerState * state);
		~Rasterizer();

		// ----------------------------------------------------------------------
		// Rendering surface
		// ----------------------------------------------------------------------
		void SetState(RasterizerState * state);
		RasterizerState * GetState() const;

		const VaryingInfo * GetVaryingInfo() const;

		void SetSurface(Surface * surface);
		Surface * GetSurface() const;

		void SetTexture(size_t unit, MultiTexture * texture);
		MultiTexture * GetTexture(size_t unit)				{ return m_Texture[unit]; }
		const MultiTexture * GetTexture(size_t unit) const	{ return m_Texture[unit]; }
		void PrepareTexture();

		// ----------------------------------------------------------------------
		// Actual rasterization of primitives
		// These functions take care of anything downstream of the
		// scissor test, scan conversion, texturing, compositing, depth &
		// stencil test.
		// ----------------------------------------------------------------------

		void RasterPoint(const Vertex& point, EGL_Fixed size);
		void RasterLine(Vertex& from, Vertex& to);
		void RasterTriangle(const Vertex& a, const Vertex& b,
			const Vertex& c);

		// inner loops of rasterization of triangles
		PixelMask RasterBlockDepthStencil(const Variables * variables, PixelMask * pixelMask);
		PixelMask RasterBlockEdgeDepthStencil(const Variables * variables, const Edges * edges, PixelMask * pixelMask);
		void RasterBlockColorAlpha(I32 varying[][2][2], const PixelMask * pixelMask);

		// ----------------------------------------------------------------------
		// State management
		// ----------------------------------------------------------------------

		void Prepare();
		void PreparePoint();
		void PrepareLine();
		void PrepareTriangle();
		void Finish();
		//void UpdateWindowClipping(void);

		// ----------------------------------------------------------------------
		// Include actual rasterization functions here
		// ----------------------------------------------------------------------
		//#include "generated_rasterization_function_declarations.h"

	private:
		// ----------------------------------------------------------------------
		// allocate varying variables
		// ----------------------------------------------------------------------
		void AllocateVaryings();

		// ----------------------------------------------------------------------
		// Rasterization of triangle
		//
		// Variations are: All, +/- color [Cc], +/- texture [Tt], +/- depth [Dd], +/- fog [Ff], +/- scissor [Ss]
		// combination ct does not exist
		// ----------------------------------------------------------------------

		enum RasterTriangleBits {
			RasterTriangleColor,
			RasterTriangleTexture,
			RasterTriangleDepth,
			RasterTriangleFog,
			RasterTriangleScissor,
			RasterTriangleStencil,
			RasterTriangleCount,
		};

		// ----------------------------------------------------------------------
		// Rasterization of fragment
		// ----------------------------------------------------------------------

		void Fragment(I32 x, I32 y, EGL_Fixed depth, EGL_Fixed tu[], EGL_Fixed tv[],
			EGL_Fixed fogDensity, const Color& baseColor);
			// will have special cases based on settings
			// the coordinates are integer coordinates

		void Fragment(const RasterInfo * rasterInfo, I32 x, EGL_Fixed depth,
					  EGL_Fixed tu[], EGL_Fixed tv[],
					  const Color& baseColor, EGL_Fixed fog);
			// fragment rendering with signature corresponding to function fragment
			// generated by code generator

		bool FragmentDepthStencil(const RasterInfo * rasterInfo, const SurfaceInfo * surfaceInfo,
								  I32 x, EGL_Fixed depth);
			// fragment rendering with signature corresponding to function fragment
			// generated by code generator
			// return true if the fragment passed depth and stencil tests

		void FragmentColorAlpha(const RasterInfo * rasterInfo, const SurfaceInfo * surfaceInfo,
								I32 x, EGL_Fixed tu[], EGL_Fixed tv[],
								const Color& baseColor, EGL_Fixed fog);
			// fragment rendering with signature corresponding to function fragment
			// generated by code generator

		Color GetTexColor(const RasterizerState::TextureState * state, const Texture * texture, EGL_Fixed tu, EGL_Fixed tv,
						  RasterizerState::FilterMode filterMode);
			// retrieve the texture color from a texture plane

		Color GetRawTexColor(const RasterizerState::TextureState * state, const Texture * texture, EGL_Fixed tu, EGL_Fixed tv);
			// retrieve the texture color from a texture plane

	private:
		// ----------------------------------------------------------------------
		// other settings
		// ----------------------------------------------------------------------

		RasterInfo				m_RasterInfo;
		Surface *				m_Surface;			// rendering surface
		MultiTexture *			m_Texture[EGL_NUM_TEXTURE_UNITS];	// current texture
		RasterizerState *		m_State;			// current rasterization settings
		FunctionCache *			m_FunctionCache;

		// compiled inner loops for various raster operations
		LineFunction *					m_LineFunction;
		PointFunction *					m_PointFunction;
		BlockDepthStencilFunction *		m_BlockDepthStencilFunction;
		BlockEdgeDepthStencilFunction *	m_BlockEdgeDepthStencilFunction;
		BlockColorAlphaFunction *		m_BlockColorAlphaFunction;

		// ----------------------------------------------------------------------
		// internal state
		// ----------------------------------------------------------------------
		EGL_Fixed				m_MinX;
		EGL_Fixed				m_MaxX;
		EGL_Fixed				m_MinY;
		EGL_Fixed				m_MaxY;

		VaryingInfo				m_VaryingInfo;
		bool					m_UseMipmap[EGL_NUM_TEXTURE_UNITS];
	};


	// --------------------------------------------------------------------------
	// Inline member definitions
	// --------------------------------------------------------------------------

	inline const VaryingInfo * Rasterizer :: GetVaryingInfo() const {
		return &m_VaryingInfo;
	}

	inline void Rasterizer :: SetSurface(Surface * surface) {
		m_Surface = surface;
	}


	inline Surface * Rasterizer :: GetSurface() const {
		return m_Surface;
	}


//	inline void Rasterizer :: RasterTriangle(const Vertex& a, const Vertex& b, const Vertex& c) {
//		(this->*m_RasterTriangleFunction)(a, b, c);
//	}


#	if EGL_USE_JIT

	inline void Rasterizer :: RasterPoint(const Vertex& point, EGL_Fixed size) {
		m_PointFunction(&m_RasterInfo, &point, size);
	}

	inline void Rasterizer :: RasterLine(Vertex& p_from, Vertex& p_to) {
		p_from.m_WindowCoords.x = ((p_from.m_WindowCoords.x + 0x800) & ~0xfff);
		p_from.m_WindowCoords.y = ((p_from.m_WindowCoords.y + 0x800) & ~0xfff);
		p_to.m_WindowCoords.x = ((p_to.m_WindowCoords.x + 0x800) & ~0xfff);
		p_to.m_WindowCoords.y = ((p_to.m_WindowCoords.y + 0x800) & ~0xfff);

		m_LineFunction(&m_RasterInfo, &p_from, &p_to);
	}

#	endif // EGL_USE_JIT

}



#endif //ndef EGL_RASTERIZER_H

