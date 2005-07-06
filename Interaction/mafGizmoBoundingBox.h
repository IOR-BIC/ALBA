/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoBoundingBox.h,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:23 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#ifndef __mafGizmoBoundingBox_H__
#define __mafGizmoBoundingBox_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafISAGeneric;
class mafISACompositor;
class mafVME;
class vtkOutlineSource;
class vtkCylinderSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class mafVMEGizmo;
class mafMatrix;

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGizmoBoundingBox :
//----------------------------------------------------------------------------
/**           
   Bounding box gizmo for highlighting a region of interest.
   @sa mafGizmoROI               
*/
class mafGizmoBoundingBox: public mafObserver 
{
public:
           mafGizmoBoundingBox(mafVME *input, mafObserver *listener = NULL);
  virtual ~mafGizmoBoundingBox(); 
  
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
  Set/Get the gizmo bounds */
  void SetBounds(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
  void SetBounds(double bounds[6]);
  double *GetBounds();
  void GetBounds(double bounds[6]);

  /** 
  Set the gizmo abs pose */
  void SetAbsPose(mafMatrix *absPose);
  mafMatrix *GetAbsPose();

  /**
  Set the gizmo local pose*/
  void SetPose(mafMatrix *pose);
  mafMatrix *GetPose();

protected:
  /** Set gizmo color*/
  void SetColor(double col[3]);
  void SetColor(double colR, double colG, double colB);
  
  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  /** Register input vme*/
  mafVME *InputVme;

  vtkOutlineSource *BoxOutline;

  mafVMEGizmo *BoxGizmo;
};
#endif
