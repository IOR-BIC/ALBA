/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoScaleAxis
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoScaleAxis_H__
#define __albaGizmoScaleAxis_H__

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

class vtkCubeSource;
class vtkCylinderSource;
class vtkTransformPolyDataFilter;
class vtkTransform;

/** Basic gizmo component used to perform constrained scaling along one axis.
  
  @sa albaGizmoScale
*/
class ALBA_EXPORT albaGizmoScaleAxis: public albaGizmoInterface 
{
public:
           albaGizmoScaleAxis(albaVME *input, albaObserver *listener = NULL);
  virtual ~albaGizmoScaleAxis(); 
  
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
  int  GetAxis() const; 
  
  //----------------------------------------------------------------------------
  // highlight and show 
  //----------------------------------------------------------------------------

  /** Highlight the gizmo*/
  void Highlight(bool highlight);
    
  /** Show the gizmo */
  void Show(bool show);

  
  /** 
  Set the abs pose */
  void SetAbsPose(albaMatrix *absPose);
  albaMatrix *GetAbsPose();
  
  //----------------------------------------------------------------------------
  // cube stuff
  //----------------------------------------------------------------------------
    
  /** Set/Get the length of the cone*/
  void   SetCubeLength(double length);
  double  GetCubeLength() const;
 
  //----------------------------------------------------------------------------
  // cylinder stuff
  //----------------------------------------------------------------------------
 
  /** Set/Get the length of the cylinder*/
  void   SetCylinderLength(double length);
  double  GetCylinderLength() const;

protected:

  /** 
  Set the constrain ref sys */
  void SetRefSysMatrix(albaMatrix *constrain);

  /** Cube gizmo */
  albaVMEGizmo *m_CubeGizmo;

  /** cylinder gizmo*/
  albaVMEGizmo *m_CylGizmo;

  /** Register input vme*/
  albaVME *m_InputVme;

  enum GIZMOPARTS {CYLINDER = 0, CUBE};
  
  /** Register the gizmo axis */
  int m_Axis;
  
  /** Cube source*/
  vtkCubeSource *m_Cube;
  
  /** Cylinder source*/
  vtkCylinderSource *m_Cylinder;
  
  /** translate PDF for cylinder and cube*/
  vtkTransformPolyDataFilter *m_TranslatePDF[2];
  
  /** translation transform for cylinder and cube*/
  vtkTransform *m_TranslateTr[2];
 
  /** rotate PDF for cylinder and cube*/
  vtkTransformPolyDataFilter *m_RotatePDF[2];

  /** rotation transform for cylinder and cube*/
  vtkTransform *m_RotationTr;
  
  /** Create vtk objects needed*/
  void CreatePipeline();

  /** Create isa stuff */
  void CreateISA();

  /** isa compositor*/
  albaInteractorCompositorMouse *m_IsaComp[2];

  /** isa generic*/
  albaInteractorGenericMouse *m_IsaGen[2];

  /** Gizmo color setting facilities; part can be CYLINDER or CUBE*/
  void SetColor(int part, double col[3]);
  void SetColor(int part, double colR, double colG, double colB);
  void SetColor(double cylCol[3], double coneCol[3]);
  void SetColor(double cylR, double cylG, double cylB, double coneR, double coneG, double coneB);

  /**
  Register the event receiver object*/
  albaObserver *m_Listener;

  /** 
  Register the highlight status */
  bool m_Highlight;

  /** 
  register the show status */
  bool m_Show;

  /** Test friend */
  friend class albaGizmoScaleAxisTest;
};
#endif
