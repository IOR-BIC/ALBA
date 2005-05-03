/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiGeneric.h,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:37 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiGeneric_h
#define __mmiGeneric_h

#include "mafInteractor.h"
#include "mmiConstraint.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafRefSys;
class vtkMatrix4x4;
class mflTransform;
class vtkAbstractTransform;
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
  @todo
  - togliere shortcut per i vari ref sys (ved todo mafRefSys) 
  - fare esempi semplici da usare come tutorial 
*/
class mmiGeneric : public mafInteractor
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  MFL_EVT_DEC(MoveActionEvent); ///< Issued a move action has been performed

  static mmiGeneric *New();
  vtkTypeMacro(mmiGeneric,mafInteractor);

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
    for rotation transform. Notice, the argument is copied and not referenced, but
    transform stored in the mafRefSys are always referenced!*/ 
  void SetPivotRefSys(mafRefSys &pivot_frame );
  void SetPivotRefSys(mafRefSys *pivot_frame );
  /** 
    Return the Pivot frame. The origin of this frame is used as a pivot point 
    for rotation transform. */
  mafRefSys *GetPivotRefSys() {return PivotRefSys;}

  //----------------------------------------------------------------------------
  // Transform Enabling:
  //----------------------------------------------------------------------------

  /**
    Set the translation/rotation/scale to on/off and assign it to a button
    or mouse button shift/ctrl combo (int mouse button = 1,2,4,8...) */
  void EnableTranslation      (bool enable) {TranslationFlag=enable;}; 
  void EnableRotation         (bool enable) {RotationFlag=enable;}; 
  void EnableScaling          (bool enable) {ScalingFlag=enable;}; 
  void EnableUniformScaling   (bool enable) {UniformScalingFlag=enable;}; 

  bool GetTranslationFlag() {return TranslationFlag;}
  bool GetRotationFlag() {return RotationFlag;}
  bool GetScalingFlag() {return ScalingFlag;} 
  bool GetUniformScalingFlag() {return UniformScalingFlag;}
        
  /**
    Translation may be constrained on a specified axis, plane or unconstrained;
    allowed constrains are  LOCK, FREE, SNAP_STEP, SNAP_ARRAY. To set the constraint
    retrieve the constrain object and set it.*/
  mmiConstraint *GetTranslationConstraint() {return TranslationConstraint;}
  /** 
    Set the translation constraint. Notice the constraint is copied and
    not referenced */
  void SetTranslationConstraint(mmiConstraint *constrain);
       
  /**
    Rotation may be constrained on a specified axis, two axis, or unconstrained;
    allowed constrains are  LOCK, FREE, SNAP_STEP, SNAP_ARRAY. To set the constraint
    retrieve the constrain object and set it.*/
  mmiConstraint *GetRotationConstraint() {return RotationConstraint;}
  /** 
    Set the rotaion constraint. Notice the constraint is copied and
    not referenced */
  void SetRotationConstraint(mmiConstraint *constrain);

  /**
    Scale may be constrained on a specified axis, two axis, or unconstrained;
    allowed constrains are  LOCK, FREE, SNAP_STEP, SNAP_ARRAY. To set the constraint
    retrieve the constrain object and set it.*/
  mmiConstraint *GetScaleConstraint() {return ScaleConstraint;}
  /** 
    Set the scaling constraint. Notice the constraint is copied and
    not referenced */
  void SetScaleConstraint(mmiConstraint *constrain);
        
  /**
    The Result Matrix is an optional matrix to which the interactor concatenates the
    OutputTransform at each interaction. The USER may set it e.g. to have the interactor implicitly
    control a vme.*/
  void SetResultMatrix(vtkMatrix4x4 *result);
  vtkMatrix4x4 *GetResultMatrix();

  /**
    The Result Transform is an optional transform to which the interactor concatenates the
    OutputTransform at each interaction. The USER may set it to have the interactor implicitly
    control a vme.*/
  void SetResultTransform(mflTransform *result);
  mflTransform *GetResultTransform() { return ResultTransform; }


  /** redefined to set the renderer also in the constraints */
  virtual void SetRenderer(vtkRenderer *ren);

  /** Set the node to be transformed */
  virtual void SetVME(mflVME *vme);

protected:
  mmiGeneric();
  virtual ~mmiGeneric();
  
  mmiConstraint  *TranslationConstraint; ///< the constrain for translation
  mmiConstraint  *RotationConstraint;    ///< the constrain for rotation
  mmiConstraint  *ScaleConstraint;       ///< the constrain for scaling
  
  mafRefSys      *TargetRefSys; ///< the target ref_sys matrix
  mafRefSys      *PivotRefSys; ///< the pivot point (only position is considered)
  mflTransform   *ResultTransform; ///< store the pointer to the matrix to be updated

  bool            TranslationFlag; 
  bool            RotationFlag;
  bool            ScalingFlag;
  bool            UniformScalingFlag;

private:
  mmiGeneric(const mmiGeneric&);  // Not implemented.
  void operator=(const mmiGeneric&);  // Not implemented.
};

#endif 

