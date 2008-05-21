/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoScaleIsotropic.h,v $
  Language:  C++
  Date:      $Date: 2008-05-21 10:11:32 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoScaleIsotropic_H__
#define __mafGizmoScaleIsotropic_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmiGenericMouse;
class mmiCompositorMouse;
class mafVMEGizmo;
class mafMatrix;
class mafVME;
class vtkCubeSource;
class vtkTransformPolyDataFilter;
class vtkTransform;

/** Gizmo component used to perform isotropic scaling.
  
  @sa mafGizmoScale
*/
class mafGizmoScaleIsotropic: public mafObserver 
{
public:
           mafGizmoScaleIsotropic(mafVME *input, mafObserver *listener = NULL);
  virtual ~mafGizmoScaleIsotropic(); 
  
  /** 
  Set the gizmo generating vme; the gizmo will be centered on this vme*/
  void SetInput(mafVME *vme); 
  mafVME *GetInput() {return this->InputVme;};

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
  mafVMEGizmo *CubeGizmo;

  /** Register input vme*/
  mafVME *InputVme;
  
  /** Cube source*/
  vtkCubeSource *Cube;
    
  /** Cylinder and cube gizmo vme data*/
  //mafVmeData *GizmoData;

  /** translate PDF for cylinder and cube*/
  vtkTransformPolyDataFilter *TranslatePDF;
  
  /** Create vtk objects needed*/
  void CreatePipeline();

  /** Create isa stuff */
  void CreateISA();

  /** isa compositor*/
  mmiCompositorMouse *IsaComp;

  /** isa generic*/
  mmiGenericMouse *IsaGen;

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
