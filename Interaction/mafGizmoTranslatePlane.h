/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoTranslatePlane.h,v $
  Language:  C++
  Date:      $Date: 2008-04-18 16:05:10 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoTranslatePlane_H__
#define __mafGizmoTranslatePlane_H__

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
class mafVME;
class mafVMEGizmo;
class mafMatrix;
class vtkLineSource;
class vtkPlaneSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkTubeFilter;
//----------------------------------------------------------------------------

/** Basic gizmo component used to perform constrained translation on a plane.
  
  @sa mafGizmoTranslate 
*/
class mafGizmoTranslatePlane: public mafObserver 
{
public:
           mafGizmoTranslatePlane(mafVME *input, mafObserver *listener = NULL);
  virtual ~mafGizmoTranslatePlane(); 
  
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

  /** Plane enum*/
  enum PLANE {YZ = 0, XZ, XY};
  
  /** Set/Get gizmo plane, default plane is YZ*/        
  void SetPlane(int axis); 
  
  /** Get gizmo Plane*/
  int  GetPlane() {return ActivePlane;}; 
  
  //----------------------------------------------------------------------------
  // highlight and show 
  //----------------------------------------------------------------------------
  
  /** Highlight gizmo*/
  void Highlight(bool highlight);
    
  /** Show gizmo */
  void Show(bool show);
  
  
  //----------------------------------------------------------------------------
  // size setting 
  //----------------------------------------------------------------------------
  
  /** Set/Get the side length of the gizmo*/
  void   SetSizeLength(double length);
  double GetSizeLength() {return m_Length;};

  
  //----------------------------------------------------------------------------
  // activation status 
  //----------------------------------------------------------------------------

  /** Set/Get the activation status of the gizmo, When the gizmo is active
  it is sending pose matrices to the listener */
  void SetIsActive(bool highlight) {IsActive = highlight;};
  bool GetIsActive() {return IsActive;}

   /** 
  Set the abs pose */
  void SetAbsPose(mafMatrix *absPose);
  mafMatrix *GetAbsPose();

  /** 
  Set the constrain ref sys */
  void SetConstrainRefSys(mafMatrix *constrain);
 
  /**
  Set the constraint modality for the given axis; allowed constraint modality are:
  LOCK, FREE, BOUNDS, SNAP_STEP, SNAP_ARRAY.*/
  void SetConstraintModality(int axis, int constrainModality);

  /** Set the step value for snap step constraint type for the given axis*/
  void SetStep(int axis, double step);

protected:
  /** Segments gizmo */
  mafVMEGizmo *Gizmo[3];

  /** Register input vme*/
  mafVME *InputVme;

  /**

      z              z             y
      ^              ^             ^
      |              |             |
      |              |             |
      |              |             |
       -------> y     ------->x     -------> x
  
         YZ              XZ            XY

      z
      ^  S2
      |-----
      |     |         
      | SQ  |S1          
      |     |         
       --------> y    
  */

  enum GIZMOPARTS {S1 = 0, S2, SQ};
  enum GIZMO_STATUS {SELECTED = 0, NOT_SELECTED};
  
  /** Register the gizmo plane */
  int ActivePlane;
 
  /** Register the gizmo square plane side length*/
  double m_Length;

  /** Line source*/
  vtkLineSource *Line[2];

  /** Tube filter for lines */
  vtkTubeFilter *LineTF[2];

  /** Plane source*/
  vtkPlaneSource *Plane;
  
  /** S1, S2 and SQ gizmo data*/
  //mafVmeData *GizmoData[3];
 
  /** rotate PDF for gizmo parts */
  vtkTransformPolyDataFilter *RotatePDF[3];

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

  /** Used by mmiGenericMouse */
  vtkTransform *PivotTransform;

  /** Gizmo color setting facilities for gizmo segments;*/
  void SetColor(int part, double col[3]);
  void SetColor(int part, double colR, double colG, double colB);
  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  /** Hide/show the square */
  void ShowSquare(bool show);

  /** Register Gizmo status*/
  bool IsActive;

  friend class mafGizmoTranslatePlaneTest;
};
#endif
