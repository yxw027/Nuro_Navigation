// ==========================================================================
//
// ContextMatrix	Rendering Context Class for 3D Rendering Library
//
//					Matrix and transformaton related settings
//
// --------------------------------------------------------------------------
//
// 10-13-2003		Hans-Martin Will	adaptation from prototype
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
#include "RasterizerState.h"

using namespace EGL;


// --------------------------------------------------------------------------
// Manipulation of the matrix stacks
// --------------------------------------------------------------------------

void Context :: MatrixMode(GLenum mode) {
	switch (mode) {
	case GL_MODELVIEW:
		m_CurrentMatrixStack = &m_ModelViewMatrixStack;
		break;

	case GL_PROJECTION:
		m_CurrentMatrixStack = &m_ProjectionMatrixStack;
		break;

	case GL_TEXTURE:
		m_CurrentMatrixStack = &m_TextureMatrixStack[m_ActiveTexture];
		break;

	default:
		RecordError(GL_INVALID_ENUM);
		return;
	}

	m_MatrixMode = mode;
}

void Context :: RebuildMatrices(void) {
	if (m_CurrentMatrixStack == &m_ModelViewMatrixStack) {
		m_InverseModelViewMatrix = m_ModelViewMatrixStack.CurrentMatrix().InverseUpper3(m_RescaleNormalEnabled);
		m_FullInverseModelViewMatrix = m_ModelViewMatrixStack.CurrentMatrix().Inverse();
	}
}

void Context :: UpdateInverseModelViewMatrix(void) {
	m_InverseModelViewMatrix = m_ModelViewMatrixStack.CurrentMatrix().InverseUpper3(m_RescaleNormalEnabled);
}

void Context :: LoadIdentity(void) { 

	CurrentMatrixStack()->LoadIdentity();
	RebuildMatrices();
}

void Context :: LoadMatrixx(const GLfixed *m) { 

	CurrentMatrixStack()->LoadMatrix(m);
	RebuildMatrices();
}

void Context :: MultMatrix(const Matrix4x4 & m) {
	CurrentMatrixStack()->MultMatrix(m);
	RebuildMatrices();
}

void Context :: MultMatrixx(const GLfixed *m) { 
	MultMatrix(Matrix4x4(m));
}

void Context :: PopMatrix(void) { 

	if (CurrentMatrixStack()->PopMatrix()) {
		RebuildMatrices();
		RecordError(GL_NO_ERROR);
	} else {
		RecordError(GL_STACK_UNDERFLOW);
	}
}

void Context :: PushMatrix(void) { 
	if (CurrentMatrixStack()->PushMatrix()) {
		RecordError(GL_NO_ERROR);
	} else {
		RecordError(GL_STACK_OVERFLOW);
	}
}

// --------------------------------------------------------------------------
// Calculation of specific matrixes
// --------------------------------------------------------------------------

void Context :: Rotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z) { 
	MultMatrix(Matrix4x4::CreateRotate(angle, x, y, z));
}

void Context :: Scalex(GLfixed x, GLfixed y, GLfixed z) { 
	MultMatrix(Matrix4x4::CreateScale(x, y, z));
}

void Context :: Translatex(GLfixed x, GLfixed y, GLfixed z) { 
	MultMatrix(Matrix4x4::CreateTranslate(x, y, z));
}

void Context :: Frustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar) { 

	if (left == right || top == bottom || zNear <= 0 || zFar <= 0) {
		RecordError(GL_INVALID_VALUE);
		return;
	}

	MultMatrix(Matrix4x4::CreateFrustrum(left, right, bottom, top, zNear, zFar));
}

void Context :: Orthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar) { 
	MultMatrix(Matrix4x4::CreateOrtho(left, right, bottom, top, zNear, zFar));
}

// --------------------------------------------------------------------------
// Matrix querying extension
// --------------------------------------------------------------------------

GLbitfield Context :: QueryMatrixx(GLfixed mantissa[16], GLint exponent[16]) {

	const Matrix4x4* currentMatrix = &CurrentMatrixStack()->CurrentMatrix();

	for (int index = 0; index < 16; ++index) {
		mantissa[index] = currentMatrix->Element(index);
		exponent[index] = 0;
	}

	return 0;
}

