/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorTransform
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaBehaviorTransform_h
#define __albaBehaviorTransform_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaInteractionBehavior.h"
#include "albaInteractorConstraint.h"

//----------------------------------------------------------------------------
//forward ref
//----------------------------------------------------------------------------
class albaMatrix;
class albaRefSys;
class albaTransform;

class vtkCamera;
class vtkDoubleArray;

/** Constrained interaction with mouse device: the left button moves the VME (see OnLeftButtonDown).
  It can be set as behavior of a VME, as the gizmos do. Subclasses with more modes, like
  albaBehaviorRotateTranslate, redefine PrepareTransform, which at each button down chooses
  the transform of that button and modifier keys (flags and rotation constraint), or refuses the button.

  Among the other features we have: translation constrains, like projection on an axis or plane,
  and rotational constrains like rotation around a single axis or translation snapping on specified
  positions or rotation snap on specified angles.

  When working unconstrained (default) the interactor makes objects rotate around their center and
  translate along view axis (i.e. aligned as camera's ViewTransform). This means the TranslationConstraint
  has a reference system set to "CAMERA" and the RotationConstraint has the reference system set to "OBJECT".
  The interactor uses the pointer to the Renderer to retrieve the camera's ViewTransform and
  retrieve the object position from the VME pointer. In case this last is NULL the identity
  (i.e. rotation around the world center is used)

  The interactor produces an output matrix (sent by means of MoveActionEvent to the listener)
  which can be post-multiplied to the original one to obtain a transformed matrix.

  By setting the ResultMatrix the interactor will automatically concatenate the transformation
  matrix to the ResultMatrix.
*/
class ALBA_EXPORT albaBehaviorTransform : public albaInteractionBehavior
{
public:
  albaTypeMacro(albaBehaviorTransform, albaInteractionBehavior);

  /**
    Get the Target ref sys, i.e. the object hosting the output matrix 
    reference system. You can change such a reference system by setting its
    reference matrix or copying from another reference system*/
  albaRefSys *GetTargetRefSys();
  /** 
    Set the Target reference system, i.e. the frame with respect to which is
    expressed the output transform.Notice, the argument is copied and not referenced, but
    transform stored in the albaRefSys are always referenced! (DEFAULT GLOBAL)*/
  void SetTargetRefSys(albaRefSys &ref_sys);
  void SetTargetRefSys(albaRefSys *ref_sys);
  
  /** 
    Set the Pivot frame. The origin of this frame is used as a pivot point 
    for rotation transform and as snapping point.
    Notice, the argument is copied and not referenced, but transform stored
    in the albaRefSys are always referenced!*/ 
  void SetPivotRefSys(albaRefSys &pivot_frame );
  void SetPivotRefSys(albaRefSys *pivot_frame );
  /** 
    Return the Pivot frame. The origin of this frame is used as a pivot point 
    for rotation transform. */
  albaRefSys *GetPivotRefSys() {return m_PivotRefSys;}

  //----------------------------------------------------------------------------
  // Transform Enabling:
  //----------------------------------------------------------------------------

  /**
    Only one flag is on: the drags translate or rotate. Enabling one disables the other. */
  void EnableTranslation(bool enable);
  void EnableRotation(bool enable);

  /**
    Return the translation flag
  */
  bool GetTranslationFlag() {return m_TranslationFlag;}
  bool GetRotationFlag() {return m_RotationFlag;}
        
  
  //----------------------------------------------------------------------------
  // Constraint Set/Get:
  //----------------------------------------------------------------------------

  /**
    Get the translation constraint. To set the constraint retrieve the constrain object and set it.*/
  albaInteractorConstraint *GetTranslationConstraint() {return m_TranslationConstraint;}

  /**
    Get the rotation constraint. To set the constraint retrieve the constrain object and set it.*/
  albaInteractorConstraint *GetRotationConstraint() {return m_RotationConstraint;}

  /**
    Get the scale constraint. To set the constraint retrieve the constrain object and set it.*/
  albaInteractorConstraint *GetScaleConstraint() {return m_ScaleConstraint;}
             
   //----------------------------------------------------------------------------

  /**
    The Result Matrix is an optional matrix to which the interactor concatenates the
    OutputTransform at each interaction. The result matrix is referenced. The USER may set it e.g. 
    to have the interactor implicitly
    control a vme.*/
  void SetResultMatrix(albaMatrix *result);
  albaMatrix *GetResultMatrix();

  /**
    The Result Transform is an optional transform to which the interactor concatenates the
    OutputTransform at each interaction. The USER may set it to have the interactor implicitly
    control a vme.*/
  void SetResultTransform(albaTransform *result);
  albaTransform *GetResultTransform() { return m_ResultTransform; }

  /** redefined to set the renderer also in the constraint */
  virtual void SetRendererAndView(vtkRenderer *ren, albaView *view);

  /** Set the node to be transformed */
  virtual void SetVME(albaVME *vme);

  /** If Surface Snap modifier is on translation and rotation will be constrained to picked surfaces if possible*/
  void SetSurfaceSnap(bool flag) {m_SurfaceSnap=flag;}
  bool GetSurfaceSnap() {return m_SurfaceSnap;}
  void SurfaceSnapOn() {SetSurfaceSnap(true);}
  void SurfaceSnapOff() {SetSurfaceSnap(false);}

	/** If Surface Snap modifier is on translation and rotation will be constrained to picked surfaces if possible*/
	void SetSurfaceNormal(bool flag) {m_SurfaceNormal=flag;}
	bool GetSurfaceNormal() {return m_SurfaceNormal;}
	void SurfaceNormalOn() {SetSurfaceNormal(true);}
	void SurfaceNormalOff() {SetSurfaceNormal(false);}

  
  /** The left and middle buttons start the transform if PrepareTransform accepts them;
    the PER sets the picked position first. Return false without a renderer. */
  virtual bool OnLeftButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  virtual bool OnMiddleButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);

  /** The drags change the transform */
  virtual bool OnDrag(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);

  /** The release of the button that started the transform ends it */
  virtual bool OnLeftButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  virtual bool OnMiddleButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);

  //----------------------------------------------------------------------------
  // Actions performed on mouse driven events
  //----------------------------------------------------------------------------
 
  /**
  Action to be performed on button down event: it starts from the position picked by the PER */
  void OnButtonDownAction(int X, int Y);

  /**
  Action to be performed on mouse move event*/
  void OnMouseMoveAction(int X, int Y);

  /**
  Action to be performed on button up action*/
  void OnButtonUpAction();
  
  /**
  Used to notify the listener about the current mouse action performed;
  the Arg field of the ID_TRANSFORM event is set with this enum
  */
  enum MouseAction {MOUSE_ACTION_DOWN = 0, MOUSE_ACTION_MOVE, MOUSE_ACTION_UP};

  // modified by Rafael. 9-9-2004
  // Made this function public
  void SendTransformMatrix(const albaMatrix &matrix, int mouseAction = MOUSE_ACTION_MOVE, double rotationAngle = 0);  

  /** Set the current camera */
  virtual void SetCurrentCamera(vtkCamera *camera) {m_CurrentCamera=camera;}

  //----------------------------------------------------------------------------
  // result matrix 
  //----------------------------------------------------------------------------

  /** Set the semantic of the concatenation, default is postmultiply
  ie the transform is concatenated after all the other transformations. */
  void SetResultMatrixConcatenationSemanticToPostMultiply() 
       { this->m_ResultMatrixConcatenationSemantic = POSTMULTIPLY; }
  void SetResultMatrixConcatenationSemanticToPreMultiply()
       { this->m_ResultMatrixConcatenationSemantic = PREMULTIPLY; }
  int  GetResultMatrixConcatenationSemantic()
       { return this->m_ResultMatrixConcatenationSemantic; }

  /**
  Set/Get if the concatenation is active */
  void SetResultMatrixConcatenation(bool active) {this->m_ResultMatrixConcatenation = active;}
  bool GetResultMatrixConcatenation() {return this->m_ResultMatrixConcatenation;};

  /**
  Enable/Disable the concatenation of the transform matrix to be sent 
  to the result matrix     
  */
  void ResultMatrixConcatenationOn() {this->m_ResultMatrixConcatenation = true;};
  void ResultMatrixConcatenationOff() {this->m_ResultMatrixConcatenation = false;};

protected:

  albaBehaviorTransform();
  ~albaBehaviorTransform();

  /** Called at each button down, after setting renderer and view and resetting the current rotation
    constraint to the rotation constraint: return false if the button is not used; subclasses can choose
    here the transform of the button and of the modifier keys. Default: only the left button, with the
    configured flags and constraints. */
  virtual bool PrepareTransform(int button);

  /** Start the transform of the button, if PrepareTransform accepts it */
  bool StartTransform(int button, albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);

  /** End the transform, if started by the button */
  bool StopTransform(int button);

  int m_TransformButton;  ///< the button that started the transform, -1 if none
  albaInteractorConstraint *m_CurrentRotationConstraint; ///< the rotation constraint of the current transform, chosen by PrepareTransform
  int m_LastDragPos[2];   ///< the last position of the drags, the drags without a change of position are discarded

  /** Perform translation */
  void Translate(double *p1, double *p2);  

  /** Perform rotation */
  void Rotate(double *p1, double *p2, double *viewup);

  /** Perform scaling */
  void Scale(double *p1, double *p2, double *viewup);

  /**
  Binary search for pos into array.
  If the helper point is on the grid returns the index of the corresponding 
  grid point and set helper_point_status to ON_GRID_POINT otherwise returns
  the index of the first element array on the left of the helper point (-1
  if the helper point pos is lower than first array component) and set  
  helper_point_status to NOT_ON_GRID_POINT
         ^
         |                   
  -------A->----0-------1-----i-----|-----i+1------N-1------->-
                              A     B 
                                      
  pos = A => BinarySearch returns i && helper_point_status = ON_GRID_POINT
  pos = B => BinarySearch returns i && helper_point_status = NOT_ON_GRID_POINT*/
  int BinarySearch(double pos, vtkDoubleArray *array, int& helper_point_status);

  //----------------------------------------------------------------------------
  //manage the state of the helper point
  //----------------------------------------------------------------------------

  // Register the status of the helper point: it could be coincident with
  // one of the grid points (ON_GRID_POINT) or not (NOT_ON_GRID_POINT)
  int m_HelperPointStatus;

  enum HELPER_POINT_STATUS
  {
    NOT_ON_GRID_POINT,
    ON_GRID_POINT,
  };

  // If m_HelperPointStatus is NOT_ON_GRID_POINT this is the  index of the first array 
  // element on the left of the helper point. If m_HelperPointStatus is ON_GRID_POINT this is 
  // the array index corresponding to the helper point position 
  int m_HelpPIndex;

  albaInteractorConstraint  *m_TranslationConstraint; ///< the constrain for translation
  albaInteractorConstraint  *m_RotationConstraint;    ///< the constrain for rotation
  albaInteractorConstraint  *m_ScaleConstraint;       ///< the constrain for scaling
  
  albaRefSys      *m_TargetRefSys; ///< the target ref_sys matrix
  albaRefSys      *m_PivotRefSys; ///< the pivot point (only position is considered)
  albaTransform   *m_ResultTransform; ///< store the pointer to the matrix to be updated

  bool            m_TranslationFlag; 
  bool            m_RotationFlag;

  bool            m_SurfaceSnap; //< toggle surface snap
	bool            m_SurfaceNormal; //< toggle surface snap

  //the constrain ref sys matrix
  albaMatrix *m_ConstrainRefSys;

  //----------------------------------------------------------------------------
  // result matrix: the matrix to keep updated 
  //----------------------------------------------------------------------------
  // Register the pointer to the matrix to be updated
  albaMatrix *m_ResultMatrix;
  
  int m_ResultMatrixConcatenationSemantic;

  enum
  {
    PREMULTIPLY,
    POSTMULTIPLY,  
  };

  //----------------------------------------------------------------------------
  // trackball interaction style stuff 
  //----------------------------------------------------------------------------

	virtual void TrackballRotate();
	virtual void TrackballTranslate();
	virtual void TrackballRoll();

  //----------------------------------------------------------------------------
  // snap on surface
  //----------------------------------------------------------------------------
  void SnapOnSurface();

  /** Pick the surface under the mouse, ignoring the actors of the moved VME; return true if something is picked */
  bool PickUnderVME(double pickedPos[3], albaVME *&pickedVme);


	//----------------------------------------------------------------------------
	// normal on surface
	//----------------------------------------------------------------------------
	void NormalOnSurface();

	//----------------------------------------------------------------------------
	// trackball interaction style stuff 
	//----------------------------------------------------------------------------

	int m_LastX, m_LastY;

	//----------------------------------------------------------------------------

	int m_MousePointer2DPosition[2];
	int m_LastMousePointer2DPosition[2];

	vtkCamera *m_CurrentCamera; ///< Stores camera to which the interaction is currently assigned

	double  m_LastPickPosition[3];

private:

  albaBehaviorTransform(const albaBehaviorTransform&);  // Not implemented.
  void operator=(const albaBehaviorTransform&);   // Not implemented.
	        
  // Projection Accumulator; accumulates projections of the motion
  // vector along mouse move events 
  //   
  // P1  P2   P3      PN
  // ->----->----->-------->
  // -------m_ProjectionAccumulator-------->
  //
  double m_ProjectionAccumulator;

  /**
  Send the transform matrix to the listener. Also concatenate the matrix
  to m_ResultMatrix according to current m_ResultMatrix concatenation semantic. 
  If the action is MOUSE_ACTION_DOWN the picked position is send as vtkMatrix in
  the VtkObj field of the event.
  If the action is MOUSE_ACTION_MOVE and a rotation is performed the angle of
  rotation in the single move event is sent as float in the float field of
  the event*/
  void SendTransformMatrix(double *versor, double translation, int mouseAction = MOUSE_ACTION_MOVE);
  void SendTransformMatrix(double *vector, int mouseAction = MOUSE_ACTION_MOVE);

  /** Concatenate the transform matrix to the result matrix 
  according to selected semantic */
  void ConcatenateToResultMatrix(const albaMatrix &matrix);

  bool m_ResultMatrixConcatenation;

};
#endif
