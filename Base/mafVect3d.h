/*=========================================================================

 Program: MAF2
 Module: mafVect3d
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVect3d_H__
#define __mafVect3d_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafVect3d.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: mafVect3d.
  High optimized class for 3-Dimensional vectors management. 
*/
class MAF_EXPORT mafVect3d
{
public:

  /** Default constructor, init to zero */
  mafVect3d();

  /** Constructor, sets the vector values */
  mafVect3d(double x, double y, double z);

  /** Constructor, sets the vector values */
  mafVect3d(double *values);

  /** Return a vector whit the abs of each element*/
  mafVect3d Abs(void);

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
  mafVect3d Normal(void);

  /** Return the distance between the vectors*/
  double Distance(mafVect3d &vector);

  /** Return the distance2 (quadratic distance) between the vectors*/
  double Distance2(mafVect3d &vector);

  /** Return the dot product between the vectors */
  double Dot(mafVect3d &vector);

  /** Return the cross product between the vectors */
  mafVect3d Cross(mafVect3d &vector);

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
  int operator==(mafVect3d &vect);

  /** Operator: Return true if the vectors are equals */
  int operator==(double *vect);

  /** Operator: Return the sum of the vectors*/
  mafVect3d operator+(mafVect3d &vect);
  
  /** Operator: Return the sum of the vectors*/
  mafVect3d operator+(double *vect);

  /** Operator: Return the difference of the vectors*/
  mafVect3d operator-(mafVect3d &vect);

  /** Operator: Return the difference of the vectors*/
  mafVect3d operator-(double *vect);
  
  /** Operator: Return the scalar multiply*/
  mafVect3d operator*(double num);

  /** Operator: Return the scalar division*/
  mafVect3d operator/(double num);

  /** Operator: Sets the values to the sum between the vectors*/
  mafVect3d &operator+=(mafVect3d &vect);

  /** Operator: Sets the values to the sum between the vectors*/
  mafVect3d &operator+=(double *vect);

  /** Operator: Sets the values to the difference between the vectors*/
  mafVect3d &operator-=(mafVect3d &vect);

  /** Operator: Sets the values to the difference between the vectors*/
  mafVect3d &operator-=(double *vect);

  /** Operator: Sets the values to the product with the scalar*/
  mafVect3d &operator*=(double val);

  /** Operator: Sets the values to the division with the scalar*/
  mafVect3d &operator/=(double val);

  /** Operator: Gets the vect values with *double style
      ie: vect[0]==vect.GetX() */
  double& operator[](int pos);

  /** Returns the angle between two vectors */
  double AngleBetweenVectors( mafVect3d &vect );

private:

  double m_X, m_Y, m_Z;
};
#endif
