/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrix
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaMatrix_h
#define __albaMatrix_h

#include "albaReferenceCounted.h"
#include "albaMTime.h"

typedef double (*albaMatrixElements)[4];

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
#ifdef ALBA_USE_VTK
class vtkMatrix4x4;
#endif


/** albaMatrix - Time stamped 4x4 Matrix.
  This class defines a TimeStamped 4x4 Matrix class. If ALBA has been compiled
  with VTK support, this class can be used wherever a vtkMatrix4x4 is requested, 
  and indeed the internal representation is a vtkMatrix4x4. Also GetVTKMatrix() explicitly
  return a vtkMatrix4x4 pointer.
  Also albaMatrix can reference a vtkMatrix4x4, i.e. register it and share the
  same Elements vector, with SetVTKMatrix.
  @sa albaReferenceCounted vtkMatrix4x4
*/
class ALBA_EXPORT albaMatrix : public albaReferenceCounted
{
public:
  albaTypeMacro(albaMatrix,albaReferenceCounted);
  virtual void Print(std::ostream& os, const int indent=0) const;

  albaMatrix();
  virtual ~albaMatrix();

  albaMatrix(const albaMatrix &mat);

  albaMatrix &operator=(const albaMatrix &mat);

  bool operator==(const albaMatrix& mat) const;

  bool Equals(const albaMatrix *mat) const;

  void DeepCopy(const albaMatrix* mat) {*this=*mat;}

#ifdef ALBA_USE_VTK

  /** this constructor references the given matrix instead of copying it */
  albaMatrix(vtkMatrix4x4 *mat);

  /** set internal VTK matrix reference to the given pointer */
  void SetVTKMatrix(vtkMatrix4x4 *mat,albaTimeStamp t=0);

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
  albaMatrixElements GetElements() const;

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
  void SetTimeStamp(albaTimeStamp t) {if (!albaEquals(m_TimeStamp,t)){m_TimeStamp=t; Modified();}};
  albaTimeStamp GetTimeStamp() const {return m_TimeStamp;};

  /** Get the given matrix versor. Static version. */
  static void GetVersor(const int axis, const albaMatrix &matrix, double versor[3]);
  /** Get the given matrix versor*/
  void GetVersor(const int axis, double versor[3]) const {GetVersor(axis,(*this),versor);}

  /** Copy the 3x3 rotation matrix from another 4x4 matrix */
  void CopyRotation(const albaMatrix &source) {CopyRotation(source,*this);}
  /** Copy the 3x3 rotation matrix from a 4x4 matrix to another. Static version.  */
  static void CopyRotation(const albaMatrix &source, albaMatrix &target);

  /** Set all of the elements to zero. Static version. */
  void Zero() { albaMatrix::Zero(*GetElements()); Modified(); }
  /** Set all of the elements to zero. */  
  static void Zero(double elements[16]);

  /** Set equal to Identity matrix */
  void Identity() { albaMatrix::Identity(*GetElements()); Modified();}
  static void Identity(double elements[16]);

	/** Return true if is an Identity Matrix */
	bool IsIdentity();

  /**
    Matrix Inversion (adapted from Richard Carling in "Graphics Gems," 
    Academic Press, 1990). static version.*/
  static void Invert(const albaMatrix &in, albaMatrix &out) {albaMatrix::Invert(*in.GetElements(),*out.GetElements()); out.Modified();}
  /**
    Matrix Inversion (adapted from Richard Carling in "Graphics Gems," 
    Academic Press, 1990). */
  void Invert() { albaMatrix::Invert(*this,*this); }
  /**
    Matrix Inversion, (adapted from Richard Carling in "Graphics Gems," 
    Academic Press, 1990). static version.*/
  static void Invert(const double inElements[16], double outElements[16]);
  
  /** Matrix determinant */
  static double Determinant(const double Elements[16]);
  double Determinant() const {return Determinant(*GetElements());};

  /** Matrix adjoint */
  static void Adjoint(const double inElements[16], double outElements[16]);
  void Adjoint(const albaMatrix &inMat, albaMatrix &outMat) {Adjoint(*(inMat.GetElements()),*(outMat.GetElements()));outMat.Modified();};
  void Adjoint() {Adjoint(*GetElements(),*GetElements());Modified();}

  /** Transpose the matrix and put it into out. static version.*/
  static void Transpose(const albaMatrix &in, albaMatrix &out) 
    {albaMatrix::Transpose(*in.GetElements(),*out.GetElements()); out.Modified(); }
  /** Transpose the matrix and put it into out. */
  void Transpose() { albaMatrix::Transpose(*this,*this); }
  /** Transpose the matrix and put it into out. static version.*/
  static void Transpose(const double inElements[16], double outElements[16]);

  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = A*in.
    The in[4] and out[4] can be the same array. */
  void MultiplyPoint(const double in[4], double out[4]) const
    {albaMatrix::MultiplyPoint(*GetElements(),in,out); }
  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = A*in.
    The in[4] and out[4] can be the same array. */
  static void MultiplyPoint(const double Elements[16], 
                            const double in[4], double out[4]);

  /** Multiplies matrices a and b and stores the result in c.*/
  static void Multiply4x4(const albaMatrix &a, const albaMatrix &b, albaMatrix &c) {
    albaMatrix::Multiply4x4(*a.GetElements(),*b.GetElements(),*c.GetElements());c.Modified();};
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
	
	void SetFromDirectionCosines(const double orientation[6]);
protected:
  albaTimeStamp m_TimeStamp;

#ifdef ALBA_USE_VTK
  vtkMatrix4x4 *m_VTKMatrix; ///< Use a VTK matrix to store the 4x4 elements
#else
  double m_Elements[4][4]; ///< has its own internal representation
  albaMTime m_MTime;
#endif
  
};

#endif 
