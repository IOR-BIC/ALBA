/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeMesh.h,v $
Language:  C++
Date:      $Date: 2007-04-17 10:17:06 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi - Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeMesh_H__
#define __mafPipeMesh_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkDataSetMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class mafAxes;

//----------------------------------------------------------------------------
// mafPipeMesh :
//----------------------------------------------------------------------------
class mafPipeMesh : public mafPipe
{
public:
	mafTypeMacro(mafPipeMesh,mafPipe);

	mafPipeMesh();
	virtual     ~mafPipeMesh();

	/** process events coming from gui */
	virtual void OnEvent(mafEventBase *maf_event);

	virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
	virtual void Select(bool select); 

	/** IDs for the GUI */
	enum PIPE_SURFACE_WIDGET_ID
	{
		ID_LAST = Superclass::ID_LAST,
	};

	/** Set the actor picking*/
	void SetActorPicking(int enable = true);

protected:
	vtkDataSetMapper	      *m_Mapper;
	vtkActor                *m_Actor;
	vtkOutlineCornerFilter  *m_OutlineBox;
	vtkPolyDataMapper       *m_OutlineMapper;
	vtkProperty             *m_OutlineProperty;
	vtkActor                *m_OutlineActor;
	mafAxes                 *m_Axes;

	void UpdateProperty(bool fromTag = false);

	virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeMesh_H__
