/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoScaleAxis.h,v $
  Language:  C++
  Date:      $Date: 2009-04-22 09:42:43 $
  Version:   $Revision: 1.1.22.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoScaleAxis_H__
#define __mafGizmoScaleAxis_H__

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
class vtkCylinderSource;
class vtkTransformPolyDataFilter;
class vtkTransform;

/** Basic gizmo component used to perform constrained scaling along one axis.
  
  @sa mafGizmoScale
*/
class mafGizmoScaleAxis: public mafObserver 
{
public:
           mafGizmoScaleAxis(mafVME *input, mafObserver *listener = NULL);
  virtual ~mafGizmoScaleAxis(); 
  
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
  void SetAbsPose(mafMatrix *absPose);
  mafMatrix *GetAbsPose();
  
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
  void SetRefSysMatrix(mafMatrix *constrain);

  /** Cube gizmo */
  mafVMEGizmo *m_CubeGizmo;

  /** cylinder gizmo*/
  mafVMEGizmo *m_CylGizmo;

  /** Register input vme*/
  mafVME *m_InputVme;

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
  mmiCompositorMouse *m_IsaComp[2];

  /** isa generic*/
  mmiGenericMouse *m_IsaGen[2];

  /** Gizmo color setting facilities; part can be CYLINDER or CUBE*/
  void SetColor(int part, double col[3]);
  void SetColor(int part, double colR, double colG, double colB);
  void SetColor(double cylCol[3], double coneCol[3]);
  void SetColor(double cylR, double cylG, double cylB, double coneR, double coneG, double coneB);

  /**
  Register the event receiver object*/
  mafObserver *m_Listener;
};
#endif
