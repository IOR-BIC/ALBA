/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotateCircle
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGRotateCircle_H__
#define __albaGRotateCircle_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGizmoInterface.h"
#include "albaSmartPointer.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaInteractorGenericInterface;
class albaInteractorGenericMouse;
class albaInteractorCompositorMouse;
class albaVMEGizmo;
class albaMatrix;
class albaVME;
class vtkDiskSource;
class vtkCleanPolyData;
class vtkTubeFilter;
class vtkTransformPolyDataFilter;
class vtkTransform;

template class ALBA_EXPORT albaAutoPointer<albaMatrix>;

//----------------------------------------------------------------------------
/** Basic gizmo component used to perform constrained rotation around an axis.
  
  @sa albaGizmoRotate
*/
class ALBA_EXPORT albaGizmoRotateCircle: public albaGizmoInterface
{
public:
           albaGizmoRotateCircle(albaVME *input, albaObserver *listener = NULL, albaString name = "Circle");
  virtual ~albaGizmoRotateCircle(); 
  
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
  
  /** Set/Get gizmo axis, default axis is X*/        
  void SetAxis(int axis); 
  
  /** Get egizmo axis*/
  int  GetAxis(); 
  
  //----------------------------------------------------------------------------
  // highlight and show 
  //----------------------------------------------------------------------------
  
  /** Highlight gizmo*/
  void Highlight(bool highlight);
    
  /** Show gizmo */
  void Show(bool show);
  
  //----------------------------------------------------------------------------
  // radius setting 
  //----------------------------------------------------------------------------
  
  /** Set/Get the radius of the gizmo*/
  void   SetRadius(double radius);
  double GetRadius();

  //----------------------------------------------------------------------------
  // activation status 
  //----------------------------------------------------------------------------

  /** Set/Get the activation status of the gizmo, When the gizmo is active
  it is sending pose matrices to the listener */
  void SetIsActive(bool highlight);
  bool GetIsActive();
 
  /** 
  Set the abs pose */
  void SetAbsPose(albaMatrix *absPose);
  albaMatrix *GetAbsPose();

  /**
  Get the gizmo interactor*/
  albaInteractorGenericInterface *GetInteractor();

  /** Superclass override */
  void SetMediator(albaObserver *mediator);

protected:

  /**
  Set the reference system matrix and the Pivot ref sys matrix.
  Both reference system type are set to CUSTOM.*/
  void SetRefSysMatrix(albaMatrix *matrix);

  albaAutoPointer<albaMatrix> m_AbsInputMatrix;
  
  /** Circle gizmo */
  albaVMEGizmo *m_GizmoCircle;

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
  
  /** Register the gizmo axis */
  int m_ActiveAxis;
  
  /** Create the circle polydata*/
  vtkDiskSource *m_Circle;

  /** Clean the circle polydata */
  vtkCleanPolyData *m_CleanCircle;
    
  /** Tube filter for circle */
  vtkTubeFilter *m_CircleTF;
 
  /** rotate PDF for gizmo */
  vtkTransformPolyDataFilter *m_RotatePDF;

  /** rotation transform */
  vtkTransform *m_RotationTr;
  
  /** Create vtk objects needed*/
  void CreatePipeline();

  /** Create isa stuff */
  void CreateISA();

  /** isa compositor*/
  albaInteractorCompositorMouse *m_IsaComp;

  /** isa generic*/
  albaInteractorGenericMouse *m_IsaGen;

  /** Gizmo color setting facilities for gizmo segments;*/
  void SetColor(double col[3]);
  void SetColor(double colR, double colG, double colB);
  
  /**
  Register the event receiver object*/
  albaObserver *m_Listener;

  /** Register Gizmo status*/
  bool m_IsActive;
  
  /** Register gizmo radius */
  double m_Radius;

  /** friend test */
  friend class albaGizmoRotateCircleTest;
};

#endif
