/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewPanoramic.h,v $
Language:  C++
Date:      $Date: 2007-02-05 20:48:09 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafViewPanoramic_H__
#define __mafViewPanoramic_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEAdvancedProber;
class mafGizmoSlice;
class mafSceneNode;
class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// mafViewPanoramic :
//----------------------------------------------------------------------------
/** 
mafViewPanoramic is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class mafViewPanoramic: public mafViewVTK
{
public:
	mafViewPanoramic(wxString label = "Image", int camera_position = CAMERA_FRONT, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
	virtual ~mafViewPanoramic(); 

	mafTypeMacro(mafViewPanoramic, mafViewVTK);

	virtual mafView*  Copy(mafObserver *Listener);
	virtual void			OnEvent(mafEventBase *maf_event);

	void Create();

	/** 
	Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph mafView*/
	virtual int GetNodeStatus(mafNode *vme);

	/** IDs for the view GUI */
	enum VIEW_SLICE_WIDGET_ID
	{
		ID_NONE = Superclass::ID_LAST,
		ID_LAST
	};

	/** 
	Create the visual pipe for the node passed as argument. 
	To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
	otherwise ask to vme which is its visual pipe. */
	//virtual void VmeCreatePipe(mafNode *vme);

	/** 
	Delete vme's visual pipe. It is called when vme is removed from visualization.*/
	//virtual void VmeDeletePipe(mafNode *vme);

	/** Create the background for the slice. */
	void BorderCreate(double col[3]);

	/** Delete the background of the slice. */
	void BorderDelete();

	void VmeShow(mafNode *node, bool show);

	void VmeDeletePipe(mafNode *vme);

protected:
	virtual mmgGui *CreateGui();

	/**Create gizmos slice*/
	void GizmoCreate();

	mafGizmoSlice	*m_GizmoSlice[1];
	double			 m_Pos[6];

	double			 m_GizmoColor[1][3];

	mafVMEAdvancedProber	*m_CurrentPanoramic;

};
#endif
