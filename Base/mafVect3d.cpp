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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDecl.h"
#include "mafVect3d.h"
#include <math.h>



//----------------------------------------------------------------------------
mafVect3d::mafVect3d()
//----------------------------------------------------------------------------
{
  //sets each element to zero for default constructor
  m_X=m_Y=m_Z=0.0;
}

//----------------------------------------------------------------------------
mafVect3d::mafVect3d(double x, double y, double z)
//----------------------------------------------------------------------------
{
  //specified constructor, sets each element to the specified value
  m_X = x; m_Y = y; m_Z = z;
}

//----------------------------------------------------------------------------
mafVect3d::mafVect3d( double *values )
//----------------------------------------------------------------------------
{
  //specified constructor, sets each element to the specified value
  //from the input array
   m_X = values[0]; m_Y = values[1]; m_Z = values[2];
}

//----------------------------------------------------------------------------
mafVect3d mafVect3d::Abs(void)
//----------------------------------------------------------------------------
{
  //Generate a new vector whit the absolute
  //value of each original element
  return mafVect3d (fabsf(m_X),fabsf(m_Y),fabsf(m_Z));
}

//----------------------------------------------------------------------------
void mafVect3d::Setzero(void)
//----------------------------------------------------------------------------
{
  //sets each element to zero
  m_X=m_Y=m_Z=0.0;
}

//----------------------------------------------------------------------------
int mafVect3d::operator==(mafVect3d &vect)
//----------------------------------------------------------------------------
{
  //return true if each element is equal to each other pair
  return ( (vect.m_X==m_X) && (vect.m_Y==m_Y) && (vect.m_Z==m_Z) );
}

//----------------------------------------------------------------------------
int mafVect3d::operator==( double *vect )
//----------------------------------------------------------------------------
{
  return ( (vect[0]==m_X) && (vect[1]==m_Y) && (vect[2]==m_Z) );
}

//----------------------------------------------------------------------------
mafVect3d mafVect3d::operator+(mafVect3d &vect)
//----------------------------------------------------------------------------
{
  //Generating a new vector witch the sum of each element 
  return mafVect3d(vect.m_X + m_X, vect.m_Y + m_Y, vect.m_Z + m_Z);
}

//----------------------------------------------------------------------------
mafVect3d mafVect3d::operator+( double *vect )
//----------------------------------------------------------------------------
{
  //Generating a new vector witch the sum of each element 
  return mafVect3d(vect[0] + m_X, vect[1] + m_Y, vect[2] + m_Z);
}

//----------------------------------------------------------------------------
mafVect3d &mafVect3d::operator+=(mafVect3d &vect)
//----------------------------------------------------------------------------
{
  //assign at each value the sum whit the correspondent element
  m_X+=vect.m_X;
  m_Y+=vect.m_Y;
  m_Z+=vect.m_Z;
  //Return a pointer to this object (for concatenating)
  //i.e. a+=(b+=c)
  return *this;
}

mafVect3d &mafVect3d::operator+=(double *vect)
{
  //assign at each value the sum whit the correspondent element
  m_X+=vect[0];
  m_Y+=vect[1];
  m_Z+=vect[2];
  //Return a pointer to this object (for concatenating)
  //i.e. a+=(b+=c)
  return *this;
}

//----------------------------------------------------------------------------
mafVect3d &mafVect3d::operator-=(mafVect3d &vect)
//----------------------------------------------------------------------------
{
  //assign at each value the difference whit the correspondent element
  m_X-=vect.m_X;
  m_Y-=vect.m_Y;
  m_Z-=vect.m_Z;
  //Return a pointer to this object (for concatenating)
  //i.e. a-=(b-=c)
  return *this;
}

//----------------------------------------------------------------------------
mafVect3d & mafVect3d::operator-=( double *vect )
//----------------------------------------------------------------------------
{
  //assign at each value the difference whit the correspondent element
  m_X-=vect[0];
  m_Y-=vect[1];
  m_Z-=vect[2];
  //Return a pointer to this object (for concatenating)
  //i.e. a-=(b-=c)
  return *this;
}

//----------------------------------------------------------------------------
mafVect3d &mafVect3d::operator*=(double val)
//----------------------------------------------------------------------------
{
  //assign at each value the product whit the correspondent element
  m_X*=val;
  m_Y*=val;
  m_Z*=val;
  //Return a pointer to this object (for concatenating)
  //i.e. a*=(b*=c)
  return *this;
}
//----------------------------------------------------------------------------
mafVect3d &mafVect3d::operator/=(double val)
//----------------------------------------------------------------------------
{
  val=1.0/val;
  //assign at each value the sum whit the correspondent element
  m_X*=val;
  m_Y*=val;
  m_Z*=val;
  //Return a pointer to this object (for concatenating)
  //i.e. a+=(b+=c)
  return *this;
}

//----------------------------------------------------------------------------
mafVect3d mafVect3d::operator-(mafVect3d &vect)
//----------------------------------------------------------------------------
{
  //Generating a new vector witch the difference of each element 
  return mafVect3d(m_X - vect.m_X, m_Y - vect.m_Y, m_Z - vect.m_Z);
}

//----------------------------------------------------------------------------
mafVect3d mafVect3d::operator-( double *vect )
//----------------------------------------------------------------------------
{
  //Generating a new vector witch the difference of each element 
  return mafVect3d(m_X - vect[0], m_Y - vect[1], m_Z - vect[2]);
}

//----------------------------------------------------------------------------
mafVect3d mafVect3d::operator*(double num)
//----------------------------------------------------------------------------
{
  //Generating a new vector witch the multiplication of each element 
  return mafVect3d(m_X * num, m_Y * num, m_Z * num);
}


//----------------------------------------------------------------------------
mafVect3d mafVect3d::operator/(double num)
//----------------------------------------------------------------------------
{
  //Generating a new vector witch the division of each element     
  num=1.0/num;
  return mafVect3d(m_X * num, m_Y * num, m_Z * num);
}


//----------------------------------------------------------------------------
mafVect3d mafVect3d::Cross(mafVect3d &vector)
//----------------------------------------------------------------------------
{
  //Generating a new vector witch the cross product of the vectors
  return mafVect3d(m_Y * vector.m_Z - m_Z * vector.m_Y, m_Z * vector.m_X - m_X * vector.m_Z, m_X * vector.m_Y - m_Y * vector.m_X);
}


//----------------------------------------------------------------------------
double mafVect3d::Magnitude(void)
//----------------------------------------------------------------------------
{
  //calculating the magnitude of the vector
  return (double)sqrt( (m_X *m_X) + (m_Y * m_Y) + (m_Z * m_Z) );
}


//----------------------------------------------------------------------------
void mafVect3d::Normalize(void)
//----------------------------------------------------------------------------
{
  //normalize this vector
  double magnitude = Magnitude();
  //Avoid division by zero
  if (magnitude != 0)
  {
    // Divide the X value of our normal by it's magnitude
    m_X /= magnitude;	
    // Divide the Y value of our normal by it's magnitude
    m_Y /= magnitude;
    // Divide the Z value of our normal by it's magnitude
    m_Z /= magnitude;
  }
}

//----------------------------------------------------------------------------
mafVect3d mafVect3d::Normal(void)
//----------------------------------------------------------------------------
{
  //Generating a new vector witch the normalized values of each element   
  double magnitude = Magnitude();
  //Avoid division by zero
  if (magnitude != 0)
  {
    // Divide the X, Y, Z values of our vector by it's magnitude
    return mafVect3d(m_X/magnitude,m_Y/magnitude,m_Z/magnitude); 
  }
  else return mafVect3d(0.0,0.0,0.0);
}

//----------------------------------------------------------------------------
double mafVect3d::Dot(mafVect3d &vector)
//----------------------------------------------------------------------------
{
  //returns the dot product of the vector
  return m_X * vector.m_X + m_Y * vector.m_Y + m_Z * vector.m_Z;
}


//----------------------------------------------------------------------------
double mafVect3d::Distance(mafVect3d &vect)
//----------------------------------------------------------------------------
{
  //calculating the distance between two vector
  return sqrt( ((m_X-vect.m_X) * (m_X-vect.m_X)) +
    ((m_Y-vect.m_Y) * (m_Y-vect.m_Y)) +
    ((m_Z-vect.m_Z) * (m_Z-vect.m_Z)) );
}

//----------------------------------------------------------------------------
double mafVect3d::Distance2(mafVect3d &vect)
//----------------------------------------------------------------------------
{
  //calculating the quadratic distance between two vector
  return ((m_X-vect.m_X) * (m_X-vect.m_X)) +
    ((m_Y-vect.m_Y) * (m_Y-vect.m_Y)) +
    ((m_Z-vect.m_Z) * (m_Z-vect.m_Z)) ;
}

//----------------------------------------------------------------------------
void mafVect3d::SetValues( double *values )
//----------------------------------------------------------------------------
{
  //update the values
  m_X = values[0]; m_Y = values[1]; m_Z = values[2];
}

//----------------------------------------------------------------------------
void mafVect3d::SetValues( double x, double y, double z )
//----------------------------------------------------------------------------
{
  //update the values
  m_X = x; m_Y = y; m_Z = z;
}
 
//-----------------------------------------------------------------------
double mafVect3d::AngleBetweenVectors( mafVect3d &vect )
//-----------------------------------------------------------------------
{
  // Get the dot product of the vectors
  double dotProduct = this->Dot(vect);

  // Get the product of both of the vectors magnitudes
  double vectorsMagnitude = this->Magnitude() * vect.Magnitude() ;

  // Get the angle in radians between the 2 vectors
  double angle = acos( dotProduct / vectorsMagnitude );


  // Here we make sure that the angle is not a -1.#IND0000000 number, which means indefinite
  if(_isnan(angle))
    return 0;

  // Return the angle in radians
  return( angle );
}

//----------------------------------------------------------------------------
double & mafVect3d::operator[]( int pos )
//----------------------------------------------------------------------------
{
  return *((&this->m_X + pos));
}


