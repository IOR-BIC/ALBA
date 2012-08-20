/*=========================================================================

 Program: MAF2Medical
 Module: medVect3dTest
 Authors: Grazia Di Cosmo
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medVect3dTest.h"
#include "medVect3d.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void medVect3dTest::TestSetGetVector()
//----------------------------------------------------------------------------
{
  double vector[3];
  medVect3d vect3d;
  double x, y, z;  
  vector[0] = 4; 
  vector[1] = 5;
  vector[2] = 6;
  x = 1;
  y = 2;
  z = 3;

  // Test Construct1 med3dVector and get the vector created
  medVect3d tmpVect3d(3,4,5);
  double *tmp;
  tmp = tmpVect3d.GetVect();
  CPPUNIT_ASSERT(  tmp[0] == 3 &&  tmp[1] == 4 && tmp[2] == 5 );

  // Test Construct2 med3dVector and get the vector created
  double tmp1[3];
  double *tmp2;
  tmp1[0] = 9;
  tmp1[1] = 8;
  tmp1[2] = 7;
  medVect3d tmp1Vect3d(tmp1);
  tmp2 = tmp1Vect3d.GetVect();
  CPPUNIT_ASSERT( tmp2[0] == tmp1[0] &&  tmp2[1] == tmp1[1] && tmp2[2] == tmp1[2] );

  // Test setting a vector and get the vector saved  
  double *vect3dPoint;
  vect3d.SetValues(vector);
  vect3dPoint = vect3d.GetVect();
  CPPUNIT_ASSERT(  vect3dPoint[0] == vector[0] &&  vect3dPoint[1] == vector[1] && vect3dPoint[2] == vector[2] );

  // Test setting each element of a vector and get each element saved
  double newVector[3];
  vect3d.SetValues(x, y, z);
  newVector[0]=vect3d.GetX();
  newVector[1]=vect3d.GetY();
  newVector[2]=vect3d.GetZ();
  CPPUNIT_ASSERT(  newVector[0] == x &&  newVector[1] == y && newVector[2] == z );
  
  // Test setting new first and second element of a vector and get each element saved
  double newX, newY, newZ;
  newX = 0;
  newY = 12;
  newZ = 22;
  vect3d.SetX(newX);
  vect3d.SetY(newY);
  newVector[0]=vect3d.GetX();
  newVector[1]=vect3d.GetY();
  CPPUNIT_ASSERT(  newVector[0] == newX &&  newVector[1] == newY && newVector[2] == z );

  // Test setting the third component of a vector an get the element saved
  vect3d.SetZ(newZ);
  newVector[2]=vect3d.GetZ();
  CPPUNIT_ASSERT( newVector[2] == newZ );

  // Test setting all component of a vector to zero
  vect3d.Setzero();
  CPPUNIT_ASSERT(  vect3d.GetX() == 0 && vect3d.GetY() == 0 && vect3d.GetZ() == 0 );
	
}
//----------------------------------------------------------------------------
void medVect3dTest::TestOperators()
//----------------------------------------------------------------------------
{
  double vector1[3];
  double vector2[3];
  vector1[0] = 90;
  vector1[1] = 40;
  vector1[2] = 20;
  vector2[0] = 80;
  vector2[1] = 23;
  vector2[2] = 30;

  // Construct vectors as med3dVect
  medVect3d vect3d1(vector1);
  medVect3d vect3d2(vector2);

  // Test + operator, we expect sum between two vectors
  double sumVector[3];
  medVect3d vectSum;
  sumVector[0] = vector1[0] + vector2[0];
  sumVector[1] = vector1[1] + vector2[1];
  sumVector[2] = vector1[2] + vector2[2];
  vectSum = vect3d1 + vect3d2;
  CPPUNIT_ASSERT(  vectSum.GetX() == sumVector[0] && vectSum.GetY() == sumVector[1] && vectSum.GetZ() == sumVector[2] );
  
  // Test - operator, we expect the difference between two vectors
  double diffVector[3];
  medVect3d vectDiff;
  diffVector[0] = vector1[0] - vector2[0];
  diffVector[1] = vector1[1] - vector2[1];
  diffVector[2] = vector1[2] - vector2[2];
  vectDiff = vect3d1 - vect3d2;
  CPPUNIT_ASSERT(  vectDiff.GetX() == diffVector[0] && vectDiff.GetY() == diffVector[1] && vectDiff.GetZ() == diffVector[2] );

  // Test * operator, we expect the product between a vectors and a scalar
  double multVector[3];
  medVect3d vectMult;  
  double scalar = 3;
  multVector[0] = vector1[0] * scalar;
  multVector[1] = vector1[1] * scalar;
  multVector[2] = vector1[2] * scalar;
  vectMult = vect3d1 * scalar;
  CPPUNIT_ASSERT(  vectMult.GetX() == multVector[0] && vectMult.GetY() == multVector[1] && vectMult.GetZ() == multVector[2] );

  // Test / operator, we expect the division between a vectors and a scalar
  double divVector[3];
  medVect3d vectDiv;
  divVector[0] = vector1[0] * (1.0/scalar);
  divVector[1] = vector1[1] * (1.0/scalar);
  divVector[2] = vector1[2] * (1.0/scalar);
  vectDiv = vect3d1 / scalar;
  CPPUNIT_ASSERT(  vectDiv.GetX() == divVector[0] && vectDiv.GetY() == divVector[1] && vectDiv.GetZ() == divVector[2] );

  // Test += operator, we expect the operator sets the values to the sum between the vectors
  sumVector[0] += vector1[0];
  sumVector[1] += vector1[1];
  sumVector[2] += vector1[2];
  vectSum += vect3d1;
  CPPUNIT_ASSERT(  vectSum.GetX() == sumVector[0] && vectSum.GetY() == sumVector[1] && vectSum.GetZ() == sumVector[2] );

  // Test -= operator, we expect the operator sets the values to the difference between the vectors
  diffVector[0] -= vector1[0];
  diffVector[1] -= vector1[1];
  diffVector[2] -= vector1[2];
  vectDiff -= vect3d1;
  CPPUNIT_ASSERT(  vectDiff.GetX() == diffVector[0] && vectDiff.GetY() == diffVector[1] && vectDiff.GetZ() == diffVector[2] );

  // Test *= operator, we expect the operator sets the values to the product with the scalar
  multVector[0] *= scalar;
  multVector[1] *= scalar;
  multVector[2] *= scalar;
  vectMult *= scalar;
  CPPUNIT_ASSERT(  vectMult.GetX() == multVector[0] && vectMult.GetY() == multVector[1] && vectMult.GetZ() == multVector[2] );

  // Test /= operator, we expect the operator sets the values to the division with the scalar
  divVector[0] *= (1.0/scalar);
  divVector[1] *= (1.0/scalar);
  divVector[2] *= (1.0/scalar);
  vectDiv /= scalar;
  CPPUNIT_ASSERT(  vectDiv.GetX() == divVector[0] && vectDiv.GetY() == divVector[1] && vectDiv.GetZ() == divVector[2] );

  // Test == operator, we expect the operator return if the vectors are equals or not
  int value;
  if (  vector1[0] == vector2[0] && vector1[1] == vector2[1] &&  vector1[2] == vector2[2] )
    value = true;
  else
    value = false;
  int valueVect = (vect3d1 == vect3d2);
  CPPUNIT_ASSERT(  value == valueVect );

}

//----------------------------------------------------------------------------
void medVect3dTest::TestOperation2Vector()
//----------------------------------------------------------------------------
{
  double vector1[3];
  double vector2[3];
  vector1[0] = 90;
  vector1[1] = 40;
  vector1[2] = -20;
  vector2[0] = 80;
  vector2[1] = -23;
  vector2[2] = 30;

  // Construct vectors as med3dVect
  medVect3d vect3d1(vector1);
  medVect3d vect3d2(vector2);

  // Test Absolute, calculate the absolute value of a vector
  double absVect[3];
  medVect3d vectAbs;
  absVect[0]= fabsf(vector1[0]);
  absVect[1]= fabsf(vector1[1]);
  absVect[2]= fabsf(vector1[2]);
  vectAbs = vect3d1.Abs();
  CPPUNIT_ASSERT(  vectAbs.GetX() == absVect[0] && vectAbs.GetY() == absVect[1] && vectAbs.GetZ() == absVect[2] );

  // Test Magnitude of a vector, calculate the magnitude of a vector
  double magnitudeVect; 
  double vectMagnitude;
  magnitudeVect = (double)sqrt( (vector1[0] *vector1[0]) + (vector1[1] * vector1[1]) + (vector1[2] * vector1[2]) );
  vectMagnitude = vect3d1.Magnitude();
  CPPUNIT_ASSERT( vectMagnitude == magnitudeVect );

  // Test Normalize a vector, return the vector normalized
  // we avoid division by zero
  if (magnitudeVect != 0)
  {
    vector1[0] /= magnitudeVect;	
    vector1[1] /= magnitudeVect;
    vector1[2] /= magnitudeVect;
  }
  vect3d1.Normalize();
  CPPUNIT_ASSERT(  vect3d1.GetX() == vector1[0] && vect3d1.GetY() == vector1[1] && vect3d1.GetZ() == vector1[2] );

  // Test Normal, calculate the normal of the vector as a new vector
  double normalVector[3];
  medVect3d vectNormal;
  double magnitudeVect2;
  magnitudeVect2 = (double)sqrt( (vector2[0] *vector2[0]) + (vector2[1] * vector2[1]) + (vector2[2] * vector2[2]) );
  if (magnitudeVect2 != 0)
  {
    normalVector[0] =  vector2[0]/magnitudeVect2;
    normalVector[1]  = vector2[1]/magnitudeVect2;
    normalVector[2]  = vector2[2]/magnitudeVect2;
  }
  vectNormal = vect3d2.Normal();
  CPPUNIT_ASSERT(  vectNormal.GetX() == normalVector[0] && vectNormal.GetY() == normalVector[1] && vectNormal.GetZ() == normalVector[2] );

  // Test Distance, calculate the distance between two vectors
  double distVect, vectDist;
  distVect = sqrt( ((vector1[0]-vector2[0]) * (vector1[0]-vector2[0])) + ((vector1[1]-vector2[1]) * (vector1[1]-vector2[1])) + ((vector1[2]-vector2[2]) * (vector1[2]-vector2[2])) );
  vectDist = vect3d1.Distance(vect3d2);
  CPPUNIT_ASSERT( vectDist == distVect );
  
  // Test Distance2, calculate the quadratic distance between two vectors
  distVect = ((vector1[0]-vector2[0]) * (vector1[0]-vector2[0])) + ((vector1[1]-vector2[1]) * (vector1[1]-vector2[1])) + ((vector1[2]-vector2[2]) * (vector1[2]-vector2[2]));
  vectDist = vect3d1.Distance2(vect3d2);
  CPPUNIT_ASSERT( vectDist == distVect );

  // Test Dot, calculate the dot product of the vectors
  double dotVect, vectDot;
  dotVect = vector1[0] * vector2[0] + vector1[1] * vector2[1] + vector1[2] * vector2[2];
  vectDot = vect3d1.Dot(vect3d2);
  CPPUNIT_ASSERT( vectDot == dotVect );

  // Test Cross, calculate the cross product between two vectors
  double crossVector[3];  
  medVect3d vectCross;
  crossVector[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
  crossVector[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2];
  crossVector[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];
  vectCross = vect3d1.Cross(vect3d2);
  CPPUNIT_ASSERT(  vectCross.GetX() == crossVector[0] && vectCross.GetY() == crossVector[1] && vectCross.GetZ() == crossVector[2] );

  // Test AngleBetweenVectors, calculate the angle between two vectors 
  double angleVect, vectAngle, magnVect1, magnVect2, vectorsMagnitude;
  magnVect1 = (double)sqrt( (vector1[0] *vector1[0]) + (vector1[1] * vector1[1]) + (vector1[2] * vector1[2]) );
  magnVect2 = (double)sqrt( (vector2[0] *vector2[0]) + (vector2[1] * vector2[1]) + (vector2[2] * vector2[2]) );
  vectorsMagnitude = magnVect1 * magnVect2;
  angleVect = acos( dotVect / vectorsMagnitude );
  vectAngle = vect3d1.AngleBetweenVectors(vect3d2);
  CPPUNIT_ASSERT( vectAngle == angleVect );

}