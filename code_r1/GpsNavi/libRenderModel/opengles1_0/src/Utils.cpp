// ==========================================================================
//
// Utils.cpp		Helper functions for 3D Rendering Library
//
// --------------------------------------------------------------------------
//
// 09-14-2004	Hans-Martin Will	initial version
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
#include "Utils.h"


namespace EGL {
	void CopyVector(const Vec3D & vector, GLfixed *params) {
		params[0] = vector.x();
		params[1] = vector.y();
		params[2] = vector.z();
	}

	void CopyVector(const Vec4D & vector, GLfixed *params) {
		params[0] = vector.x();
		params[1] = vector.y();
		params[2] = vector.z();
		params[3] = vector.w();
	}

	void CopyMatrix(const Matrix4x4& matrix, GLfixed *params) {
		for (size_t index = 0; index < 16; ++index) {
			params[index] = matrix.Element(index);
		}
	}

	void CopyColor(const FractionalColor & color, GLfixed *params) {
		params[0] = color.r;
		params[1] = color.g;
		params[2] = color.b;
		params[3] = color.a;
	}

#ifndef EGL_USE_CLZ
	U32 CountLeadingZeros(U32 x) {
		U32 exp = 31;

		if (x & 0xffff0000) { 
			exp -= 16; 
			x >>= 16; 
		}

		if (x & 0xff00) { 
			exp -= 8; 
			x >>= 8; 
		}
		
		if (x & 0xf0) { 
			exp -= 4; 
			x >>= 4; 
		}

		if (x & 0xc) { 
			exp -= 2; 
			x >>= 2; 
		}
		
		if (x & 0x2) { 
			exp -= 1; 
		}

		return exp;
	}
#endif

}


