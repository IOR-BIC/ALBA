/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceEditor
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeSurfaceEditor_H__
#define __mafPipeSurfaceEditor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkGlyph3D;
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkAppendPolyData;
class vtkActor;
class vtkLookupTable;
class vtkMAFFixedCutter;
class vtkPlane;

/**
  Class Name: mafPipeSurfaceEditor.
  Default pipe of the mafVMESurfaceEditor, that is a vme that permit the editing of topology.
*/
class MAF_EXPORT mafPipeSurfaceEditor : public mafPipe
{
public:
  /** RTTI Macro */
	mafTypeMacro(mafPipeSurfaceEditor,mafPipe);

  /** constructor. */
	mafPipeSurfaceEditor();
  /** destructor. */
	virtual ~mafPipeSurfaceEditor();

	/** process events coming from gui */
	virtual void OnEvent(mafEventBase *maf_event);

  /** function that create the pipeline instancing vtk graphic pipe. */
	virtual void Create(mafSceneNode *n);
  /** Show can be used for hide or show elements (actually empty) */
	void Show(bool show); 
  /** During selection of vme, it can visualize graphic element (actually empty)*/
	virtual void Select(bool select);


protected:
  /** Creation of the gui that will be attached to visual prop panel.*/
	virtual mafGUI  *CreateGui();

	vtkPolyDataMapper	*m_Mapper;
	vtkActor					*m_Actor;

	vtkLookupTable		*m_LUT;
};
#endif // __mafPipeSurfaceEditor_H__
