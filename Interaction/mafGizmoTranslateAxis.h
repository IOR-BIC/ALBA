/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoTranslateAxis.h,v $
  Language:  C++
  Date:      $Date: 2005-12-07 11:21:32 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoTranslateAxis_H__
#define __mafGizmoTranslateAxis_H__

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
class vtkConeSource;
class vtkCylinderSource;
class vtkTransformPolyDataFilter;
class vtkTransform;

/** Basic gizmo component used to perform constrained translation on one axis.
  
  @sa mafGizmoTranslate 
*/
class mafGizmoTranslateAxis: public mafObserver 
{
public:
           mafGizmoTranslateAxis(mafVME *input, mafObserver *listener = NULL);
  virtual ~mafGizmoTranslateAxis(); 
  
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
  // axis setting 
  //----------------------------------------------------------------------------

  /** Axis enum*/
  enum AXIS {X = 0, Y, Z};
  
  /** Set/Get gizmo axis, default axis is X*/        
  void SetAxis(int axis); 
  int  GetAxis(); 
  
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
  double GetConeLength();
 
  //----------------------------------------------------------------------------
  // cylinder stuff
  //----------------------------------------------------------------------------
 
  /** Set/Get the length of the cylinder*/
  void   SetCylinderLength(double length);
  double GetCylinderLength();
  
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

protected:
  /** 
  Set the constrain ref sys */
  void SetRefSysMatrix(mafMatrix *constrain);

  /** Cone gizmo */
  mafVMEGizmo *ConeGizmo;

  /** cylinder gizmo*/
  mafVMEGizmo *CylGizmo;

  /** Register input vme*/
  mafVME *InputVme;

  enum GIZMOPARTS {CYLINDER = 0, CONE};
  
  /** Register the gizmo axis */
  int Axis;
  
  /** Cone source*/
  vtkConeSource *Cone;
  
  /** Cylinder source*/
  vtkCylinderSource *Cylinder;
  
  /** Cylinder and cone gizmo vme data*/
  //mafVmeData *GizmoData[2];

  /** translate PDF for cylinder and cone*/
  vtkTransformPolyDataFilter *TranslatePDF[2];
  
  /** translation transform for cylinder and cone*/
  vtkTransform *TranslateTr[2];
 
  /** rotate PDF for cylinder and cone*/
  vtkTransformPolyDataFilter *RotatePDF[2];

  /** rotation transform for cylinder and cone*/
  vtkTransform *RotationTr;
  
  /** Create vtk objects needed*/
  void CreatePipeline();

  /** Create isa stuff */
  void CreateISA();

  /** isa compositor*/
  mmiCompositorMouse *IsaComp[2];

  /** isa generic*/
  mmiGenericMouse *IsaGen[2];

  /** Gizmo color setting facilities; part can be CYLINDER or CONE*/
  void SetColor(int part, double col[3]);
  void SetColor(int part, double colR, double colG, double colB);
  void SetColor(double cylCol[3], double coneCol[3]);
  void SetColor(double cylR, double cylG, double cylB, double coneR, double coneG, double coneB);

  /**
  Register the event receiver object*/
  mafObserver *m_Listener;
};
#endif
