/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceEditor
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeSurfaceEditor_H__
#define __albaPipeSurfaceEditor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkGlyph3D;
class vtkALBATubeFilter;
class vtkPolyDataMapper;
class vtkAppendPolyData;
class vtkActor;
class vtkLookupTable;
class vtkALBAFixedCutter;
class vtkPlane;

/**
  Class Name: albaPipeSurfaceEditor.
  Default pipe of the albaVMESurfaceEditor, that is a vme that permit the editing of topology.
*/
class ALBA_EXPORT albaPipeSurfaceEditor : public albaPipe
{
public:
  /** RTTI Macro */
	albaTypeMacro(albaPipeSurfaceEditor,albaPipe);

  /** constructor. */
	albaPipeSurfaceEditor();
  /** destructor. */
	virtual ~albaPipeSurfaceEditor();

	/** process events coming from gui */
	virtual void OnEvent(albaEventBase *alba_event);

  /** function that create the pipeline instancing vtk graphic pipe. */
	virtual void Create(albaSceneNode *n);
  /** Show can be used for hide or show elements (actually empty) */
	void Show(bool show); 
  /** During selection of vme, it can visualize graphic element (actually empty)*/
	virtual void Select(bool select);


protected:
  /** Creation of the gui that will be attached to visual prop panel.*/
	virtual albaGUI  *CreateGui();

	vtkPolyDataMapper	*m_Mapper;
	vtkActor					*m_Actor;

	vtkLookupTable		*m_LUT;
};
#endif // __albaPipeSurfaceEditor_H__
