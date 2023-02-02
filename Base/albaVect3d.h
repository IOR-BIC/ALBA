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

#ifndef __albaVect3d_H__
#define __albaVect3d_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVect3d.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: albaVect3d.
  High optimized class for 3-Dimensional vectors management. 
*/
class ALBA_EXPORT albaVect3d
{
public:

  /** Default constructor, init to zero */
  albaVect3d();

  /** Constructor, sets the vector values */
  albaVect3d(double x, double y, double z);

  /** Constructor, sets the vector values */
  albaVect3d(double *values);

  /** Return a vector whit the abs of each element*/
  albaVect3d Abs(void);

  /** Sets the vector to zero*/
  void Setzero(void);

  /** Sets the vector values*/
  void SetValues(double *values);

  /** Sets the vector values*/
  void SetValues(double x, double y, double z);

  /**  Returns the magnitude of the vector */
  double Magnitude(void);

  /** Normalize the components of the vector */
  void  Normalize(void);

  /** Return the normal of the vector (the original vector is not modified() */
  albaVect3d Normal(void);

  /** Return the distance between the vectors*/
  double Distance(albaVect3d &vector);

  /** Return the distance2 (quadratic distance) between the vectors*/
  double Distance2(albaVect3d &vector);

  /** Return the dot product between the vectors */
  double Dot(albaVect3d &vector);

  /** Return the cross product between the vectors */
  albaVect3d Cross(albaVect3d &vector);

  /** Return the Value of the X-element*/
  inline double GetX(){return m_X;};
  
  /** Return the Value of the Y-element*/
  inline double GetY(){return m_Y;};

  /** Return the Value of the Z-element*/
  inline double GetZ(){return m_Z;};

  /**  the Value of the X-element*/
  inline void SetX(double x){m_X=x;};

  /**  the Value of the Y-element*/
  inline void SetY(double y){m_Y=y;};

  /**  the Value of the Z-element*/
  inline void SetZ(double z){m_Z=z;};

  /** Return a pointer to the vector of double */
  inline double* GetVect(){return (double*)this;};

  /** Operator: Return true if the vectors are equals */
  int operator==(albaVect3d &vect);

  /** Operator: Return true if the vectors are equals */
  int operator==(double *vect);

  /** Operator: Return the sum of the vectors*/
  albaVect3d operator+(albaVect3d &vect);
  
  /** Operator: Return the sum of the vectors*/
  albaVect3d operator+(double *vect);

  /** Operator: Return the difference of the vectors*/
  albaVect3d operator-(albaVect3d &vect);

  /** Operator: Return the difference of the vectors*/
  albaVect3d operator-(double *vect);
  
  /** Operator: Return the scalar multiply*/
  albaVect3d operator*(double num);

  /** Operator: Return the scalar division*/
  albaVect3d operator/(double num);

  /** Operator: Sets the values to the sum between the vectors*/
  albaVect3d &operator+=(albaVect3d &vect);

  /** Operator: Sets the values to the sum between the vectors*/
  albaVect3d &operator+=(double *vect);

  /** Operator: Sets the values to the difference between the vectors*/
  albaVect3d &operator-=(albaVect3d &vect);

  /** Operator: Sets the values to the difference between the vectors*/
  albaVect3d &operator-=(double *vect);

  /** Operator: Sets the values to the product with the scalar*/
  albaVect3d &operator*=(double val);

  /** Operator: Sets the values to the division with the scalar*/
  albaVect3d &operator/=(double val);

  /** Operator: Gets the vect values with *double style
      ie: vect[0]==vect.GetX() */
  double& operator[](int pos);

  /** Returns the angle between two vectors */
  double AngleBetweenVectors( albaVect3d &vect );

	/** Do a spherical linear interpolation of two vectors*/
	static albaVect3d Slerp(albaVect3d start, albaVect3d end, double percent);

private:

  double m_X, m_Y, m_Z;
};
#endif
