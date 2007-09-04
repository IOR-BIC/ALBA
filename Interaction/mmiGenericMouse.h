/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiGenericMouse.h,v $
  Language:  C++
  Date:      $Date: 2007-09-04 13:09:10 $
  Version:   $Revision: 1.7 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiGenericMouse_h
#define __mmiGenericMouse_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmiGenericInterface.h"
#include "mmiConstraint.h"

//----------------------------------------------------------------------------
//forward ref
//----------------------------------------------------------------------------
class mmdMouse;
class mafMatrix;

class vtkCamera;
class vtkDoubleArray;

/** Constrained interaction with mouse device.
  Use mmiCompositorMouse in order to associate instances of this class
  to mouse buttons with keys modifiers.

  @sa
  - mmoTransformGeneric for examples on how to use this class with mmiCompositorMouse
*/
class mmiGenericMouse : public mmiGenericInterface
{
public:
  mafTypeMacro(mmiGenericMouse, mafInteractor);
  
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
  void SendTransformMatrix(const mafMatrix &matrix, int mouseAction = MOUSE_MOVE, double rotationAngle = 0);  

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

  mmiGenericMouse();
  ~mmiGenericMouse();

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
  mafMatrix *m_ConstrainRefSys;

  // the input refsys matrix
  mafMatrix *m_InputRefSys;
  
  //the target refsys type
  int m_TargetRefSysType;

  //----------------------------------------------------------------------------
  // result matrix: the matrix to keep updated 
  //----------------------------------------------------------------------------
  // Register the pointer to the matrix to be updated
  mafMatrix *m_ResultMatrix;
  
  int m_ResultMatrixConcatenationSemantic;

  enum
  {
    PREMULTIPLY,
    POSTMULTIPLY,  
  };

  //----------------------------------------------------------------------------
  // trackball interaction style stuff 
  //----------------------------------------------------------------------------

  void TrackballRotate();
  void TrackballTranslate();
  void TrackballRoll();

  //----------------------------------------------------------------------------
  // snap on surface
  //----------------------------------------------------------------------------
  void SnapOnSurface();


	//----------------------------------------------------------------------------
	// normal on surface
	//----------------------------------------------------------------------------
	void NormalOnSurface();

 

private:

  mmiGenericMouse(const mmiGenericMouse&);  // Not implemented.
  void operator=(const mmiGenericMouse&);   // Not implemented.

  double  m_LastPickPosition[3]; 
        
  // Build vector with origin in p1 pointing to p2
  void BuildVector(double *p1, double *p2, double *vec)
  {
    if (vec)
    {
        vec[0] = p2[0] - p1[0];
        vec[1] = p2[1] - p1[1];
        vec[2] = p2[2] - p1[2];
    }
  }

  // Build vector [coeff * inVector];
  void BuildVector(double coeff, const double *inVector, double *outVector, int refSysType = mafRefSys::LOCAL, int localAxis = mmiConstraint::X);
 

  // Project in_vector on in_axis direction; in_axis does not need to be 
  // normalised. The projection signed value is returned
  double ProjectVectorOnAxis(const double *in_vector, const double *in_axis, double *projection = NULL);

  // Project in_vector on the plane identified by the normal vector in_plane_normal;
  // in_plane_normal does not need to be normalised. The norm of the projection 
  // is returned and the projection vector is written in out_projection vector if provided.
  double ProjectVectorOnPlane(const double *in_vector, const double *in_plane_normal, double *out_projection = NULL);

  // Projection Accumulator; accumulates projections of the motion
  // vector along mouse move events 
  //   
  // P1  P2   P3      PN
  // ->----->----->-------->
  // -------m_ProjAcc-------->
  //
  double m_ProjAcc;

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
  void ConcatenateToResultMatrix(const mafMatrix &matrix);

  bool m_ResultMatrixConcatenation;

  //----------------------------------------------------------------------------
  // trackball interaction style stuff 
  //----------------------------------------------------------------------------

  int m_LastX, m_LastY;

  //----------------------------------------------------------------------------
  
  int m_MousePose[2];
  int m_LastMousePose[2];
  int m_ButtonPressed;
  
  vtkCamera *m_CurrentCamera; ///< Stores camera to which the interaction is currently assigned

};
#endif
