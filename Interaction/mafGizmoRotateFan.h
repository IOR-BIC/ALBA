/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoRotateFan.h,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:25 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoRotateFan_H__
#define __mafGizmoRotateFan_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"

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

/** Basic gizmo component used to give rotation angle feedback.
  This gizmo draws a fan centered at rotation origin giving visual feedback
  for rotation angle value.

      z               x              y
      ^               ^              ^
      |               |              |
      |   /|          |  /|          |   /|
      | /  |          |/  |          | /  |
      x-------> y     y-------> z    z-------> x
  
          X               Y            Z
  
  @sa mafGizmoRotate 
*/
class mafGizmoRotateFan: public mafObserver 
{
public:
           mafGizmoRotateFan(mafVME *input, mafObserver *listener = NULL);
  virtual ~mafGizmoRotateFan(); 
  
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
  mafVME *InputVme;

  /**
  Register the event receiver object*/
  mafObserver *m_Listener;
 
  /** Register the gizmo axis */
  int ActiveAxis;
  
  /** Create the sphere*/
  vtkSphereSource *Sphere;

  /** Transform to rotate the fan around Z axi after creation
  to match new StartTheta angle*/
  vtkTransform *RotateFanTr;

  vtkTransformPolyDataFilter *RotateFanTPDF;

  /** sphere transform */
  vtkTransform *ChangeFanAxisTr;

  /** rotate PDF for sphere*/
  vtkTransformPolyDataFilter *ChangeFanAxisTPDF;
  
   /** Fan gizmo */
  mafVMEGizmo *Gizmo;

  /** gizmo data */
  //mafVmeData *GizmoData;
 
  /** Create vtk objects needed*/
  void CreatePipeline();

  /** Gizmo color setting facilities for gizmo segments;*/
  void SetColor(double col[3]);
  void SetColor(double colR, double colG, double colB);

  /** Start angle and end angle*/
  double StartTheta;
  double EndTheta;

  enum MIRROR_STATUS {OFF = 0, ON};

  /** Register the mirror status of the fan; default status is off*/
  int MirrorStatus;

  vtkTransform *MirrorTr;
  vtkTransform *BufferTr;
  vtkTransformPolyDataFilter *MirrorTPDF;

  /** Get the start theta from abs pick coordinates */
  double PointPickedToStartTheta(double xp, double yp, double zp);

   /**
  Set the reference system matrix. Reference system type is set to CUSTOM.*/
  void SetRefSysMatrix(mafMatrix *matrix);

  mafRefSys *RefSys;
};
#endif
