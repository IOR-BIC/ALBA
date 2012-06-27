/*=========================================================================

 Program: MAF2
 Module: mafGizmoScaleIsotropic
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGizmoScaleIsotropic_H__
#define __mafGizmoScaleIsotropic_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafInteractorGenericMouse;
class mafInteractorCompositorMouse;
class mafVMEGizmo;
class mafMatrix;
class mafVME;
class vtkCubeSource;
class vtkTransformPolyDataFilter;
class vtkTransform;

/** Gizmo component used to perform isotropic scaling.
  
  @sa mafGizmoScale
*/
class MAF_EXPORT mafGizmoScaleIsotropic: public mafGizmoInterface 
{
public:
           mafGizmoScaleIsotropic(mafVME *input, mafObserver *listener = NULL);
  virtual ~mafGizmoScaleIsotropic(); 
  
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
  // highlight and show 
  //----------------------------------------------------------------------------

  /** Highlight the gizmo*/
  void Highlight(bool highlight);
    
  /** Show the gizmo */
  void Show(bool show);

  /** 
  Set the abs pose */
  void SetAbsPose(mafMatrix *absPose);
  mafMatrix *GetAbsPose();
  
  //----------------------------------------------------------------------------
  // cube stuff
  //----------------------------------------------------------------------------
    
  /** Set/Get the length of the cone*/
  void   SetCubeLength(double length);
  double  GetCubeLength() const;
 
protected:
  /** 
  Set the constrain ref sys */
  void SetRefSysMatrix(mafMatrix *constrain);

  /** Cube gizmo */
  mafVMEGizmo *m_CubeGizmo;

  /** Register input vme*/
  mafVME *m_InputVme;
  
  /** Cube source*/
  vtkCubeSource *m_Cube;
    
  /** Cylinder and cube gizmo vme data*/
  //mafVmeData *GizmoData;

  /** translate PDF for cylinder and cube*/
  vtkTransformPolyDataFilter *m_TranslatePDF;
  
  /** Create vtk objects needed*/
  void CreatePipeline();

  /** Create isa stuff */
  void CreateISA();

  /** isa compositor*/
  mafInteractorCompositorMouse *m_IsaComp;

  /** isa generic*/
  mafInteractorGenericMouse *m_IsaGen;

  /** Gizmo color setting facilities*/
  void SetColor(double col[3]);
  void SetColor(double colR, double colG, double colB);
 
  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  /** friend test */
  friend class mafGizmoScaleIsotropicTest;
};
#endif
