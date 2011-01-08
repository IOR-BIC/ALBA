/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGizmoCrossRotateAxis.h,v $
  Language:  C++
  Date:      $Date: 2011-01-08 17:06:37 $
  Version:   $Revision: 1.1.2.4 $
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

#ifndef __medGizmoCrossRotateAxis_H__
#define __medGizmoCrossRotateAxis_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGizmoInterface.h"
#include "mafSmartPointer.h"
#include "vtkConeSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkDiskSource.h"
#include "vtkCleanPolyData.h"
#include "vtkTubeFilter.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafInteractorGenericInterface;
class mafInteractorGenericMouse;
class mafInteractorCompositorMouse;
class mafVMEGizmo;
class mafMatrix;
class mafVME;
class vtkLineSource;
class vtkCleanPolyData;
class vtkTubeFilter;
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkAppendPolyData;

/** 
	Basic gizmo component used to perform constrained rotation around an axis.
*/
class medGizmoCrossRotateAxis: public mafGizmoInterface
{
public:
           medGizmoCrossRotateAxis(mafVME *input, mafObserver *listener = NULL);
  virtual ~medGizmoCrossRotateAxis(); 
  
  /** 
  Set the gizmo generating vme; the gizmo will be centered on this vme*/
  void SetInput(mafVME *vme); 
  mafVME *GetInput();

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------
  
  /** Set the event receiver object*/
  void  SetListener(mafObserver *Listener);
  
  /** Events handling*/        
  virtual void OnEvent(mafEventBase *maf_event);
  
  //----------------------------------------------------------------------------
  // axis setting 
  //----------------------------------------------------------------------------

  /** axis enum*/
  enum AXIS {X = 0, Y, Z};
  
  /** Set/Get the gizmo axis, default axis is X*/        
  void SetAxis(int axis); 
  
  /** Get the gizmo axis*/
  int  GetAxis(); 
  
  //----------------------------------------------------------------------------
  // highlight and show 
  //----------------------------------------------------------------------------
  
  /** Highlight gizmo*/
  void Highlight(bool highlight);
    
  /** Show the translation gizmo */
  void Show(bool show);

  /** Show the translation feedback arrows  */
  void ShowTranslationFeedbackArrows(bool show);

  //----------------------------------------------------------------------------
  // radius setting 
  //----------------------------------------------------------------------------
  
  /** Get the radius of the gizmo*/
  double GetRadius();

  //----------------------------------------------------------------------------
  // activation status 
  //----------------------------------------------------------------------------

  /** Set/Get the activation status of the gizmo, When the gizmo is active
  it is sending pose matrices to the listener */
  void SetIsActive(bool isActive);
  bool GetIsActive();
 
  /** 
  Set the abs pose */
  void SetAbsPose(mafMatrix *absPose);
  mafMatrix *GetAbsPose();

  /**
  Get the gizmo interactor*/
  mafInteractorGenericInterface *GetInteractor();

protected:

  /**
  Set the reference system matrix and the Pivot ref sys matrix.
  Both reference system type are set to CUSTOM.*/
  void SetRefSysMatrix(mafMatrix *matrix);

  mafAutoPointer<mafMatrix> m_AbsInputMatrix;
  
  /** Circle gizmo */
  mafVMEGizmo *m_GizmoCross;

  /** Register input vme*/
  mafVME *m_InputVme;

  /**

      z              z             y
      ^              ^             ^
      |              |             |
      |              |             |
      |              |             |
       -------> y     ------->x     -------> x
  
         YZ              XZ            XY
  */

  enum GIZMO_STATUS {SELECTED = 0, NOT_SELECTED};
  
  /** Register the gizmo axis */
  int m_ActiveAxis;
  
  /** Create the circle polydata*/
  vtkLineSource *m_LineSourceEast;
  vtkLineSource *m_LineSourceWest;
  vtkLineSource *m_LineSourceNorth;
  vtkLineSource *m_LineSourceSouth;

  vtkCleanPolyData *m_LinesCleanCircle; 

  vtkTubeFilter *m_LinesTubeFilter;
  vtkAppendPolyData *m_LinesAppendPolyData;

  /** rotate PDF for gizmo */
  vtkTransformPolyDataFilter *m_LinesRotatePDF;

  /** rotation transform */
  vtkTransform *m_LinesRotationTransform;
  
  /** Create moving gizmo vtk objects*/
  void CreatePipeline();

  /** Create motion feedback gizmo vtk objects*/
  void CreateFeedbackGizmoPipeline();

  /** Create isa stuff */
  void CreateISA();

  /** isa compositor*/
  mafInteractorCompositorMouse *m_IsaComp;

  /** isa generic*/
  mafInteractorGenericMouse *m_IsaGen;

  /** Gizmo color setting facilities for gizmo segments;*/
  void SetColor(double col[3]);
  void SetColor(double colR, double colG, double colB);
  
  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  /** Register Gizmo status*/
  bool m_IsActive;
  
  /** Register gizmo radius */
  double m_Radius;

  vtkConeSource *m_FeedbackConeSource;

  vtkTransform *m_LeftUpFeedbackConeTransform;
  vtkTransform *m_LeftDownFeedbackConeTransform;
  vtkTransform *m_RightDownFeedbackConeTransform;
  vtkTransform *m_RightUpFeedbackConeTransform;

  vtkTransformPolyDataFilter *m_LeftUpFeedbackConeTransformPDF;
  vtkTransformPolyDataFilter *m_LeftDownFeedbackConeTransformPDF;
  vtkTransformPolyDataFilter *m_RightUpFeedbackConeTransformPDF;
  vtkTransformPolyDataFilter *m_RightDownFeedbackConeTransformPDF;

  vtkDiskSource *m_FGCircle;
  vtkCleanPolyData *m_FGCleanCircle;
  vtkTubeFilter *m_FGCircleTubeFilter;
  vtkTransform *m_FGRotationTransform;
  vtkTransformPolyDataFilter *m_FGRotatePDF;
  vtkTubeFilter *m_FGCircleTF;
  vtkTransform *m_FGRotationTr;

  vtkAppendPolyData *m_FeedbackStuffAppendPolydata;
  mafVMEGizmo *m_RotationFeedbackGizmo;

};
#endif
