/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrix3x3.h,v $
  Language:  C++
  Date:      $Date: 2004-11-25 19:16:43 $
  Version:   $Revision: 1.1 $
  Authors:   Based on vtkMath code (www.vtk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafMatrix3x3_h
#define __mafMatrix3x3_h

#include "mafObject.h"

typedef double (*mafMatrix3x3Elements)[3];


/** mafMatrix3x3 - Simple 3x3 Matrix.
  This class defines a simple 3x3 Matrix class, and some operators over it.
  This is tipically used for internal algorithms, use mafMatrix for complex usage.
  @sa mafMatrix
*/
class MAF_EXPORT mafMatrix3x3
{
public:
  mafTypeMacro(mafMatrix3x3,mafObject);

  mafMatrix3x3();
  ~mafMatrix3x3();

  mafMatrix3x3(mafMatrix3x3 &mat);

  mafMatrix3x3Elements GetElements() const {return (mafMatrix3x3Elements)m_Elements;}

  /** 
    Sets the element i,j in the matrix. Remember to call explicitly
    Modified when using this function. */
  void SetElement(int i, int j, double value) {GetElements()[i][j]=value;}

  /** Returns the element i,j from the matrix. */
  double GetElement(int i, int j) const {return GetElements()[i][j];}

  /** Get the given matrix versor. Static version. */
  static void GetVersor(int axis, const mafMatrix3x3 &matrix, double versor[3]);
  /** Get the given matrix versor*/
  void GetVersor(int axis, double versor[3]) {GetVersor(axis,*this,versor);}

  /** Set all of the elements to zero. Static version. */
  void Zero() { mafMatrix3x3::Zero(*GetElements()); Modified(); }
  /** Set all of the elements to zero. */  
  static void Zero(double elements[9]);

  /** Set equal to Identity matrix */
  void Identity() { mafMatrix3x3::Identity(*GetElements()); Modified();}
  static void Identity(double elements[9]);
  
  /**
   Orthogonalize a 3x3 matrix and put the result in B.  If matrix A
   has a negative determinant, then B will be a rotation plus a flip
   i.e. it will have a determinant of -1. */
  static void Orthogonalize(const double A[3][3], double B[3][3]);
  
  /** 
   Orthogonalize this matrix inplace.  If this matrix 
   has a negative determinant, then the result will be a
   rotation plus a flip i.e. it will have a determinant of -1. */
  static void Orthogonalize() {Orthogonalize(GetElements(),GetElements());}

  /** Return the determinant of a 3x3 matrix. */
  static double Determinant(double A[3][3]);

  /** Compute the determinant of the matrix and return it.*/
  double Determinant() {return Determinant(GetElements());}
  
  /**
    Convert a quaternion to a 3x3 rotation matrix.  The quaternion
    does not have to be normalized beforehand. */
  static void QuaternionToMatrix(const double quat[4], double A[3][3]); 
  
  /**
    Convert a quaternion to a 3x3 rotation matrix.  The quaternion
    does not have to be normalized beforehand. */
  void QuaternionToMatrix(const double quat[4], double A[3][3]); 

  /**
   Convert a 3x3 matrix into a quaternion.  This will provide the
   best possible answer even if the matrix is not a pure rotation matrix.
   The method used is that of B.K.P. Horn. */
  static void MatrixToQuaternion(const double A[3][3], double quat[4]);
  
  /**
   Convert a 3x3 matrix into a quaternion.  This will provide the
   best possible answer even if the matrix is not a pure rotation matrix.
   The method used is that of B.K.P. Horn. */
  void MatrixToQuaternion(double quat[4]) {MatrixToQuaternion(GetElements(),quat);}
  
  /**
    Perform singular value decomposition on a 3x3 matrix.  This is not
    done using a conventional SVD algorithm, instead it is done using
    Orthogonalize3x3 and Diagonalize3x3.  Both output matrices U and VT
    will have positive determinants, and the w values will be arranged
    such that the three rows of VT are aligned as closely as possible
    with the x, y, and z axes respectively.  If the determinant of A is
    negative, then the three w values will be negative. */                                            
  static void SingularValueDecomposition3x3(const double A[3][3],
                                            double U[3][3], double w[3],
                                            double VT[3][3]);
                                            
  /** Perform singular value decomposition on a 3x3 matrix. See the static version. */ 
  static void SingularValueDecomposition3x3(double U[3][3], double w[3],
                                            double VT[3][3]);

  /** bracket operator to access & write single elements */
  double *operator[](const unsigned int i) {return &(GetElements()[i][0]);}

  /** bracket operator to access single elements */
  const double *operator[](unsigned int i) const { return &(GetElements()[i][0]); }  
  
protected:
  mafTimeStamp m_TimeStamp;

#ifdef MAF_USE_VTK
  vtkMatrix4x4 *m_VTKMatrix;  /< Use a VTK matrix to store the 4x4 elements
#else
  double m_Elements[4][4];  /< has its own internal representation
  mafMTime m_MTime;
#endif
  
};

 ------------------------------------------------------------------------------
inline unsigned long mafMatrix3x3::GetMTime()
 ------------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  return m_VTKMatrix->GetMTime();
#else
  return m_MTime.GetMTime();
#endif
}

 ------------------------------------------------------------------------------
inline void mafMatrix3x3::Modified()
 ------------------------------------------------------------------------------
{
  #ifdef MAF_USE_VTK
  m_VTKMatrix->Modified();
#else
  m_MTime.Modified();
#endif
}

#endif 
