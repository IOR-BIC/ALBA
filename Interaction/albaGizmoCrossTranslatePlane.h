/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoCrossTranslatePlane
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 

#ifndef __albaGizmoCrossTranslatePlane_H__
#define __albaGizmoCrossTranslatePlane_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGizmoInterface.h"
#include "vtkConeSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCylinderSource.h"
#include "vtkAppendPolyData.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaInteractorGenericMouse;
class albaInteractorCompositorMouse;
class albaVME;
class albaVMEGizmo;
class albaMatrix;
class vtkLineSource;
class vtkPlaneSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkTubeFilter;
//----------------------------------------------------------------------------

/** Basic gizmo component used to perform constrained translation on a plane.
  
  @sa albaGizmoTranslate 
*/
class albaGizmoCrossTranslatePlane: public albaGizmoInterface 
{
public:
           albaGizmoCrossTranslatePlane(albaVME *input, albaObserver *listener = NULL);
  virtual ~albaGizmoCrossTranslatePlane(); 
  
  /** 
  Set the gizmo generating vme; the gizmo will be centered on this vme*/
  void SetInput(albaVME *vme); 
  albaVME *GetInput() {return this->m_InputVme;};

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------
  
  /** Set the event receiver object*/
  void  SetListener(albaObserver *Listener) {m_Listener = Listener;};
  
  /** Events handling*/        
  virtual void OnEvent(albaEventBase *alba_event);
  
  //----------------------------------------------------------------------------
  // axis setting 
  //----------------------------------------------------------------------------

  /** Plane enum*/
  enum PLANE {X_NORMAL = 0, Y_NORMAL, Z_NORMAL};
  
  /** Set/Get gizmo plane, default plane is X_NORMAL*/        
  void SetPlane(int axis); 
  
  /** Get gizmo Plane*/
  int  GetPlane() {return m_ActivePlane;}; 
  
  //----------------------------------------------------------------------------
  // highlight and show 
  //----------------------------------------------------------------------------
  
  /** Highlight gizmo*/
  void Highlight(bool highlight);
    
  /** Show plane translation gizmo */
  void Show(bool show);

  /** Show the translation feedback arrows  */
  void ShowTranslationFeedbackArrows(bool show);
  
  //----------------------------------------------------------------------------
  // activation status 
  //----------------------------------------------------------------------------

  /** Set/Get the activation status of the gizmo, When the gizmo is active
  it is sending pose matrices to the listener */
  void SetIsActive(bool highlight) {m_IsActive = highlight;};
  bool GetIsActive() {return m_IsActive;}

   /** 
  Set the abs pose */
  void SetAbsPose(albaMatrix *absPose);
  albaMatrix *GetAbsPose();

  /** 
  Set the constrain ref sys */
  void SetConstrainRefSys(albaMatrix *constrain);
 
  /**
  Set the constraint modality for the given axis; allowed constraint modality are:
  LOCK, FREE, BOUNDS, SNAP_STEP, SNAP_ARRAY.*/
  void SetConstraintModality(int axis, int constrainModality);

  /** Set the step value for snap step constraint type for the given axis*/
  void SetStep(int axis, double step);

  /** Gizmo color setting facilities for gizmo segments;*/
  void SetColor(int part, double col[3]);
  void SetColor(int part, double colR, double colG, double colB);

    /**

      z              z             y
      ^              ^             ^
      |              |             |
      |              |             |
      |              |             |
       -------> y     ------->x     -------> x
  
         YZ              XZ            XY

      z
      ^  S2
      |-----
      |     |         
      | SQ  |S1          
      |     |         
       --------> y    
  */

  enum GIZMOPARTS {S0 = 0, S1, NUM_GIZMO_PARTS};
  enum GIZMO_STATUS {SELECTED = 0, NOT_SELECTED};

protected:

  /** Set/Get the side length of the gizmo*/
  void   SetSizeLength(double length);
  double GetSizeLength() {return m_Length;};

  double m_Color[3][2];
  double m_LastColor[3][2];

  /** Segments gizmo */
  albaVMEGizmo *m_Gizmo[3];

  /** Register input vme*/
  albaVME *m_InputVme;

  
  /** Register the gizmo plane */
  int m_ActivePlane;
 
  /** Register the gizmo square plane side length*/
  double m_Length;

  /** Line source*/
  vtkLineSource *m_Line[2];

  /** Tube filter for lines */
  vtkTubeFilter *m_LineTF[2];

  /** rotate PDF for gizmo parts */
  vtkTransformPolyDataFilter *m_RotatePDF[3];

  /** rotation transform for cylinder and cone*/
  vtkTransform *m_RotationTr;
  
  /** Create vtk objects needed */
  void CreatePipeline();

  /** Create feedback gizmo vtk objects */
  void CreateFeedbackGizmoPipeline();

  /** Create isa stuff */
  void CreateISA();

  /** isa compositor*/
  albaInteractorCompositorMouse *m_IsaComp[2];

  /** isa generic*/
  albaInteractorGenericMouse *m_IsaGen[2];

  /** Used by albaInteractorGenericMouse */
  vtkTransform *m_PivotTransform;

  /**
  Register the event receiver object*/
  albaObserver *m_Listener;

  /** Register Gizmo status*/
  bool m_IsActive;

  /** Translation feedback arrows gizmo*/
  albaVMEGizmo *m_TranslationFeedbackGizmo;

  vtkConeSource *m_FeedbackConeSource;

  vtkTransform *m_LeftFeedbackConeTransform;
  vtkTransform *m_RightFeedbackConeTransform;
  vtkTransform *m_UpFeedbackConeTransform;
  vtkTransform *m_DownFeedbackConeTransform;

  vtkTransformPolyDataFilter *m_LeftFeedbackConeTransformPDF;
  vtkTransformPolyDataFilter *m_RightFeedbackConeTransformPDF;
  vtkTransformPolyDataFilter *m_UpFeedbackConeTransformPDF;
  vtkTransformPolyDataFilter *m_DownFeedbackConeTransformPDF;

  vtkCylinderSource *m_FeedbackCylinderSource;

  vtkTransform *m_HorizontalFeedbackCylinderTransform;
  vtkTransform *m_VerticalFeedbackCylinderTransform;

  vtkTransformPolyDataFilter *m_VerticalFeedbackCylinderTransformPDF;
  vtkTransformPolyDataFilter *m_HorizontalFeedbackCylinderTransformPDF;
  
  vtkAppendPolyData *m_FeedbackStuffAppendPolydata;

  friend class albaGizmoTranslatePlaneTest;
};
#endif
