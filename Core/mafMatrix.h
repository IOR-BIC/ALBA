/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrix.h,v $
  Language:  C++
  Date:      $Date: 2004-11-19 18:20:45 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafMatrix_h
#define __mafMatrix_h

#include "mafObject.h"
#include "mafMTime.h"

#ifdef MAF_USE_VTK
  #include "vtkMatrix4x4.h"
#endif

typedef double (*mafMatrixElements)[4];

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------


/** mafMatrix - Time stamped 4x4 Matrix.
  This class defines a TimeStamped 4x4 Matrix class. If MAF has been compiled
  with VTK support, this class can be used whereever a vtkMatrix4x4 is requested, 
  and indeed the internal representation is a vtkMatrix4x4. Also GetVTKMatrix() explicitelly
  return a vtkMatrix4x4 pointer.
  Also mafMatrix can reference a vtkMatrix4x4, i.e. register it and share the
  same Elements vector, with SetVTKMatrix.
  Currently this is defined as a mafObject, i.e. with RTTI information, but not
  as a mafSmartObject (i.e. reference counting).
  @sa mafObject vtkMatrix4x4 vnl_matrix
*/
class MAF_EXPORT mafMatrix : public mafObject
{
public:
  mafTypeMacro(mafMatrix,mafObject);

  mafMatrix();
  ~mafMatrix();

  mafMatrix(mafMatrix &mat);

  bool operator==(mafMatrix& m);

#ifdef MAF_USE_VTK
  mafMatrix(vtkMatrix4x4* mat, mafTimeStamp t=0);

  /** this only checks the 4x4 matrix with VTK ones, not the time stamp */
  bool operator==(vtkMatrix4x4 *m);

  /** return pointer to elements vector */
  mafMatrixElements GetElements() const {return (mafMatrixElements)m_VTKMatrix->Element;}
#else
  mafMatrixElements GetElements() const {return (mafMatrixElements)m_Elements;}
#endif

  /** 
    Sets the element i,j in the matrix. Remember to call explicitly
    Modified when using this function. */
  void SetElement(int i, int j, double value) {GetElements()[i][j]=value;}

  /** Returns the element i,j from the matrix. */
  double GetElement(int i, int j) const {return GetElements()[i][j];}

  /** return modification time for this object */
  unsigned long GetMTime();

  /** update modification time stamp for this object */
  void Modified();
  
  /** Set the TimeStamp for this matrix */
  void SetTimeStamp(mafTimeStamp t) {m_TimeStamp=t;};
  mafTimeStamp GetTimeStamp() {return m_TimeStamp;};

  /** Get the given matrix versor. Static version. */
  static void GetVersor(int axis, const mafMatrix &matrix, double versor[3]);
  /** Get the given matrix versor*/
  void GetVersor(int axis, double versor[3]) {GetVersor(axis,*this,versor);}

  /** Copy the 3x3 rotation matrix from another 4x4 matrix */
  void CopyRotation(const mafMatrix &source) {CopyRotation(source,*this);}
  /** Copy the 3x3 rotation matrix from a 4x4 matrix to another. Static version.  */
  static void CopyRotation(const mafMatrix &source, mafMatrix &target);

  /** Set all of the elements to zero. Static version. */
  void Zero() { mafMatrix::Zero(*GetElements()); Modified(); }
  /** Set all of the elements to zero. */  
  static void Zero(double elements[16]);

  /** Set equal to Identity matrix */
  void Identity() { mafMatrix::Identity(*GetElements()); Modified();}
  static void Identity(double elements[16]);

  /**
    Matrix Inversion (adapted from Richard Carling in "Graphics Gems," 
    Academic Press, 1990). static version.*/
  static void Invert(const mafMatrix &in, mafMatrix &out) {mafMatrix::Invert(*in.GetElements(),*out.GetElements()); out.Modified();}
  /**
    Matrix Inversion (adapted from Richard Carling in "Graphics Gems," 
    Academic Press, 1990). */
  void Invert() { mafMatrix::Invert(*this,*this); }
  /**
    Matrix Inversion, (adapted from Richard Carling in "Graphics Gems," 
    Academic Press, 1990). static version.*/
  static void Invert(const double inElements[16], double outElements[16]);

  /** Transpose the matrix and put it into out. static version.*/
  static void Transpose(const mafMatrix &in, mafMatrix &out) 
    {mafMatrix::Transpose(*in.GetElements(),*out.GetElements()); out.Modified(); }
  /** Transpose the matrix and put it into out. */
  void Transpose() { mafMatrix::Transpose(*this,*this); }
  /** Transpose the matrix and put it into out. static version.*/
  static void Transpose(const double inElements[16], double outElements[16]);

  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = A*in.
    The in[4] and out[4] can be the same array. */
  void MultiplyPoint(const double in[4], double out[4]) 
    {mafMatrix::MultiplyPoint(*GetElements(),in,out); }
  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = A*in.
    The in[4] and out[4] can be the same array. */
  static void MultiplyPoint(const double Elements[16], 
                            const double in[4], double out[4]);

  /** Multiplies matrices a and b and stores the result in c.*/
  static void Multiply4x4(mafMatrix &a, mafMatrix &b, mafMatrix &c) {
    mafMatrix::Multiply4x4(*a.GetElements(),*b.GetElements(),*c.GetElements()); };
  /** Multiplies matrices a and b and stores the result in c. this works with arrays. */
  static void Multiply4x4(const double a[16], const double b[16], 
                          double c[16]);

  /** Compute adjoint of the matrix and put it into out.*/
  void Adjoint(const mafMatrix &in, mafMatrix &out) 
    {mafMatrix::Adjoint(*in.GetElements(),*out.GetElements());}
  static void Adjoint(const double inElements[16], double outElements[16]);

  /** Compute the determinant of the matrix and return it.*/
  double Determinant() {return mafMatrix::Determinant(*GetElements());}
  static double Determinant(const double Elements[16]);

  /** bracket operator to access & write single elements */
  double *operator[](const unsigned int i) {return &(GetElements()[i][0]);}

  /** bracket operator to access single elements */
  const double *operator[](unsigned int i) const { return &(GetElements()[i][0]); }  

  //double *operator[][](const unsigned int i,const unsigned int j) {return &(GetElements()[i][j]);}
  
  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = in*A.
    The in[4] and out[4] can be the same array. */  
  static void PointMultiply(const double Elements[16], 
                            const float in[4], float out[4]);
  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = in*A.
    The in[4] and out[4] can be the same array. */
  static void PointMultiply(const double Elements[16], 
                            const double in[4], double out[4]);
protected:
  mafTimeStamp m_TimeStamp;

#ifdef MAF_USE_VTK
  vtkMatrix4x4 *m_VTKMatrix; ///< Use a VTK matrix to store the 4x4 elements
#else
  double m_Elements[4][4]; ///< has its own internal representation
  mafMTime m_MTime;
#endif
  
};

//------------------------------------------------------------------------------
inline unsigned long mafMatrix::GetMTime()
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  return m_VTKMatrix->GetMTime();
#else
  return m_MTime.GetMTime();
#endif
}

//------------------------------------------------------------------------------
inline void mafMatrix::Modified()
//------------------------------------------------------------------------------
{
  #ifdef MAF_USE_VTK
  m_VTKMatrix->Modified();
#else
  m_MTime.Modified();
#endif
}

#endif 
