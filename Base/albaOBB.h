/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOBB
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaOBB_h
#define __albaOBB_h
#include "albaBase.h" 
#include "albaTimeStamped.h"

#include "albaDefines.h"
#include "albaMTime.h"
#include "albaMatrix.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------


/** This class simply stores a Box boundary (i.e. an array of 6 doubles).
  This class stores a Box Boundary (i.e. array of 6 double), plus a Modified
  timestamp of the boundary to be used when re-computing.
  @todo
  - a Test program 
*/
class ALBA_EXPORT albaOBB : public albaBase, public albaTimeStamped
{
public:
  albaOBB();
  albaOBB(double source[6]);
  ~albaOBB();

  static albaOBB *New() {return new albaOBB;}

  /**
    Return 1 if the two bounds are equivalent. Use the float overload to compare with
  VTK bounds, since VTK manages bounds as floats!*/
  bool Equals(float bounds[6]) const;
  bool Equals(double bounds[6]) const;
  bool Equals(albaOBB &bounds) const;
  bool Equals(albaOBB *bounds) {return Equals(*bounds);}


  /**
    Return true if the BBox is valid*/
  bool IsValid() const {return (m_Bounds[0]<=m_Bounds[1] && \
    m_Bounds[2]<=m_Bounds[3] && \
    m_Bounds[4]<=m_Bounds[5]); \
  }

  /**
    Reset this object bounds to an invalid value [0,-1,0,-1,0,-1]*/
  void Reset();

  /**
    Copy another bounds from another object*/
  void DeepCopy(double bounds[6]);
  void DeepCopy(float bounds[6]);
  void DeepCopy(albaOBB *);
  void DeepCopy(albaOBB &source) {DeepCopy(&source);}
  void CopyTo(double target[6]) const;
  void CopyTo(float target[6]) const;

  /**
    Apply the internally stored transform to the bounding box, recompute the
    internally stored bounding box. */
  void ApplyTransform() {ApplyTransform(m_Matrix,*this);}

  /**
    Apply the internally stored transform to the bounding box, recompute the
    bounding box and stores new BBOx in the given object. This version doesn't 
    change the internal representation but simply return the result in 'newbounds'*/
  void ApplyTransform(albaOBB &newbounds) {ApplyTransform(m_Matrix,newbounds);}

  /** 
    Apply the given transform to the internal bounds (do not concatenate to
    the internal Matrix). */
  static void ApplyTransform(const albaMatrix &mat, albaOBB &newbounds);
  void ApplyTransform(const albaMatrix &mat) {ApplyTransform(mat,*this);}

  /**
    Merge this objects's bounds with the given one. If one of the two bounds is
    invalid the other one is chosen.*/
  void MergeBounds(albaOBB &bounds);

  /**
    Return true if the point is inside the bounds*/
  bool IsInside(double point[3]) const;
  bool IsInside(float point[3]) const;
  bool IsInside(double x,double y,double z) const;

  /**
    This is a static function to merge two different space bounds. The two bounds
    are merged and the result is placed in the first argument.*/
  static void MergeBounds(double b1[6], double b2[6]);

  /** return dimensions of this box */
  void GetDimensions(float dims[3]) const;
  /** return dimensions of this box */
  void GetDimensions(double dims[3]) const;

  /** return center of the box */
  void GetCenter(float center[3]) const;
  /** return center of the box */
  void GetCenter(double center[3]) const;

  /** translate the box to the new center */
  void SetCenter(double center[3]);

  /** translate the box to the new center */
  void SetCenter(float center[3]);

  /** set dimensions of the box preserving its center */
  void SetDimensions(double dims[3]);

  /** set dimensions of the box preserving its center */
  void SetDimensions(float dims[3]);

  /** return the X dimension */
  double GetWidth() const;

  /** return the Y dimension */
  double GetHeight() const;

  /** return the Z dimension */
  double GetDepth() const;

  /** set orientation of the internal matrix */
  void SetOrientation(double rx, double ry, double rz);

  /** extract orientation from internal matrix */
  void GetOrientation(double rxyz[3]);

  /** dump the bounding box */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  // member variables are left public to simplify access

  albaMatrix m_Matrix; ///< the pose matrix of the OBB
  double    m_Bounds[6]; ///< the bounding box
};

//-------------------------------------------------------------------------
inline bool albaOBB::IsInside(double point[3]) const
//-------------------------------------------------------------------------
{
  return IsInside(point[0],point[1],point[2]);
}

//-------------------------------------------------------------------------
inline bool albaOBB::IsInside(float point[3]) const
//-------------------------------------------------------------------------
{
  return IsInside(point[0],point[1],point[2]);
}

//-------------------------------------------------------------------------
inline bool albaOBB::IsInside(double x,double y,double z) const
//-------------------------------------------------------------------------
{
  return ((x>=m_Bounds[0])&&(x<=m_Bounds[1])&& \
          (y>=m_Bounds[2])&&(y<=m_Bounds[3])&& \
          (z>=m_Bounds[4])&&(z<=m_Bounds[5]));
}

#endif 

