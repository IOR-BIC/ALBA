/*=========================================================================

Program: ALBA (Agile Library for Biomedical Applications)
Module: albaQuaternion
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaQuaternion_H__
#define __albaQuaternion_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaVect3d;
class albaMatrix;

/// A quaternion.
/// This quaternion class is generic and may be non-unit, however most
/// anticipated uses of quaternions are typically unit cases representing
/// a rotation 2*acos(w) about the axis (x,y,z).
class albaQuaternion
{
public:
		/** Default constructor */
		albaQuaternion();
		/** Constructor quaternion from real component w and imaginary x,y,z.*/
		albaQuaternion(double w, double x, double y, double z);
		/** Construct quaternion from angle - axis */
		albaQuaternion(double angle, albaVect3d &axis);
			/** Construct quaternion from rotation albaMatrix. */
		albaQuaternion( albaMatrix &matr);

		/** Convert quaternion to albaMatrix. */
		albaMatrix ToMatrix();

		/** Convert quaternion to angle - axis. */
		void AngleAxis(double &angle, albaVect3d &axis);

		/** Set quaternion to zero.*/
		void Zero();

		/** Set quaternion to identity. */
		void Identity();

		/** Add another quaternion to this quaternion.*/
		void Add( albaQuaternion &q);
		
		/** Subtract another quaternion from this quaternion.*/
		void Subtract( albaQuaternion &q);

		/** Multiply this quaternion by a scalar.*/
		void Multiply(double s);

		/** Divide this quaternion by a scalar. */
		void Divide(double s);

		/** Multiply this quaternion with another quaternion. */
		void Multiply( albaQuaternion &q);
		
		/** Multiply this quaternion with another quaternion and store result in parameter. */
		void Multiply( albaQuaternion &q, albaQuaternion &result) ;

		/** Dot product of two quaternions. */
		albaQuaternion Dot( albaQuaternion &q);
				
		/** Dot product of two quaternions writing result to parameter. */
		void Dot( albaQuaternion &q, albaQuaternion &result);

		/** Calculate conjugate of quaternion.*/
		albaQuaternion Conjugate();
		
		/** Calculate conjugate of quaternion and store result in parameter. */
		void Conjugate(albaQuaternion &result);

		/** Calculate and returns the length of quaternion */
		double GetLength();

		/** Calculate and returns the norm of quaternion.*/
		double GetNorm();

		/** Normalize the quaternion.*/
		void Normalize();

		/** return true if the quaternion is normalized */
		bool Normalized();

		/** Calculate inverse of quaternion */
		albaQuaternion Inverse();

		/** Calculate inverse of quaternion and store result in parameter. */
		void Inverse(albaQuaternion &result) ;

		/** Spherical linear interpolation */
		static albaQuaternion Slerp(albaQuaternion &a, albaQuaternion &b, double t);

		/** Equals operator */
		bool operator ==( albaQuaternion &other) ;

		/** Not equals operator*/
		bool operator !=( albaQuaternion &other) ;

		double& operator [](int i);

    albaQuaternion operator+(  albaQuaternion b);
		albaQuaternion operator-(  albaQuaternion b);
		albaQuaternion operator*(  albaQuaternion b);
		albaQuaternion& operator+=(  albaQuaternion b);
		albaQuaternion& operator-=(  albaQuaternion b);
		albaQuaternion& operator*=( const albaQuaternion b);

		bool operator ==( double scalar);
		bool operator !=( double scalar);

	
		albaQuaternion operator-();
		albaQuaternion operator*( double s);
		albaQuaternion operator/( double s);
		albaQuaternion& operator*=( double s);
		albaQuaternion& operator/=(double s);

protected:
		double m_W;        ///< w component of quaternion
		double m_X;        ///< x component of quaternion
		double m_Y;        ///< y component of quaternion
		double m_Z;        ///< z component of quaternion
};



#endif