/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformBase
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaTransformBase_h
#define __albaTransformBase_h

#include "albaReferenceCounted.h"
#include "albaMTime.h"
#include "albaMutexLock.h"
#include "albaMatrix.h"
#include "albaSmartPointer.h"
#include "albaEventBroadcaster.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class vtkLinearTransform;
class vtkALBAToLinearTransform;

#ifdef ALBA_EXPORTS
template class ALBA_EXPORT albaAutoPointer<albaMatrix>;
#endif

/**  Superclass for Homogeneous transformations.
  albaTransformBase is the superclass for ALBA geometric, and currently homogeneous
  only, transformations. The idea behind a albaTransformBase is the Update() method
  should always be called to update the output, which is a albaMatrix internally stored.
  @sa albaTransform
  @todo
  - change SetTimeStamp to apply a Modified(), than change matrix pipes to not call it!!!
  - implement issuing of a MATRIX_UPDATED event: add an event source member
  */
class ALBA_EXPORT albaTransformBase : public albaReferenceCounted, public albaEventBroadcaster
{
public:
  albaTransformBase();
  ~albaTransformBase();

  /** copy constructor */
  albaTransformBase(const albaTransformBase&);

  albaAbstractTypeMacro(albaTransformBase,albaReferenceCounted);
  virtual void Print(std::ostream& os, const int indent=0) const;

  /** update and return internal transform matrix */
  virtual const albaMatrix &GetMatrix() {Update();return *m_Matrix;}

  /** 
    return pointer to internal matrix (after updating).
    BEWARE: do not change the matrix directly. */
  albaMatrix *GetMatrixPointer() {Update(); return m_Matrix;}

  /**
    Apply the transformation to a coordinate.  You can use the same 
    array to store both the input and output point. */
  void TransformPoint(const float in[3], float out[3]) \
    { this->Update(); this->InternalTransformPoint(in,out); };

  /**
    Apply the transformation to a double-precision coordinate.  
    You can use the same array to store both the input and output point.*/
  void TransformPoint(const double in[3], double out[3]) \
    { this->Update(); this->InternalTransformPoint(in,out); };

  
  /**
    Apply the transformation to a normal at the specified vertex.  If the
    transformation is a vtkLinearTransform, you can use TransformNormal()
    instead. */
  void TransformNormalAtPoint(const float point[3], const float in[3],
                              float out[3]);
  /**
    Apply the transformation to a normal at the specified vertex.  If the
    transformation is a vtkLinearTransform, you can use TransformNormal()
    instead. */
  void TransformNormalAtPoint(const double point[3], const double in[3],
                              double out[3]);

  /**
    Apply the transformation to a vector at the specified vertex.  If the
    transformation is a vtkLinearTransform, you can use TransformVector()
    instead.*/
  void TransformVectorAtPoint(const float point[3], const float in[3],
                              float out[3]);
  /**
    Apply the transformation to a vector at the specified vertex.  If the
    transformation is a vtkLinearTransform, you can use TransformVector()
    instead.*/
  void TransformVectorAtPoint(const double point[3], const double in[3],
                              double out[3]);

  /**
    Update the transform to account for any changes which
    have been made.  You should not need to call this method 
    yourself, it is called automatically whenever the
    transform needs an update. For redefining the transform
    behavior redefine InternalUpdate() function */
  virtual void Update();

  /** update modification time stamp for this object */
  void Modified();

  /**
    This will calculate the transformation without calling Update.
    Meant for use only within other VTK classes. */
  virtual void InternalTransformPoint(const double in[3], double out[3]);
  virtual void InternalTransformPoint(const float in[3], float out[3]);

  /** Make another transform of the same type. */
  //virtual albaTransformBase *MakeTransform() = 0;

  /**
    Check for self-reference. */
  //virtual int CircuitCheck(albaTransformBase *transform);

  /** Return current modification time. */
  virtual vtkMTimeType GetMTime() {return m_MTime.GetMTime();}

  /** return last update time */
  virtual unsigned long GetUpdateTime() {return m_UpdateTime.GetMTime();}

  /** set the timestamp for the output matrix */
  void SetTimeStamp(albaTimeStamp t) {if (!albaEquals(m_TimeStamp,t)){m_TimeStamp=t; Modified();}}

  albaTimeStamp GetTimeStamp() {return m_TimeStamp;}

#ifdef ALBA_USE_VTK
  /** Return a VTK transform connected to this transform */
  vtkLinearTransform *GetVTKTransform();

  /** Return a VTK transform connected to this transform */
  operator vtkLinearTransform*() {return GetVTKTransform();}
#endif

protected:
  albaAutoPointer<albaMatrix> m_Matrix;        ///< internally stored matrix.
  albaMTime        m_MTime;           ///< modification time
  albaMTime        m_UpdateTime;      ///< We need to record the time of the last update
  albaMutexLock    m_UpdateMutex; ///< we also need to do mutex locking so updates don't collide.

  albaTimeStamp    m_TimeStamp;   ///< the timestamp to assign to the output matrix (default=0)

  #ifdef ALBA_USE_VTK
  vtkALBAToLinearTransform *m_VTKTransform; ///< VTK transform used to link to VTK process objects
  #endif

  /**  Perform any subclass-specific Update. */
  virtual void InternalUpdate() = 0;

private:

};

//------------------------------------------------------------------------------
inline void albaTransformBase::Modified()
//------------------------------------------------------------------------------
{
  m_MTime.Modified();
}


#endif





