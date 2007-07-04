/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrix.h,v $
  Language:  C++
  Date:      $Date: 2007-07-04 07:43:04 $
  Version:   $Revision: 1.11 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafMatrix_h
#define __mafMatrix_h

#include "mafReferenceCounted.h"
#include "mafMTime.h"

typedef double (*mafMatrixElements)[4];

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
#ifdef MAF_USE_VTK
class vtkMatrix4x4;
#endif


/** mafMatrix - Time stamped 4x4 Matrix.
  This class defines a TimeStamped 4x4 Matrix class. If MAF has been compiled
  with VTK support, this class can be used wherever a vtkMatrix4x4 is requested, 
  and indeed the internal representation is a vtkMatrix4x4. Also GetVTKMatrix() explicitly
  return a vtkMatrix4x4 pointer.
  Also mafMatrix can reference a vtkMatrix4x4, i.e. register it and share the
  same Elements vector, with SetVTKMatrix.
  @sa mafReferenceCounted vtkMatrix4x4
*/
class MAF_EXPORT mafMatrix : public mafReferenceCounted
{
public:
  mafTypeMacro(mafMatrix,mafReferenceCounted);
  virtual void Print(std::ostream& os, const int indent=0) const;

  mafMatrix();
  virtual ~mafMatrix();

  mafMatrix(const mafMatrix &mat);

  mafMatrix &operator=(const mafMatrix &mat);

  bool operator==(const mafMatrix& mat) const;

  bool Equals(const mafMatrix *mat) const;

  void DeepCopy(const mafMatrix* mat) {*this=*mat;}

#ifdef MAF_USE_VTK

  /** this constructor references the given matrix instead of copying it */
  mafMatrix(vtkMatrix4x4 *mat);

  /** set internal VTK matrix reference to the given pointer */
  void SetVTKMatrix(vtkMatrix4x4 *mat,mafTimeStamp t=0);

  /** copy matrix from the given VTK matrix */
  void DeepCopy(vtkMatrix4x4 *mat);

  /** return the pointer to the internal VTK matrix */
  vtkMatrix4x4 *GetVTKMatrix() const {return m_VTKMatrix;}

  /** 
    this simply compares the 4x4 matrix with the VTK one,
    but does not compare the time stamp. */
  bool operator==(vtkMatrix4x4 *mat) const;  
#endif

  /** return pointer to elements matrix: returned type is a double [4][4] object */
  mafMatrixElements GetElements() const;

  /** 
    Sets the element i,j in the matrix. Remember to call explicitly
    Modified when using this function. */
  void SetElement(const int i, const int j, double value) {GetElements()[i][j]=value;Modified();}

  /** Returns the element i,j from the matrix. */
  double GetElement(const int i, const int j) const {return GetElements()[i][j];}

  /** return modification time for this object */
  unsigned long GetMTime() const;

  /** update modification time stamp for this object */
  void Modified();
  
  /** Set the TimeStamp for this matrix */
  void SetTimeStamp(mafTimeStamp t) {if (!mafEquals(m_TimeStamp,t)){m_TimeStamp=t; Modified();}};
  mafTimeStamp GetTimeStamp() const {return m_TimeStamp;};

  /** Get the given matrix versor. Static version. */
  static void GetVersor(const int axis, const mafMatrix &matrix, double versor[3]);
  /** Get the given matrix versor*/
  void GetVersor(const int axis, double versor[3]) const {GetVersor(axis,(*this),versor);}

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
  
  /** Matrix determinant */
  static double Determinant(const double Elements[16]);
  double Determinant() const {return Determinant(*GetElements());};

  /** Matrix adjoint */
  static void Adjoint(const double inElements[16], double outElements[16]);
  void Adjoint(const mafMatrix &inMat, mafMatrix &outMat) {Adjoint(*(inMat.GetElements()),*(outMat.GetElements()));outMat.Modified();};
  void Adjoint() {Adjoint(*GetElements(),*GetElements());Modified();}

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
  void MultiplyPoint(const double in[4], double out[4]) const
    {mafMatrix::MultiplyPoint(*GetElements(),in,out); }
  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = A*in.
    The in[4] and out[4] can be the same array. */
  static void MultiplyPoint(const double Elements[16], 
                            const double in[4], double out[4]);

  /** Multiplies matrices a and b and stores the result in c.*/
  static void Multiply4x4(const mafMatrix &a, const mafMatrix &b, mafMatrix &c) {
    mafMatrix::Multiply4x4(*a.GetElements(),*b.GetElements(),*c.GetElements());c.Modified();};
  /** Multiplies matrices a and b and stores the result in c. this works with arrays. */
  static void Multiply4x4(const double a[16], const double b[16], 
                          double c[16]);

  /** bracket operator to access & write single elements */
  double *operator[](const unsigned int i) {return &(GetElements()[i][0]);Modified();}

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

#endif 
