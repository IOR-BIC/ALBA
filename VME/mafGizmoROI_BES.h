/*=========================================================================

 Program: MAF2
 Module: mafGizmoROI_BES
 Authors: Stefano Perticoni, Josef Kohout
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __mafGizmoROI_BES_H__
#define __mafGizmoROI_BES_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafGizmoInterface.h"
#include "mafGizmoHandle.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafMatrix;
class mafVME;
class mafGizmoTranslatePlane;
class mafGizmoTranslateAxis;
class mafObserver;
class mafGizmoBoundingBox;

//----------------------------------------------------------------------------
// mafGizmoROI_BES :
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
BES: It also composed of three translation axis (mafGizmoTranslateAxis) and planes (mafGizmoTranslatePlane)

This object works by creating smaller components that are parented to the
input vme ie works in local coordinates.
In order to use it in your client create an instance of it and ask for the region of interest. 

@sa mafGizmoHandle mafGizmoBoundingBox
*/

class MAF_EXPORT mafGizmoROI_BES: public mafGizmoInterface
{
public:
  //BES: backward compatibility
	enum CONSTRAINT_MODALITY {
		VTK_OUTPUT_BOUNDS = mafGizmoHandle::BOUNDS, 
    FREE = mafGizmoHandle::FREE,
		USER_BOUNDS
	};

public:
	mafGizmoROI_BES(mafVME *input, mafObserver* listener = NULL , 
		int constraintModality = mafGizmoROI_BES::VTK_OUTPUT_BOUNDS,
		mafVME* parent = NULL, double* usrBounds = NULL);

	virtual ~mafGizmoROI_BES(); 

	/** Set input vme for the gizmo*/
	virtual void SetInput(mafVME *vme); 

	/** Events handling*/        
	virtual void OnEvent(mafEventBase *maf_event);

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

	// Show or hide translation axis
	void ShowAxis(bool show);

	// Show or hide translation planes
	void ShowPlanes(bool show);

	/** Show gizmo cube */
	void ShowROI(bool show);

	/** Set the constrain ref sys*/
	void SetConstrainRefSys(mafMatrix *constrain);

	/** Set/Get the gizmo bounds */
	void SetBounds(double bounds[6]);
	double *GetBounds();
	void GetBounds(double bounds[6]);

	/** Reset the gizmo*/
	void Reset();

	//modifies the constraint modality
	void SetConstraintModality(int constraintModality, double* usrBounds = NULL);
protected:
	//returns ID of gizmo component denoted by the reference in sender
	//-1 is returned if sender is not a gizmo component
	int FindGizmoComponent(void* sender);

	//returns active gizmo component ID
	inline int GetActiveGizmoComponent() {
		return m_ActiveGizmoComponent;
	}

	//returns active gizmo component type
	inline int GetActiveGizmoComponentType() {
		return m_ActiveGizmoComponent / 8;			
	}

	//returns active gizmo component type
	inline int GetActiveGizmoComponentIndex() {
		return m_ActiveGizmoComponent % 8;			
	}

	/** Highlight one component of the gizmo and turn off the highlight others;
	setting highlight to false it is turned off to all gizmos' components*/
	void Highlight(int component, bool highlight); 	

	/** After one handle movement recenter other handles*/
	void UpdateHandlePositions();

	/** Update gizmo outline bounds based on handles position. 
	This method must be called each time a gizmo handle is moved*/
	void UpdateOutlineBounds();

	//updates the position of all translation gizmos so it is in the centre
	//of the current bounding box (ROI)
	void UpdateTranslationGizmos();

protected:
	//Updates the position of all gizmos according to the current 
	//position of active gizmo component (m_ActiveComponent)
	//NB: the position of active gizmo component may be changed here as well
	virtual void UpdateGizmos();

	/** Update the gizmos length. 
	This function determines how Gizmos will be resized while moving.
	Superclasses should redefine this in order to change the way gizmos are resized.*/
	virtual void UpdateGizmosLength();

	/** Process events from gizmo components*/
	virtual void OnEventGizmoComponents(mafEventBase *maf_event);

protected:
	enum ACTIVE_COMPONENT {
		NONE = -1, 
		HANDLE_XMIN = 0, HANDLE_XMAX, 
		HANDLE_YMIN, HANDLE_YMAX, 
		HANDLE_ZMIN, HANDLE_ZMAX, 
		
		AXIS_X = 8, AXIS_Y, AXIS_Z, 
		
		PLANE_XN = 16, PLANE_YN, PLANE_ZN,
	};

	enum ACTIVE_COMPONENT_TYPE {
		GHANDLE = 0,
		GAXIS = 1,
		GPLANE = 2,
	};

protected:
	//constraint modality
	int m_ConstraintModality;
	double m_UserBounds[6];

	//container of six gizmo handles, three gizmo translate vectors and 
	//three gizmo translate planes
	mafGizmoHandle* m_GHandle[6];
	mafGizmoTranslateAxis* m_GAxis[3];
	mafGizmoTranslatePlane* m_GPlane[3];

	//the active gizmo component registered at MOUSE_DOWN
	int m_ActiveGizmoComponent;

	//The gizmo bounding box
	mafGizmoBoundingBox* m_OutlineGizmo;

  /** friend test class */
  friend class mafGizmoROI_BESTest;
};

#endif
