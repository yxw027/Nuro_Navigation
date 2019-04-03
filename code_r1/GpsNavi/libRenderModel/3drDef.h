#ifndef _3DR_DEF_NY_20111102_H_
#define _3DR_DEF_NY_20111102_H_

#include "NuroDefine.h"
#include "EglInc.h"
#include <new>

//#define _SHOW_3DR_PART

#define _3DR_MAX_VERTEX_CNT     1000
#define _3DR_MAX_TRIANGLE_CNT   1000

typedef GLfixed  _3drVertex_t[3];
typedef GLfixed  _3drTexCoord_t[2];
typedef GLushort _3drIndex_t[3];

typedef struct tag_3DR_DATA
{
    /// 模型数据
    GLuint          nTriangleCnt;       //< 三角形数量
    _3drVertex_t   *pafxTriVertex;      //< 顶点坐标数据
    _3drTexCoord_t *pafxTriTexCoord;    //< 纹理坐标数据
    /// 用于快速裁剪计算
    _3drVertex_t    aafxAABB[8];        //< AABB包围盒
    nuUINT          nVertexCnt;         //< 顶点坐标数量
    GLushort       *psVertexIdx;        //< 顶点坐标三角形索引
    _3drVertex_t   *pafxVertex;         //< 未展开的顶点坐标数据
    //
    nuBYTE         *pbyBuf;
} _3DR_DATA;

typedef struct tag_3DR_BMP
{
    GLuint  nWidth;
    GLuint  nHeight;
    GLbyte *pbyBmpData;
    //
    nuBYTE *pbyBuf;
} _3DR_BMP;

typedef struct tag_3DR_MDL
{
    _3DR_DATA stData;
    GLuint    nTexId;
} _3DR_MDL;

#endif
