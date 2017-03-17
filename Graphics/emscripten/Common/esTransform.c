//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// ESUtil.c
//
//    A utility library for OpenGL ES.  This library provides a
//    basic common framework for the example applications in the
//    OpenGL ES 2.0 Programming Guide.
//

///
//  Includes
//
#include "esUtil.h"
#include <math.h>
#include <string.h>

#define PI 3.1415926535897932384626433832795f

void ESUTIL_API
esScale(ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz)
{
    result->m[0][0] *= sx;
    result->m[0][1] *= sx;
    result->m[0][2] *= sx;
    result->m[0][3] *= sx;

    result->m[1][0] *= sy;
    result->m[1][1] *= sy;
    result->m[1][2] *= sy;
    result->m[1][3] *= sy;

    result->m[2][0] *= sz;
    result->m[2][1] *= sz;
    result->m[2][2] *= sz;
    result->m[2][3] *= sz;
}

void ESUTIL_API
esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz)
{
    result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
    result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
    result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
    result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

void ESUTIL_API
esRotate(ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
   GLfloat sinAngle, cosAngle;
   GLfloat mag = sqrtf(x * x + y * y + z * z);
      
   sinAngle = sinf ( angle * PI / 180.0f );
   cosAngle = cosf ( angle * PI / 180.0f );
   if ( mag > 0.0f )
   {
      GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
      GLfloat oneMinusCos;
      ESMatrix rotMat;
   
      x /= mag;
      y /= mag;
      z /= mag;

      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * sinAngle;
      ys = y * sinAngle;
      zs = z * sinAngle;
      oneMinusCos = 1.0f - cosAngle;

      rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
      rotMat.m[0][1] = (oneMinusCos * xy) - zs;
      rotMat.m[0][2] = (oneMinusCos * zx) + ys;
      rotMat.m[0][3] = 0.0F; 

      rotMat.m[1][0] = (oneMinusCos * xy) + zs;
      rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
      rotMat.m[1][2] = (oneMinusCos * yz) - xs;
      rotMat.m[1][3] = 0.0F;

      rotMat.m[2][0] = (oneMinusCos * zx) - ys;
      rotMat.m[2][1] = (oneMinusCos * yz) + xs;
      rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
      rotMat.m[2][3] = 0.0F; 

      rotMat.m[3][0] = 0.0F;
      rotMat.m[3][1] = 0.0F;
      rotMat.m[3][2] = 0.0F;
      rotMat.m[3][3] = 1.0F;

      esMatrixMultiply( result, &rotMat, result );
   }
}

void ESUTIL_API
esFrustum(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    ESMatrix    frust;

    if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
         (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
         return;

    frust.m[0][0] = 2.0f * nearZ / deltaX;
    frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

    frust.m[1][1] = 2.0f * nearZ / deltaY;
    frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

    frust.m[2][0] = (right + left) / deltaX;
    frust.m[2][1] = (top + bottom) / deltaY;
    frust.m[2][2] = -(nearZ + farZ) / deltaZ;
    frust.m[2][3] = -1.0f;

    frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

    esMatrixMultiply(result, &frust, result);
}


void ESUTIL_API 
esPerspective(ESMatrix *result, float fovy, float aspect, float nearZ, float farZ)
{
   GLfloat frustumW, frustumH;
   
   frustumH = tanf( fovy / 360.0f * PI ) * nearZ;
   frustumW = frustumH * aspect;

   esFrustum( result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ );
}

void ESUTIL_API
esOrtho(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    ESMatrix    ortho;

    if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
        return;

    esMatrixLoadIdentity(&ortho);
    ortho.m[0][0] = 2.0f / deltaX;
    ortho.m[3][0] = -(right + left) / deltaX;
    ortho.m[1][1] = 2.0f / deltaY;
    ortho.m[3][1] = -(top + bottom) / deltaY;
    ortho.m[2][2] = -2.0f / deltaZ;
    ortho.m[3][2] = -(nearZ + farZ) / deltaZ;

    esMatrixMultiply(result, &ortho, result);
}


void ESUTIL_API
esMatrixMultiply(ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB)
{
    ESMatrix    tmp;
    int         i;

	for (i=0; i<4; i++)
	{
		tmp.m[i][0] =	(srcA->m[i][0] * srcB->m[0][0]) +
						(srcA->m[i][1] * srcB->m[1][0]) +
						(srcA->m[i][2] * srcB->m[2][0]) +
						(srcA->m[i][3] * srcB->m[3][0]) ;

		tmp.m[i][1] =	(srcA->m[i][0] * srcB->m[0][1]) + 
						(srcA->m[i][1] * srcB->m[1][1]) +
						(srcA->m[i][2] * srcB->m[2][1]) +
						(srcA->m[i][3] * srcB->m[3][1]) ;

		tmp.m[i][2] =	(srcA->m[i][0] * srcB->m[0][2]) + 
						(srcA->m[i][1] * srcB->m[1][2]) +
						(srcA->m[i][2] * srcB->m[2][2]) +
						(srcA->m[i][3] * srcB->m[3][2]) ;

		tmp.m[i][3] =	(srcA->m[i][0] * srcB->m[0][3]) + 
						(srcA->m[i][1] * srcB->m[1][3]) +
						(srcA->m[i][2] * srcB->m[2][3]) +
						(srcA->m[i][3] * srcB->m[3][3]) ;
	}
    memcpy(result, &tmp, sizeof(ESMatrix));
}


void ESUTIL_API
esMatrixLoadIdentity(ESMatrix *result)
{
    memset(result, 0x0, sizeof(ESMatrix));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;
}

void ESUTIL_API
esVectorSubtract(ESVector* result, ESVector* a, ESVector* b)
{
    result->v[0] = a->v[0] - b->v[0];
    result->v[1] = a->v[1] - b->v[1];
    result->v[2] = a->v[2] - b->v[2];
}

void ESUTIL_API
esNormalizeVector(ESVector* vector)
{
    GLfloat norm = vector->v[0]*vector->v[0] +
                   vector->v[1]*vector->v[1] + 
                   vector->v[2]*vector->v[2];
    norm = sqrtf(norm);
    vector->v[0] = vector->v[0] / norm;
    vector->v[1] = vector->v[1] / norm;
    vector->v[2] = vector->v[2] / norm;
}

void ESUTIL_API
esCrossProduct(ESVector* result, ESVector* a, ESVector* b)
{
    result->v[0] = a->v[1] * b->v[2] - a->v[2] * b->v[1];
    result->v[1] = a->v[2] * b->v[0] - a->v[0] * b->v[2];
    result->v[2] = a->v[0] * b->v[1] - a->v[1] * b->v[0];
}

void ESUTIL_API
esLookAt(ESMatrix* result, ESVector* eye, ESVector* target, ESVector* up)
{
    ESVector f, s, u;
    esVectorSubtract(&f, target, eye);
    esNormalizeVector(&f);
    esNormalizeVector(up);
    esCrossProduct(&s, &f, up);
    esCrossProduct(&u, &s, &f);
    ESMatrix M, T;
    esMatrixLoadIdentity(&M);
    esMatrixLoadIdentity(&T);
    for(int i = 0; i < 3; i++)
    {
        M.m[0][i] = s.v[i];
        M.m[1][i] = u.v[i];
        M.m[2][i] = -f.v[i];
        T.m[i][3] = -eye->v[i];
    }
    esMatrixMultiply(result, &M, &T);
}

void ESUTIL_API
esMatrixInverse(ESMatrix* result, ESMatrix* matrix)
{
  GLfloat tmp_0 = matrix->m[2][2] * matrix->m[3][3];
  GLfloat tmp_1 = matrix->m[3][2] * matrix->m[2][3];
  GLfloat tmp_2 = matrix->m[1][2] * matrix->m[3][3];
  GLfloat tmp_3 = matrix->m[3][2] * matrix->m[1][3];
  GLfloat tmp_4 = matrix->m[1][2] * matrix->m[2][3];
  GLfloat tmp_5 = matrix->m[2][2] * matrix->m[1][3];
  GLfloat tmp_6 = matrix->m[0][2] * matrix->m[3][3];
  GLfloat tmp_7 = matrix->m[3][2] * matrix->m[0][3];
  GLfloat tmp_8 = matrix->m[0][2] * matrix->m[2][3];
  GLfloat tmp_9 = matrix->m[2][2] * matrix->m[0][3];
  GLfloat tmp_10 = matrix->m[0][2] * matrix->m[1][3];
  GLfloat tmp_11 = matrix->m[1][2] * matrix->m[0][3];
  GLfloat tmp_12 = matrix->m[2][0] * matrix->m[3][1];
  GLfloat tmp_13 = matrix->m[3][0] * matrix->m[2][1];
  GLfloat tmp_14 = matrix->m[1][0] * matrix->m[3][1];
  GLfloat tmp_15 = matrix->m[3][0] * matrix->m[1][1];
  GLfloat tmp_16 = matrix->m[1][0] * matrix->m[2][1];
  GLfloat tmp_17 = matrix->m[2][0] * matrix->m[1][1];
  GLfloat tmp_18 = matrix->m[0][0] * matrix->m[3][1];
  GLfloat tmp_19 = matrix->m[3][0] * matrix->m[0][1];
  GLfloat tmp_20 = matrix->m[0][0] * matrix->m[2][1];
  GLfloat tmp_21 = matrix->m[2][0] * matrix->m[0][1];
  GLfloat tmp_22 = matrix->m[0][0] * matrix->m[1][1];
  GLfloat tmp_23 = matrix->m[1][0] * matrix->m[0][1];

  GLfloat t0 = (tmp_0 * matrix->m[1][1] + tmp_3 * matrix->m[2][1] + tmp_4 * matrix->m[3][1]) -
      (tmp_1 * matrix->m[1][1] + tmp_2 * matrix->m[2][1] + tmp_5 * matrix->m[3][1]);
  GLfloat t1 = (tmp_1 * matrix->m[0][1] + tmp_6 * matrix->m[2][1] + tmp_9 * matrix->m[3][1]) -
      (tmp_0 * matrix->m[0][1] + tmp_7 * matrix->m[2][1] + tmp_8 * matrix->m[3][1]);
  GLfloat t2 = (tmp_2 * matrix->m[0][1] + tmp_7 * matrix->m[1][1] + tmp_10 * matrix->m[3][1]) -
      (tmp_3 * matrix->m[0][1] + tmp_6 * matrix->m[1][1] + tmp_11 * matrix->m[3][1]);
  GLfloat t3 = (tmp_5 * matrix->m[0][1] + tmp_8 * matrix->m[1][1] + tmp_11 * matrix->m[2][1]) -
      (tmp_4 * matrix->m[0][1] + tmp_9 * matrix->m[1][1] + tmp_10 * matrix->m[2][1]);

  GLfloat d = 1.0 / (matrix->m[0][0] * t0 + matrix->m[1][0] * t1 + matrix->m[2][0] * t2 + matrix->m[3][0] * t3);

  result->m[0][0] = d * t0;
  result->m[0][1] = d * t1;
  result->m[0][2] = d * t2;
  result->m[0][3] = d * t3;
  result->m[1][0] = d * ((tmp_1 * matrix->m[1][0] + tmp_2 * matrix->m[2][0] + tmp_5 * matrix->m[3][0]) -
                    (tmp_0 * matrix->m[1][0] + tmp_3 * matrix->m[2][0] + tmp_4 * matrix->m[3][0]));
  result->m[1][1] = d * ((tmp_0 * matrix->m[0][0] + tmp_7 * matrix->m[2][0] + tmp_8 * matrix->m[3][0]) -
                    (tmp_1 * matrix->m[0][0] + tmp_6 * matrix->m[2][0] + tmp_9 * matrix->m[3][0]));
  result->m[1][2] = d * ((tmp_3 * matrix->m[0][0] + tmp_6 * matrix->m[1][0] + tmp_11 * matrix->m[3][0]) -
          (tmp_2 * matrix->m[0][0] + tmp_7 * matrix->m[1][0] + tmp_10 * matrix->m[3][0]));
  result->m[1][3] = d * ((tmp_4 * matrix->m[0][0] + tmp_9 * matrix->m[1][0] + tmp_10 * matrix->m[2][0]) -
          (tmp_5 * matrix->m[0][0] + tmp_8 * matrix->m[1][0] + tmp_11 * matrix->m[2][0]));
  result->m[2][0] = d * ((tmp_12 * matrix->m[1][3] + tmp_15 * matrix->m[2][3] + tmp_16 * matrix->m[3][3]) -
          (tmp_13 * matrix->m[1][3] + tmp_14 * matrix->m[2][3] + tmp_17 * matrix->m[3][3]));
  result->m[2][1] = d * ((tmp_13 * matrix->m[0][3] + tmp_18 * matrix->m[2][3] + tmp_21 * matrix->m[3][3]) -
          (tmp_12 * matrix->m[0][3] + tmp_19 * matrix->m[2][3] + tmp_20 * matrix->m[3][3]));
  result->m[2][2] = d * ((tmp_14 * matrix->m[0][3] + tmp_19 * matrix->m[1][3] + tmp_22 * matrix->m[3][3]) -
          (tmp_15 * matrix->m[0][3] + tmp_18 * matrix->m[1][3] + tmp_23 * matrix->m[3][3]));
  result->m[2][3] = d * ((tmp_17 * matrix->m[0][3] + tmp_20 * matrix->m[1][3] + tmp_23 * matrix->m[2][3]) -
          (tmp_16 * matrix->m[0][3] + tmp_21 * matrix->m[1][3] + tmp_22 * matrix->m[2][3]));
  result->m[3][0] = d * ((tmp_14 * matrix->m[2][2] + tmp_17 * matrix->m[3][2] + tmp_13 * matrix->m[1][2]) -
          (tmp_16 * matrix->m[3][2] + tmp_12 * matrix->m[1][2] + tmp_15 * matrix->m[2][2]));
  result->m[3][1] = d * ((tmp_20 * matrix->m[3][2] + tmp_12 * matrix->m[0][2] + tmp_19 * matrix->m[2][2]) -
          (tmp_18 * matrix->m[2][2] + tmp_21 * matrix->m[3][2] + tmp_13 * matrix->m[0][2]));
  result->m[3][2] = d * ((tmp_18 * matrix->m[1][2] + tmp_23 * matrix->m[3][2] + tmp_15 * matrix->m[0][2]) -
          (tmp_22 * matrix->m[3][2] + tmp_14 * matrix->m[0][2] + tmp_19 * matrix->m[1][2]));
  result->m[3][3] = d * ((tmp_22 * matrix->m[2][2] + tmp_16 * matrix->m[0][2] + tmp_21 * matrix->m[1][2]) -
          (tmp_20 * matrix->m[1][2] + tmp_23 * matrix->m[2][2] + tmp_17 * matrix->m[0][2]));
};

void ESUTIL_API
esVectorScale(ESVector* result, ESVector* source, GLfloat factor)
{
    result->v[0] = source->v[0] * factor;
    result->v[1] = source->v[1] * factor;
    result->v[2] = source->v[2] * factor;
}

void ESUTIL_API
esGetAxis(ESVector* result, ESMatrix* matrix, int axis)
{
    result->v[0] = matrix->m[axis][0];
    result->v[1] = matrix->m[axis][1];
    result->v[2] = matrix->m[axis][2];
}

void ESUTIL_API
esAddVector(ESVector* result, ESVector* a, ESVector *b)
{
    result->v[0] = a->v[0] + b->v[0];
    result->v[1] = a->v[1] + b->v[1];
    result->v[2] = a->v[2] + b->v[2];
}

void ESUTIL_API
esMatrixTranspose(ESMatrix* result, ESMatrix* matrix)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result->m[i][j] = matrix->m[j][i];
        }
    }
}