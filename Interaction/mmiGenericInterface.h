/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiGenericInterface.h,v $
  Language:  C++
  Date:      $Date: 2005-05-21 07:55:51 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiGenericInterface_h
#define __mmiGenericInterface_h

#include "mafInteractor.h"
#include "mmiConstraint.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafRefSys;
class mafTransform;
class vtkRenderer;

/** Abstract class for general purpose interactor.
  This abstract class defines the interface for a general purpose interactor.
  Specialized classes should be able to listen to different kind of input
  devices and implement all the interface functions. Among the other features we have:
  translation constrains, like projection on an axis or plane, and rotational constrains
  like rotation around a single axis or translation snapping on specified positions
  or rotation snap on specified angles. It can also provide movement snapped on predefined
  positions.
  
  When working unconstrained (default) the interactor makes objects rotate around their center and
  translate along view axis (i.e. aligned as camera's ViewTransform). This means the TranslationConstraintts
  has a reference system set to "CAMERA" and the RotationConstraintt has the reference system set to "OBJECT".
  The interactor uses the pointer to the Renderer to retrieve the camera's ViewTransform and
  retrieve the object position from the  VME pointer. In case this last is NULL the identity
  (i.e. rotation around the world center is used)

  The interactor produces an output matrix (sent by means of MoveActionEvent to the listener)
  which can be post-multiplied to the original one to obtain a transformed matrix.

  By setting the ResultMatrix the interactor will automatically concatenate the transformation
  matrix to the ResultMatrix.

  @sa
  - mmiConstraint
 
  @todo
  - togliere shortcut per i vari ref sys (ved todo mafRefSys) 
  - fare esempi semplici da usare come tutorial 
*/
class mmiGenericInterface : public mafInteractor
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  
  //MAF_ID_DEC(MOVE_EVENT); ///< Issued when a move action has been performed

  mafAbstractTypeMacro(mmiGenericInterface,mafInteractor);

  /**
    Get the Target ref sys, i.e. the object hosting the output matrix 
    reference system. You can change such a reference system by setting its
    reference matrix or copying from another reference system*/
  mafRefSys *GetTargetRefSys();
  /** 
    Set the Target reference system, i.e. the frame with respect to which is
    expressed the output transform.Notice, the argument is copied and not referenced, but
    transform stored in the mafRefSys are always referenced! (DEFAULT GLOBAL)*/
  void SetTargetRefSys(mafRefSys &ref_sys);
  void SetTargetRefSys(mafRefSys *ref_sys);
  
  /** 
    Set the Pivot frame. The origin of this frame is used as a pivot point 
    for rotation transform and as snapping point.
    Notice, the argument is copied and not referenced, but transform stored
    in the mafRefSys are always referenced!*/ 
  void SetPivotRefSys(mafRefSys &pivot_frame );
  void SetPivotRefSys(mafRefSys *pivot_frame );
  /** 
    Return the Pivot frame. The origin of this frame is used as a pivot point 
    for rotation transform. */
  mafRefSys *GetPivotRefSys() {return m_PivotRefSys;}

  //----------------------------------------------------------------------------
  // Transform Enabling:
  //----------------------------------------------------------------------------

  /**
    Set the translation/rotation/scale to on/off
  */
  void EnableTranslation(bool enable);
  void EnableRotation(bool enable);
  void EnableScaling(bool enable);
  void EnableUniformScaling(bool enable);

  /** 
    Return the translation flag
  */
  bool GetTranslationFlag() {return m_TranslationFlag;}
  bool GetRotationFlag() {return m_RotationFlag;}
  bool GetScalingFlag() {return m_ScalingFlag;} 
  bool GetUniformScalingFlag() {return m_UniformScalingFlag;}
        
  
  //----------------------------------------------------------------------------
  // Constraint Set/Get:
  //----------------------------------------------------------------------------

  /**
    Get the translation constraint. To set the constraint retrieve the constrain object and set it.*/
  mmiConstraint *GetTranslationConstraint() {return m_TranslationConstraint;}
 
  /** 
    Set the translation constraint. Notice the constraint is copied and not referenced */
  void SetTranslationConstraint(mmiConstraint *constrain);
      
  /** 
    Get the rotation constraint. To set the constraint retrieve the constrain object and set it.*/
  mmiConstraint *GetRotationConstraint() {return m_RotationConstraint;}
  
  /** 
    Set the rotation constraint. Notice the constraint is copied and not referenced */
  void SetRotationConstraint(mmiConstraint *constrain);

  /**
    Get the scale constraint. To set the constraint retrieve the constrain object and set it.*/
  mmiConstraint *GetScaleConstraint() {return m_ScaleConstraint;}
  /** 
    Set the scaling constraint. Notice the constraint is copied and
    not referenced */
  void SetScaleConstraint(mmiConstraint *constrain);
             
   //----------------------------------------------------------------------------

  /**
    The Result Matrix is an optional matrix to which the interactor concatenates the
    OutputTransform at each interaction. The result matrix is referenced. The USER may set it e.g. 
    to have the interactor implicitly
    control a vme.*/
  void SetResultMatrix(mafMatrix *result);
  mafMatrix *GetResultMatrix();

  /**
    The Result Transform is an optional transform to which the interactor concatenates the
    OutputTransform at each interaction. The USER may set it to have the interactor implicitly
    control a vme.*/
  void SetResultTransform(mafTransform *result);
  mafTransform *GetResultTransform() { return m_ResultTransform; }

  /** redefined to set the renderer also in the constraint */
  virtual void SetRenderer(vtkRenderer *ren);

  /** Set the node to be transformed */
  virtual void SetVME(mafVME *vme);

  /** If Surface Snap modifier is on translation and rotation will be constrained to picked surfaces if possible*/
  void SetSurfaceSnap(bool flag) {m_SurfaceSnap=flag;}
  bool GetSurfaceSnap() {return m_SurfaceSnap;}
  void SurfaceSnapOn() {SetSurfaceSnap(true);}
  void SurfaceSnapOff() {SetSurfaceSnap(false);}

protected:
  mmiGenericInterface();
  virtual ~mmiGenericInterface();
  
 /** Dervived classes should override this methods in order to set internal flags in
  a consistent way. For example the mouse can only translate or rotate during a single 
  mouse move action so setting translation flag to true must set the other flags to false
  while the tracker can translate and rotate in a single move event so it can have both
  translate and rotate flags set to true*/
  virtual void EnableTranslationInternal(bool enable);
  virtual void EnableRotationInternal(bool enable);
  virtual void EnableScalingInternal(bool enable);
  virtual void EnableUniformScalingInternal(bool enable);

  mmiConstraint  *m_TranslationConstraint; ///< the constrain for translation
  mmiConstraint  *m_RotationConstraint;    ///< the constrain for rotation
  mmiConstraint  *m_ScaleConstraint;       ///< the constrain for scaling
  
  mafRefSys      *m_TargetRefSys; ///< the target ref_sys matrix
  mafRefSys      *m_PivotRefSys; ///< the pivot point (only position is considered)
  mafTransform   *m_ResultTransform; ///< store the pointer to the matrix to be updated

  bool            m_TranslationFlag; 
  bool            m_RotationFlag;
  bool            m_ScalingFlag;
  bool            m_UniformScalingFlag;

  bool            m_SurfaceSnap; //< toggle surface snap
    
private:
  mmiGenericInterface(const mmiGenericInterface&);  // Not implemented.
  void operator=(const mmiGenericInterface&);  // Not implemented.
};

#endif 

