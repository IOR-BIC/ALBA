/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOBB.h,v $
  Language:  C++
  Date:      $Date: 2004-12-22 14:06:34 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafOBB_h
#define __mafOBB_h
#include "mafBase.h" 

#include "mafDefines.h"
#include "mafMTime.h"
#include "mafMatrix.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------


/** This class simply stores a Box boundary (i.e. an array of 6 doubles).
  This class stores a Box Boundary (i.e. array of 6 double), plus a Modified
  timestamp of the boundary to be used when re-computing.
  @todo
  - a Test program
*/
class MAF_EXPORT mafOBB : public mafBase
{
public:
  mafOBB();
  mafOBB(double source[6]);
  ~mafOBB();

  unsigned long int GetMTime() {return MTime.GetMTime();}
  void Modified() {this->MTime.Modified();}

  /**
    Return 1 if the two bounds are equivalent. Use the float overload to compare with
  VTK bounds, since VTK manages bounds as floats!*/
  int Equals(float bounds[6]);
  int Equals(double bounds[6]);
  int Equals(mafOBB &bounds);
  int Equals(mafOBB *bounds) {return this->Equals(*bounds);}


  /**
    Return true if the BBox is valid*/
  bool IsValid() {return (this->Bounds[0]<=this->Bounds[1] && \
    this->Bounds[2]<=this->Bounds[3] && \
    this->Bounds[4]<=this->Bounds[5]); \
  }

  /**
    Reset this object bounds to an invalid value [0,-1,0,-1,0,-1]*/
  void Reset();

  /**
    Copy another bounds from another object*/
  void DeepCopy(double bounds[6]);
  void DeepCopy(float bounds[6]);
  void DeepCopy(mafOBB *);
  void DeepCopy(mafOBB &source) {this->DeepCopy(&source);}
  void CopyTo(double target[6]);
  void CopyTo(float target[6]);

  /**
    Apply a transform to the internally stored matrix */
  //void ApplyTransform(mafMatrix &mat, mafOBB &newbounds);
  //void ApplyTransform(mafMatrix &mat) {this->ApplyTransform(mat,*this);}

  /**
    Apply the internally stored transform to the bounding box, recompute the
    internally stored bounding box. */
  void ApplyTransform();

  /**
    Apply the internally stored transform to the bounding box, recompute the
    bounding box and stores new BBOx in the given object. This version doesn't 
    change the internal representation but simply return the result in 'newbounds'*/
  void ApplyTransform(mafOBB &newbounds);

  /**
    Merge this objects's bounds with the given one. If one of the two bounds is
    invalid the other one is chosen.*/
  void MergeBounds(mafOBB &bounds);

  /**
    Return true if the point is inside the bounds*/
  bool IsInside(double point[3]);
  bool IsInside(float point[3]);
  bool IsInside(double x,double y,double z);

  /**
    This is a static function to merge two different space bounds. The two bounds
    are merged and the result is placed in the first argument.*/
  static void MergeBounds(double b1[6], double b2[6]);

  /** return dimensions of this box */
  void GetDimensions(float dims[3]);
  /** return dimensions of this box */
  void GetDimensions(double dims[3]);

  /** return center of the box */
  void GetCenter(float center[3]);
  /** return center of the box */
  void GetCenter(double center[3]);

  /** translate the box to the new center */
  void SetCenter(double center[3]);

  /** translate the box to the new center */
  void SetCenter(float center[3]);

  /** set dimensions of the box preserving its center */
  void SetDimensions(double dims[3]);

  /** set dimensions of the box preserving its center */
  void SetDimensions(float dims[3]);

  /** return the X dimension */
  double GetWidth();

  /** return the Y dimension */
  double GetHeight();

  /** return the Z dimension */
  double GetDepth();
  
  mafMatrix Matrix;
  double    Bounds[6];
  mafMTime  MTime;

};


//-------------------------------------------------------------------------
inline bool mafOBB::IsInside(double point[3])
{
  return this->IsInside(point[0],point[1],point[2]);
}

//-------------------------------------------------------------------------
inline bool mafOBB::IsInside(float point[3])
{
  return this->IsInside(point[0],point[1],point[2]);
}

//-------------------------------------------------------------------------
inline bool mafOBB::IsInside(double x,double y,double z)
{
  return ((x>=Bounds[0])&&(x<=Bounds[1])&& \
          (y>=Bounds[2])&&(y<=Bounds[3])&& \
          (z>=Bounds[4])&&(z<=Bounds[5]));
}

#endif 

