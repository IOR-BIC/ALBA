/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoScaleIsotropic
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoScaleIsotropic_H__
#define __albaGizmoScaleIsotropic_H__

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
class vtkTransformPolyDataFilter;
class vtkTransform;

/** Gizmo component used to perform isotropic scaling.
  
  @sa albaGizmoScale
*/
class ALBA_EXPORT albaGizmoScaleIsotropic: public albaGizmoInterface 
{
public:
           albaGizmoScaleIsotropic(albaVME *input, albaObserver *listener = NULL);
  virtual ~albaGizmoScaleIsotropic(); 
  
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
 
protected:
  /** 
  Set the constrain ref sys */
  void SetRefSysMatrix(albaMatrix *constrain);

  /** Cube gizmo */
  albaVMEGizmo *m_CubeGizmo;

  /** Register input vme*/
  albaVME *m_InputVme;
  
  /** Cube source*/
  vtkCubeSource *m_Cube;
    
  /** Cylinder and cube gizmo vme data*/
  //albaVmeData *GizmoData;

  /** translate PDF for cylinder and cube*/
  vtkTransformPolyDataFilter *m_TranslatePDF;
  
  /** Create vtk objects needed*/
  void CreatePipeline();

  /** Create isa stuff */
  void CreateISA();

  /** isa compositor*/
  albaInteractorCompositorMouse *m_IsaComp;

  /** isa generic*/
  albaInteractorGenericMouse *m_IsaGen;

  /** Gizmo color setting facilities*/
  void SetColor(double col[3]);
  void SetColor(double colR, double colG, double colB);
 
  /**
  Register the event receiver object*/
  albaObserver *m_Listener;

  /** friend test */
  friend class albaGizmoScaleIsotropicTest;
};
#endif
