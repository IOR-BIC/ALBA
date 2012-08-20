/*=========================================================================

 Program: MAF2Medical
 Module: medVect3d
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medVect3d_H__
#define __medVect3d_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "medVect3d.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: medVect3d.
  Class for handle the high level logic of a medical application.
*/
class MED_COMMON_EXPORT medVect3d
{
public:

  /** Default constructor, init to zero */
  medVect3d();

  /** Constructor, sets the vector values */
  medVect3d(double x, double y, double z);

  /** Constructor, sets the vector values */
  medVect3d(double *values);

  /** Return a vector whit the abs of each element*/
  medVect3d Abs(void);

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
  medVect3d Normal(void);

  /** Return the distance between the vectors*/
  double Distance(medVect3d vector);

  /** Return the distance2 (quadratic distance) between the vectors*/
  double Distance2(medVect3d vector);

  /** Return the dot product between the vectors */
  double Dot(medVect3d vector);

  /** Return the cross product between the vectors */
  medVect3d Cross(medVect3d vector);

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
  int operator==(medVect3d vect);
  
  /** Operator: Return the sum of the vectors*/
  medVect3d operator+(medVect3d vect);
  
  /** Operator: Return the difference of the vectors*/
  medVect3d operator-(medVect3d vect);
  
  /** Operator: Return the scalar multiply*/
  medVect3d operator*(double num);

  /** Operator: Return the scalar division*/
  medVect3d operator/(double num);

  /** Operator: Sets the values to the sum between the vectors*/
  medVect3d operator+=(medVect3d vect);

  /** Operator: Sets the values to the difference between the vectors*/
  medVect3d operator-=(medVect3d vect);

  /** Operator: Sets the values to the product with the scalar*/
  medVect3d operator*=(double val);

  /** Operator: Sets the values to the division with the scalar*/
  medVect3d operator/=(double val);

  /** Returns the angle between two vectors */
  double AngleBetweenVectors( medVect3d vect );

private:

  double m_X, m_Y, m_Z;
};
#endif
