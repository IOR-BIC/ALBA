/*=========================================================================

Program: ALBA (Agile Library for Biomedical Applications)
Module: albaVect3d
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "albaQuaternion.h"
#include "albaMatrix.h"
#include "albaVect3d.h"

#include "vtkMatrix4x4.h"

#define EPSILON 0.00001f

//----------------------------------------------------------------------------
albaQuaternion::albaQuaternion() 
{
	m_W = m_X = m_Y = m_Z = 0;
}

//----------------------------------------------------------------------------
albaQuaternion::albaQuaternion(double w, double x, double y, double z)
{
	m_W = w;
	m_X = x;
	m_Y = y;
	m_Z = z;
}

//----------------------------------------------------------------------------
albaQuaternion::albaQuaternion(double angle,  albaVect3d &axis)
{
	double a = angle * 0.5f;
	double s = (double) sin(a);
	double c = (double) cos(a);

	m_W = c;
	m_X = axis.GetX() * s;
	m_Y = axis.GetY() * s;
	m_Z = axis.GetZ() * s;
}

//----------------------------------------------------------------------------
albaQuaternion::albaQuaternion( albaMatrix &matr)
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "quaternion Calculus and Fast Animation".

	double trace = matr.GetElement(1,1) + matr.GetElement(2,2) + matr.GetElement(3,3);

	if (trace>0)
	{
		// |w| > 1/2, may as well choose w > 1/2

		double root = sqrt(trace + 1.0f);  // 2w
		m_W = 0.5f * root;
		root = 0.5f / root;  // 1/(4w)
		m_X = (matr.GetElement(3,2)-matr.GetElement(2,3)) * root;
		m_Y = (matr.GetElement(1,3)-matr.GetElement(3,1)) * root;
		m_Z = (matr.GetElement(2,1)-matr.GetElement(1,2)) * root;
	}
	else
	{
		// |w| <= 1/2

		static int next[3] = { 2, 3, 1 };

		int i = 1;
		if (matr.GetElement(2,2)>matr.GetElement(1,1))  i = 2;
		if (matr.GetElement(3,3)>matr.GetElement(i,i)) i = 3;
		int j = next[i];
		int k = next[j];

		double root = sqrt(matr.GetElement(i,i)-matr.GetElement(j,j)-matr.GetElement(k,k) + 1.0f);
		double *quaternion[3] = { &m_X, &m_Y, &m_Z };
		*quaternion[i] = 0.5f * root;
		root = 0.5f / root;
		m_W = (matr.GetElement(k,j)-matr.GetElement(j,k))*root;
		*quaternion[j] = (matr.GetElement(j,i)+matr.GetElement(i,j))*root;
		*quaternion[k] = (matr.GetElement(k,i)+matr.GetElement(i,k))*root;
	}
}


//----------------------------------------------------------------------------
albaMatrix albaQuaternion::ToMatrix()
{
	double fTx  = 2.0f*m_X;
	double fTy  = 2.0f*m_Y;
	double fTz  = 2.0f*m_Z;
	double fTwx = fTx*m_W;
	double fTwy = fTy*m_W;
	double fTwz = fTz*m_W;
	double fTxx = fTx*m_X;
	double fTxy = fTy*m_X;
	double fTxz = fTz*m_X;
	double fTyy = fTy*m_Y;
	double fTyz = fTz*m_Y;
	double fTzz = fTz*m_Z;

	double values[16] = { 1.0f - (fTyy + fTzz),	fTxy - fTwz,        fTxz + fTwy,        0,
												fTxy + fTwz,        1.0f - (fTxx + fTzz), fTyz - fTwx,        0,
												fTxz - fTwy,        fTyz + fTwx,					1.0f - (fTxx + fTyy), 0,
												0,									0,										0,									1 };
	albaMatrix mtr;
	mtr.GetVTKMatrix()->DeepCopy(values);

	return mtr;
}

//----------------------------------------------------------------------------
void albaQuaternion::AngleAxis(double &angle, albaVect3d &axis)
{
	double squareLength = m_X*m_X + m_Y*m_Y + m_Z*m_Z;

	angle = 2.0f * (double) acos(m_W);
	double inverseLength = 1.0f / (double) pow(squareLength, 0.5f);
	axis.SetX( m_X * inverseLength);
	axis.SetY( m_Y * inverseLength);
	axis.SetZ( m_Z * inverseLength);

}

//----------------------------------------------------------------------------
void albaQuaternion::Zero()
{
	m_W = 0;
	m_X = 0;
	m_Y = 0;
	m_Z = 0;
}

//----------------------------------------------------------------------------
void albaQuaternion::Identity()
{
	m_W = 1;
	m_X = 0;
	m_Y = 0;
	m_Z = 0;
}

//----------------------------------------------------------------------------
void albaQuaternion::Add( albaQuaternion &q)
{
	m_W += q.m_W;
	m_X += q.m_X;
	m_Y += q.m_Y;
	m_Z += q.m_Z;
}

//----------------------------------------------------------------------------
void albaQuaternion::Subtract( albaQuaternion &q)
{
	m_W -= q.m_W;
	m_X -= q.m_X;
	m_Y -= q.m_Y;
	m_Z -= q.m_Z;
}

//----------------------------------------------------------------------------
void albaQuaternion::Multiply(double s)
{
	m_W *= s;
	m_X *= s;
	m_Y *= s;
	m_Z *= s;
}

//----------------------------------------------------------------------------
void albaQuaternion::Divide(double s)
{
	double inv = 1.0f / s;
	m_W *= inv;
	m_X *= inv;
	m_Y *= inv;
	m_Z *= inv;
}

//----------------------------------------------------------------------------
void albaQuaternion::Multiply( albaQuaternion &q)
{
	double rw = m_W*q.m_W - m_X*q.m_X - m_Y*q.m_Y - m_Z*q.m_Z;
	double rx = m_W*q.m_X + m_X*q.m_W + m_Y*q.m_Z - m_Z*q.m_Y;
	double ry = m_W*q.m_Y - m_X*q.m_Z + m_Y*q.m_W + m_Z*q.m_X;
	double rz = m_W*q.m_Z + m_X*q.m_Y - m_Y*q.m_X + m_Z*q.m_W;
	m_W = rw;
	m_X = rx;
	m_Y = ry;
	m_Z = rz;
}

//----------------------------------------------------------------------------
void albaQuaternion::Multiply( albaQuaternion &q, albaQuaternion &result)
{
	result.m_W = m_W*q.m_W - m_X*q.m_X - m_Y*q.m_Y - m_Z*q.m_Z;
	result.m_X = m_W*q.m_X + m_X*q.m_W + m_Y*q.m_Z - m_Z*q.m_Y;
	result.m_Y = m_W*q.m_Y - m_X*q.m_Z + m_Y*q.m_W + m_Z*q.m_X;
	result.m_Z = m_W*q.m_Z + m_X*q.m_Y - m_Y*q.m_X + m_Z*q.m_W;
}

//----------------------------------------------------------------------------
albaQuaternion albaQuaternion::Dot( albaQuaternion &q)
{
	return albaQuaternion(m_W*q.m_W + m_X*q.m_X + m_Y*q.m_Y + m_Z*q.m_Z, 0, 0, 0);
}

//----------------------------------------------------------------------------
void albaQuaternion::Dot( albaQuaternion &q, albaQuaternion &result)
{
	result = albaQuaternion(m_W*q.m_W + m_X*q.m_X + m_Y*q.m_Y + m_Z*q.m_Z, 0, 0, 0);
}

//----------------------------------------------------------------------------
albaQuaternion albaQuaternion::Conjugate()
{
	return albaQuaternion(m_W, -m_X, -m_Y, -m_Z);
}

//----------------------------------------------------------------------------
void albaQuaternion::Conjugate(albaQuaternion &result)
{
	result = albaQuaternion(m_W, -m_X, -m_Y, -m_Z);
}

//----------------------------------------------------------------------------
double albaQuaternion::GetLength()
{
	return sqrt(m_W*m_W + m_X*m_X + m_Y*m_Y + m_Z*m_Z);
}



double albaQuaternion::GetNorm()
{
	return m_W*m_W + m_X*m_X + m_Y*m_Y + m_Z*m_Z;
}

//----------------------------------------------------------------------------
void albaQuaternion::Normalize()
{
	double length = GetLength();

	if (length == 0)
	{
		m_W = 1;
		m_X = 0;
		m_Y = 0;
		m_Z = 0;
	}
	else
	{
		double inv = 1.0f / length;
		m_X = m_X * inv;
		m_Y = m_Y * inv;
		m_Z = m_Z * inv;
		m_W = m_W * inv;
	}
}


//----------------------------------------------------------------------------
bool albaQuaternion::Normalized()
{
	albaQuaternion nomalized(*this);
	nomalized.Normalize();

	return *this == nomalized;
}

//----------------------------------------------------------------------------
albaQuaternion albaQuaternion::Inverse()
{
	double n = GetNorm();
	if (n!=0) return albaQuaternion(m_W/n, -m_X/n, -m_Y/n, -m_Z/n);
	else return albaQuaternion(-1.0,0.0,0.0,0.0);
}

//----------------------------------------------------------------------------
void albaQuaternion::Inverse(albaQuaternion &result)
{
	double n = GetNorm();
	if (n!=0) result=albaQuaternion(m_W/n, -m_X/n, -m_Y/n, -m_Z/n);
	else result=albaQuaternion(-1.0,0.0,0.0,0.0);
}

//----------------------------------------------------------------------------
albaQuaternion albaQuaternion::Slerp(albaQuaternion &a, albaQuaternion &b, double t)
{
	double flip = 1;

	double cosine = a.m_W*b.m_W + a.m_X*b.m_X + a.m_Y*b.m_Y + a.m_Z*b.m_Z;

	if (cosine < 0)
	{
		cosine = -cosine;
		flip = -1;
	}

	if ((1 - cosine) < EPSILON)
		return a * (1 - t) + b * (t*flip);

	double theta = (double)acos(cosine);
	double sine = (double)sin(theta);
	double beta = (double)sin((1 - t)*theta) / sine;
	double alpha = (double)sin(t*theta) / sine * flip;

	return a * beta + b * alpha;
}



//----------------------------------------------------------------------------
bool albaQuaternion::operator ==( albaQuaternion &other)
{
	if ( m_W==other.m_W && m_X==other.m_X && m_Y==other.m_Y && m_Z==other.m_Z)
		return true;
	else
		return false;
}

//----------------------------------------------------------------------------
bool albaQuaternion::operator !=( albaQuaternion &other)
{
	return !(*this==other);
}

//----------------------------------------------------------------------------
albaQuaternion albaQuaternion::operator-()
{
	return albaQuaternion(-m_W, -m_X, -m_Y, -m_Z);
}

//----------------------------------------------------------------------------
double& albaQuaternion::operator[](int i)
{
	switch (i)
	{
		case 0:
			return m_W;
			break;
		case 1:
			return m_X;
			break;
		case 2:
			return m_Y;
			break;
		case 3:
			return m_Z;
			break;
		default:
			//returning w component to avoid errors
			assert(false);
			return m_W;
			break;
	}
}

//----------------------------------------------------------------------------
albaQuaternion albaQuaternion::operator+(  albaQuaternion b)
{
	return albaQuaternion(m_W+b.m_W, m_X+b.m_X, m_Y+b.m_Y, m_Z+b.m_Z);
}

//----------------------------------------------------------------------------
albaQuaternion albaQuaternion::operator-(  albaQuaternion b)
{
	return albaQuaternion(m_W-b.m_W, m_X-b.m_X, m_Y-b.m_Y, m_Z-b.m_Z);
}

//----------------------------------------------------------------------------
 albaQuaternion albaQuaternion::operator*(  albaQuaternion b)
{
	return albaQuaternion( 	m_W*b.m_W - m_X*b.m_X - m_Y*b.m_Y - m_Z*b.m_Z,
						m_W*b.m_X + m_X*b.m_W + m_Y*b.m_Z - m_Z*b.m_Y,
						m_W*b.m_Y - m_X*b.m_Z + m_Y*b.m_W + m_Z*b.m_X,
						m_W*b.m_Z + m_X*b.m_Y - m_Y*b.m_X + m_Z*b.m_W );
}

 //----------------------------------------------------------------------------
 albaQuaternion& albaQuaternion::operator+=( albaQuaternion b)
{
	m_W += b.m_W;
	m_X += b.m_X;
	m_Y += b.m_Y;
	m_Z += b.m_Z;
	return *this;
}

//----------------------------------------------------------------------------
albaQuaternion& albaQuaternion::operator-=(  albaQuaternion b)
{
	m_W -= b.m_W;
	m_X -= b.m_X;
	m_Y -= b.m_Y;
	m_Z -= b.m_Z;
	return *this;
}

//----------------------------------------------------------------------------
albaQuaternion& albaQuaternion::operator*=( albaQuaternion b)
{
	Multiply(b);
	return *this;
}

//----------------------------------------------------------------------------
bool albaQuaternion::operator ==( double scalar)
{
	if ( m_W==scalar && m_X==0 && m_Y==0 && m_Z==0)
		return true;
	else
		return false;
}

//----------------------------------------------------------------------------
bool albaQuaternion::operator !=(double scalar)
{
	return (*this!=scalar );
}

//----------------------------------------------------------------------------
albaQuaternion albaQuaternion::operator*(double s)
{
	return albaQuaternion(m_W*s, m_X*s, m_Y*s, m_Z*s);
}

//----------------------------------------------------------------------------
albaQuaternion albaQuaternion::operator/( double s)
{
	double us=1.0/s;
	return albaQuaternion(m_W*us, m_X*us, m_Y*us, m_Z*us);
}

//----------------------------------------------------------------------------
albaQuaternion& albaQuaternion::operator*=( double s)
{
	Multiply(s);
	return *this;
}

//----------------------------------------------------------------------------
albaQuaternion& albaQuaternion::operator/=( double s)
{
	Divide(s);
	return *this;
}


