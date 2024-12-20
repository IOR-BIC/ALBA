/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoTranslateAxis
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoTranslateAxis_H__
#define __albaGizmoTranslateAxis_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGizmoInterface.h"

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

/** Basic gizmo component used to perform constrained translation on one axis.
  
  @sa albaGizmoTranslate 
*/
class ALBA_EXPORT albaGizmoTranslateAxis: public albaGizmoInterface 
{
public:
           albaGizmoTranslateAxis(albaVME *input, albaObserver *listener = NULL, albaString name = "");
  virtual ~albaGizmoTranslateAxis(); 
  
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

  //----------------------------------------------------------------------------
  // cone stuff
  //----------------------------------------------------------------------------
    
  /** Set/Get the length of the cone*/
  void   SetConeLength(double length);
  double GetConeLength() {return m_ConeLength;};
 
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

protected:
  /** 
  Set the constrain ref sys */
  void SetRefSysMatrix(albaMatrix *constrain);

  /** Cone gizmo */
  albaVMEGizmo *m_ConeGizmo;

  /** cylinder gizmo*/
  albaVMEGizmo *m_CylGizmo;

  /** Register input vme*/
  albaVME *m_InputVme;

  enum GIZMOPARTS {CYLINDER = 0, CONE};
  
  /** Register the gizmo axis */
  int m_Axis;
  
  /** Cone source*/
  vtkConeSource *m_Cone;

  /** Cone length*/
  double m_ConeLength;

  /** Cylinder source*/
  vtkCylinderSource *m_Cylinder;
  
  /** Cylinder length*/
  double m_CylinderLength;

  /** Cylinder and cone gizmo vme data*/
  //albaVmeData *GizmoData[2];

  /** translate PDF for cylinder and cone*/
  vtkTransformPolyDataFilter *m_TranslatePDF[2];
  
  /** translation transform for cylinder and cone*/
  vtkTransform *m_TranslateTr[2];
 
  /** rotate PDF for cylinder and cone*/
  vtkTransformPolyDataFilter *m_RotatePDF[2];

  /** rotation transform for cylinder and cone*/
  vtkTransform *m_RotationTr;
  
  /** Create vtk objects needed*/
  void CreatePipeline();

  /** Create isa stuff */
  void CreateISA();

  /** isa compositor*/
  albaInteractorCompositorMouse *m_IsaComp[2];

  /** isa generic*/
  albaInteractorGenericMouse *m_IsaGen[2];

  /** Gizmo color setting facilities; part can be CYLINDER or CONE*/
  void SetColor(int part, double col[3]);
  void SetColor(int part, double colR, double colG, double colB);
  void SetColor(double cylCol[3], double coneCol[3]);
  void SetColor(double cylR, double cylG, double cylB, double coneR, double coneG, double coneB);

  /**
  Register the event receiver object*/
  albaObserver *m_Listener;

  /** test friend */
  friend class albaGizmoTranslateAxisTest;
};
#endif
