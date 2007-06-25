/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoROI.h,v $
  Language:  C++
  Date:      $Date: 2007-06-25 10:03:01 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#ifndef __mafGizmoROI_H__
#define __mafGizmoROI_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGizmoInterface.h"
#include "mafObserver.h"
#include "mafGizmoBoundingBox.h"
#include "mafGizmoHandle.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoHandle;
class mafMatrix;
class mafVME;

//----------------------------------------------------------------------------
// mafGizmoROI :
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

  Gizmo composed of 6 mafGizmoHandle and 1 mafGizmoBoundingBox used to select a region of interest.

  This object works by creating smaller components that are parented to the
  input vme ie works in local coordinates.
  In order to use it in your client create an instance of it and ask for the region of interest. 

  @sa mafGizmoHandle mafGizmoBoundingBox
  */

class mafGizmoROI: public mafGizmoInterface
{
public:
	mafGizmoROI(mafVME *input, mafObserver* listener = NULL , int constraintModality=mafGizmoHandle::BOUNDS,mafVME* parent=NULL);
  virtual ~mafGizmoROI(); 

  /** 
  Set input vme for the gizmo*/
  virtual void SetInput(mafVME *vme); 
  
  /**
  Events handling*/        
  virtual void OnEvent(mafEventBase *maf_event);

  /**
  Highlight the given component and set highlight to false for the others */
  void Highlight(int component);

  /**
  Set highlight to false for all gizmo camponents*/
  void HighlightOff(); 

  //----------------------------------------------------------------------------
  // show 
  //----------------------------------------------------------------------------
  
  /**
  Show the gizmo*/
  void Show(bool show);

  /**
  Show gizmo handles */
  void ShowHandles(bool show);

  /**
  Show gizmo cube */
  void ShowROI(bool show);

  /** 
  Set the constrain ref sys*/
  void SetConstrainRefSys(mafMatrix *constrain);

  /**
  Set/Get the gizmo bounds */
  void SetBounds(double bounds[6]);
  double *GetBounds();
  void GetBounds(double bounds[6]);

  /**
  Reset the gizmo*/
  void Reset();

protected:

  /**
  Highlight one component and dehighlight other components;
  setting highlight to false dehighlight every gizmos*/
  void Highlight(int component, bool highlight); 

  /** 
  Register the active gizmo component at MOUSE_DOWN*/
  int ActiveGizmoComponent;

  /** 
  After one handle movement recenter other handles*/
  void UpdateHandlePositions();

  /**
  Update gizmo outline bounds based on handles position; this method must be called each time a 
  gizmo handle is moved*/
  void UpdateOutlineBounds();

  /**
  Update the gizmos length. This function determines how Gizmos will be resized while moving.
  Superclasses should redefine this in order to change the way gizmos are resized.*/
  virtual void UpdateGizmosLength();

  /** 
  Process events from gizmo components*/
  void OnEventGizmoComponents(mafEventBase *maf_event);

	int m_ConstraintModality;

	/** Array holding the six gizmo handles*/  
	mafGizmoHandle *GHandle[6];

	/**
	The gizmo bounding box*/
	mafGizmoBoundingBox *OutlineGizmo;

};
#endif
