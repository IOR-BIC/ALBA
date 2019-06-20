/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoBoundingBox
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __albaGizmoBoundingBox_H__
#define __albaGizmoBoundingBox_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"
#include "albaVMEGizmo.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaISAGeneric;
class albaISACompositor;
class albaVME;
class vtkOutlineSource;
class vtkCylinderSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class albaMatrix;

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// albaGizmoBoundingBox :
//----------------------------------------------------------------------------
/**           
   Bounding box gizmo for highlighting a region of interest.
   @sa albaGizmoROI               
*/
class ALBA_EXPORT albaGizmoBoundingBox: public albaObserver, public albaServiceClient 
{
public:
           albaGizmoBoundingBox(albaVME *input, albaObserver *listener = NULL,albaVME* parent=NULL);
  virtual ~albaGizmoBoundingBox(); 
  
  /** Set the gizmo generating vme; the gizmo will be centered on this vme*/
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
  
  /** Set/Get the gizmo bounds */
  void SetBounds(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
  void SetBounds(double bounds[6]);
  double *GetBounds();
  void GetBounds(double bounds[6]);

  /** Set the gizmo abs pose */
  void SetAbsPose(albaMatrix *absPose);
  albaMatrix *GetAbsPose();

  /** Set the gizmo local pose*/
  void SetPose(albaMatrix *pose);
	albaMatrix *GetPose();

protected:
  /** Set gizmo color*/
  void SetColor(double col[3]);
  
  /** Set gizmo color*/
  void SetColor(double colR, double colG, double colB);
  
	albaObserver *m_Listener;///<Register the event receiver object

	albaVME *m_InputVme;///<Register input vme

  vtkOutlineSource *m_BoxOutline;

  albaVMEGizmo *m_BoxGizmo;

  /** test friends */
  friend class albaGizmoBoundingBoxTest;
  friend class albaGizmoROITest;
};
#endif
