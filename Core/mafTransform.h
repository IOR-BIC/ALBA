/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTransform.h,v $
  Language:  C++
  Date:      $Date: 2004-11-23 15:17:41 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni, Marco Petrone, Stefania Paperini
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mflTransform_h
#define __mflTransform_h

#include "vtkLinearTransform.h"
#include "mflCoreWin32Header.h"
#include "mflEvent.h"

class vtkMatrix4x4;

//----------------------------------------------------------------------------	
//  Constants:
//----------------------------------------------------------------------------	
enum { POST_MULTIPLY=0, PRE_MULTIPLY };
#define FLT_MAX		3.40282346638528860e38F

//----------------------------------------------------------------------------
// declarations for polar_decomp algorithm from Graphics Gems IV,
// by Ken Shoemake <shoemake@graphics.cis.upenn.edu>
//----------------------------------------------------------------------------
typedef struct {float x, y, z, w;} Quat; /* Quaternion */
enum QuatPart {X, Y, Z, W};
typedef Quat HVect; /* Homogeneous 3D vector */
typedef struct {
	HVect t;	/* Translation components */
	Quat  q;	/* Essential rotation	  */
	Quat  u;	/* Stretch rotation	  */
	HVect k;	/* Stretch factors	  */
	float f;	/* Sign of determinant	  */
} AffineParts;
	
typedef float HMatrix[4][4]; /* Right-handed, for column vectors */
float polar_decomp(HMatrix M, HMatrix Q, HMatrix S);
//----------------------------------------------------------------------------	

/** mflTransform - class for multi frame homogeneous transformations.
  mflTransform provides functionalities for homogeneous transformations, including
  an algorithm for polar decomposition, used to keep the 3x3 sub matrix orthogonal.
  The idea of this class is to represent by itself a transformation, obtained by changing
  the an input transformation for its original coordinate system to a target coordinate 
  system. Coordinate systems are by default the world coords system. Reference systems can 
  be expressed as a vtkLinearTransform.

  @sa vtkLinearTransform
  @todo

  - Modifica GetMTime per considerare anche l'MTime di InputFrame e TargetFrame
  - Rimuovi il vecchio ConversionRToHelicalAxis e  usa il nuovo ConversionTranslationMatrixToHelicalAxis al suo posto (aggiorna la opTransform di conseguenza)
  - Aggiorna il test della mflTransform per testare le funzioni di conversione degli angoli
*/
class MFL_CORE_EXPORT mflTransform : public vtkLinearTransform
{
 public:

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  MFL_EVT_DEC(UpdateEvent); // Event rised by updates of the internal matrix
	
  static mflTransform *New();
  vtkTypeMacro(mflTransform,vtkLinearTransform);
  void PrintSelf (ostream& os, vtkIndent indent);

  /**
  Directly set the internal Matrix. It's overwritten by Update if Input or InputFrame !=NULL
  By default this function make a copy of the input matrix, but you can force a refenrece. */
  virtual void SetMatrix(vtkMatrix4x4 *input,int copy=true);

  /**
  Set the input transform.  Any modifications to the matrix will be
  reflected in the transformation.*/
  void SetInput(vtkMatrix4x4 *input);
  virtual void SetInput(vtkLinearTransform *);
  vtkGetObjectMacro(Input,vtkLinearTransform);
  //void SetInput(vtkMatrix4x4 *)
		
  /**
  The input transform is left as-is, but the transformation matrix
  is inverted.*/
  void Inverse();
  void SetInverseFlag(int f) {this->InverseFlag=f;}
  int GetInverseFlag() {return this->InverseFlag;}
  vtkBooleanMacro(InverseFlag,int);
  

  /**
  Get the MTime: this is the bit of magic that makes everything work.*/
  unsigned long GetMTime();

  /**
  Make a new transform of the same type.*/
  vtkAbstractTransform *MakeTransform();

  /**
  Set/Get the input reference system, i.e. the reference system of the 
  input matrix.*/
  void SetInputFrame(vtkMatrix4x4 *frame);
  void SetInputFrame(vtkLinearTransform *frame);
  vtkLinearTransform *GetInputFrame() {return this->InputFrame;}

  /**
  Set/Get the output reference system, i.e. the reference system of the output
  matrix or the target reference system for point transformation.*/
  void SetTargetFrame(vtkMatrix4x4 *frame);
  void SetTargetFrame(vtkLinearTransform *frame);
  vtkLinearTransform *GetTargetFrame() {return this->TargetFrame;}

	/**
	Polar Decomposition of matrix M in Q * S.*/
	float PolarDecomp(vtkMatrix4x4 *M, vtkMatrix4x4 *Q, vtkMatrix4x4 *S, float translation[3]);
  
  /** set internal matrix to Identity */
  void Identity();

  /**
	Get the x, y, z orientation angles from the transformation matrix as an
  array of three floating point values. Copied from vtkTransform::GetOrientation()*/
  static void GetOrientation(vtkMatrix4x4 *in_matrix,double orientation[3]);
  static void GetOrientation(vtkMatrix4x4 *in_matrix,float orientation[3]);
  void GetOrientation(double orientation[3]) { this->GetOrientation(this->GetMatrix(),orientation);}
  void GetOrientation(float orient[3]) {
    double temp[3]; this->GetOrientation(temp); 
    orient[0] = static_cast<float>(temp[0]); 
    orient[1] = static_cast<float>(temp[1]); 
    orient[2] = static_cast<float>(temp[2]); };

  /**
	Return the wxyz angle+axis representing the current orientation.
  Copied from vtkTransform::GetVTKOrientationWXYZ()*/
  static void GetOrientationWXYZ(vtkMatrix4x4 *in_matrix, double wxyz[4]);
  void GetOrientationWXYZ(double wxyz[4]) \
      { this->GetOrientationWXYZ(this->GetMatrix(),wxyz);}
  void GetOrientationWXYZ(float wxyz[3]) {
    double temp[4]; this->GetOrientationWXYZ(temp); 
    wxyz[0]=static_cast<float>(temp[0]); 
    wxyz[1]=static_cast<float>(temp[1]); 
    wxyz[2]=static_cast<float>(temp[2]); 
    wxyz[3]=static_cast<float>(temp[3]);};

  /**
  Return the position from the current transformation matrix as an array
  of three floating point numbers. This is simply returning the translation 
  component of the 4x4 matrix. Copied from vtkTransform::GetPosition()*/
  static void GetPosition(vtkMatrix4x4 *matrix,double position[3]);
  static void GetPosition(vtkMatrix4x4 *matrix,float position[3]);
  void GetPosition(double position[3]) {this->GetPosition(this->GetMatrix(),position);}
  void GetPosition(float position[3]) {
    double temp[3]; this->GetPosition(temp); 
    position[0] = static_cast<float>(temp[0]); 
    position[1] = static_cast<float>(temp[1]); 
    position[2] = static_cast<float>(temp[2]); };

  /**
  This function set the position column of the matrix. Notice, the non static functions
  work on the internal matrix and can be used in conjuction with SetMatrix, but if a pipeline
  is defined (input, input_frame or target_frame) it's overwritten whenever the Update() is perfromed.*/
  static void SetPosition(vtkMatrix4x4 *matrix,double position[3]);
  void SetPosition(double position[3]) {this->SetPosition(this->GetMatrix(),position);}
  void SetPosition(double x,double y,double z) {this->SetPosition(this->GetMatrix(),x,y,z);}
  static void SetPosition(vtkMatrix4x4 *matrix, double x,double y,double z) {
    double temp[3];
    temp[0]=x;temp[1]=y;temp[2]=z;
    SetPosition(matrix,temp);}

  /**
  This function set the translation column of the matrix by adding the transaltion provided
  as argument. Notice, the non static functions work on the internal matrix and can be used
  in conjuction with SetMatrix, but if a pipeline  is defined (input, input_frame or 
  target_frame) it's overwritten whenever the Update() is perfromed.*/
  static void Translate(vtkMatrix4x4 *matrix,double translation[3],int premultiply);
  void Translate(double translation[3],int premultiply) {this->Translate(this->GetMatrix(),translation,premultiply);}
  void Translate(double x,double y,double z,int premultiply) {this->Translate(this->GetMatrix(),x,y,z,premultiply);}
  static void Translate(vtkMatrix4x4 *matrix, double x,double y,double z,int premultiply) {
    double temp[3];
    temp[0]=x;temp[1]=y;temp[2]=z;
    Translate(matrix,temp,premultiply);}

  /**
  This function rotate the matrix around the specified axis. Notice, the non static functions
  work on the internal matrix and can be used in conjuction with SetMatrix, but if a pipeline
  is defined (input, input_frame or target_frame) it's overwritten whenever the Update() is perfromed.*/
  static void RotateWXYZ(vtkMatrix4x4* source,vtkMatrix4x4 *target,double angle,double x, double y, double z,int premultiply);
  void RotateWXYZ(double angle,double x, double y, double z,int premultiply) {this->RotateWXYZ(this->GetMatrix(),this->GetMatrix(),angle,x,y,z,premultiply);}
  void RotateWXYZ(double angle,double rot[3],int premultiply) {this->RotateWXYZ(this->GetMatrix(),this->GetMatrix(),angle, rot[0], rot[1], rot[2],premultiply);}


  /**
  This functions rotate the internal matrix around the specified axis. It can be used in conjuction with
  SetMatrix, but if a pipeline is defined (input, input_frame or target_frame) it's overwritten whenever
  the Update() is performed.*/
  static void RotateX(vtkMatrix4x4 *matrix,double angle,int premultiply) { mflTransform::RotateWXYZ(matrix,matrix,angle, 1, 0, 0,premultiply); };
  static void RotateY(vtkMatrix4x4 *matrix,double angle,int premultiply) { mflTransform::RotateWXYZ(matrix,matrix,angle, 0, 1, 0,premultiply); };
  static void RotateZ(vtkMatrix4x4 *matrix,double angle,int premultiply) { mflTransform::RotateWXYZ(matrix,matrix,angle, 0, 0, 1,premultiply); };
  void RotateX(double angle,int premultiply) { this->RotateX(this->GetMatrix(),angle,premultiply); };
  void RotateY(double angle,int premultiply) { this->RotateY(this->GetMatrix(),angle,premultiply); };
  void RotateZ(double angle,int premultiply) { this->RotateZ(this->GetMatrix(),angle,premultiply); };

  /** Pre or Post multiply the  internal matrix for given matrix and store result in the internal matrix */
  void Concatenate(vtkMatrix4x4 *matrix, int premultiply);

  /** Pre or Post multiply the  internal matrix for given transform and store result in the internal matrix */
  void Concatenate(vtkLinearTransform *trans, int premultiply) {Concatenate(trans->GetMatrix(),premultiply);}

  /**
  This function set the orientation (acc. to VTK convention) of the matrix. Notice, the non static functions
  work on the internal matrix and can be used in conjuction with SetMatrix, but if a pipeline
  is defined (input, input_frame or target_frame) it's overwritten whenever the Update() is performed.*/
  static void SetOrientation(vtkMatrix4x4 *matrix,double orientation[3]);
  void SetOrientation(double orientation[3]) {this->SetOrientation(this->GetMatrix(),orientation);}
  void SetOrientation(double rx,double ry,double rz) {this->SetOrientation(this->GetMatrix(),rx,ry,rz);}
  static void SetOrientation(vtkMatrix4x4 *matrix,double rx,double ry,double rz) {
    double temp[3];
    temp[0]=rx;temp[1]=ry;temp[2]=rz;
    SetOrientation(matrix,temp);}

  /**
  Return the scale factors of the current transformation matrix as 
  an array of three float numbers.  These scale factors are not necessarily
  about the x, y, and z axes unless unless the scale transformation was
  applied before any rotations. Copied from vtkTransform::GetScale()*/
  static void GetScale(vtkMatrix4x4 *matrix,double scale[3]);
  void GetScale(double scale[3]) {this->GetScale(this->GetMatrix(),scale);}
  void GetScale(float scale[3]) {
    double temp[3]; this->GetScale(temp); 
    scale[0] = static_cast<float>(temp[0]); 
    scale[1] = static_cast<float>(temp[1]); 
    scale[2] = static_cast<float>(temp[2]); };

  /** apply a scale transform. By default the scale matrix is premultiplied */
  static void Scale(vtkMatrix4x4 *matrix,double scalex,double scaley,double scalez,int premultiply);
  void Scale(double scalex,double scaley,double scalez,int premultiply) \
    { Scale(Matrix,scalex,scaley,scalez,premultiply);}
  /**
  Extract the given matrix versor*/
  static void GetVersor(int axis, const vtkMatrix4x4 *matrix, double versor[3]);
  static void GetVersor(int axis, const vtkMatrix4x4 *matrix, float versor[3]) {
    double temp[3]; mflTransform::GetVersor(axis, matrix, temp); 
    versor[0] = static_cast<float>(temp[0]); 
    versor[1] = static_cast<float>(temp[1]); 
    versor[2] = static_cast<float>(temp[2]); };
  
  /** */
  void GetVersor(int axis, double versor[3]) {GetVersor(axis,this->GetMatrix(),versor);}
  void GetVersor(int axis, float versor[3]) {
    double temp[3]; this->GetVersor(axis, this->GetMatrix(), temp);
    versor[0] = static_cast<float>(temp[0]); 
    versor[1] = static_cast<float>(temp[1]); 
    versor[2] = static_cast<float>(temp[2]); };


  /**
  Copy the 3x3 rotation matrix from another 4x4 matrix into the specified matrix, or in the internal matrix.*/
  static void CopyRotation(const vtkMatrix4x4 *source, vtkMatrix4x4 *target);
  void CopyRotation(const vtkMatrix4x4 *source) {this->CopyRotation(source,this->Matrix);}

  /** Copy the translation vector */
  static void CopyTranslation(const vtkMatrix4x4 *source, vtkMatrix4x4 *target);
  void CopyTranslation(const vtkMatrix4x4 *source) {this->CopyTranslation(source,this->Matrix);}


  /** */
	int ConversionRToHelicalAxis(vtkMatrix4x4 *matrix,
											 double helical_axis[3],double& angle);

  /** */
	int ConversionRToAttitudeVector(vtkMatrix4x4 *matrix,
										        double attitude_vector[3]);

  /** */
	int ConversionRToEulerCardanicAngle(vtkMatrix4x4 *matrix,
													int i,int j,int k,
													double euler_cardan[3],
													double tentative_euler_cardan_first,
													double tentative_euler_cardan_second,
													double tentative_euler_cardan_third);

  /** */
	int ConversionRToQuaternion(vtkMatrix4x4 *matrix, double quaternion[4]);

  /** */
	int ConversionQuaternionToR(double quaternion[4],	vtkMatrix4x4 *matrix);

  /** */
	int ConversionHelicalAxisToR(double helical_axis[3],double angle, vtkMatrix4x4 *matrix);

  /** */
	int ConversionAttitudeVectorToR(double attitude_vector[3], vtkMatrix4x4 *matrix);

  /** */
	int ConversionEulerCardanicAngleToR(double euler_cardan[3],
													int i,int j,int k,
													vtkMatrix4x4 *matrix);

  /** */
	int ConversionTranslationMatrixToHelicalAxis(vtkMatrix4x4 *v_matrix,
										 											 double helical_axis[3],double point[3],
																					 double& phi,double& t, int intersect);
protected:
  mflTransform();
  ~mflTransform();

  void InternalUpdate();
  void InternalDeepCopy(vtkAbstractTransform *transform);

  void SetInputMatrix();

  int InverseFlag;
  vtkLinearTransform  *Input;

  vtkLinearTransform  *InputFrame;
  vtkLinearTransform  *TargetFrame;

  double			sign(double a); 

private:
  mflTransform(const mflTransform&);  // Not implemented.
  void operator=(const mflTransform&);  // Not implemented.

	

	

};

#endif
