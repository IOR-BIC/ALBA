/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoROI
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __albaGizmoROI_H__
#define __albaGizmoROI_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGizmoInterface.h"
#include "albaObserver.h"
#include "albaGizmoBoundingBox.h"
#include "albaGizmoHandle.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGizmoHandle;
class albaMatrix;
class albaVME;

//----------------------------------------------------------------------------
// albaGizmoROI :
//----------------------------------------------------------------------------
/**
        5                   5
    _________           _________
   |         |         |         |
   |         |         |         |
  0|    3    |1       3|    1    |2 
   |         |         |         |   
   |_________|         |_________| 
        4                   4
  Z                     Z
  ^                     ^
  |                     |
   ->X                   ->Y

  -----------------------------------------

        3
    _________
   |         |
   |         |
  0|    5    |1
   |         |
   |_________|
        2
  Y 
  ^
  |
   ->X

  Gizmo composed of 6 albaGizmoHandle and 1 albaGizmoBoundingBox used to select a region of interest.

  This object works by creating smaller components that are parented to the
  input vme ie works in local coordinates.
  In order to use it in your client create an instance of it and ask for the region of interest. 

  @sa albaGizmoHandle albaGizmoBoundingBox
  */

class ALBA_EXPORT albaGizmoROI: public albaGizmoInterface
{
public:
	albaGizmoROI(albaVME *input, albaObserver* listener = NULL , int constraintModality=albaGizmoHandle::BOUNDS,albaVME* parent=NULL,bool showShadingPlane=false);
  virtual ~albaGizmoROI(); 

  /** Set input vme for the gizmo*/
  virtual void SetInput(albaVME *vme); 
  
  /** Events handling*/        
  virtual void OnEvent(albaEventBase *alba_event);

  /** Highlight the given component and set highlight to false for the others */
  void Highlight(int component);

  /** Set highlight to false for all gizmo components*/
  void HighlightOff(); 

  //----------------------------------------------------------------------------
  // show 
  //----------------------------------------------------------------------------
  
  /** Show the gizmo*/
  void Show(bool show);

  /** Show gizmo handles */
  void ShowHandles(bool show);

  /** Show shading planes */
  void ShowShadingPlane(bool show);

  /** Show gizmo cube */
  void ShowROI(bool show);

  /** Set the constrain ref sys*/
  void SetConstrainRefSys(albaMatrix *constrain);

  /** Set/Get the gizmo bounds */
  void SetBounds(double bounds[6]);
  double *GetBounds();
  void GetBounds(double bounds[6]);

  /** Reset the gizmo*/
  void Reset();

  /** Minimum handles size */
  void EnableMinimumHandleSize(bool value){m_EnableMinimumHandleSize = value;}
  void SetMinimumHandleSize(double value){m_MinimumHandleSize = value;}
  void EnableMinimumDistanceBetweenGizmo(bool value){m_EnableMinimumDistanceBetweenGizmo = value;}

protected:

  /** Highlight one component of the gizmo and turn off the highlight others;
  setting highlight to false it is turned off to all gizmos' components*/
  void Highlight(int component, bool highlight); 

  int m_ActiveGizmoComponent; ///<Register the active gizmo component at MOUSE_DOWN

  /** After one handle movement recenter other handles*/
  void UpdateHandlePositions();

  /** Update gizmo outline bounds based on handles position. 
  This method must be called each time a gizmo handle is moved*/
  void UpdateOutlineBounds();

  /** Update the gizmos length. 
  This function determines how Gizmos will be resized while moving.
  Superclasses should redefine this in order to change the way gizmos are resized.*/
  virtual void UpdateGizmosLength();

  /** Process events from gizmo components*/
  void OnEventGizmoComponents(albaEventBase *alba_event);

	int m_ConstraintModality;


	albaGizmoHandle *m_GHandle[6];///<Array holding the six gizmo handles
  double m_Center[6][3];
	albaGizmoBoundingBox *m_OutlineGizmo;///<The gizmo bounding box

  double m_Accumulator;
  double m_MinimumHandleSize;
  bool m_EnableMinimumHandleSize;
  bool m_EnableMinimumDistanceBetweenGizmo;

  /** friend test */
  friend class albaGizmoROITest ;

};
#endif
