/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoCrossTranslateAxis
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

#ifndef __albaGizmoCrossTranslateAxis_H__
#define __albaGizmoCrossTranslateAxis_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGizmoInterface.h"
#include "vtkConeSource.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaInteractorGenericMouse;
class albaInteractorCompositorMouse;
class albaVMEGizmo;
class albaMatrix;
class albaVME;
class vtkConeSource;
class vtkCylinderSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkAppendPolyData;

/** Basic gizmo component used to perform constrained translation on one axis.
  
  @sa albaGizmoCrossTranslate 
*/
class albaGizmoCrossTranslateAxis: public albaGizmoInterface 
{
public:
           albaGizmoCrossTranslateAxis(albaVME *input, albaObserver *listener = NULL);
  virtual ~albaGizmoCrossTranslateAxis(); 
  
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

  /** Axis enum*/
  enum AXIS {X = 0, Y, Z};
  
  /** Set/Get gizmo axis, default axis is X*/        
  void SetAxis(int axis); 
  int  GetAxis() {return m_Axis;}; 
  
  //----------------------------------------------------------------------------
  // highlight and show 
  //----------------------------------------------------------------------------

  /** Highlight the gizmo*/
  void Highlight(bool highlight);
    
  /** Show the gizmo */
  void Show(bool show);
 
  /** Show the translation feedback arrows  */
  void ShowTranslationFeedbackArrows(bool show);
  //----------------------------------------------------------------------------
  // cylinder stuff
  //----------------------------------------------------------------------------
 
  /** Set/Get the length of the cylinder*/
  void   SetCylinderLength(double length);
  double GetCylinderLength() {return m_CylinderLength;};
  
  /** 
  Set the abs pose */
  void SetAbsPose(albaMatrix *absPose);
  albaMatrix *GetAbsPose();

  /**
  Set the constraint modality for the given axis; allowed constraint modality are:
  LOCK, FREE, BOUNDS, SNAP_STEP, SNAP_ARRAY.*/
  void SetConstraintModality(int axis, int constrainModality);

  /** Set the step value for snap step constraint type for the given axis*/
  void SetStep(int axis, double step);

  /** Gizmo color setting facilities; part can be CYLINDER or CONE*/
  void SetColor(double col[3]);
  void SetColor(double cylR, double cylG, double cylB);

protected:

  /** Pickable translation cylinder gizmo*/
  albaVMEGizmo *m_TranslationCylinderGizmo;

  
  double m_Color[3];
  double m_LastColor[3];

  /** Translation feedback arrows gizmo*/
  albaVMEGizmo *m_TranslationFeedbackGizmo;

  vtkConeSource *m_FeedbackConeSource;

  vtkTransform *m_LeftUpFeedbackConeTransform;
  vtkTransform *m_LeftDownFeedbackConeTransform;
  vtkTransform *m_RightDownFeedbackConeTransform;
  vtkTransform *m_RightUpFeedbackConeTransform;

  vtkCylinderSource *m_FeedbackCylinderSource;

  vtkTransform *m_LeftFeedbackCylinderTransform;
  vtkTransform *m_RightFeedbackCylinderTransform;

  vtkTransformPolyDataFilter *m_LeftUpFeedbackConeTransformPDF;
  vtkTransformPolyDataFilter *m_LeftDownFeedbackConeTransformPDF;
  vtkTransformPolyDataFilter *m_RightUpFeedbackConeTransformPDF;
  vtkTransformPolyDataFilter *m_RightDownFeedbackConeTransformPDF;
  
  vtkTransformPolyDataFilter *m_LeftFeedbackCylinderTransformPDF;
  vtkTransformPolyDataFilter *m_RightFeedbackCylinderTransformPDF;

  vtkAppendPolyData *m_FeedbackStuffAppendPolydata;

  /** 
  Set the constrain ref sys */
  void SetRefSysMatrix(albaMatrix *constrain);
  
  /** Register input vme*/
  albaVME *m_InputVme;

  /** Register the gizmo axis */
  int m_Axis;
 
  /** Cylinder source*/
  vtkCylinderSource *m_RightCylinder;
  
  /** Cylinder length*/
  double m_CylinderLength;

  /** Cylinder and cone gizmo vme data*/
  //albaVmeData *GizmoData[2];

  /** translate PDF for cylinder and cone*/
  vtkTransformPolyDataFilter *m_RightTranslatePDF;
  
  /** translation transform for cylinder and cone*/
  vtkTransform *m_RightTranslateTr;
 
  /** rotate PDF for cylinder and cone*/
  vtkTransformPolyDataFilter *m_RightCylinderRotatePDF;

  /** rotation transform for cylinder and cone*/
  vtkTransform *m_RightCylinderRotationTr;
  
  /** Cylinder source*/
  vtkCylinderSource *m_LeftCylinder;

  /** translate PDF for cylinder and cone*/
  vtkTransformPolyDataFilter *m_LeftTranslatePDF;

  /** translation transform for cylinder and cone*/
  vtkTransform *m_LeftTranslateTr;

  /** rotate PDF for cylinder and cone*/
  vtkTransformPolyDataFilter *m_LeftCylinderRotatePDF;

  /** rotation transform for cylinder and cone*/
  vtkTransform *m_LeftCylinderRotationTr;

  /** append two cylinders */
  vtkAppendPolyData *m_Append;

  /** Create vtk objects needed*/
  void CreateTranslationGizmoPipeline();

  /** Create isa stuff */
  void CreateISA();

  /** Create translation feedback arrows stuff */
  void CreateFeedbackGizmoPipeline();
  
  /** isa compositor*/
  albaInteractorCompositorMouse *m_IsaComp;

  /** isa generic*/
  albaInteractorGenericMouse *m_IsaGen;

  /**
  Register the event receiver object*/
  albaObserver *m_Listener;

  /** test friend */
  friend class albaGizmoTranslateAxisTest;
};
#endif