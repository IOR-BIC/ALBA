/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medLogicWithManagers.h,v $
Language:  C++
Date:      $Date: 2012-04-06 08:19:51 $
Version:   $Revision: 1.2.2.3 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
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
  inline double* GetVect(){return &m_X;};

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
