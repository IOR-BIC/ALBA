/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorGenericMouse
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractorGenericMouse_h
#define __albaInteractorGenericMouse_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaInteractorGenericInterface.h"
#include "albaInteractorConstraint.h"

//----------------------------------------------------------------------------
//forward ref
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;
class albaMatrix;

class vtkCamera;
class vtkDoubleArray;

/** Constrained interaction with mouse device.
  Use albaInteractorCompositorMouse in order to associate instances of this class
  to mouse buttons with keys modifiers.

  @sa
  - mmoTransformGeneric for examples on how to use this class with albaInteractorCompositorMouse
*/
class ALBA_EXPORT albaInteractorGenericMouse : public albaInteractorGenericInterface
{
public:
  albaTypeMacro(albaInteractorGenericMouse, albaInteractor);
  
  //----------------------------------------------------------------------------
  // Actions performed on mouse driven events
  //----------------------------------------------------------------------------
 
  /**
  Action to be performed on button down event*/
  void OnButtonDownAction(int X, int Y);

  /**
  Action to be performed on mouse move event*/
  void OnMouseMoveAction(int X, int Y);

  /**
  Action to be performed on button up action*/
  void OnButtonUpAction();
  
  /**
  Used to notify the listener about the current mouse action performed;
  event Id field is set with this enum
  */
  enum MouseAction {MOUSE_DOWN = 0, MOUSE_MOVE, MOUSE_UP};

  // modified by Rafael. 9-9-2004
  // Made this function public
  void SendTransformMatrix(const albaMatrix &matrix, int mouseAction = MOUSE_MOVE, double rotationAngle = 0);  

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
       { this->m_ResultMatrixConcatenationSemantic = POSTMULTIPLY; }
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

  albaInteractorGenericMouse();
  ~albaInteractorGenericMouse();

  /** Superclass override; only one flag can be true*/
  void EnableTranslationInternal(bool enable);
  void EnableRotationInternal(bool enable);
  void EnableScalingInternal(bool enable);
  void EnableUniformScalingInternal(bool enable);

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

  albaInteractorGenericMouse(const albaInteractorGenericMouse&);  // Not implemented.
  void operator=(const albaInteractorGenericMouse&);   // Not implemented.
	        
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
  If the action is MOUSE_DOWN the picked position is send as vtkMatrix in
  the VtkObj field of the event.
  If the action is MOUSE_MOVE and a rotation is performed the angle of
  rotation in the single move event is sent as float in the float field of
  the event*/
  void SendTransformMatrix(double *versor, double translation, int mouseAction = MOUSE_MOVE);
  void SendTransformMatrix(double *vector, int mouseAction = MOUSE_MOVE);

  /** Concatenate the transform matrix to the result matrix 
  according to selected semantic */
  void ConcatenateToResultMatrix(const albaMatrix &matrix);

  bool m_ResultMatrixConcatenation;

};
#endif
