/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGizmoCrossTranslateAxis.h,v $
  Language:  C++
  Date:      $Date: 2011-01-08 17:06:37 $
  Version:   $Revision: 1.1.2.5 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
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

#ifndef __medGizmoCrossTranslateAxis_H__
#define __medGizmoCrossTranslateAxis_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGizmoInterface.h"
#include "vtkConeSource.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafInteractorGenericMouse;
class mafInteractorCompositorMouse;
class mafVMEGizmo;
class mafMatrix;
class mafVME;
class vtkConeSource;
class vtkCylinderSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkAppendPolyData;

/** Basic gizmo component used to perform constrained translation on one axis.
  
  @sa medGizmoCrossTranslate 
*/
class medGizmoCrossTranslateAxis: public mafGizmoInterface 
{
public:
           medGizmoCrossTranslateAxis(mafVME *input, mafObserver *listener = NULL);
  virtual ~medGizmoCrossTranslateAxis(); 
  
  /** 
  Set the gizmo generating vme; the gizmo will be centered on this vme*/
  void SetInput(mafVME *vme); 
  mafVME *GetInput() {return this->m_InputVme;};

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------
  
  /** Set the event receiver object*/
  void  SetListener(mafObserver *Listener) {m_Listener = Listener;};
  
  /** Events handling*/        
  virtual void OnEvent(mafEventBase *maf_event);
  
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
  void SetAbsPose(mafMatrix *absPose);
  mafMatrix *GetAbsPose();

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
  mafVMEGizmo *m_TranslationCylinderGizmo;

  
  double m_Color[3];
  double m_LastColor[3];

  /** Translation feedback arrows gizmo*/
  mafVMEGizmo *m_TranslationFeedbackGizmo;

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
  void SetRefSysMatrix(mafMatrix *constrain);
  
  /** Register input vme*/
  mafVME *m_InputVme;

  /** Register the gizmo axis */
  int m_Axis;
 
  /** Cylinder source*/
  vtkCylinderSource *m_RightCylinder;
  
  /** Cylinder length*/
  double m_CylinderLength;

  /** Cylinder and cone gizmo vme data*/
  //mafVmeData *GizmoData[2];

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
  mafInteractorCompositorMouse *m_IsaComp;

  /** isa generic*/
  mafInteractorGenericMouse *m_IsaGen;

  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  /** test friend */
  friend class mafGizmoTranslateAxisTest;
};
#endif