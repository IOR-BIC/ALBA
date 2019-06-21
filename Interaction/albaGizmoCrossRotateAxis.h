/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoCrossRotateAxis
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

#ifndef __albaGizmoCrossRotateAxis_H__
#define __albaGizmoCrossRotateAxis_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGizmoInterface.h"
#include "albaSmartPointer.h"
#include "vtkConeSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkDiskSource.h"
#include "vtkCleanPolyData.h"
#include "vtkTubeFilter.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaInteractorGenericInterface;
class albaInteractorGenericMouse;
class albaInteractorCompositorMouse;
class albaVMEGizmo;
class albaMatrix;
class albaVME;
class vtkLineSource;
class vtkCleanPolyData;
class vtkTubeFilter;
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkAppendPolyData;

/** 
	Basic gizmo component used to perform constrained rotation around an axis.
*/
class albaGizmoCrossRotateAxis: public albaGizmoInterface
{
public:
           albaGizmoCrossRotateAxis(albaVME *input, albaObserver *listener = NULL);
  virtual ~albaGizmoCrossRotateAxis(); 
  
  /** 
  Set the gizmo generating vme; the gizmo will be centered on this vme*/
  void SetInput(albaVME *vme); 
  albaVME *GetInput();

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------
  
  /** Set the event receiver object*/
  void  SetListener(albaObserver *Listener);
  
  /** Events handling*/        
  virtual void OnEvent(albaEventBase *alba_event);
  
  //----------------------------------------------------------------------------
  // axis setting 
  //----------------------------------------------------------------------------

  /** axis enum*/
  enum AXIS {X = 0, Y, Z};
  
  /** Set/Get the gizmo axis, default axis is X*/        
  void SetRotationAxis(int axis); 
  
  /** Get the gizmo axis*/
  int  GetAxis(); 

  enum GIZMO_DIRECTION {EW = 0, NS = 1 };

  
  /** Set the gizmo visual representation: NS vs EW
  
    N
    |
    x      E---x---W
	|
	S

	x is the normal axis ie the gizmo is constraining rotation around it. Default is NS
  
  */
  void SetGizmoDirection(int direction);
  int GetGizmoDirection() {return m_GizmoDirection;};

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
  void SetAbsPose(albaMatrix *absPose);
  albaMatrix *GetAbsPose();

  /**
  Get the gizmo interactor*/
  albaInteractorGenericInterface *GetInteractor();

  /** Gizmo color setting facilities for gizmo segments;*/
  void SetColor(double col[3]);
  void SetColor(double colR, double colG, double colB);

protected:

  double m_Color[3];
  double m_LastColor[3];

  /**
  Set the reference system matrix and the Pivot ref sys matrix.
  Both reference system type are set to CUSTOM.*/
  void SetRefSysMatrix(albaMatrix *matrix);

  albaAutoPointer<albaMatrix> m_AbsInputMatrix;
  
  /** Circle gizmo */
  albaVMEGizmo *m_GizmoCross;

  /** Register input vme*/
  albaVME *m_InputVme;

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
  

  int m_GizmoDirection;

  /** Register the gizmo axis */
  int m_RotationAxis;
  
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
  albaInteractorCompositorMouse *m_IsaComp;

  /** isa generic*/
  albaInteractorGenericMouse *m_IsaGen;
  
  /**
  Register the event receiver object*/
  albaObserver *m_Listener;

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
  albaVMEGizmo *m_RotationFeedbackGizmo;

};
#endif
