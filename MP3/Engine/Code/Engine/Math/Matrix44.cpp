/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "Engine/Math/Matrix44.hpp"
#include "Engine/MathUtils.hpp"
#include <string>

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/

mat44_fl mat44_fl::identity(1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f);

/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL VARIABLES                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL FUNCTIONS                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
//
// void MatrixTranspose(mat44_fl *mat)
// {
// 	float *data = mat->data;
// 	for (unsigned int y = 1; y < 4; y++)
// 	{
// 		for (unsigned int x = 0; x < y; x++)
// 		{
// 			//Swap(data[(x * 4) + y], data[(y * 4) + x]);
// 			int aIndex = (x * 4) + y;
// 			int bIndex = (y * 4) + x;
// 
// 			float a = data[aIndex];
// 			float b = data[bIndex];
// 
// 			data[aIndex] = b;
// 			data[bIndex] = a;
// 		}
// 	}
// }



//////////////////////////////////////////////////////////////////////////
//
Vector3 MatrixTransform(mat44_fl const *m, Vector3 const &v)
{
	Vector4 v4 = Vector4(v, 1.0f);
	MatrixTransform(m, v4);

	Vector3 ret = Vector3(v4.x, v4.y, v4.z);
	return ret;
}


//////////////////////////////////////////////////////////////////////////
//
Vector4 MatrixTransform(mat44_fl const *m, Vector4 const &v)
{
	m->col[0];
	Vector4 ret = Vector4(
		DotProduct(m->col[0], v),
		DotProduct(m->col[1], v),
		DotProduct(m->col[2], v),
		DotProduct(m->col[3], v));

	return ret;
}

//////////////////////////////////////////////////////////////////////////
//
void MatrixNormalize(mat44_fl *m)
{
	Vector3 f, r, u, o;

	MatrixGetBasis(m, &r, &u, &f, &o);

	f.SetNormalized();
	u.SetNormalized();
// 	Normalize3(&f);
// 	Normalize3(&u);

	r = CrossProduct(u, f);
	u = CrossProduct(f, r);

	MatrixSetBasis(m, r, u, f, o);
}


//////////////////////////////////////////////////////////////////////////
// Is the rotational part orthogonal [ignores the translation]
bool MatrixIsOrthogonal(mat44_fl const *m)
{
	Vector4 col0 = m->col[0];
	Vector4 col1 = m->col[1];
	Vector4 col2 = m->col[2];

	col0.w = 0.0f;
	col1.w = 0.0f;
	col2.w = 0.0f;

	return ((DotProduct(col0, col1) == 0.0f)
		&& (DotProduct(col0, col2) == 0.0f)
		&& (DotProduct(col1, col2) == 0.0f));
}



//////////////////////////////////////////////////////////////////////////
// Lifted from GLU
float MatrixGetDeterminant(mat44_fl const *mat)
{
	float inv[4];

	float det;
	float const *m = mat->data;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[1] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[2] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[3] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];


	det = m[0] * inv[0] + m[1] * inv[1] + m[2] * inv[2] + m[3] * inv[3];
	return det;
}

//------------------------------------------------------------------------
void MatrixGetColumn(mat44_fl const *m, int col, Vector4 *out)
{
	*out = m->col[col];
}

//------------------------------------------------------------------------
void MatrixSetColumn(mat44_fl *m, int col, Vector4 const &v)
{
	m->col[col] = v;
}

//------------------------------------------------------------------------
void MatrixSetColumns(mat44_fl *m, Vector4 const &c0, Vector4 const &c1, Vector4 const &c2, Vector4 const &c3)
{
	m->col[0] = c0;
	m->col[1] = c1;
	m->col[2] = c2;
	m->col[3] = c3;
}

//------------------------------------------------------------------------
void MatrixGetRow(mat44_fl const *m, int row, Vector4 *out)
{
	*out = Vector4(m->data[row + 0],
		m->data[row + 4],
		m->data[row + 8],
		m->data[row + 12]);
}

//------------------------------------------------------------------------
void MatrixSetRow(mat44_fl *m, int row, Vector4 const &v)
{
	m->data[row + 0] = v.x;
	m->data[row + 4] = v.y;
	m->data[row + 8] = v.z;
	m->data[row + 12] = v.w;
}

//------------------------------------------------------------------------
Vector3 MatrixGetOffset(mat44_fl const *m)
{
	return Vector3(m->data[12],
		m->data[13],
		m->data[14]);
}

//------------------------------------------------------------------------
void MatrixSetOffset(mat44_fl *m, Vector3 const &o)
{
	m->data[12] = o.x;
	m->data[13] = o.y;
	m->data[14] = o.z;
}



//////////////////////////////////////////////////////////////////////////
// Only works on orthoganl matrices
// 
void MatrixInvertOrthogonal(mat44_fl *mat)
{
	// ASSERT( MatrixIsOrthogonal(mat) );

	Vector3 translation = MatrixGetOffset(mat);
	MatrixTranspose(mat);
	MatrixSetColumn(mat, 3, Vector4(0.0f, 0.0f, 0.0f, 1.0f));

	translation = -translation * (*mat);
	MatrixSetOffset(mat, translation);
}

//////////////////////////////////////////////////////////////////////////
// Lifted from GLU
void MatrixInvert(mat44_fl *mat)
{
	float inv[16];
	float det;
	float *const m = mat->data;
	int i;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	//ASSERT_RETURN(!FlEqual(det, 0.0f));  // Not invertible

	det = 1.0f / det;

	for (i = 0; i < 16; i++) {
		m[i] = inv[i] * det;
	}
}

void MatrixTranspose(mat44_fl *mat)
{
	mat;
}


//------------------------------------------------------------------------
void MatrixMultiply(mat44_fl *out, mat44_fl const *lhs, mat44_fl const *rhs)
{
	float result[16];
	result[0] = lhs->data[0] * rhs->data[0] + lhs->data[1] * rhs->data[4] + lhs->data[2] * rhs->data[8] + lhs->data[3] * rhs->data[12];
	result[1] = lhs->data[0] * rhs->data[1] + lhs->data[1] * rhs->data[5] + lhs->data[2] * rhs->data[9] + lhs->data[3] * rhs->data[13];
	result[2] = lhs->data[0] * rhs->data[2] + lhs->data[1] * rhs->data[6] + lhs->data[2] * rhs->data[10] + lhs->data[3] * rhs->data[14];
	result[3] = lhs->data[0] * rhs->data[3] + lhs->data[1] * rhs->data[7] + lhs->data[2] * rhs->data[11] + lhs->data[3] * rhs->data[15];

	result[4] = lhs->data[4] * rhs->data[0] + lhs->data[5] * rhs->data[4] + lhs->data[6] * rhs->data[8] + lhs->data[7] * rhs->data[12];
	result[5] = lhs->data[4] * rhs->data[1] + lhs->data[5] * rhs->data[5] + lhs->data[6] * rhs->data[9] + lhs->data[7] * rhs->data[13];
	result[6] = lhs->data[4] * rhs->data[2] + lhs->data[5] * rhs->data[6] + lhs->data[6] * rhs->data[10] + lhs->data[7] * rhs->data[14];
	result[7] = lhs->data[4] * rhs->data[3] + lhs->data[5] * rhs->data[7] + lhs->data[6] * rhs->data[11] + lhs->data[7] * rhs->data[15];

	result[8] = lhs->data[8] * rhs->data[0] + lhs->data[9] * rhs->data[4] + lhs->data[10] * rhs->data[8] + lhs->data[11] * rhs->data[12];
	result[9] = lhs->data[8] * rhs->data[1] + lhs->data[9] * rhs->data[5] + lhs->data[10] * rhs->data[9] + lhs->data[11] * rhs->data[13];
	result[10] = lhs->data[8] * rhs->data[2] + lhs->data[9] * rhs->data[6] + lhs->data[10] * rhs->data[10] + lhs->data[11] * rhs->data[14];
	result[11] = lhs->data[8] * rhs->data[3] + lhs->data[9] * rhs->data[7] + lhs->data[10] * rhs->data[11] + lhs->data[11] * rhs->data[15];

	result[12] = lhs->data[12] * rhs->data[0] + lhs->data[13] * rhs->data[4] + lhs->data[14] * rhs->data[8] + lhs->data[15] * rhs->data[12];
	result[13] = lhs->data[12] * rhs->data[1] + lhs->data[13] * rhs->data[5] + lhs->data[14] * rhs->data[9] + lhs->data[15] * rhs->data[13];
	result[14] = lhs->data[12] * rhs->data[2] + lhs->data[13] * rhs->data[6] + lhs->data[14] * rhs->data[10] + lhs->data[15] * rhs->data[14];
	result[15] = lhs->data[12] * rhs->data[3] + lhs->data[13] * rhs->data[7] + lhs->data[14] * rhs->data[11] + lhs->data[15] * rhs->data[15];
	
	memcpy(out->data, result, sizeof(result));
}

//------------------------------------------------------------------------
void MatrixMakeIdentity(mat44_fl *mat)
{
	float const values[] = {
		1.0f,	0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,	0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,	0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	memcpy(mat->data, values, sizeof(values));
}

//------------------------------------------------------------------------
//
void MatrixMakeScale(mat44_fl *mat, float c)
{
	float const values[] = {
		c,		0.0f, 0.0f, 0.0f,
		0.0f, c,		0.0f, 0.0f,
		0.0f, 0.0f, c,		0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// column major sets are a tad quicker, and this one is the same transposed.
	memcpy(mat->data, values, sizeof(values));
}

//------------------------------------------------------------------------
//
void MatrixMakeTranslation(mat44_fl *mat, Vector3 const &offset)
{
	MatrixMakeIdentity(mat);
	MatrixSetOffset(mat, offset);
}

//------------------------------------------------------------------------
//
void MatrixMakeRotationAroundX(mat44_fl *mat, float const rad)
{
	float cosr = cos(rad);
	float sinr = sin(rad);

	float const values[] = {
		1.0f,		0.0f,		0.0f,		0.0f,
		0.0f,		cosr,		sinr,	   0.0f,
		0.0f,		-sinr,	cosr,		0.0f,
		0.0f,		0.0f,		0.0f,		1.0f
	};

	memcpy(mat->data, values, sizeof(values));
}

//------------------------------------------------------------------------
//
void MatrixMakeRotationAroundY(mat44_fl *mat, float const rad)
{
	float cosr = cos(rad);
	float sinr = sin(rad);

	float const values[] = {
		cosr,		0.0f,		sinr,	   0.0f,
		0.0f,		1.0f,		0.0f,		0.0f,
		-sinr,	0.0f,		cosr,		0.0f,
		0.0f,		0.0f,		0.0f,		1.0f
	};

	memcpy(mat->data, values, sizeof(values));
}

//------------------------------------------------------------------------
//
void MatrixMakeRotationAroundZ(mat44_fl *mat, float const rad)
{
	float cosr = cos(rad);
	float sinr = sin(rad);

	float const values[] = {
		cosr,		sinr,   0.0f,		0.0f,
		-sinr,	cosr,		0.0f,		0.0f,
		0.0f,		0.0f,		1.0f,		0.0f,
		0.0f,		0.0f,		0.0f,		1.0f
	};

	memcpy(mat->data, values, sizeof(values));
}

//shhh

//------------------------------------------------------------------------
void EulerRight(Vector3 *v, float const yaw, float const pitch, float const roll)
{
	float sx = sin(pitch);
	float cx = cos(pitch);

	float sy = sin(yaw);
	float cy = cos(yaw);

	float sz = sin(roll);
	float cz = cos(roll);

	*v = Vector3(cy*cz + sx*sy*sz, -cx*sx, -cz*sy + cy*sx*sz);
}

//------------------------------------------------------------------------
void EulerUp(Vector3 *v, float const yaw, float const pitch, float const roll)
{
	float sx = sin(pitch);
	float cx = cos(pitch);

	float sy = sin(yaw);
	float cy = cos(yaw);

	float sz = sin(roll);
	float cz = cos(roll);

	*v = Vector3(cy*sz - cz*sx*sy, cx*cz, -sy*sz - cy*cz*sx);
}

//------------------------------------------------------------------------
void EulerForward(Vector3 *v, float const yaw, float const pitch, float const roll)
{
	roll;
	float sx = sin(pitch);
	float cx = cos(pitch);

	float sy = sin(yaw);
	float cy = cos(yaw);

	*v = Vector3(cx*sy, sx, cx*cy);
}



//------------------------------------------------------------------------
void MatrixGenerateBasis(mat44_fl *outm, Vector3 const &forward)
{
	Vector3 f = forward.GetNormalized();
	Vector3 r;
	Vector3 u;

	Vector3 up = Vector3(0.f, 0.f, 1.f);
	Vector3 right = Vector3(1.f, 0.f, 0.f);

	if (IsColinear(forward, up)) {
		u = CrossProduct(f, right);
		r = CrossProduct(u, f);
	}
	else {
		r = CrossProduct(up, f);
		u = CrossProduct(f, r);
	}

	MatrixSetBasis(outm, r, u, f, Vector3::ZERO);
}

//------------------------------------------------------------------------
//
void MatrixMakeLookTo(mat44_fl *mat, Vector3 const &pos, Vector3 const &dir, Vector3 const &up)
{
	Vector3 u = up;
	Vector3 f = dir;


	u.SetNormalized();
	f.SetNormalized();
// 	Normalize3(&u);
// 	Normalize3(&f);
	Vector3 r = CrossProduct(u, f);
	u = CrossProduct(f, r);
	u.SetNormalized();
	//Normalize3(&u);

	MatrixSetBasis(mat, r, u, f, pos);
}

//------------------------------------------------------------------------
//
void MatrixMakeLookAt(mat44_fl *mat, Vector3 const &from, Vector3 const &to, Vector3 const &up)
{
	MatrixMakeLookTo(mat, from, to - from, up);
}

//------------------------------------------------------------------------
//
void MatrixMakeProjOrthogonal(mat44_fl *mat, float nx, float fx, float ny, float fy, float nz, float fz)
{
	float sx = 1.0f / (fx - nx);
	float sy = 1.0f / (fy - ny);
	float sz = 1.0f / (fz - nz);

	// maps (n, f) to (-1, 1)
	// == (x - n) / (f - n) * (2) - 1;
	// == 2(x - n) / (f - n) - (f - n)/(f-n)
	// == (2x / (f - n)) + (-2n - f + n)/(f - n)
	// == (2x / (f - n)) - (f + n) / (f - n);

	// So x = n, (- f - n + 2n) / (f - n) = -(f - n)/(f - n) == -1, checks out
	// And x = f, (-f - n + 2f) / (f - n) = (f - n) / (f - n) = 1, checks out

	float const values[] = {
		2.0f * sx,	0.0f,			0.0f,			-(fx + nx) * sx,
		0.0f,			2.0f * sy,	0.0f,			-(fy + ny) * sy,
		0.0f,			0.0f,			2.0f * sz,  -(fz + nz) * sz,
		0.0f,			0.0f,			0.0f,			1.0f,
	};

	memcpy(mat->data, values, sizeof(values));
}

//------------------------------------------------------------------------
void MatrixMakeProjOrthogonal(mat44_fl *mat, float width, float height, float nz, float fz)
{
	float hw = width / 2.0f;
	float hh = height / 2.0f;
	MatrixMakeProjOrthogonal(mat, -hw, hw, -hh, hh, nz, fz);
}

//////////////////////////////////////////////////////////////////////////
//
void MatrixMakeProjPerspective(mat44_fl *mat, float ratio, float rads, float nz, float fz)
{
	float size = atan(rads / 2.0f);

	float w = size;
	float h = size;
	if (ratio > 1.0f) {
		w *= ratio;
	}
	else {
		h /= ratio;
	}

	float q = 1.0f / (fz - nz);

	float const values[] = {
		1.0f / w,	0.0f,			0.0f,			   0.0f,
		0.0f,			1.0f / h,	0.0f,			   0.0f,
		0.0f,			0.0f,			(fz + nz) * q,	-2.0f * nz * fz * q,
		0.0f,			0.0f,			1.0f,			   0.0f,
	};

	memcpy(mat->data, values, sizeof(values));
}

//------------------------------------------------------------------------
void MatrixTranslate(mat44_fl *mat, Vector3 const &o)
{
	Vector4 v;
	MatrixGetRow(mat, 3, &v);
	v.x += o.x;
	v.y += o.y;
	v.z += o.z;
	MatrixSetRow(mat, 3, v);
}

//------------------------------------------------------------------------
void MatrixGetBasis(mat44_fl const *mat, Vector3 *r, Vector3 *u, Vector3 *f, Vector3 *o)
{
	Vector4 x, y, z, w;
	MatrixGetRows(mat, &x, &y, &z, &w);
	*r = Vector3(x.x, x.y, x.z);
	*u = Vector3(y.x, y.y, y.z);
	*f = Vector3(z.x, z.y, z.z);
	*o = Vector3(w.x, w.y, w.z);
}

//------------------------------------------------------------------------
void MatrixSetBasis(mat44_fl *mat, Vector3 const &r, Vector3 const &u, Vector3 const &f, Vector3 const &o)
{
	MatrixSetRows(mat,
		Vector4(r, 0.0f),
		Vector4(u, 0.0f),
		Vector4(f, 0.0f),
		Vector4(o, 1.0f));
}

//------------------------------------------------------------------------
Vector3 MatrixGetRight(mat44_fl const *mat)
{
	Vector4 row;
	MatrixGetRow(mat, 0, &row);
	return row.GetXYZ();
}

//------------------------------------------------------------------------
Vector3 MatrixGetUp(mat44_fl const *mat)
{
	Vector4 row;
	MatrixGetRow(mat, 1, &row);
	return row.GetXYZ();
}

//------------------------------------------------------------------------
Vector3 MatrixGetForward(mat44_fl const *mat)
{
	Vector4 row;
	MatrixGetRow(mat, 2, &row);
	return row.GetXYZ();
}

//------------------------------------------------------------------------
void MatrixGetRows(mat44_fl const *mat, Vector4 *x, Vector4 *y, Vector4 *z, Vector4 *o)
{
	MatrixGetRow(mat, 0, x);
	MatrixGetRow(mat, 1, y);
	MatrixGetRow(mat, 2, z);
	MatrixGetRow(mat, 3, o);
}

//------------------------------------------------------------------------
void MatrixSetRows(mat44_fl *mat, Vector4 const &x, Vector4 const &y, Vector4 const &z, Vector4 const &o)
{
	MatrixSetRow(mat, 0, x);
	MatrixSetRow(mat, 1, y);
	MatrixSetRow(mat, 2, z);
	MatrixSetRow(mat, 3, o);
}
