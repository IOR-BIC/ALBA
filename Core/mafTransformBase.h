/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTransformBase.h,v $
  Language:  C++
  Date:      $Date: 2004-11-25 19:16:43 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafTransformBase_h
#define __mafTransformBase_h

#include "mafObject.h"
#include "mafMTime.h"
#include "mafMutexLock.h"
#include "mafMatrix.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class vtkLinearTransform;

/**  Superclass for Homogeneous transformations.
  mafTransformBase is the superclass for MAF geometric, and currently homogeneous
  only, transformations. The idea behind a mafTransformBase is the Update() method
  should always be called to update the output, which is a mafMatrix internally stored.
  @sa mafTransform
  */
class MAF_EXPORT mafTransformBase : public mafObject
{
public:
  mafTransformBase();
  ~mafTransformBase();

  /** copy constructor */
  mafTransformBase(const mafTransformBase&);

  mafAbstractTypeMacro(mafTransformBase,mafObject);
  //void PrintSelf(ostream& os, vtkIndent indent);

  /** update and return internal transform matrix */
  const mafMatrix &GetMatrix() {Update();return m_Matrix;}

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
    Get the inverse of this transform.  If you modify this transform,
    the returned inverse transform will automatically update.*/
  mafTransformBase *GetInverse();

  /**
    Update the transform to account for any changes which
    have been made.  You do not have to call this method 
    yourself, it is called automatically whenever the
    transform needs an update. */
  void Update();

  /** update modification time stamp for this object */
  void Modified();

  /**
    This will calculate the transformation without calling Update.
    Meant for use only within other VTK classes. */
  virtual void InternalTransformPoint(const float in[3], float out[3]) = 0;
  virtual void InternalTransformPoint(const double in[3], double out[3]) = 0;

  /** Make another transform of the same type. */
  //virtual mafTransformBase *MakeTransform() = 0;

  /**
    Check for self-reference. */
  //virtual int CircuitCheck(mafTransformBase *transform);

  /** Return current modification time. */
  virtual unsigned long GetMTime() {return m_MTime.GetMTime();}

  /** return last update time */
  virtual unsigned long GetUpdateTime() {return m_UpdateTime.GetMTime();}

#ifdef MAF_USE_VTK
  /** Return a VTK transform connected to this transform */
  vtkLinearTransform *GetVTKTransform();

  /** Return a VTK transform connected to this transform */
  operator vtkLinearTransform*() {return GetVTKTransform();}
#endif

protected:
  mafMatrix m_Matrix; ///< internally stored matrix.
  mafMTime m_MTime; ///< modification time
  mafMTime m_UpdateTime; ///< We need to record the time of the last update
  mafMutexLock m_UpdateMutex; ///< we also need to do mutex locking so updates don't collide.

  mafMutexLock m_InverseMutex;
  mafTransformBase *m_MyInverse;

  #ifdef MAF_USE_VTK
  vtkLinearTransform *m_VTKTransform; ///< VTK transform used to link to VTK process objects
  #endif

  /**  Perform any subclass-specific Update. */
  virtual void InternalUpdate() = 0;

private:

};

//------------------------------------------------------------------------------
inline void mafTransformBase::Modified()
//------------------------------------------------------------------------------
{
  m_MTime.Modified();
}


#endif





