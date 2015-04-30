/*=========================================================================

 Program: MAF2
 Module: mafGizmoCrossRotateFan
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 

#ifndef __mafGizmoCrossRotateFan_H__
#define __mafGizmoCrossRotateFan_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class mafVME;
class mafVMEGizmo;
class mafMatrix;
class mafRefSys;

/** 
  
  Basic gizmo component used to give rotation angle feedback.
  This gizmo draws a fan centered at rotation origin giving visual feedback
  for rotation angle value.

      z               x              y
      ^               ^              ^
      |               |              |
      |   /|          |  /|          |   /|
      | /  |          |/  |          | /  |
      x-------> y     y-------> z    z-------> x
  
          X               Y            Z
  
  This component notifies the listener with the activeGizmoAxis  
  and the localRotationAngle around the activGizmoAxis

  mafString activeGizmoAxis = *(event->GetString()); // "X", "Y" or "Z" string
  double localRotationAngle = event->GetDouble();

*/
class mafGizmoCrossRotateFan: public mafGizmoInterface
{
public:
           mafGizmoCrossRotateFan(mafVME *input, mafObserver *listener = NULL);
  virtual ~mafGizmoCrossRotateFan(); 
  
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

  /** axis enum*/
  enum AXIS {X = 0, Y, Z};
  
  /** Set/Get gizmo axis, default axis is X*/        
  void SetAxis(int axis); 
  
  /** Get gizmo axis*/
  int  GetAxis(); 
  
  //----------------------------------------------------------------------------
  // show/ hide the gizmo
  //----------------------------------------------------------------------------
 
  /** Show gizmo */
  void Show(bool show);
  
  //----------------------------------------------------------------------------
  // radius setting 
  //----------------------------------------------------------------------------
  
  /** Set/Get the radius of the gizmo*/
  void   SetRadius(double radius);
  double GetRadius();
 
  /** 
  Set the abs pose */
  void SetAbsPose(mafMatrix *absPose);
  
protected:
  
 
  /** Register input vme*/
  mafVME *m_InputVme;

  /**
  Register the event receiver object*/
  mafObserver *m_Listener;
 
  /** Register the gizmo axis */
  int m_ActiveAxis;
  
  /** Create the sphere*/
  vtkSphereSource *m_Sphere;

  /** Transform to rotate the fan around Z axis after creation
  to match new m_StartTheta angle*/
  vtkTransform *m_RotateFanTransform;

  vtkTransformPolyDataFilter *m_RotateFanTPDF;

  /** sphere transform */
  vtkTransform *m_ChangeFanAxisTransform;

  /** rotate PDF for sphere*/
  vtkTransformPolyDataFilter *m_ChangeFanAxisTPDF;
  
   /** Fan gizmo */
  mafVMEGizmo *m_Gizmo;

  /** gizmo data */
  //mafVmeData *GizmoData;
 
  /** Create vtk objects needed*/
  void CreatePipeline();

  /** m_Gizmo color setting facilities for gizmo segments;*/
  void SetColor(double col[3]);
  void SetColor(double colR, double colG, double colB);

  /** Start angle and end angle*/
  double m_StartTheta;
  double m_EndTheta;

  enum MIRROR_STATUS {OFF = 0, ON};

  /** Register the mirror status of the fan; default status is off*/
  int m_MirrorStatus;

  vtkTransform *m_MirrorTr;
  vtkTransform *m_BufferTr;
  vtkTransformPolyDataFilter *m_MirrorTPDF;

  /** Get the start theta from abs pick coordinates */
  double PointPickedToStartTheta(double xp, double yp, double zp);

  /**
  Set the reference system matrix. Reference system type is set to CUSTOM.*/
  void SetRefSysMatrix(mafMatrix *matrix);

  mafRefSys *m_RefSys;

  /** Test Friend */
  friend class mafGizmoRotateFanTest;
};
#endif
