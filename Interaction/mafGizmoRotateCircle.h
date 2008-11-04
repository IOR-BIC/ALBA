/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoRotateCircle.h,v $
  Language:  C++
  Date:      $Date: 2008-11-04 18:03:33 $
  Version:   $Revision: 1.2.22.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGRotateCircle_H__
#define __mafGRotateCircle_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafSmartPointer.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmiGenericInterface;
class mmiGenericMouse;
class mmiCompositorMouse;
class mafVMEGizmo;
class mafMatrix;
class mafVME;
class vtkDiskSource;
class vtkCleanPolyData;
class vtkTubeFilter;
class vtkTransformPolyDataFilter;
class vtkTransform;

//----------------------------------------------------------------------------
/** Basic gizmo component used to perform constrained rotation around an axis.
  
  @sa mafGizmoRotate
*/
class mafGizmoRotateCircle: public mafObserver
{
public:
           mafGizmoRotateCircle(mafVME *input, mafObserver *listener = NULL);
  virtual ~mafGizmoRotateCircle(); 
  
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
  void SetAbsPose(mafMatrix *absPose);
  mafMatrix *GetAbsPose();

  /**
  Get the gizmo interactor*/
  mmiGenericInterface *GetInteractor();

protected:

  /**
  Set the reference system matrix and the Pivot ref sys matrix.
  Both reference system type are set to CUSTOM.*/
  void SetRefSysMatrix(mafMatrix *matrix);

  mafAutoPointer<mafMatrix> m_AbsInputMatrix;
  
  /** Circle gizmo */
  mafVMEGizmo *m_Gizmo;

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
  mmiCompositorMouse *m_IsaComp;

  /** isa generic*/
  mmiGenericMouse *m_IsaGen;

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

  /** friend test */
  friend class mafGizmoRotateCircleTest;
};
#endif
