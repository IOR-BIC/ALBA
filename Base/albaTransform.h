/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransform
 Authors: Marco Petrone, Stefano Perticoni,Stefania Paperini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaTransform_h
#define __albaTransform_h

#include "albaTransformBase.h"
#include "albaInteractorConstraint.h"

#include "albaUtility.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class vtkMatrix4x4;

//----------------------------------------------------------------------------	
//  Constants:
//----------------------------------------------------------------------------	
enum ALBA_EXPORT MATRIX_MULTIPLICATION_ID
{ 
  POST_MULTIPLY = 0, 
  PRE_MULTIPLY 
};
//#define ALBA_FLT_MAX		3.40282346638528860e38F

/** albaTransform - class for homogeneous transformations.
  albaTransform provides functionalities for homogeneous transformations, including
  an algorithm for polar decomposition, used to keep the 3x3 sub matrix orthogonal.
  The idea of this class is to represent by itself a transformation, obtained by changing
  the an input transformation for its original coordinate system to a target coordinate 
  system. Coordinate systems are by default the world coords system. Reference systems can 
  be expressed as a albaTransformBase.

  @sa albaTransformBase
  @todo 
  - Update test for albaTransform to test functions to transform angles
*/
class ALBA_EXPORT albaTransform : public albaTransformBase
{
 public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  //ALBA_ID_DEC(UpdateEvent); // Event rised by updates of the internal matrix
	
  albaTransform();
  ~albaTransform();

  //----------------------------------------------------------------------------
  // Ref Sys Type:
  //----------------------------------------------------------------------------
  enum 
  {
    CUSTOM = 0, ///< auxiliar ref sys 
    GLOBAL, 
    PARENT,
    LOCAL,   ///< the local ref sys of the VME
    VIEW     ///< the view ref sys
  };

  /** copy constructor */
  albaTransform(const albaTransform&);

  /** RTTI stuff */
  albaTypeMacro(albaTransform,albaTransformBase);
  
  /**
    Directly set the internal Matrix. It's overwritten by Update if Input or InputFrame !=NULL
    This function makes a copy of the input matrix. */
  virtual void SetMatrix(const albaMatrix &input) {*m_Matrix=input;SetTimeStamp(input.GetTimeStamp());Modified();}

  /** set the internal matrix pointer to the given matrix. Do not use this if you don't know what you are doing */
  void SetMatrixPointer(albaMatrix *matrix);

	/**
	  Polar Decomposition of matrix M in Q * S.*/
	static double PolarDecomp(const albaMatrix &M, albaMatrix &Q, albaMatrix &S, double translation[3]);
  double PolarDecomp(albaMatrix &Q, albaMatrix &S, double translation[3]) {return PolarDecomp(GetMatrix(),Q,S,translation);}
  
  /** set internal matrix to Identity */
  void Identity();

  /** Invert internal matrix */
  void Invert();

  /**
	  Get the x, y, z orientation angles from the transformation matrix as an
    array of three floating point values. Copied from vtkTransform::GetOrientation()*/
  static void GetOrientation(const albaMatrix &in_matrix,double orientation[3]);
  static void GetOrientation(const albaMatrix &in_matrix,float orientation[3]);
  void GetOrientation(double orientation[3]) { this->GetOrientation(GetMatrix(),orientation);}
  void GetOrientation(float orient[3]) {
    double temp[3]; this->GetOrientation(temp); 
    orient[0] = static_cast<float>(temp[0]); 
    orient[1] = static_cast<float>(temp[1]); 
    orient[2] = static_cast<float>(temp[2]); };

  /**
	  Return the wxyz angle+axis representing the current orientation.
    Copied from vtkTransform::GetVTKOrientationWXYZ()*/
  static void GetOrientationWXYZ(const albaMatrix &in_matrix, double wxyz[4]);
  void GetOrientationWXYZ(double wxyz[4]) \
      { GetOrientationWXYZ(GetMatrix(),wxyz);}
  void GetOrientationWXYZ(float wxyz[3]) {
    double temp[4]; GetOrientationWXYZ(temp); 
    wxyz[0]=static_cast<float>(temp[0]); 
    wxyz[1]=static_cast<float>(temp[1]); 
    wxyz[2]=static_cast<float>(temp[2]); 
    wxyz[3]=static_cast<float>(temp[3]);};

  /**
    Return the position from the current transformation matrix as an array
    of three floating point numbers. This is simply returning the translation 
    component of the 4x4 matrix. Copied from vtkTransform::GetPosition()*/
  static void GetPosition(const albaMatrix &matrix,double position[3]);
  static void GetPosition(const albaMatrix &matrix,float position[3]);
  void GetPosition(double position[3]) {this->GetPosition(GetMatrix(),position);}
  void GetPosition(float position[3]) {
    double temp[3]; this->GetPosition(temp); 
    position[0] = static_cast<float>(temp[0]); 
    position[1] = static_cast<float>(temp[1]); 
    position[2] = static_cast<float>(temp[2]); };

  /**
    This function set the position column of the matrix. Notice, the non static functions
    work on the internal matrix and can be used in conjuction with SetMatrix, but if a pipeline
    is defined (input, input_frame or target_frame) it's overwritten whenever the Update() is perfromed.*/
  static void SetPosition(albaMatrix &matrix,double position[3]);
  void SetPosition(double position[3]) {this->SetPosition(*m_Matrix,position);}
  void SetPosition(double x,double y,double z) {this->SetPosition(*m_Matrix,x,y,z);}
  static void SetPosition(albaMatrix &matrix, double x,double y,double z) {
    double temp[3];
    temp[0]=x;temp[1]=y;temp[2]=z;
    SetPosition(matrix,temp);}

  /**
    This function set the translation column of the matrix by adding the transaltion provided
    as argument. Notice, the non static functions work on the internal matrix and can be used
    in conjuction with SetMatrix, but if a pipeline  is defined (input, input_frame or 
    target_frame) it's overwritten whenever the Update() is perfromed.*/
  static void Translate(albaMatrix &matrix,double translation[3],int premultiply);
  void Translate(double translation[3],int premultiply) {this->Translate(*m_Matrix,translation,premultiply);}
  void Translate(double x,double y,double z,int premultiply) {this->Translate(*m_Matrix,x,y,z,premultiply);}
  static void Translate(albaMatrix &matrix, double x,double y,double z,int premultiply) {
    double temp[3];
    temp[0]=x;temp[1]=y;temp[2]=z;
    Translate(matrix,temp,premultiply);}

  /**
    This function rotate the matrix around the specified axis. Notice, the non static functions
    work on the internal matrix and can be used in conjuction with SetMatrix, but if a pipeline
    is defined (input, input_frame or target_frame) it's overwritten whenever the Update() is perfromed.*/
  static void RotateWXYZ(const albaMatrix &source,albaMatrix &target,double angle,double x, double y, double z,int premultiply);
  void RotateWXYZ(double angle,double x, double y, double z,int premultiply) {this->RotateWXYZ(*m_Matrix,*m_Matrix,angle,x,y,z,premultiply);}
  void RotateWXYZ(double angle,double rot[3],int premultiply) {this->RotateWXYZ(*m_Matrix,*m_Matrix,angle, rot[0], rot[1], rot[2],premultiply);}


  /**
    This functions rotate the internal matrix around the specified axis. It can be used in conjuction with
    SetMatrix, but if a pipeline is defined (input, input_frame or target_frame) it's overwritten whenever
    the Update() is performed.*/
  static void RotateX(albaMatrix &matrix,double angle,int premultiply) { albaTransform::RotateWXYZ(matrix,matrix,angle, 1, 0, 0,premultiply); };
  static void RotateY(albaMatrix &matrix,double angle,int premultiply) { albaTransform::RotateWXYZ(matrix,matrix,angle, 0, 1, 0,premultiply); };
  static void RotateZ(albaMatrix &matrix,double angle,int premultiply) { albaTransform::RotateWXYZ(matrix,matrix,angle, 0, 0, 1,premultiply); };
  void RotateX(double angle,int premultiply) { this->RotateX(*m_Matrix,angle,premultiply); };
  void RotateY(double angle,int premultiply) { this->RotateY(*m_Matrix,angle,premultiply); };
  void RotateZ(double angle,int premultiply) { this->RotateZ(*m_Matrix,angle,premultiply); };

  /** Pre or Post multiply the  internal matrix for given matrix and store result in the internal matrix */
  void Concatenate(const albaMatrix &matrix, int premultiply);

  /** Pre or Post multiply the  internal matrix for given transform and store result in the internal matrix */
  void Concatenate(albaTransformBase *trans, int premultiply) {Concatenate(trans->GetMatrix(),premultiply);}

  /**
    This function set the orientation (acc. to VTK convention) of the matrix. Notice, the non static functions
    work on the internal matrix and can be used in conjuction with SetMatrix, but if a pipeline
    is defined (input, input_frame or target_frame) it's overwritten whenever the Update() is performed.*/
  static void SetOrientation(albaMatrix &matrix,double orientation[3]);
  void SetOrientation(double orientation[3]) {this->SetOrientation(*m_Matrix,orientation);}
  void SetOrientation(double rx,double ry,double rz) {this->SetOrientation(*m_Matrix,rx,ry,rz);}
  static void SetOrientation(albaMatrix &matrix,double rx,double ry,double rz) {
    double temp[3];
    temp[0]=rx;temp[1]=ry;temp[2]=rz;
    SetOrientation(matrix,temp);}

  /**
    Return the scale factors of the current transformation matrix as 
    an array of three float numbers.  These scale factors are not necessarily
    about the x, y, and z axes unless unless the scale transformation was
    applied before any rotations. Copied from vtkTransform::GetScale()*/
  static void GetScale(const albaMatrix &matrix,double scale[3]);
  void GetScale(double scale[3]) {this->GetScale(*m_Matrix,scale);}
  void GetScale(float scale[3]) {
    double temp[3]; this->GetScale(temp); 
    scale[0] = static_cast<float>(temp[0]); 
    scale[1] = static_cast<float>(temp[1]); 
    scale[2] = static_cast<float>(temp[2]); };

  /** Apply a scale transform. By default the scale matrix is premultiplied */
  static void Scale(albaMatrix &matrix,double scalex,double scaley,double scalez,int premultiply);
  void Scale(double scalex,double scaley,double scalez,int premultiply) \
    { Scale(*m_Matrix,scalex,scaley,scalez,premultiply);}

  /** Set/Get internal matrix versor*/
  static void SetVersor(int axis, double versor[3], albaMatrix &matrix);
  
  static void GetVersor(int axis, const albaMatrix &matrix, double versor[3]) {albaMatrix::GetVersor(axis,matrix,versor);}
  static void GetVersor(int axis, const albaMatrix &matrix, float versor[3]) \
    { \
      double temp[3]; GetVersor(axis, matrix, temp); \
      versor[0] = static_cast<float>(temp[0]); \
      versor[1] = static_cast<float>(temp[1]); \
      versor[2] = static_cast<float>(temp[2]); };
  
  void GetVersor(int axis, double versor[3]) {GetVersor(axis,GetMatrix(),versor);}
  void GetVersor(int axis, float versor[3]) {GetVersor(axis,GetMatrix(),versor);}

  /**
    Copy the 3x3 rotation matrix from another 4x4 matrix into the specified matrix, or in the internal matrix.*/
  static void CopyRotation(const albaMatrix &source, albaMatrix &target);
  void CopyRotation(const albaMatrix &source) {this->CopyRotation(source,*m_Matrix);}

  /** Copy the translation vector */
  static void CopyTranslation(const albaMatrix &source, albaMatrix &target);
  void CopyTranslation(const albaMatrix &source) {this->CopyTranslation(source,*m_Matrix);}
  
  /** Add two vectors */
  static void AddVectors( double inV0[3],double inV1[3],double outSum[3] );

  /** Build vector with origin in p1 pointing to p2 */
  static void BuildVector(double *p1, double *p2, double *out_vector);

  /** Build vector [coeff * inVector] */
  static void BuildVector(double coeff, const double *inVector, double *outVector, int refSysType = LOCAL, int localAxis = albaInteractorConstraint::X);

  /** Project in_vector on in_axis direction; in_axis does not need to be 
  normalised. The projection signed value is returned */
  static double ProjectVectorOnAxis(const double *in_vector, const double *in_axis, double *out_projection = NULL);

  /** Project in_vector on the plane identified by the normal vector in_plane_normal;
  in_plane_normal does not need to be normalised. The norm of the projection 
  is returned and the projection vector is written in out_projection vector if provided. */
  static double ProjectVectorOnPlane(const double *in_vector, const double *in_plane_normal, double *out_projection = NULL);

  /** Find perpendicular versors to input versor N */
  static void FindPerpendicularVersors(double inVersorN[3], double outVersorP[3], double outVersorQ[3]);

  /** Multiply vector by scalar */
  static void MultiplyVectorByScalar(double s, double *vin, double *vout);

  /** rotation representation conversion */
	int MatrixToAttitudeVector(const albaMatrix &matrix,
										        double attitude_vector[3]);

  /** rotation representation conversion */
	int MatrixToEulerCardanicAngle(const albaMatrix &matrix,
													int i,int j,int k,
													double euler_cardan[3],
													double tentative_euler_cardan_first,
													double tentative_euler_cardan_second,
													double tentative_euler_cardan_third);

  /** rotation representation conversion */
	int MatrixTommuQuaternion(const albaMatrix &matrix, double quaternion[4]);

  /** rotation representation conversion */
	int QuaternionToMatrix(double quaternion[4],	albaMatrix &matrix);

  /** rotation + translation representation conversion */
	int HelicalAxisToMatrix(double helical_axis[3],double angle, albaMatrix &matrix);

  /** rotation representation conversion */
	int AttitudeVectorToMatrix(double attitude_vector[3], albaMatrix &matrix);

  /** rotation representation conversion */
	int EulerCardanicAngleToMatrix(double euler_cardan[3],
													int i,int j,int k,
													albaMatrix &matrix);

  /** rotation + translation representation conversion */
	int MatrixToHelicalAxis(const albaMatrix &matrix,
										 											 double helical_axis[3],double point[3],
																					 double& phi,double& t, int intersect);


  //----------------------------------------------------------------------------
  // declarations for polar_decomp algorithm from Graphics Gems IV,
  // by Ken Shoemake <shoemake@graphics.cis.upenn.edu>
  //----------------------------------------------------------------------------
  enum mmuQuatPart {X, Y, Z, W};

  typedef struct {double x, y, z, w;} mmuQuat; ///< mmuQuaternion 
  typedef mmuQuat HVect; ///< Homogeneous 3D vector 
  
  typedef struct {
	  HVect t;	///< Translation components
	  mmuQuat  q;	///< Essential rotation
	  mmuQuat  u;	///< Stretch rotation
	  HVect k;	///< Stretch factors
	  double f;	///< Sign of determinant
  } mmuAffineParts;
  
  typedef double HMatrix[4][4]; /* Right-handed, for column vectors */
  static double PolarDecomp(HMatrix M, HMatrix Q, HMatrix S);
  static void DecompAffine(HMatrix A, mmuAffineParts *parts);
  static HVect SpectDecomp(HMatrix S, HMatrix U);
  static mmuQuat QuaternionFromMatrix(HMatrix mat);
  static void InvertAffine(mmuAffineParts *parts, mmuAffineParts *inverse);
  static mmuQuat Snuggle(mmuQuat q, HVect *k);
  //----------------------------------------------------------------------------
protected:
  /**
    This only sets the timestamp for the output matrix: output matrix is not computed
    inside InternalUpdate since this is not a procedural transform. */
  virtual void InternalUpdate() {m_Matrix->SetTimeStamp(m_TimeStamp);}
private:
};

#endif

